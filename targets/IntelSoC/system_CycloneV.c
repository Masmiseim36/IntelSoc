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

#include <stdbool.h>
#include "CycloneV.h"
#include "system_CycloneV.h"
#include "irq_ctrl.h"


void SystemInit (void)
{
	SystemInitHook ();
}

void SystemInitHook (void)
{
	// Void implementation of the weak function.
}


//*****************************************************************************
// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.
//*****************************************************************************
__attribute__ ((weak)) void InteruptDefaultHandler (void)
{
	while (true)
		;
}

__attribute__ ((weak)) void InteruptReservedHandler (void)
{
	while (true)
		;
}



__attribute__ ((weak, alias("InteruptDefaultHandler"))) void GlobalTimer_IRQHandler                (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void Legacy_nFIQ_IRQHandler                (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void CoreTimer_IRQHandler                  (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void CoreWatchdog_IRQHandler               (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void Legacy_nIRQ_IRQHandler                (void);


__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu0_parityfail_IRQHandler            (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu0_parityfail_BTAC_IRQHandler       (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu0_parityfail_GHB_IRQHandler        (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu0_parityfail_I_Tag_IRQHandler      (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu0_parityfail_I_Data_IRQHandler     (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu0_parityfail_TLB_IRQHandler        (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu0_parityfail_D_Outer_IRQHandler    (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu0_parityfail_D_Tag_IRQHandler      (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu0_parityfail_D_Data_IRQHandler     (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu0_deflags0_IRQHandler              (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu0_deflags1_IRQHandler              (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu0_deflags2_IRQHandler              (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu0_deflags3_IRQHandler              (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu0_deflags4_IRQHandler              (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu0_deflags5_IRQHandler              (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu0_deflags6_IRQHandler              (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu1_parityfail_IRQHandler            (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu1_parityfail_BTAC_IRQHandler       (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu1_parityfail_GHB_IRQHandler        (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu1_parityfail_I_Tag_IRQHandler      (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu1_parityfail_I_Data_IRQHandler     (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu1_parityfail_TLB_IRQHandler        (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu1_parityfail_D_Outer_IRQHandler    (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu1_parityfail_D_Tag_IRQHandler      (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu1_parityfail_D_Data_IRQHandler     (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu1_deflags0_IRQHandler              (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu1_deflags1_IRQHandler              (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu1_deflags2_IRQHandler              (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu1_deflags3_IRQHandler              (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu1_deflags4_IRQHandler              (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu1_deflags5_IRQHandler              (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void cpu1_deflags6_IRQHandler              (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void scu_parityfail0_IRQHandler            (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void scu_parityfail1_IRQHandler            (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void scu_ev_abort_IRQHandler               (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void l2_ecc_byte_wr_IRQHandler             (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void l2_ecc_corrected_IRQHandler           (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void l2_ecc_uncorrected_IRQHandler         (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void l2_combined_IRQHandler                (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void ddr_ecc_error_IRQHandler              (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ0_IRQHandler                  (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ1_IRQHandler                  (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ2_IRQHandler                  (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ3_IRQHandler                  (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ4_IRQHandler                  (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ5_IRQHandler                  (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ6_IRQHandler                  (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ7_IRQHandler                  (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ8_IRQHandler                  (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ9_IRQHandler                  (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ10_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ11_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ12_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ13_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ14_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ15_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ16_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ17_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ18_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ19_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ20_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ21_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ22_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ23_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ24_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ25_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ26_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ27_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ28_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ29_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ30_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ31_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ32_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ33_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ34_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ35_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ36_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ37_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ38_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ39_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ40_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ41_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ42_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ43_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ44_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ45_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ46_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ47_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ48_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ49_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ50_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ51_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ52_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ53_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ54_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ55_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ56_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ57_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ58_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ59_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ60_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ61_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ62_IRQHandler                 (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void FPGA_IRQ63_IRQHandler                 (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void dma_IRQ0_IRQHandler                   (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void dma_IRQ1_IRQHandler                   (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void dma_IRQ2_IRQHandler                   (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void dma_IRQ3_IRQHandler                   (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void dma_IRQ4_IRQHandler                   (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void dma_IRQ5_IRQHandler                   (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void dma_IRQ6_IRQHandler                   (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void dma_IRQ7_IRQHandler                   (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void dma_irq_abort_IRQHandler              (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void dma_ecc_corrected_IRQHandler          (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void dma_ecc_uncorrected_IRQHandler        (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void emac0_IRQHandler                      (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void emac0_tx_ecc_corrected_IRQHandler     (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void emac0_tx_ecc_uncorrected_IRQHandler   (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void emac0_rx_ecc_corrected_IRQHandler     (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void emac0_rx_ecc_uncorrected_IRQHandler   (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void emac1_IRQHandler                      (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void emac1_tx_ecc_corrected_IRQHandler     (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void emac1_tx_ecc_uncorrected_IRQHandler   (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void emac1_rx_ecc_corrected_IRQHandler     (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void emac1_rx_ecc_uncorrected_IRQHandler   (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void usb0_IRQHandler                       (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void usb0_ecc_corrected_IRQHandler         (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void usb0_ecc_uncorrected_IRQHandler       (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void usb1_IRQHandler                       (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void usb1_ecc_corrected_IRQHandler         (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void usb1_ecc_uncorrected_IRQHandler       (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void can0_sts_IRQHandler                   (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void can0_mo_IRQHandler                    (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void can0_ecc_corrected_IRQHandler         (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void can0_ecc_uncorrected_IRQHandler       (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void can1_sts_IRQHandler                   (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void can1_mo_IRQHandler                    (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void can1_ecc_corrected_IRQHandler         (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void can1_ecc_uncorrected_IRQHandler       (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void sdmmc_IRQHandler                      (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void sdmmc_porta_ecc_corrected_IRQHandler  (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void sdmmc_porta_ecc_uncorrected_IRQHandler (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void sdmmc_portb_ecc_corrected_IRQHandler  (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void sdmmc_portb_ecc_uncorrected_IRQHandler (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void nand_IRQHandler                       (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void nandr_ecc_corrected_IRQHandler        (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void nandr_ecc_uncorrected_IRQHandler      (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void nandw_ecc_corrected_IRQHandler        (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void nandw_ecc_uncorrected_IRQHandler      (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void nande_ecc_corrected_IRQHandler        (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void nande_ecc_uncorrected_IRQHandler      (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void qspi_IRQHandler                       (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void qspi_ecc_corrected_IRQHandler         (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void qspi_ecc_uncorrected_IRQHandler       (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void spi0_IRQHandler                       (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void spi1_IRQHandler                       (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void spi2_IRQHandler                       (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void spi3_IRQHandler                       (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void i2c0_IRQHandler                       (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void i2c1_IRQHandler                       (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void i2c2_IRQHandler                       (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void i2c3_IRQHandler                       (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void uart0_IRQHandler                      (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void uart1_IRQHandler                      (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void gpio0_IRQHandler                      (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void gpio1_IRQHandler                      (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void gpio2_IRQHandler                      (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void timer_l4sp_0_IRQHandler               (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void timer_l4sp_1_IRQHandler               (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void timer_osc1_0_IRQHandler               (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void timer_osc1_1_IRQHandler               (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void wdog0_IRQHandler                      (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void wdog1_IRQHandler                      (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void clkmgr_IRQHandler                     (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void mpuwakeup_IRQHandler                  (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void fpga_man_IRQHandler                   (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void nCTIIRQ0_IRQHandler                   (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void nCTIIRQ1_IRQHandler                   (void);

__attribute__ ((weak, alias("InteruptDefaultHandler"))) void ram_ecc_corrected_IRQHandler          (void);
__attribute__ ((weak, alias("InteruptDefaultHandler"))) void ram_ecc_uncorrected_IRQHandler        (void);


static IRQHandler_t IrqTable [IRQ_GIC_LINE_COUNT] =
{
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,
	InteruptReservedHandler,

	GlobalTimer_IRQHandler,
	Legacy_nFIQ_IRQHandler,
	CoreTimer_IRQHandler,
	CoreWatchdog_IRQHandler,
	Legacy_nIRQ_IRQHandler,

	cpu0_parityfail_IRQHandler,
	cpu0_parityfail_BTAC_IRQHandler,
	cpu0_parityfail_GHB_IRQHandler,
	cpu0_parityfail_I_Tag_IRQHandler,
	cpu0_parityfail_I_Data_IRQHandler,
	cpu0_parityfail_TLB_IRQHandler,
	cpu0_parityfail_D_Outer_IRQHandler,
	cpu0_parityfail_D_Tag_IRQHandler,
	cpu0_parityfail_D_Data_IRQHandler,
	cpu0_deflags0_IRQHandler,
	cpu0_deflags1_IRQHandler,
	cpu0_deflags2_IRQHandler,
	cpu0_deflags3_IRQHandler,
	cpu0_deflags4_IRQHandler,
	cpu0_deflags5_IRQHandler,
	cpu0_deflags6_IRQHandler,

	cpu1_parityfail_IRQHandler,
	cpu1_parityfail_BTAC_IRQHandler,
	cpu1_parityfail_GHB_IRQHandler,
	cpu1_parityfail_I_Tag_IRQHandler,
	cpu1_parityfail_I_Data_IRQHandler,
	cpu1_parityfail_TLB_IRQHandler,
	cpu1_parityfail_D_Outer_IRQHandler,
	cpu1_parityfail_D_Tag_IRQHandler,
	cpu1_parityfail_D_Data_IRQHandler,
	cpu1_deflags0_IRQHandler,
	cpu1_deflags1_IRQHandler,
	cpu1_deflags2_IRQHandler,
	cpu1_deflags3_IRQHandler,
	cpu1_deflags4_IRQHandler,
	cpu1_deflags5_IRQHandler,
	cpu1_deflags6_IRQHandler,

	scu_parityfail0_IRQHandler,
	scu_parityfail1_IRQHandler,
	scu_ev_abort_IRQHandler,

	l2_ecc_byte_wr_IRQHandler,
	l2_ecc_corrected_IRQHandler,
	l2_ecc_uncorrected_IRQHandler,
	l2_combined_IRQHandler,

	ddr_ecc_error_IRQHandler,

	FPGA_IRQ0_IRQHandler,
	FPGA_IRQ1_IRQHandler,
	FPGA_IRQ2_IRQHandler,
	FPGA_IRQ3_IRQHandler,
	FPGA_IRQ4_IRQHandler,
	FPGA_IRQ5_IRQHandler,
	FPGA_IRQ6_IRQHandler,
	FPGA_IRQ7_IRQHandler,
	FPGA_IRQ8_IRQHandler,
	FPGA_IRQ9_IRQHandler,
	FPGA_IRQ10_IRQHandler,
	FPGA_IRQ11_IRQHandler,
	FPGA_IRQ12_IRQHandler,
	FPGA_IRQ13_IRQHandler,
	FPGA_IRQ14_IRQHandler,
	FPGA_IRQ15_IRQHandler,
	FPGA_IRQ16_IRQHandler,
	FPGA_IRQ17_IRQHandler,
	FPGA_IRQ18_IRQHandler,
	FPGA_IRQ19_IRQHandler,
	FPGA_IRQ20_IRQHandler,
	FPGA_IRQ21_IRQHandler,
	FPGA_IRQ22_IRQHandler,
	FPGA_IRQ23_IRQHandler,
	FPGA_IRQ24_IRQHandler,
	FPGA_IRQ25_IRQHandler,
	FPGA_IRQ26_IRQHandler,
	FPGA_IRQ27_IRQHandler,
	FPGA_IRQ28_IRQHandler,
	FPGA_IRQ29_IRQHandler,
	FPGA_IRQ30_IRQHandler,
	FPGA_IRQ31_IRQHandler,
	FPGA_IRQ32_IRQHandler,
	FPGA_IRQ33_IRQHandler,
	FPGA_IRQ34_IRQHandler,
	FPGA_IRQ35_IRQHandler,
	FPGA_IRQ36_IRQHandler,
	FPGA_IRQ37_IRQHandler,
	FPGA_IRQ38_IRQHandler,
	FPGA_IRQ39_IRQHandler,
	FPGA_IRQ40_IRQHandler,
	FPGA_IRQ41_IRQHandler,
	FPGA_IRQ42_IRQHandler,
	FPGA_IRQ43_IRQHandler,
	FPGA_IRQ44_IRQHandler,
	FPGA_IRQ45_IRQHandler,
	FPGA_IRQ46_IRQHandler,
	FPGA_IRQ47_IRQHandler,
	FPGA_IRQ48_IRQHandler,
	FPGA_IRQ49_IRQHandler,
	FPGA_IRQ50_IRQHandler,
	FPGA_IRQ51_IRQHandler,
	FPGA_IRQ52_IRQHandler,
	FPGA_IRQ53_IRQHandler,
	FPGA_IRQ54_IRQHandler,
	FPGA_IRQ55_IRQHandler,
	FPGA_IRQ56_IRQHandler,
	FPGA_IRQ57_IRQHandler,
	FPGA_IRQ58_IRQHandler,
	FPGA_IRQ59_IRQHandler,
	FPGA_IRQ60_IRQHandler,
	FPGA_IRQ61_IRQHandler,
	FPGA_IRQ62_IRQHandler,
	FPGA_IRQ63_IRQHandler,

	dma_IRQ0_IRQHandler,
	dma_IRQ1_IRQHandler,
	dma_IRQ2_IRQHandler,
	dma_IRQ3_IRQHandler,
	dma_IRQ4_IRQHandler,
	dma_IRQ5_IRQHandler,
	dma_IRQ6_IRQHandler,
	dma_IRQ7_IRQHandler,
	dma_irq_abort_IRQHandler,
	dma_ecc_corrected_IRQHandler,
	dma_ecc_uncorrected_IRQHandler,

	emac0_IRQHandler,
	emac0_tx_ecc_corrected_IRQHandler,
	emac0_tx_ecc_uncorrected_IRQHandler,
	emac0_rx_ecc_corrected_IRQHandler,
	emac0_rx_ecc_uncorrected_IRQHandler,
	emac1_IRQHandler,
	emac1_tx_ecc_corrected_IRQHandler,
	emac1_tx_ecc_uncorrected_IRQHandler,
	emac1_rx_ecc_corrected_IRQHandler,
	emac1_rx_ecc_uncorrected_IRQHandler,

	usb0_IRQHandler,
	usb0_ecc_corrected_IRQHandler,
	usb0_ecc_uncorrected_IRQHandler,
	usb1_IRQHandler,
	usb1_ecc_corrected_IRQHandler,
	usb1_ecc_uncorrected_IRQHandler,

	can0_sts_IRQHandler,
	can0_mo_IRQHandler,
	can0_ecc_corrected_IRQHandler,
	can0_ecc_uncorrected_IRQHandler,
	can1_sts_IRQHandler,
	can1_mo_IRQHandler,
	can1_ecc_corrected_IRQHandler,
	can1_ecc_uncorrected_IRQHandler,

	sdmmc_IRQHandler,
	sdmmc_porta_ecc_corrected_IRQHandler,
	sdmmc_porta_ecc_uncorrected_IRQHandler,
	sdmmc_portb_ecc_corrected_IRQHandler,
	sdmmc_portb_ecc_uncorrected_IRQHandler,

	nand_IRQHandler,
	nandr_ecc_corrected_IRQHandler,
	nandr_ecc_uncorrected_IRQHandler,
	nandw_ecc_corrected_IRQHandler,
	nandw_ecc_uncorrected_IRQHandler,
	nande_ecc_corrected_IRQHandler,
	nande_ecc_uncorrected_IRQHandler,

	qspi_IRQHandler,
	qspi_ecc_corrected_IRQHandler,
	qspi_ecc_uncorrected_IRQHandler,

	spi0_IRQHandler,
	spi1_IRQHandler,
	spi2_IRQHandler,
	spi3_IRQHandler,

	i2c0_IRQHandler,
	i2c1_IRQHandler,
	i2c2_IRQHandler,
	i2c3_IRQHandler,

	uart0_IRQHandler,
	uart1_IRQHandler,

	gpio0_IRQHandler,
	gpio1_IRQHandler,
	gpio2_IRQHandler,

	timer_l4sp_0_IRQHandler,
	timer_l4sp_1_IRQHandler,
	timer_osc1_0_IRQHandler,
	timer_osc1_1_IRQHandler,

	wdog0_IRQHandler,
	wdog1_IRQHandler,

	clkmgr_IRQHandler,
	mpuwakeup_IRQHandler,

	fpga_man_IRQHandler,

	nCTIIRQ0_IRQHandler,
	nCTIIRQ1_IRQHandler,

	ram_ecc_corrected_IRQHandler,
	ram_ecc_uncorrected_IRQHandler,
};


int32_t IRQ_Initialize (void)
{
	return 0;
}


/// Register interrupt handler.
int32_t IRQ_SetHandler (IRQn_ID_t irqn, IRQHandler_t handler)
{
	if ((irqn >= 0) && (irqn < (IRQn_ID_t)IRQ_GIC_LINE_COUNT))
	{
		IrqTable[irqn] = handler;
		return 0;
	}

	return -1;
}


/// Get the registered interrupt handler.
IRQHandler_t IRQ_GetHandler (IRQn_ID_t irqn)
{
	// Ignore CPUID field (software generated interrupts)
	irqn &= 0x3FFU;

	if ((irqn >= 0) && (irqn < (IRQn_ID_t)IRQ_GIC_LINE_COUNT))
	{
		return IrqTable[irqn];
	}

	return (IRQHandler_t)0;
}

__WEAK void IRQ_Handler (void)
{
	IRQn_Type irqn = GIC_AcknowledgePending ();
	if (irqn < IRQ_GIC_LINE_COUNT)
		IrqTable[irqn]();
	GIC_EndInterrupt (irqn);
}
