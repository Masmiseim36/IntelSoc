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
 * $Id: //depot/embedded/rel/15.0/examples/software/Altera-SoCFPGA-HardwareLib-MPL/core/mpl.c#1 $
 */

#include <stdio.h>
#include "build.h"
#include "sdram/sdram_config.h"
#include "reset_config.h"
#include "alt_fpga_manager.h"
#include "socal/alt_l3.h"
#include "socal/alt_rstmgr.h"
#include "socal/alt_sysmgr.h"
#include "socal/alt_sdr.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "mpl_config.h"
#include "mpl_common.h"

#include "alt_bridge_manager.h"
#include "alt_address_space.h"


// TODO
#ifndef CONFIG_PRELOADER_DEBUG_MEMORY_ADDR  // default needed for startup.s
#define CONFIG_PRELOADER_DEBUG_MEMORY_ADDR	0xfffffd00
#define CONFIG_PRELOADER_DEBUG_MEMORY_SIZE	(0x200)
#endif
uint32_t debug_memory_ptr = CONFIG_PRELOADER_DEBUG_MEMORY_ADDR;

/////

ALT_STATUS_CODE board_init(void);

/////

//
// Index information:
//  - 0: AXI Bridge (ALT_RSTMGR_BRGMODRST_ADDR)
//  - 1: L3 Remap (ALT_L3_REMAP_ADDR)
//  - 2: Peripheral controller to FPGA bridge (ALT_SYSMGR_FPGAINTF_MODULE_ADDR)
//  - 3: FPGA 2 HPS SDRAM Bridge (ALT_SDR_CTL_FPGAPORTRST_ADDR)
//  - 4: Row bits handed off by Altera tools [not a register]
//
// Source: [HANDOFF_BASE]/software/preloader/uboot-socfpga/doc/README.SOCFPGA
//
#define ALT_SYSMGR_ISW_HANDOFF_INDEX_ADDR(index) ALT_CAST(void *, (ALT_CAST(char *, ALT_SYSMGR_ISW_HANDOFF_ADDR) + (index) * sizeof(uint32_t)))

void update_handoff(void)
{
    uint32_t brgmodrst   = 0;
    uint32_t remap       = ALT_L3_REMAP_MPUZERO_SET_MSK; // For MPU, map OCRAM to 0x0. No point keeping BootRom as it has already executed.
    uint32_t fpgaintf    = 0;      // By default, HPS is in control of all HPS-side peripherals.
    uint32_t fpgaportrst = CONFIG_HPS_SDR_CTRLCFG_FPGAPORTRST;
    // uint32_t rowbits     = 0;

    // Handle CONFIG_HPS_RESET_ASSERT_HPS2FPGA
#if CONFIG_HPS_RESET_ASSERT_HPS2FPGA
    brgmodrst |= ALT_RSTMGR_BRGMODRST_H2F_SET_MSK; // Keep bridge in reset
#else
    remap     |= ALT_L3_REMAP_H2F_SET_MSK; // Make address space visible to L3 masters
#endif

    // Handle CONFIG_HPS_RESET_ASSERT_LWHPS2FPGA
#if CONFIG_HPS_RESET_ASSERT_LWHPS2FPGA
    brgmodrst |= ALT_RSTMGR_BRGMODRST_LWH2F_SET_MSK;
#else
    remap     |= ALT_L3_REMAP_LWH2F_SET_MSK;
#endif

    // Handle CONFIG_HPS_RESET_ASSERT_FPGA2HPS
#if CONFIG_HPS_RESET_ASSERT_FPGA2HPS
    brgmodrst |= ALT_RSTMGR_BRGMODRST_F2H_SET_MSK;
#else
    // No remap equivalent for this option.
#endif

    // Update the handoff registers.
    alt_write_word(ALT_SYSMGR_ISW_HANDOFF_INDEX_ADDR(0), brgmodrst);
    alt_write_word(ALT_SYSMGR_ISW_HANDOFF_INDEX_ADDR(1), remap);
    alt_write_word(ALT_SYSMGR_ISW_HANDOFF_INDEX_ADDR(2), fpgaintf);
    alt_write_word(ALT_SYSMGR_ISW_HANDOFF_INDEX_ADDR(3), fpgaportrst);
    
    // ISW Handdof registers all reset to 0.
    // alt_write_word(ALT_SYSMGR_ISW_HANDOFF_INDEX_ADDR(4), rowbits);

#if CONFIG_MPL_FPGA_LOAD
    // The FPGA image is already successfully loaded by MPL so apply the configuration now.
    alt_write_word(ALT_RSTMGR_BRGMODRST_ADDR,       brgmodrst);
    alt_write_word(ALT_L3_REMAP_ADDR,               remap);
    alt_write_word(ALT_SYSMGR_FPGAINTF_MODULE_ADDR, fpgaintf);
    alt_write_word(ALT_SDR_CTL_FPGAPORTRST_ADDR,    fpgaportrst);
#endif
}

ALT_STATUS_CODE prepare_launch(void)
{
    ALT_STATUS_CODE status = ALT_E_SUCCESS;
    
    /////

    if (status == ALT_E_SUCCESS)
    {
        // Update the peripherals that needs to be taken out of reset.
        // Bridge peripherals are handled in update_handoff().

        uint32_t rst_set_mask;

        // PERMODRST bits
        rst_set_mask = 0 |
            ALT_RSTMGR_PERMODRST_EMAC0_SET(CONFIG_HPS_RESET_ASSERT_EMAC0) |
            ALT_RSTMGR_PERMODRST_EMAC1_SET(CONFIG_HPS_RESET_ASSERT_EMAC1) |
            ALT_RSTMGR_PERMODRST_USB0_SET(CONFIG_HPS_RESET_ASSERT_USB0) |
            ALT_RSTMGR_PERMODRST_USB1_SET(CONFIG_HPS_RESET_ASSERT_USB1) |
            ALT_RSTMGR_PERMODRST_NAND_SET(CONFIG_HPS_RESET_ASSERT_NAND) |
            ALT_RSTMGR_PERMODRST_QSPI_SET(CONFIG_HPS_RESET_ASSERT_QSPI) |
#if CONFIG_PRELOADER_WATCHDOG_ENABLE == 0
            ALT_RSTMGR_PERMODRST_L4WD0_SET_MSK |
#endif
            ALT_RSTMGR_PERMODRST_L4WD1_SET(CONFIG_HPS_RESET_ASSERT_L4WD1) |
            ALT_RSTMGR_PERMODRST_OSC1TMR1_SET(CONFIG_HPS_RESET_ASSERT_OSC1TIMER1) |
            ALT_RSTMGR_PERMODRST_SPTMR0_SET(CONFIG_HPS_RESET_ASSERT_SPTIMER0) |
            ALT_RSTMGR_PERMODRST_SPTMR1_SET(CONFIG_HPS_RESET_ASSERT_SPTIMER1) |
            ALT_RSTMGR_PERMODRST_I2C0_SET(CONFIG_HPS_RESET_ASSERT_I2C0) |
            ALT_RSTMGR_PERMODRST_I2C1_SET(CONFIG_HPS_RESET_ASSERT_I2C1) |
            ALT_RSTMGR_PERMODRST_I2C2_SET(CONFIG_HPS_RESET_ASSERT_I2C2) |
            ALT_RSTMGR_PERMODRST_I2C3_SET(CONFIG_HPS_RESET_ASSERT_I2C3) |
            ALT_RSTMGR_PERMODRST_UART0_SET(CONFIG_HPS_RESET_ASSERT_UART0) |
            ALT_RSTMGR_PERMODRST_UART1_SET(CONFIG_HPS_RESET_ASSERT_UART1) |
            ALT_RSTMGR_PERMODRST_SPIM0_SET(CONFIG_HPS_RESET_ASSERT_SPIM0) |
            ALT_RSTMGR_PERMODRST_SPIM1_SET(CONFIG_HPS_RESET_ASSERT_SPIM1) |
            ALT_RSTMGR_PERMODRST_SPIS0_SET(CONFIG_HPS_RESET_ASSERT_SPIS0) |
            ALT_RSTMGR_PERMODRST_SPIS1_SET(CONFIG_HPS_RESET_ASSERT_SPIS1) |
            ALT_RSTMGR_PERMODRST_SDMMC_SET(CONFIG_HPS_RESET_ASSERT_SDMMC) |
            ALT_RSTMGR_PERMODRST_CAN0_SET(CONFIG_HPS_RESET_ASSERT_CAN0) |
            ALT_RSTMGR_PERMODRST_CAN1_SET(CONFIG_HPS_RESET_ASSERT_CAN1) |
            ALT_RSTMGR_PERMODRST_GPIO0_SET(CONFIG_HPS_RESET_ASSERT_GPIO0) |
            ALT_RSTMGR_PERMODRST_GPIO1_SET(CONFIG_HPS_RESET_ASSERT_GPIO1) |
            ALT_RSTMGR_PERMODRST_GPIO2_SET(CONFIG_HPS_RESET_ASSERT_GPIO2) |
            ALT_RSTMGR_PERMODRST_DMA_SET(CONFIG_HPS_RESET_ASSERT_DMA) |
            ALT_RSTMGR_PERMODRST_SDR_SET(CONFIG_HPS_RESET_ASSERT_SDR);
        alt_write_word(ALT_RSTMGR_PERMODRST_ADDR, rst_set_mask);

        // PER2MODRST bits
        rst_set_mask = 0 |
            ALT_RSTMGR_PER2MODRST_DMAIF0_SET(CONFIG_HPS_RESET_ASSERT_FPGA_DMA0) |
            ALT_RSTMGR_PER2MODRST_DMAIF1_SET(CONFIG_HPS_RESET_ASSERT_FPGA_DMA1) |
            ALT_RSTMGR_PER2MODRST_DMAIF2_SET(CONFIG_HPS_RESET_ASSERT_FPGA_DMA2) |
            ALT_RSTMGR_PER2MODRST_DMAIF3_SET(CONFIG_HPS_RESET_ASSERT_FPGA_DMA3) |
            ALT_RSTMGR_PER2MODRST_DMAIF4_SET(CONFIG_HPS_RESET_ASSERT_FPGA_DMA4) |
            ALT_RSTMGR_PER2MODRST_DMAIF5_SET(CONFIG_HPS_RESET_ASSERT_FPGA_DMA5) |
            ALT_RSTMGR_PER2MODRST_DMAIF6_SET(CONFIG_HPS_RESET_ASSERT_FPGA_DMA6) |
            ALT_RSTMGR_PER2MODRST_DMAIF7_SET(CONFIG_HPS_RESET_ASSERT_FPGA_DMA7);
        alt_write_word(ALT_RSTMGR_PER2MODRST_ADDR, rst_set_mask);
    
#if CONFIG_PRELOADER_STATE_REG_ENABLE

        alt_write_word(CONFIG_PRELOADER_STATE_REG, CONFIG_PRELOADER_STATE_VALID);
    
#endif
    }

    return status;
}


static void SetupBridge (void)
{
   ALT_STATUS_CODE   status;

   /* Initialize Lightweigth HPS-to-FPGA bridge */
   status = alt_bridge_init(ALT_BRIDGE_LWH2F, NULL, NULL);

   /* Initialize HPS-to-FPGA bridge */
   status = alt_bridge_init(ALT_BRIDGE_H2F, NULL, NULL);

   status = alt_addr_space_remap(ALT_ADDR_SPACE_MPU_ZERO_AT_BOOTROM,
                                 ALT_ADDR_SPACE_NONMPU_ZERO_AT_OCRAM,
                                 ALT_ADDR_SPACE_H2F_ACCESSIBLE,
                                 ALT_ADDR_SPACE_LWH2F_ACCESSIBLE);
    (void)status;
} /* SetupBridge */


int main (void)
{
   // Do board initialization (plls, sdram, pinmux/io, etc.. )
   board_init();

   // Process the launch information.
   prepare_launch();

   // Update the ISW Handoff registers.
   update_handoff();
   
   // Setup Lightweigth HPS-to-FPGA and HPS-to-FPGA bridge 
   SetupBridge();      
   
   // Signal end of prelaoder   
   __asm("bkpt 0x70");
   
   return 0;
}
