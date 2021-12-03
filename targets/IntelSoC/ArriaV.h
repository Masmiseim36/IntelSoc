#ifndef ARRIA_V_
#define ARRIA_V_

#define __CA_REV        0x0300U    /*!< Core revision r3p0                          */
#define __CORTEX_A           9U    /*!< Cortex-A9 Core                              */
#define __FPU_PRESENT        1U    /*!< FPU present                                 */
#define __GIC_PRESENT        1U    /*!< GIC present                                 */
#define __TIM_PRESENT        1U    /*!< TIM present                                 */
#define __L2C_PRESENT        1U    /*!< L2C present                                 */


#include "core_ca.h"               /* Cortex-A processor and core peripherals       */


#include "cmsis_compiler.h"


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

#include "include/soc_cv_av/alt_bridge_manager.h"
#include "include/soc_cv_av/alt_dma_periph.h"
#include "include/soc_cv_av/alt_reset_manager.h"
#include "include/soc_cv_av/alt_clock_group.h"
#include "include/soc_cv_av/alt_ecc.h"
#include "include/soc_cv_av/alt_sdram.h"
#include "include/soc_cv_av/alt_clock_manager.h"
#include "include/soc_cv_av/alt_fpga_manager.h"
#include "include/soc_cv_av/alt_system_manager.h"
#include "include/soc_cv_av/alt_config.h"
#include "include/soc_cv_av/alt_int_device.h"

#include "include/soc_cv_av/socal/hps.h"
#include "include/soc_cv_av/socal/socal.h"

#endif // ARRIA_V_