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


#ifndef ARRIA_X_
#define ARRIA_X_

#include "include/soc_a10/socal/hps.h"
#include "include/soc_a10/socal/socal.h"


#define __CA_REV        0x0401U    /*!< Core revision 0401-10                       */
#define __CORTEX_A           9U    /*!< Cortex-A9 Core                              */
#define __FPU_PRESENT        1U    /*!< FPU present                                 */
#define __GIC_PRESENT        1U    /*!< GIC present                                 */
#define __TIM_PRESENT        1U    /*!< TIM present                                 */
#define __L2C_PRESENT        1U    /*!< L2C present                                 */


#define L2C_310_BASE ALT_L2_REGS_L2TYPE_ADDR /* 0xFFFFF000 */
// Compare Programmer's Guide for Armv7-A - Chapter "18.10 Private memory region"
// and "Arria 10 Hard Processor System Technical Reference Manual" Table 9."Peripheral Region Address Map"
#define GIC_DISTRIBUTOR_BASE (0xFFFFF000 + 0x1000) /* Interrupt Controller Distributor */
#define GIC_INTERFACE_BASE   (0xFFFFF000 + 0x100)  /* Interrupt controller CPU Interface */
#define IRQ_GIC_LINE_COUNT   256
// (0xFFFFF000 + 0x000) /* Snoop Control Unit (SCU) */
// (0xFFFFF000 + 0x200) /* Global Timer */
// (0xFFFFF000 + 0x600) /* Local Timer/Watchdog */
#define TIMER_BASE (0xFFFFF000 + 0x600) /* Local Timer/Watchdog */


// Compare "Arria 10 Hard Processor System Technical Reference Manual" Table 88 "GIC Interrupt Map"
typedef enum IRQn
{
	// ARM generic Interrupts
	GlobalTimer_IRQn                  =  27, // Source Block: 
	Legacy_nFIQ_IRQn                  =  28, // Source Block: 
	CoreTimer_IRQn                    =  29, // Source Block: 
	CoreWatchdog_IRQn                 =  30, // Source Block: 
	Legacy_nIRQ_IRQn                  =  31, // Source Block: 
	// COntroller Specific Interrupts
	DERR_Global_IRQn      =  32, // Source Block: System Manager
	cpux_parityfail_IRQn  =  33, // Source Block: System Manager
	SERR_Global_IRQn      =  34, // Source Block: System Manager

	cpu0_deflags0_IRQn    =  35, // Source Block: CortexA9_0
	cpu0_deflags1_IRQn    =  36, // Source Block: CortexA9_0
	cpu0_deflags2_IRQn    =  37, // Source Block: CortexA9_0
	cpu0_deflags3_IRQn    =  38, // Source Block: CortexA9_0
	cpu0_deflags4_IRQn    =  39, // Source Block: CortexA9_0
	cpu0_deflags5_IRQn    =  40, // Source Block: CortexA9_0
	cpu0_deflags6_IRQn    =  41, // Source Block: CortexA9_0
	cpu1_deflags0_IRQn    =  42, // Source Block: CortexA9_1
	cpu1_deflags1_IRQn    =  43, // Source Block: CortexA9_1
	cpu1_deflags2_IRQn    =  44, // Source Block: CortexA9_1
	cpu1_deflags3_IRQn    =  45, // Source Block: CortexA9_1
	cpu1_deflags4_IRQn    =  46, // Source Block: CortexA9_1
	cpu1_deflags5_IRQn    =  47, // Source Block: CortexA9_1
	cpu1_deflags6_IRQn    =  48, // Source Block: CortexA9_1
	scu_ev_abort_IRQn     =  49, // Source Block: SCU
	l2_combined_IRQn      =  50, // Source Block: L2-Cache

	FPGA_IRQ0_IRQn        =  51, // Source Block: FPGA
	FPGA_IRQ1_IRQn        =  52, // Source Block: FPGA
	FPGA_IRQ2_IRQn        =  53, // Source Block: FPGA
	FPGA_IRQ3_IRQn        =  54, // Source Block: FPGA
	FPGA_IRQ4_IRQn        =  55, // Source Block: FPGA
	FPGA_IRQ5_IRQn        =  56, // Source Block: FPGA
	FPGA_IRQ6_IRQn        =  57, // Source Block: FPGA
	FPGA_IRQ7_IRQn        =  58, // Source Block: FPGA
	FPGA_IRQ8_IRQn        =  59, // Source Block: FPGA
	FPGA_IRQ9_IRQn        =  60, // Source Block: FPGA
	FPGA_IRQ10_IRQn       =  61, // Source Block: FPGA
	FPGA_IRQ11_IRQn       =  62, // Source Block: FPGA
	FPGA_IRQ12_IRQn       =  63, // Source Block: FPGA
	FPGA_IRQ13_IRQn       =  64, // Source Block: FPGA
	FPGA_IRQ14_IRQn       =  65, // Source Block: FPGA
	FPGA_IRQ15_IRQn       =  66, // Source Block: FPGA
	FPGA_IRQ16_IRQn       =  67, // Source Block: FPGA
	FPGA_IRQ17_IRQn       =  68, // Source Block: FPGA
	FPGA_IRQ18_IRQn       =  69, // Source Block: FPGA
	FPGA_IRQ19_IRQn       =  70, // Source Block: FPGA
	FPGA_IRQ20_IRQn       =  71, // Source Block: FPGA
	FPGA_IRQ21_IRQn       =  72, // Source Block: FPGA
	FPGA_IRQ22_IRQn       =  73, // Source Block: FPGA
	FPGA_IRQ23_IRQn       =  74, // Source Block: FPGA
	FPGA_IRQ24_IRQn       =  75, // Source Block: FPGA
	FPGA_IRQ25_IRQn       =  76, // Source Block: FPGA
	FPGA_IRQ26_IRQn       =  77, // Source Block: FPGA
	FPGA_IRQ27_IRQn       =  78, // Source Block: FPGA
	FPGA_IRQ28_IRQn       =  79, // Source Block: FPGA
	FPGA_IRQ29_IRQn       =  80, // Source Block: FPGA
	FPGA_IRQ30_IRQn       =  81, // Source Block: FPGA
	FPGA_IRQ31_IRQn       =  82, // Source Block: FPGA
	FPGA_IRQ32_IRQn       =  83, // Source Block: FPGA
	FPGA_IRQ33_IRQn       =  84, // Source Block: FPGA
	FPGA_IRQ34_IRQn       =  85, // Source Block: FPGA
	FPGA_IRQ35_IRQn       =  86, // Source Block: FPGA
	FPGA_IRQ36_IRQn       =  87, // Source Block: FPGA
	FPGA_IRQ37_IRQn       =  88, // Source Block: FPGA
	FPGA_IRQ38_IRQn       =  89, // Source Block: FPGA
	FPGA_IRQ39_IRQn       =  90, // Source Block: FPGA
	FPGA_IRQ40_IRQn       =  91, // Source Block: FPGA
	FPGA_IRQ41_IRQn       =  92, // Source Block: FPGA
	FPGA_IRQ42_IRQn       =  93, // Source Block: FPGA
	FPGA_IRQ43_IRQn       =  94, // Source Block: FPGA
	FPGA_IRQ44_IRQn       =  95, // Source Block: FPGA
	FPGA_IRQ45_IRQn       =  96, // Source Block: FPGA
	FPGA_IRQ46_IRQn       =  97, // Source Block: FPGA
	FPGA_IRQ47_IRQn       =  98, // Source Block: FPGA
	FPGA_IRQ48_IRQn       =  99, // Source Block: FPGA
	FPGA_IRQ49_IRQn       = 100, // Source Block: FPGA
	FPGA_IRQ50_IRQn       = 101, // Source Block: FPGA
	FPGA_IRQ51_IRQn       = 102, // Source Block: FPGA
	FPGA_IRQ52_IRQn       = 103, // Source Block: FPGA
	FPGA_IRQ53_IRQn       = 104, // Source Block: FPGA
	FPGA_IRQ54_IRQn       = 105, // Source Block: FPGA
	FPGA_IRQ55_IRQn       = 106, // Source Block: FPGA
	FPGA_IRQ56_IRQn       = 107, // Source Block: FPGA
	FPGA_IRQ57_IRQn       = 108, // Source Block: FPGA
	FPGA_IRQ58_IRQn       = 109, // Source Block: FPGA
	FPGA_IRQ59_IRQn       = 110, // Source Block: FPGA
	FPGA_IRQ60_IRQn       = 111, // Source Block: FPGA
	FPGA_IRQ61_IRQn       = 112, // Source Block: FPGA
	FPGA_IRQ62_IRQn       = 113, // Source Block: FPGA
	FPGA_IRQ63_IRQn       = 114, // Source Block: FPGA

	dma_IRQ0_IRQn         = 115, // Source Block: DMA 
	dma_IRQ1_IRQn         = 116, // Source Block: DMA 
	dma_IRQ2_IRQn         = 117, // Source Block: DMA 
	dma_IRQ3_IRQn         = 118, // Source Block: DMA 
	dma_IRQ4_IRQn         = 119, // Source Block: DMA 
	dma_IRQ5_IRQn         = 120, // Source Block: DMA 
	dma_IRQ6_IRQn         = 121, // Source Block: DMA 
	dma_IRQ7_IRQn         = 122, // Source Block: DMA 
	dma_irq_abort_IRQn    = 123, // Source Block: DMA

	emac0_IRQn            = 124, // Source Block: EMAC
	emac1_IRQn            = 125, // Source Block: EMAC
	emac2_IRQn            = 126, // Source Block: EMAC
	usb0_IRQn             = 127, // Source Block: 
	usb1_IRQn             = 128, // Source Block: 
	HMC_error_IRQn        = 129, // Source Block: 
	sdmmc_IRQn            = 130, // Source Block: 
	nand_IRQn             = 131, // Source Block: 
	qspi_IRQn             = 132, // Source Block: 
	spim0_IRQn            = 133, // Source Block: 
	spim1_IRQn            = 134, // Source Block: 
	spis0_IRQn            = 135, // Source Block: 
	spis1_IRQn            = 136, // Source Block: 
	i2c0_IRQn             = 137, // Source Block: 
	i2c1_IRQn             = 138, // Source Block: 
	i2c2_IRQn             = 139, // Source Block: 
	i2c3_IRQn             = 140, // Source Block: 
	i2c4_IRQn             = 141, // Source Block: 
	uart0_IRQn            = 142, // Source Block: 
	uart1_IRQn            = 143, // Source Block: 
	gpio0_IRQn            = 144, // Source Block: 
	gpio1_IRQn            = 145, // Source Block: 
	gpio2_IRQn            = 146, // Source Block: 
	timer_l4sp_0_IRQn     = 147, // Source Block: 
	timer_l4sp_1_IRQn     = 148, // Source Block: 
	timer_osc1_0_IRQn     = 149, // Source Block: 
	timer_osc1_1_IRQn     = 150, // Source Block: 
	wdog0_IRQn            = 151, // Source Block: 
	wdog1_IRQn            = 152, // Source Block: 
	clkmgr_IRQn           = 153, // Source Block: 
	restmgr_IRQn          = 154, // Source Block: 
	fpga_man_IRQn         = 155, // Source Block: 
	nCTIIRQ0_IRQn         = 156, // Source Block: 
	nCTIIRQ1_IRQn         = 157, // Source Block: 
	SEC_MGR_INTR_IRQn     = 158, // Source Block: 
	DATABWERR_IRQn        = 159, // Source Block: 
} IRQn_Type;

#include "core_ca.h"               /* Cortex-A processor and core peripherals       */
#include "cmsis_compiler.h"


/*
#include "include/alt_16550_uart.h"
#include "include/alt_generalpurpose_io.h"
#include "include/alt_nand.h"
#include "include/alt_address_space.h"
#include "include/alt_globaltmr.h"
#include "include/alt_printf.h"
#include "include/alt_cache.h"
#include "include/alt_i2c.h"
#include "include/alt_qspi.h"
#include "include/alt_can.h"
#include "include/alt_interrupt.h"
#include "include/alt_sdmmc.h"
#include "include/alt_can_private.h"
#include "include/alt_interrupt_common.h"
#include "include/alt_spi.h"
#include "include/alt_dma.h"
#include "include/alt_l2_p310.h"
#include "include/alt_timers.h"
#include "include/alt_dma_common.h"
#include "include/alt_mmu.h"
#include "include/alt_watchdog.h"
#include "include/alt_dma_program.h"
#include "include/alt_mpu_registers.h"
#include "include/hwlib.h"

#include "include/soc_a10/alt_bridge_manager.h"
#include "include/soc_a10/alt_ecc.h"
#include "include/soc_a10/alt_sdram.h"
#include "include/soc_a10/alt_clock_manager.h"
#include "include/soc_a10/alt_fpga_manager.h"
#include "include/soc_a10/alt_system_manager.h"
#include "include/soc_a10/alt_config.h"
#include "include/soc_a10/alt_int_device.h"
#include "include/soc_a10/alt_dma_periph.h"
#include "include/soc_a10/alt_reset_manager.h"
*/

#endif // ARRIA_X_