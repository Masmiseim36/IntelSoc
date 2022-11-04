/** Loader for Intel-SoC-Family
Copyright (C) 2019-2022 Markus Klein
https://github.com/Masmiseim36/IntelSoc

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE. */


#ifndef _SYSTEM_ARRIA10_H_
#define _SYSTEM_ARRIA10_H_

#ifdef __cplusplus
extern "C" {
#endif

/*! SystemInit
\brief Setup the microcontroller system.
SystemInit is called from startup_device file. */
void SystemInit (void);

/*! SystemInitHook
\brief SystemInit function hook.
This weak function allows to call specific initialization code during the SystemInit() execution.
This can be used when an application specific code needs to be called as close to the reset entry as possible
(for example the Multicore Manager MCMGR_EarlyInit() function call).
NOTE: No global r/w variables can be used in this hook function because the
initialization of these variables happens after this function.*/
void SystemInitHook (void);


// The interrupt service routines of the processor:
// Software Generated Interrupt (SGI)
void SGI0_IRQHandler            (void); 
void SGI1_IRQHandler            (void);
void SGI2_IRQHandler            (void);
void SGI3_IRQHandler            (void);
void SGI4_IRQHandler            (void);
void SGI5_IRQHandler            (void);
void SGI6_IRQHandler            (void);
void SGI7_IRQHandler            (void);
void SGI8_IRQHandler            (void);
void SGI9_IRQHandler            (void);
void SGI10_IRQHandler           (void);
void SG111_IRQHandler           (void);
void SGI12_IRQHandler           (void);
void SGI13_IRQHandler           (void);
void SGI14_IRQHandler           (void);
void SGI15_IRQHandler           (void);
// ARM generic interrupts
void GlobalTimer_IRQHandler     (void);
void Legacy_nFIQ_IRQHandler     (void);
void CoreTimer_IRQHandler       (void);
void CoreWatchdog_IRQHandler    (void);
void Legacy_nIRQ_IRQHandler     (void);
// Controller specific interrupts
void DERR_Global_IRQHandler     (void);
void cpux_parityfail_IRQHandler (void);
void SERR_Global_IRQHandler     (void);

void cpu0_deflags0_IRQHandler   (void);
void cpu0_deflags1_IRQHandler   (void);
void cpu0_deflags2_IRQHandler   (void);
void cpu0_deflags3_IRQHandler   (void);
void cpu0_deflags4_IRQHandler   (void);
void cpu0_deflags5_IRQHandler   (void);
void cpu0_deflags6_IRQHandler   (void);
void cpu1_deflags0_IRQHandler   (void);
void cpu1_deflags1_IRQHandler   (void);
void cpu1_deflags2_IRQHandler   (void);
void cpu1_deflags3_IRQHandler   (void);
void cpu1_deflags4_IRQHandler   (void);
void cpu1_deflags5_IRQHandler   (void);
void cpu1_deflags6_IRQHandler   (void);
void scu_ev_abort_IRQHandler    (void);
void l2_combined_IRQHandler     (void);

void FPGA_IRQ0_IRQHandler       (void);
void FPGA_IRQ1_IRQHandler       (void);
void FPGA_IRQ2_IRQHandler       (void);
void FPGA_IRQ3_IRQHandler       (void);
void FPGA_IRQ4_IRQHandler       (void);
void FPGA_IRQ5_IRQHandler       (void);
void FPGA_IRQ6_IRQHandler       (void);
void FPGA_IRQ7_IRQHandler       (void);
void FPGA_IRQ8_IRQHandler       (void);
void FPGA_IRQ9_IRQHandler       (void);
void FPGA_IRQ10_IRQHandler      (void);
void FPGA_IRQ11_IRQHandler      (void);
void FPGA_IRQ12_IRQHandler      (void);
void FPGA_IRQ13_IRQHandler      (void);
void FPGA_IRQ14_IRQHandler      (void);
void FPGA_IRQ15_IRQHandler      (void);
void FPGA_IRQ16_IRQHandler      (void);
void FPGA_IRQ17_IRQHandler      (void);
void FPGA_IRQ18_IRQHandler      (void);
void FPGA_IRQ19_IRQHandler      (void);
void FPGA_IRQ20_IRQHandler      (void);
void FPGA_IRQ21_IRQHandler      (void);
void FPGA_IRQ22_IRQHandler      (void);
void FPGA_IRQ23_IRQHandler      (void);
void FPGA_IRQ24_IRQHandler      (void);
void FPGA_IRQ25_IRQHandler      (void);
void FPGA_IRQ26_IRQHandler      (void);
void FPGA_IRQ27_IRQHandler      (void);
void FPGA_IRQ28_IRQHandler      (void);
void FPGA_IRQ29_IRQHandler      (void);
void FPGA_IRQ30_IRQHandler      (void);
void FPGA_IRQ31_IRQHandler      (void);
void FPGA_IRQ32_IRQHandler      (void);
void FPGA_IRQ33_IRQHandler      (void);
void FPGA_IRQ34_IRQHandler      (void);
void FPGA_IRQ35_IRQHandler      (void);
void FPGA_IRQ36_IRQHandler      (void);
void FPGA_IRQ37_IRQHandler      (void);
void FPGA_IRQ38_IRQHandler      (void);
void FPGA_IRQ39_IRQHandler      (void);
void FPGA_IRQ40_IRQHandler      (void);
void FPGA_IRQ41_IRQHandler      (void);
void FPGA_IRQ42_IRQHandler      (void);
void FPGA_IRQ43_IRQHandler      (void);
void FPGA_IRQ44_IRQHandler      (void);
void FPGA_IRQ45_IRQHandler      (void);
void FPGA_IRQ46_IRQHandler      (void);
void FPGA_IRQ47_IRQHandler      (void);
void FPGA_IRQ48_IRQHandler      (void);
void FPGA_IRQ49_IRQHandler      (void);
void FPGA_IRQ50_IRQHandler      (void);
void FPGA_IRQ51_IRQHandler      (void);
void FPGA_IRQ52_IRQHandler      (void);
void FPGA_IRQ53_IRQHandler      (void);
void FPGA_IRQ54_IRQHandler      (void);
void FPGA_IRQ55_IRQHandler      (void);
void FPGA_IRQ56_IRQHandler      (void);
void FPGA_IRQ57_IRQHandler      (void);
void FPGA_IRQ58_IRQHandler      (void);
void FPGA_IRQ59_IRQHandler      (void);
void FPGA_IRQ60_IRQHandler      (void);
void FPGA_IRQ61_IRQHandler      (void);
void FPGA_IRQ62_IRQHandler      (void);
void FPGA_IRQ63_IRQHandler      (void);

void dma_IRQ0_IRQHandler        (void);
void dma_IRQ1_IRQHandler        (void);
void dma_IRQ2_IRQHandler        (void);
void dma_IRQ3_IRQHandler        (void);
void dma_IRQ4_IRQHandler        (void);
void dma_IRQ5_IRQHandler        (void);
void dma_IRQ6_IRQHandler        (void);
void dma_IRQ7_IRQHandler        (void);
void dma_irq_abort_IRQHandler   (void);

void emac0_IRQHandler           (void);
void emac1_IRQHandler           (void);
void emac2_IRQHandler           (void);
void usb0_IRQHandler            (void);
void usb1_IRQHandler            (void);
void HMC_error_IRQHandler       (void);
void sdmmc_IRQHandler           (void);
void nand_IRQHandler            (void);
void qspi_IRQHandler            (void);
void spim0_IRQHandler           (void);
void spim1_IRQHandler           (void);
void spis0_IRQHandler           (void);
void spis1_IRQHandler           (void);
void i2c0_IRQHandler            (void);
void i2c1_IRQHandler            (void);
void i2c2_IRQHandler            (void);
void i2c3_IRQHandler            (void);
void i2c4_IRQHandler            (void);
void uart0_IRQHandler           (void);
void uart1_IRQHandler           (void);
void gpio0_IRQHandler           (void);
void gpio1_IRQHandler           (void);
void gpio2_IRQHandler           (void);
void timer_l4sp_0_IRQHandler    (void);
void timer_l4sp_1_IRQHandler    (void);
void timer_osc1_0_IRQHandler    (void);
void timer_osc1_1_IRQHandler    (void);
void wdog0_IRQHandler           (void);
void wdog1_IRQHandler           (void);
void clkmgr_IRQHandler          (void);
void restmgr_IRQHandler         (void);
void fpga_man_IRQHandler        (void);
void nCTIIRQ0_IRQHandler        (void);
void nCTIIRQ1_IRQHandler        (void);
void SEC_MGR_INTR_IRQHandler    (void);
void DATABWERR_IRQHandler       (void);



#ifdef __cplusplus
}
#endif

#endif // _SYSTEM_ARRIA10_H_