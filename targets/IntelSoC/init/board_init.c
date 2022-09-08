/******************************************************************************
*
* Copyright 2014 Altera Corporation. All Rights Reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
* 1. Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
* 
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
* 
* 3. Neither the name of the copyright holder nor the names of its contributors
* may be used to endorse or promote products derived from this software without
* specific prior written permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
* 
******************************************************************************/

/*
 * $Id: //depot/embedded/rel/15.0/examples/software/Altera-SoCFPGA-HardwareLib-MPL/core/board_init.c#1 $
 */

#include <stdio.h>
#include <string.h> // memcpy
// include socal headers
#include <socal/socal.h>
#include <socal/hps.h>
#include <socal/alt_rstmgr.h>
#include <socal/alt_clkmgr.h>
#if __has_include ("socal/alt_l3.h")
	#include <socal/alt_l3.h>
#endif
#include <socal/alt_sysmgr.h>
// include needed hwlib headers
#include <hwlib.h>
#include <alt_timers.h>
#include <alt_clock_manager.h>

// include altera_ip headers
#include <freeze_controller.h>
#include <scan_manager.h>
#include <sdram.h>

#include <mpl_common.h>
#include <mpl_config.h>

// passdown file headers to include
#include <sdram/sdram_config.h>  // SDRAM configuration
#include <sequencer_defines.h>  // SDRAM calibration files
#include <pinmux_config.h>  // Pin muxing configuration
#include <pll_config.h>     // PLL configuration

#ifdef CONFIG_SOCFPGA_ARRIA5
#include <iocsr_config_arria5.h>    // IO bank configuration
#else
#include <iocsr_config_cyclone5.h>  // IO bank configuration
#endif

#include <build.h>  // Build and miscellaneous settings



// utilities macros
#define CLKMGR_SDRPLLGRP_VCO_SSRC_GET(x)        (((x) & 0x00c00000) >> 22)
#define CLKMGR_MAINPLLGRP_VCO_DENOM_GET(x)      (((x) & 0x003f0000) >> 16)
#define CLKMGR_SDRPLLGRP_VCO_NUMER_GET(x)       (((x) & 0x0000fff8) >> 3)
#define CLKMGR_SDRPLLGRP_DDRDQSCLK_CNT_GET(x)   (((x) & 0x000001ff) >> 0)
#define CLKMGR_VCO_SSRC_EOSC1           0x0
#define CLKMGR_VCO_SSRC_EOSC2           0x1
#define CLKMGR_VCO_SSRC_F2S             0x2
#define CLKMGR_SDRPLLGRP_VCO_DENOM_GET(x)       (((x) & 0x003f0000) >> 16)
#define CLKMGR_SDRPLLGRP_VCO_NUMER_GET(x)       (((x) & 0x0000fff8) >> 3)
#define CLKMGR_MAINPLLGRP_VCO_DENOM_GET(x)      (((x) & 0x003f0000) >> 16)
#define CLKMGR_MAINPLLGRP_VCO_NUMER_GET(x)      (((x) & 0x0000fff8) >> 3)


// function and variable prototypes
ALT_STATUS_CODE basic_clocks_init(void);  // defined below
void sysmgr_pinmux_init(void); // defined in system_manager_pinmux.c
// for use in scan manager --- defined in iocsr_config_"board" source file
extern const unsigned long iocsr_scan_chain0_table[((CONFIG_HPS_IOCSR_SCANCHAIN0_LENGTH / 32) + 1)];
extern const unsigned long iocsr_scan_chain1_table[((CONFIG_HPS_IOCSR_SCANCHAIN1_LENGTH / 32) + 1)];
extern const unsigned long iocsr_scan_chain2_table[((CONFIG_HPS_IOCSR_SCANCHAIN2_LENGTH / 32) + 1)];
extern const unsigned long iocsr_scan_chain3_table[((CONFIG_HPS_IOCSR_SCANCHAIN3_LENGTH / 32) + 1)];

// defined below
void print_clock_info(void);
void ram_boot_setup(void);



// boart init function is responsible for doing the board/chip init
// configure re-map, reset_manager, clock manager (pll), and I/Os
// also init and calibrate sdram
ALT_STATUS_CODE board_init(void)
{
    ALT_STATUS_CODE ret;
    // board init sequence from the MPL_Boot_Flow
    // Re-setup L4 watchdog
    // if WD not enabled assert WD reset to disable it.
#if (CONFIG_PRELOADER_WATCHDOG_ENABLE == 0)
    alt_setbits_word(ALT_RSTMGR_PERMODRST_ADDR, ALT_RSTMGR_PERMODRST_L4WD0_SET_MSK);
#endif

    MPL_WATCHDOG();

    // ensure all IO banks are in frozen state via Freeze Controller
    sys_mgr_frzctrl_freeze_req(FREEZE_CHANNEL_0, FREEZE_CONTROLLER_FSM_SW);
    sys_mgr_frzctrl_freeze_req(FREEZE_CHANNEL_1, FREEZE_CONTROLLER_FSM_SW);
    sys_mgr_frzctrl_freeze_req(FREEZE_CHANNEL_2, FREEZE_CONTROLLER_FSM_SW);
    sys_mgr_frzctrl_freeze_req(FREEZE_CHANNEL_3, FREEZE_CONTROLLER_FSM_SW);

    // Assert reset to peripherals except L4wd0 and all bridges
    alt_write_word(ALT_RSTMGR_PERMODRST_ADDR, ALT_RSTMGR_PERMODRST_L4WD0_CLR_MSK);
    //alt_write_word(ALT_RSTMGR_BRGMODRST_ADDR, 0x7);

    // Enable and init OSC1 Timer0 (required during PLL-config)
    alt_clrbits_word(ALT_RSTMGR_PERMODRST_ADDR, ALT_RSTMGR_PERMODRST_OSC1TMR0_SET_MSK);  // release reset bit
    ret = alt_gpt_mode_set(ALT_GPT_OSC1_TMR0, ALT_GPT_RESTART_MODE_PERIODIC); // set-mode to restart
    if (ret != ALT_E_SUCCESS){ return ret;}
    ret = alt_gpt_counter_set(ALT_GPT_OSC1_TMR0, 0xFFFFFFFF); // set loadreg value to max
    if (ret != ALT_E_SUCCESS){ return ret;}
    ret = alt_gpt_tmr_start(ALT_GPT_OSC1_TMR0); // start timer
    if (ret != ALT_E_SUCCESS){ return ret;}

    MPL_WATCHDOG();

    // Set up the PLL RAM Boot fix, if needed
    ram_boot_setup();

    // Re-configuration of all PLLs
    ret = basic_clocks_init();
    if (ret != ALT_E_SUCCESS){ return ret;}

    // init Scan Manager to configure HPS IOCSR
    ret = scan_mgr_io_scan_chain_prg(IO_SCAN_CHAIN_0, CONFIG_HPS_IOCSR_SCANCHAIN0_LENGTH, iocsr_scan_chain0_table);
    if (ret != ALT_E_SUCCESS){ return ret;}
    ret = scan_mgr_io_scan_chain_prg(IO_SCAN_CHAIN_1, CONFIG_HPS_IOCSR_SCANCHAIN1_LENGTH, iocsr_scan_chain1_table);
    if (ret != ALT_E_SUCCESS){ return ret;}
    ret = scan_mgr_io_scan_chain_prg(IO_SCAN_CHAIN_2, CONFIG_HPS_IOCSR_SCANCHAIN2_LENGTH, iocsr_scan_chain2_table);
    if (ret != ALT_E_SUCCESS){ return ret;}
    ret = scan_mgr_io_scan_chain_prg(IO_SCAN_CHAIN_3, CONFIG_HPS_IOCSR_SCANCHAIN3_LENGTH, iocsr_scan_chain3_table);
    if (ret != ALT_E_SUCCESS){ return ret;}

    MPL_WATCHDOG();
    // config pin mux via system manager
    sysmgr_pinmux_init();

    // thaw (un-freeze) all IO banks
    sys_mgr_frzctrl_thaw_req(FREEZE_CHANNEL_0, FREEZE_CONTROLLER_FSM_SW);
    sys_mgr_frzctrl_thaw_req(FREEZE_CHANNEL_1, FREEZE_CONTROLLER_FSM_SW);
    sys_mgr_frzctrl_thaw_req(FREEZE_CHANNEL_2, FREEZE_CONTROLLER_FSM_SW);
    sys_mgr_frzctrl_thaw_req(FREEZE_CHANNEL_3, FREEZE_CONTROLLER_FSM_SW);

    MPL_WATCHDOG();

    // console init (if serial support enabled)
    ALT_PRINTF("INIT: Initializing board.\n");

    // SDRAM init and calibration
    // sdram mmr init
    ret = altr_sdram_mmr_init_full();
    if (ret != ALT_E_SUCCESS){ return ret;}
    // sdram calibration
    ret = altr_sdram_calibration_full();
    if (ret != ALT_E_SUCCESS){ return ret;}

    MPL_WATCHDOG();

    // Interconnect config
    alt_write_word(ALT_L3_SEC_LWH2F_ADDR, 0x1);  // allow sec + non-sec accesses to lw-bridge
    alt_write_word(ALT_L3_SEC_H2F_ADDR, 0x1);  // allow sec + non-sec accesses to h2f-bridge
    alt_write_word(ALT_L3_SEC_ACP_ADDR, 0x1);  // allow sec + non-sec accesses to acp port
    alt_write_word(ALT_L3_SEC_ROM_ADDR, 0x1);  // allow sec + non-sec accesses to ROM
    alt_write_word(ALT_L3_SEC_OCRAM_ADDR, 0x1);  // allow sec + non-sec accesses to OCRAM
    alt_write_word(ALT_L3_SEC_SDRDATA_ADDR, 0x1);  // allow sec + non-sec accesses to SDRAM
#ifdef BOOT_FROM_FPGA
    alt_write_word(ALT_L3_REMAP_ADDR, 0x18 | 0x3); // remap 0x0 to OCRAM, Maintain FPGA connection
#else
    alt_write_word(ALT_L3_REMAP_ADDR, 0x3); // remap 0x0 to OCRAM
#endif

    print_clock_info();

    ALT_PRINTF("INIT: Initializing successful.\n");
    return ret;
}

#define CLKMGR_PLL_VCO_RESET_VALUE  0x8001000d
#define CLKMGR_PLL_VCO_REGEXTSEL_MASK 0x80000000
#define CLKMGR_PERPLL_SRC_RESET_VALUE   0x00000015
#define CLKMGR_MAINPLL_L4SRC_RESET_VALUE    0x00000000

// PLL-init from baremetal_fw
// Setup clocks while making no assumptions of the previous state of the clocks.
// ---   Start by being paranoid and gate all sw managed clocks
// ---   Put all plls in bypass
// ---   Put all plls VCO registers back to reset value (bgpwr dwn).
// ---   Put peripheral and main pll src to reset value to avoid glitch.
// ---   Delay 5 us.
// ---   Deassert bg pwr dn and set numerator and denominator
// ---   Start 7 us timer.
// ---   set internal dividers
// ---   Wait for 7 us timer.
// ---   Enable plls
// ---   Set external dividers while plls are locking
// ---   Wait for pll lock
// ---   Assert/deassert outreset all.
// ---   Take all pll's out of bypass
// ---   Clear safe mode
// ---   set source main and peripheral clocks
// ---   Ungate clocks
ALT_STATUS_CODE basic_clocks_init(void) {
    uint32_t ret;
    volatile uint32_t temp;
    volatile uint32_t retry;
    volatile uint32_t start_count;
    volatile uint32_t clk_cycles_count;

    ret = ALT_E_SUCCESS;

    // fogbugz 61158 - we need to disable nandclk
    // and then do another apb access before disabling
    // gatting off the rest of the periperal clocks.
    alt_write_word(ALT_CLKMGR_PERPLL_EN_ADDR,
            (ALT_CLKMGR_PERPLL_EN_NANDCLK_CLR_MSK & alt_read_word(ALT_CLKMGR_PERPLL_EN_ADDR)));

    // DO NOT GATE OFF DEBUG CLOCKS
    alt_write_word(ALT_CLKMGR_MAINPLL_EN_ADDR,
            ALT_CLKMGR_MAINPLL_EN_DBGTMRCLK_SET_MSK |
            ALT_CLKMGR_MAINPLL_EN_DBGTRACECLK_SET_MSK |
            ALT_CLKMGR_MAINPLL_EN_DBGCLK_SET_MSK |
            ALT_CLKMGR_MAINPLL_EN_DBGATCLK_SET_MSK |
            ALT_CLKMGR_MAINPLL_EN_S2FUSER0CLK_SET_MSK |
            ALT_CLKMGR_MAINPLL_EN_L4MPCLK_SET_MSK);

    alt_write_word(ALT_CLKMGR_SDRPLL_EN_ADDR, 0);

    // Put all plls in bypass
    alt_write_word(ALT_CLKMGR_BYPASS_ADDR, 0 |
            ALT_CLKMGR_BYPASS_PERPLLSRC_SET(ALT_CLKMGR_BYPASS_PERPLLSRC_E_SELECT_EOSC1) |
            ALT_CLKMGR_BYPASS_PERPLL_SET_MSK |
            ALT_CLKMGR_BYPASS_SDRPLLSRC_SET(ALT_CLKMGR_BYPASS_SDRPLLSRC_E_SELECT_EOSC1) |
            ALT_CLKMGR_BYPASS_SDRPLL_SET_MSK |
            ALT_CLKMGR_BYPASS_MAINPLL_SET_MSK);

    // function to poll in the fsm busy bit
    // after bypass reg access wait for fsm
    temp = 0;
    do {
        temp = (alt_read_word(ALT_CLKMGR_STAT_ADDR) & ALT_CLKMGR_STAT_BUSY_SET_MSK);
    } while (temp);

    // Put all plls VCO registers back to reset value.
    // Some code might have messed with them.
    alt_write_word(ALT_CLKMGR_MAINPLL_VCO_ADDR, CLKMGR_PLL_VCO_RESET_VALUE & ~CLKMGR_PLL_VCO_REGEXTSEL_MASK);
    alt_write_word(ALT_CLKMGR_PERPLL_VCO_ADDR, CLKMGR_PLL_VCO_RESET_VALUE & ~CLKMGR_PLL_VCO_REGEXTSEL_MASK);
    alt_write_word(ALT_CLKMGR_SDRPLL_VCO_ADDR, CLKMGR_PLL_VCO_RESET_VALUE & ~CLKMGR_PLL_VCO_REGEXTSEL_MASK);

    // Fogbugz 60434 - the clocks to the flash devices and the L4_MAIN clocks can glitch
    // when coming out of safe mode if their source values are different from their reset
    // value.  So the trick it to put them back to their reset state, and change input
    // after exiting safe mode but before ungating the clocks.
    alt_write_word(ALT_CLKMGR_PERPLL_SRC_ADDR, CLKMGR_PERPLL_SRC_RESET_VALUE);
    alt_write_word(ALT_CLKMGR_MAINPLL_L4SRC_ADDR, CLKMGR_MAINPLL_L4SRC_RESET_VALUE);

    // read back for the required 5 us delay.
    temp = alt_read_word(ALT_CLKMGR_MAINPLL_VCO_ADDR);
    temp = alt_read_word(ALT_CLKMGR_PERPLL_VCO_ADDR);
    temp = alt_read_word(ALT_CLKMGR_SDRPLL_VCO_ADDR);

    // We made sure bgpwr down was assert for 5 us. Now deassert BG PWR DN
    // and set VCO reg with values from passdown file.
    alt_write_word(ALT_CLKMGR_MAINPLL_VCO_ADDR, 0 |
            ALT_CLKMGR_MAINPLL_VCO_DENOM_SET(CONFIG_HPS_MAINPLLGRP_VCO_DENOM) |
            ALT_CLKMGR_MAINPLL_VCO_NUMER_SET(CONFIG_HPS_MAINPLLGRP_VCO_NUMER) |
            ALT_CLKMGR_MAINPLL_VCO_PWRDN_SET(0) |
            ALT_CLKMGR_MAINPLL_VCO_EN_SET(0) |
            ALT_CLKMGR_MAINPLL_VCO_BGPWRDN_SET(0) |
            ALT_CLKMGR_MAINPLL_VCO_REGEXTSEL_SET_MSK);

    alt_write_word(ALT_CLKMGR_PERPLL_VCO_ADDR, 0 |
            ALT_CLKMGR_PERPLL_VCO_DENOM_SET(CONFIG_HPS_PERPLLGRP_VCO_DENOM) |
            ALT_CLKMGR_PERPLL_VCO_NUMER_SET(CONFIG_HPS_PERPLLGRP_VCO_NUMER) |
            ALT_CLKMGR_PERPLL_VCO_PSRC_SET(CONFIG_HPS_PERPLLGRP_VCO_PSRC) |
            ALT_CLKMGR_PERPLL_VCO_PWRDN_SET(0) |
            ALT_CLKMGR_PERPLL_VCO_EN_SET(0) |
            ALT_CLKMGR_PERPLL_VCO_BGPWRDN_SET(0) |
            ALT_CLKMGR_PERPLL_VCO_REGEXTSEL_SET_MSK);

    alt_write_word(ALT_CLKMGR_SDRPLL_VCO_ADDR, 0 |
            ALT_CLKMGR_SDRPLL_VCO_DENOM_SET(CONFIG_HPS_SDRPLLGRP_VCO_DENOM) |
            ALT_CLKMGR_SDRPLL_VCO_NUMER_SET(CONFIG_HPS_SDRPLLGRP_VCO_NUMER) |
            ALT_CLKMGR_SDRPLL_VCO_SSRC_SET(CONFIG_HPS_SDRPLLGRP_VCO_SSRC) |
            ALT_CLKMGR_SDRPLL_VCO_OUTRSTALL_SET(0) |
            ALT_CLKMGR_SDRPLL_VCO_OUTRST_SET(0) |
            ALT_CLKMGR_SDRPLL_VCO_PWRDN_SET(0) |
            ALT_CLKMGR_SDRPLL_VCO_EN_SET(0) |
            ALT_CLKMGR_SDRPLL_VCO_BGPWRDN_SET(0) |
            ALT_CLKMGR_SDRPLL_VCO_REGEXTSEL_SET_MSK);

    // Time starts here
    // must wait 7 us from BGPWRDN_SET(0) to VCO_ENABLE_SET(1)
    start_count = alt_gpt_counter_get(ALT_GPT_OSC1_TMR0);

    // main mpu cnt
    alt_write_word(ALT_CLKMGR_MAINPLL_MPUCLK_ADDR,
            ALT_CLKMGR_MAINPLL_MPUCLK_CNT_SET(CONFIG_HPS_MAINPLLGRP_MPUCLK_CNT));

    // main main clock cnt
    alt_write_word(ALT_CLKMGR_MAINPLL_MAINCLK_ADDR,
            ALT_CLKMGR_MAINPLL_MAINCLK_CNT_SET(CONFIG_HPS_MAINPLLGRP_MAINCLK_CNT));



/* The start address of the ALT_CLKMGR_ALTERAPLL component. */
#define ALT_CLKMGR_ALTERAGRP_MPUCLK_ADDR        ALT_CAST(void *, (ALT_CAST(char *, ALT_CLKMGR_ADDR) + 0xe0))
#define ALT_CLKMGR_ALTERAGRP_MAINCLK_ADDR       ALT_CAST(void *, (ALT_CAST(char *, ALT_CLKMGR_ADDR) + 0xe4))


#ifdef CONFIG_HPS_ALTERAGRP_MPUCLK
     // re-configuring fixed divider for faster MPU clock
     alt_write_word(ALT_CLKMGR_ALTERAGRP_MPUCLK_ADDR, CONFIG_HPS_ALTERAGRP_MPUCLK);
#endif

    // main for dbg cnt
    alt_write_word(ALT_CLKMGR_MAINPLL_DBGATCLK_ADDR,
            ALT_CLKMGR_MAINPLL_DBGATCLK_CNT_SET(CONFIG_HPS_MAINPLLGRP_DBGATCLK_CNT));

#ifdef CONFIG_HPS_ALTERAGRP_MAINCLK
     // re-configuring the fixed divider due to different main VCO
     alt_write_word(ALT_CLKMGR_ALTERAGRP_MAINCLK_ADDR, CONFIG_HPS_ALTERAGRP_MAINCLK);
#endif

    // main for cfgs2fuser0clk cnt
    alt_write_word(ALT_CLKMGR_MAINPLL_CFGS2FUSER0CLK_ADDR,
            ALT_CLKMGR_MAINPLL_CFGS2FUSER0CLK_CNT_SET(CONFIG_HPS_MAINPLLGRP_CFGS2FUSER0CLK_CNT));

    //Peri emac0 cnt
    alt_write_word(ALT_CLKMGR_PERPLL_EMAC0CLK_ADDR,
            ALT_CLKMGR_PERPLL_EMAC0CLK_CNT_SET(CONFIG_HPS_PERPLLGRP_EMAC0CLK_CNT));

    //Peri emac1 cnt
    alt_write_word(ALT_CLKMGR_PERPLL_EMAC1CLK_ADDR,
                ALT_CLKMGR_PERPLL_EMAC1CLK_CNT_SET(CONFIG_HPS_PERPLLGRP_EMAC1CLK_CNT));

    // Peri QSPI div
    alt_write_word(ALT_CLKMGR_MAINPLL_MAINQSPICLK_ADDR,
            ALT_CLKMGR_MAINPLL_MAINQSPICLK_CNT_SET(CONFIG_HPS_MAINPLLGRP_MAINQSPICLK_CNT));

    alt_write_word(ALT_CLKMGR_PERPLL_PERQSPICLK_ADDR,
            ALT_CLKMGR_PERPLL_PERQSPICLK_CNT_SET(CONFIG_HPS_PERPLLGRP_PERQSPICLK_CNT));

    // Peri pernandsdmmcclk cnt
    alt_write_word(ALT_CLKMGR_PERPLL_PERNANDSDMMCCLK_ADDR,
            ALT_CLKMGR_PERPLL_PERNANDSDMMCCLK_CNT_SET(CONFIG_HPS_PERPLLGRP_PERNANDSDMMCCLK_CNT));

    // Peri perbaseclk cnt
    alt_write_word(ALT_CLKMGR_PERPLL_PERBASECLK_ADDR,
            ALT_CLKMGR_PERPLL_PERBASECLK_CNT_SET(CONFIG_HPS_PERPLLGRP_PERBASECLK_CNT));

    // Peri s2fuser1clk cnt
    alt_write_word(ALT_CLKMGR_PERPLL_S2FUSER1CLK_ADDR,
            ALT_CLKMGR_PERPLL_S2FUSER1CLK_CNT_SET(CONFIG_HPS_PERPLLGRP_S2FUSER1CLK_CNT));

    // 7 us must have elapsed before we can enable vco's
    do {
        clk_cycles_count = start_count - alt_gpt_counter_get(ALT_GPT_OSC1_TMR0);
    } while (clk_cycles_count < 7);

    // Enable vco's
    // main pll vco is typically 800 MHz.
    alt_write_word(ALT_CLKMGR_MAINPLL_VCO_ADDR, 0 |
                ALT_CLKMGR_MAINPLL_VCO_DENOM_SET(CONFIG_HPS_MAINPLLGRP_VCO_DENOM) |
                ALT_CLKMGR_MAINPLL_VCO_NUMER_SET(CONFIG_HPS_MAINPLLGRP_VCO_NUMER) |
                ALT_CLKMGR_MAINPLL_VCO_PWRDN_SET(0) |
                ALT_CLKMGR_MAINPLL_VCO_EN_SET(1) |
                ALT_CLKMGR_MAINPLL_VCO_BGPWRDN_SET(0));

    // periferal pll typically 1000 MHz.
    alt_write_word(ALT_CLKMGR_PERPLL_VCO_ADDR, 0 |
                ALT_CLKMGR_PERPLL_VCO_DENOM_SET(CONFIG_HPS_PERPLLGRP_VCO_DENOM) |
                ALT_CLKMGR_PERPLL_VCO_NUMER_SET(CONFIG_HPS_PERPLLGRP_VCO_NUMER) |
                ALT_CLKMGR_PERPLL_VCO_PSRC_SET(CONFIG_HPS_PERPLLGRP_VCO_PSRC) |
                ALT_CLKMGR_PERPLL_VCO_PWRDN_SET(0) |
                ALT_CLKMGR_PERPLL_VCO_EN_SET(1) |
                ALT_CLKMGR_PERPLL_VCO_BGPWRDN_SET(0));

    // sdram pll vco 1062.5 MHz.
    alt_write_word(ALT_CLKMGR_SDRPLL_VCO_ADDR, 0 |
                ALT_CLKMGR_SDRPLL_VCO_DENOM_SET(CONFIG_HPS_SDRPLLGRP_VCO_DENOM) |
                ALT_CLKMGR_SDRPLL_VCO_NUMER_SET(CONFIG_HPS_SDRPLLGRP_VCO_NUMER) |
                ALT_CLKMGR_SDRPLL_VCO_SSRC_SET(CONFIG_HPS_SDRPLLGRP_VCO_SSRC) |
                ALT_CLKMGR_SDRPLL_VCO_OUTRSTALL_SET(0) |
                ALT_CLKMGR_SDRPLL_VCO_OUTRST_SET(0) |
                ALT_CLKMGR_SDRPLL_VCO_PWRDN_SET(0) |
                ALT_CLKMGR_SDRPLL_VCO_EN_SET(1) |
                ALT_CLKMGR_SDRPLL_VCO_BGPWRDN_SET(0));

    // setup dividers while plls are locking
    // set L3 MP and L3 SP to divide by 2 from main_clk,
    // down to 400 MHz from the 800 MHz main_clk
    alt_write_word(ALT_CLKMGR_MAINPLL_MAINDIV_ADDR,
            ALT_CLKMGR_MAINPLL_MAINDIV_L3MPCLK_SET(CONFIG_HPS_MAINPLLGRP_MAINDIV_L3MPCLK) |
            ALT_CLKMGR_MAINPLL_MAINDIV_L3SPCLK_SET(CONFIG_HPS_MAINPLLGRP_MAINDIV_L3SPCLK) |
            ALT_CLKMGR_MAINPLL_MAINDIV_L4MPCLK_SET(CONFIG_HPS_MAINPLLGRP_MAINDIV_L4MPCLK) |
            ALT_CLKMGR_MAINPLL_MAINDIV_L4SPCLK_SET(CONFIG_HPS_MAINPLLGRP_MAINDIV_L4SPCLK));

    alt_write_word(ALT_CLKMGR_MAINPLL_DBGDIV_ADDR,
            ALT_CLKMGR_MAINPLL_DBGDIV_DBGATCLK_SET(CONFIG_HPS_MAINPLLGRP_DBGDIV_DBGATCLK) |
            ALT_CLKMGR_MAINPLL_DBGDIV_DBGCLK_SET(CONFIG_HPS_MAINPLLGRP_DBGDIV_DBGCLK));

    alt_write_word(ALT_CLKMGR_MAINPLL_TRACEDIV_ADDR,
            ALT_CLKMGR_MAINPLL_TRACEDIV_TRACECLK_SET(CONFIG_HPS_MAINPLLGRP_TRACEDIV_TRACECLK));

    // set L4 MP/SP, can0, and can1, etc.. div from perif-base_clock
    alt_write_word(ALT_CLKMGR_PERPLL_DIV_ADDR,
            ALT_CLKMGR_PERPLL_DIV_USBCLK_SET(CONFIG_HPS_PERPLLGRP_DIV_USBCLK) |
            ALT_CLKMGR_PERPLL_DIV_SPIMCLK_SET(CONFIG_HPS_PERPLLGRP_DIV_SPIMCLK) |
            ALT_CLKMGR_PERPLL_DIV_CAN0CLK_SET(CONFIG_HPS_PERPLLGRP_DIV_CAN0CLK) |
            ALT_CLKMGR_PERPLL_DIV_CAN1CLK_SET(CONFIG_HPS_PERPLLGRP_DIV_CAN1CLK));

    alt_write_word(ALT_CLKMGR_PERPLL_GPIODIV_ADDR,
            ALT_CLKMGR_PERPLL_GPIODIV_GPIODBCLK_SET(CONFIG_HPS_PERPLLGRP_GPIODIV_GPIODBCLK));

#define LOCKED_MASK \
    (ALT_CLKMGR_INTER_SDRPLLLOCKED_SET_MSK  | \
    ALT_CLKMGR_INTER_PERPLLLOCKED_SET_MSK  | \
    ALT_CLKMGR_INTER_MAINPLLLOCKED_SET_MSK)

    // wait for 10 consecutive lock reads
    temp = 0;
    retry = 0;
    do {
        temp = alt_read_word(ALT_CLKMGR_INTER_ADDR) & LOCKED_MASK;
        if (temp == LOCKED_MASK){
            retry++;
        }
        else {
            retry = 0;
        }
        if (retry > 9){
            break;
        }
    } while (1);

    // write the sdram clock counters before toggling outreset all (without phase info)
    alt_write_word(ALT_CLKMGR_SDRPLL_DDRDQSCLK_ADDR,
            ALT_CLKMGR_SDRPLL_DDRDQSCLK_CNT_SET(CONFIG_HPS_SDRPLLGRP_DDRDQSCLK_CNT));

    alt_write_word(ALT_CLKMGR_SDRPLL_DDR2XDQSCLK_ADDR,
            ALT_CLKMGR_SDRPLL_DDR2XDQSCLK_CNT_SET(CONFIG_HPS_SDRPLLGRP_DDR2XDQSCLK_CNT));

    alt_write_word(ALT_CLKMGR_SDRPLL_DDRDQCLK_ADDR,
            ALT_CLKMGR_SDRPLL_DDRDQCLK_CNT_SET(CONFIG_HPS_SDRPLLGRP_DDRDQCLK_CNT));

    alt_write_word(ALT_CLKMGR_SDRPLL_S2FUSER2CLK_ADDR,
            ALT_CLKMGR_SDRPLL_S2FUSER2CLK_CNT_SET(CONFIG_HPS_SDRPLLGRP_S2FUSER2CLK_CNT));

    // after locking, but before taking out of bypass assert/deassert outresetall
    uint32_t mainvco = alt_read_word(ALT_CLKMGR_MAINPLL_VCO_ADDR);

    // assert main outresetall
    alt_write_word(ALT_CLKMGR_MAINPLL_VCO_ADDR, mainvco | ALT_CLKMGR_MAINPLL_VCO_OUTRSTALL_SET_MSK);

    uint32_t periphvco = alt_read_word(ALT_CLKMGR_PERPLL_VCO_ADDR);

    // assert pheriph outresetall
    alt_write_word(ALT_CLKMGR_PERPLL_VCO_ADDR, periphvco | ALT_CLKMGR_PERPLL_VCO_OUTRSTALL_SET_MSK);

    // assert sdram outresetall
    alt_setbits_word(ALT_CLKMGR_SDRPLL_VCO_ADDR, ALT_CLKMGR_SDRPLL_VCO_OUTRSTALL_SET_MSK);

    // deassert main outresetall
    alt_write_word(ALT_CLKMGR_MAINPLL_VCO_ADDR, mainvco & ~ALT_CLKMGR_MAINPLL_VCO_OUTRSTALL_SET_MSK);

    // deassert pheriph outresetall
    alt_write_word(ALT_CLKMGR_PERPLL_VCO_ADDR, periphvco & ~ALT_CLKMGR_PERPLL_VCO_OUTRSTALL_SET_MSK);

    // deassert sdram outresetall
    alt_clrbits_word(ALT_CLKMGR_SDRPLL_VCO_ADDR, ALT_CLKMGR_SDRPLL_VCO_OUTRSTALL_SET_MSK);

    // now that we've toggled outreset all, all the clocks are aligned nicely; so we can change any phase.
    // re-write the sdram clock counters with phase info
    alt_write_word(ALT_CLKMGR_SDRPLL_DDRDQSCLK_ADDR,
            ALT_CLKMGR_SDRPLL_DDRDQSCLK_PHASE_SET(CONFIG_HPS_SDRPLLGRP_DDRDQSCLK_PHASE) |
            ALT_CLKMGR_SDRPLL_DDRDQSCLK_CNT_SET(CONFIG_HPS_SDRPLLGRP_DDRDQSCLK_CNT));

    // SDRAM DDR2XDQSCLK 1062.5 MHz
    alt_write_word(ALT_CLKMGR_SDRPLL_DDR2XDQSCLK_ADDR,
            ALT_CLKMGR_SDRPLL_DDR2XDQSCLK_PHASE_SET(CONFIG_HPS_SDRPLLGRP_DDR2XDQSCLK_PHASE) |
            ALT_CLKMGR_SDRPLL_DDR2XDQSCLK_CNT_SET(CONFIG_HPS_SDRPLLGRP_DDR2XDQSCLK_CNT));

    alt_write_word(ALT_CLKMGR_SDRPLL_DDRDQCLK_ADDR,
            ALT_CLKMGR_SDRPLL_DDRDQCLK_PHASE_SET(CONFIG_HPS_SDRPLLGRP_DDRDQCLK_PHASE) |
            ALT_CLKMGR_SDRPLL_DDRDQCLK_CNT_SET(CONFIG_HPS_SDRPLLGRP_DDRDQCLK_CNT));

    // divide by six to get 177.08 MHz
    alt_write_word(ALT_CLKMGR_SDRPLL_S2FUSER2CLK_ADDR,
            ALT_CLKMGR_SDRPLL_S2FUSER2CLK_PHASE_SET(CONFIG_HPS_SDRPLLGRP_S2FUSER2CLK_PHASE) |
            ALT_CLKMGR_SDRPLL_S2FUSER2CLK_CNT_SET(CONFIG_HPS_SDRPLLGRP_S2FUSER2CLK_CNT));


    // Take all three PLLs out of bypass when safe mode is cleared.
    alt_write_word(ALT_CLKMGR_BYPASS_ADDR, 0 |
            ALT_CLKMGR_BYPASS_PERPLLSRC_SET(ALT_CLKMGR_BYPASS_PERPLLSRC_E_SELECT_EOSC1) |
            ALT_CLKMGR_BYPASS_PERPLL_SET(0) |
            ALT_CLKMGR_BYPASS_SDRPLLSRC_SET(ALT_CLKMGR_BYPASS_SDRPLLSRC_E_SELECT_EOSC1) |
            ALT_CLKMGR_BYPASS_SDRPLL_SET(0) |
            ALT_CLKMGR_BYPASS_MAINPLL_SET(0));

    // after bypass reg access wait for fsm
    temp = 0;
    do {
        temp = (alt_read_word(ALT_CLKMGR_STAT_ADDR) & ALT_CLKMGR_STAT_BUSY_SET_MSK);
    } while (temp);

    // clear safe mode.. W1C write 1 to clear
    alt_setbits_word(ALT_CLKMGR_CTL_ADDR, ALT_CLKMGR_CTL_SAFEMOD_SET_MSK);
    temp = 0;
    do {
        temp = (alt_read_word(ALT_CLKMGR_STAT_ADDR) & ALT_CLKMGR_STAT_BUSY_SET_MSK);
    } while (temp);

    // now that safe mode is clear with clocks gated
    // it safe to change the source mux for the flashes the the L4_MAIN
    alt_write_word(ALT_CLKMGR_PERPLL_SRC_ADDR,
            ALT_CLKMGR_PERPLL_SRC_QSPI_SET(CONFIG_HPS_PERPLLGRP_SRC_QSPI) |
            ALT_CLKMGR_PERPLL_SRC_NAND_SET(CONFIG_HPS_PERPLLGRP_SRC_NAND) |
            ALT_CLKMGR_PERPLL_SRC_SDMMC_SET(CONFIG_HPS_PERPLLGRP_SRC_SDMMC));
    alt_write_word(ALT_CLKMGR_MAINPLL_L4SRC_ADDR,
            ALT_CLKMGR_MAINPLL_L4SRC_L4MP_SET(CONFIG_HPS_MAINPLLGRP_L4SRC_L4MP) |
            ALT_CLKMGR_MAINPLL_L4SRC_L4SP_SET(CONFIG_HPS_MAINPLLGRP_L4SRC_L4SP));

    // Now ungate non-hw-managed clocks
    alt_write_word(ALT_CLKMGR_MAINPLL_EN_ADDR, ~0);
    alt_write_word(ALT_CLKMGR_PERPLL_EN_ADDR, ~0);
    alt_write_word(ALT_CLKMGR_SDRPLL_EN_ADDR, ~0);

    return ret;
}

// calculated the clock speed based on register settings
unsigned long get_mpu_clk_hz()
{
    uint32_t reg, clock;

    // get main VCO clock
    reg = alt_read_word(ALT_CLKMGR_MAINPLL_VCO_ADDR);
    clock = CONFIG_HPS_CLK_OSC1_HZ /
        (CLKMGR_MAINPLLGRP_VCO_DENOM_GET(reg) + 1);
     clock *= (CLKMGR_MAINPLLGRP_VCO_NUMER_GET(reg) + 1);

     // get MPU clock
     reg = alt_read_word(ALT_CLKMGR_ALTERAGRP_MPUCLK_ADDR);
     clock /= (reg + 1);
     reg = alt_read_word(ALT_CLKMGR_MAINPLL_MPUCLK_ADDR);
     clock /= (reg + 1);
     return clock;
 }

unsigned long get_sdram_clk_hz()
{
    uint32_t reg, clock = 0;

    // identify SDRAM PLL clock source
    reg = alt_read_word(ALT_CLKMGR_SDRPLL_VCO_ADDR);
    reg = CLKMGR_SDRPLLGRP_VCO_SSRC_GET(reg);
    if (reg == CLKMGR_VCO_SSRC_EOSC1)
        clock = CONFIG_HPS_CLK_OSC1_HZ;
    else if (reg == CLKMGR_VCO_SSRC_EOSC2)
        clock = CONFIG_HPS_CLK_OSC2_HZ;
    else if (reg == CLKMGR_VCO_SSRC_F2S)
        clock = CONFIG_HPS_CLK_F2S_SDR_REF_HZ;

    // get SDRAM VCO clock
    reg = alt_read_word(ALT_CLKMGR_SDRPLL_VCO_ADDR);
    clock /= (CLKMGR_SDRPLLGRP_VCO_DENOM_GET(reg) + 1);
    clock *= (CLKMGR_SDRPLLGRP_VCO_NUMER_GET(reg) + 1);

    // get SDRAM (DDR_DQS) clock
    reg = alt_read_word(ALT_CLKMGR_SDRPLL_DDRDQSCLK_ADDR);
    reg = CLKMGR_SDRPLLGRP_DDRDQSCLK_CNT_GET(reg);
    clock /= (reg + 1);
    return clock;
}

void print_clock_info(void)
{
    ALT_PRINTF("INIT: MPU clock = %ld MHz\n", (get_mpu_clk_hz() / 1000000));
    ALT_PRINTF("INIT: DDR clock = %ld MHz\n", (get_sdram_clk_hz() / 1000000));
}

/*
 * Setup RAM boot to ensure the clock are reset under CSEL = 0
 */
extern uint32_t reset_clock_manager_size;
extern void reset_clock_manager(void);

#define CONFIG_SYSMGR_WARMRAMGRP_ENABLE_MAGIC   0xae9efebc

/*
 * A future version of the bsp-editor will include a new value that tells
 * the code to include the warm reset PLL fix.  This fix is required under certain
 * conditions if warm and cold reset are not tied together.
 *
 * For more information,see the errata that describes this fix:
 *
 *   For Cyclone V: http://www.altera.com/literature/es/es_cyclone_v.pdf
 *   For Arria V:   http://www.altera.com/literature/es/es_arria_V.pdf
 *
 * Include the define here until it is in the handoff information and
 * by default, enable the warm boot fix.
 */

#ifndef CONFIG_PRELOADER_RAMBOOT_PLLRESET
#define CONFIG_PRELOADER_RAMBOOT_PLLRESET 1
#endif

void ram_boot_setup(void)
{
    /* We need the fix if we have been asked to put it in and we are not
     * booting from the FPGA and we are using CSEL_0 */

#if ((CONFIG_PRELOADER_RAMBOOT_PLLRESET == 1) && (CONFIG_PRELOADER_EXE_ON_FPGA == 0))

    unsigned csel, ramboot_addr;
    unsigned tmp_daddr;
    unsigned tmp_saddr;
    unsigned int bootinfo;

    bootinfo = alt_read_word(ALT_SYSMGR_BOOT_ADDR);

    csel = (bootinfo & ALT_SYSMGR_BOOT_CSEL_SET_MSK) >> ALT_SYSMGR_BOOT_CSEL_LSB;

    if (csel == ALT_SYSMGR_BOOT_CSEL_E_CSEL_0)
    {
        /*
         * Copy the ramboot program at offset 60 KiB in the on-chip RAM.
         * The copied program must be less than 4 KiB in size.
	 * The customer can modify any content within the first
	 * 60kB of on-chip RAM at any boot stage.
         */

        ramboot_addr = (uint32_t)ALT_OCRAM_ADDR + 0xf000;

        tmp_daddr=(unsigned)ramboot_addr;
        tmp_saddr=(unsigned)reset_clock_manager;
        memcpy((void*)tmp_daddr, (void*)tmp_saddr, reset_clock_manager_size);

        /* tell BootROM where the RAM boot code starts */
        alt_write_word(ALT_SYSMGR_ROMCODE_ROMCODE_WARMRAM_EXECUTION_ADDR, ramboot_addr & ALT_SYSMGR_ROMCODE_WARMRAM_EXECUTION_OFFSET_SET_MSK);

        /* Enable RAM boot */
        alt_write_word(ALT_SYSMGR_ROMCODE_ROMCODE_WARMRAM_EN_ADDR, CONFIG_SYSMGR_WARMRAMGRP_ENABLE_MAGIC);
    }

#endif /* PLLRESET fix is enabled and system did not boot from the FPGA */

}


void interrupt_handlers (void)
{
   // check if I bit is set in SPSR to detect spurious interrupts
   __asm__("stmfd sp!, {r0}");
   __asm__("mrs r0, spsr");
   __asm__("tst r0, #0x80");
   __asm__("ldmfd sp!, {r0}");
   __asm__("subnes pc, lr, #4");
   __asm__("stmfd sp!, {r0-r4, r12, lr}");

//   alt_int_handler_irq();

   // return from interrupt
   __asm__("ldmfd sp!, {r0-r4, r12, lr}");
   __asm__("subs pc, lr, #4");
}
