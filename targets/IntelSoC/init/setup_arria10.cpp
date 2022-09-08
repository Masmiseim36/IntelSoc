#include "setup.h"

#include "socal/socal.h"
#include "alt_globaltmr.h"
#include "alt_reset_manager.h"
#include "alt_clock_manager.h"
#include "alt_watchdog.h"
#include "alt_generalpurpose_io.h"
extern "C"
{
   #include "clock_manager_arria10.h"
}


void SetupController (void)
{
   socfpga_init_security_policies();


   cm_basic_init();
   // Init GPIO
   alt_write_word(0xffd0720c, PINMUX_DEDICATED_IO_4_SEL);
   alt_write_word(0xffd07210, PINMUX_DEDICATED_IO_5_SEL);
   alt_write_word(0xffd07214, PINMUX_DEDICATED_IO_6_SEL);
   alt_write_word(0xffd07218, PINMUX_DEDICATED_IO_7_SEL);
   alt_write_word(0xffd0721c, PINMUX_DEDICATED_IO_8_SEL);
   alt_write_word(0xffd07220, PINMUX_DEDICATED_IO_9_SEL);
   alt_write_word(0xffd07224, PINMUX_DEDICATED_IO_10_SEL);
   alt_write_word(0xffd07228, PINMUX_DEDICATED_IO_11_SEL);
   alt_write_word(0xffd0722c, PINMUX_DEDICATED_IO_12_SEL);
   alt_write_word(0xffd07230, PINMUX_DEDICATED_IO_13_SEL);
   alt_write_word(0xffd07234, PINMUX_DEDICATED_IO_14_SEL);
   alt_write_word(0xffd07238, PINMUX_DEDICATED_IO_15_SEL);
   alt_write_word(0xffd0723c, PINMUX_DEDICATED_IO_16_SEL);
   alt_write_word(0xffd07240, PINMUX_DEDICATED_IO_17_SEL);


   alt_write_word(0xffd072100, CONFIG_IO_BANK_VSEL);
   alt_write_word(0xffd072104, CONFIG_IO_MACRO(CONFIG_IO_1));
   alt_write_word(0xffd072108, CONFIG_IO_MACRO(CONFIG_IO_2));
   alt_write_word(0xffd07210c, CONFIG_IO_MACRO(CONFIG_IO_3));
   alt_write_word(0xffd072110, CONFIG_IO_MACRO(CONFIG_IO_4));
   alt_write_word(0xffd072114, CONFIG_IO_MACRO(CONFIG_IO_5));
   alt_write_word(0xffd072118, CONFIG_IO_MACRO(CONFIG_IO_6));
   alt_write_word(0xffd07211c, CONFIG_IO_MACRO(CONFIG_IO_7));
   alt_write_word(0xffd072120, CONFIG_IO_MACRO(CONFIG_IO_8));
   alt_write_word(0xffd072124, CONFIG_IO_MACRO(CONFIG_IO_9));
   alt_write_word(0xffd072128, CONFIG_IO_MACRO(CONFIG_IO_10));
   alt_write_word(0xffd07212c, CONFIG_IO_MACRO(CONFIG_IO_11));
   alt_write_word(0xffd072130, CONFIG_IO_MACRO(CONFIG_IO_12));
   alt_write_word(0xffd072134, CONFIG_IO_MACRO(CONFIG_IO_13));
   alt_write_word(0xffd072138, CONFIG_IO_MACRO(CONFIG_IO_14));
   alt_write_word(0xffd07213c, CONFIG_IO_MACRO(CONFIG_IO_15));
   alt_write_word(0xffd072140, CONFIG_IO_MACRO(CONFIG_IO_16));
   alt_write_word(0xffd072144, CONFIG_IO_MACRO(CONFIG_IO_17));

   alt_write_word(0xffd07400, PINMUX_RGMII0_USEFPGA_SEL);
   alt_write_word(0xffd07404, PINMUX_RGMII1_USEFPGA_SEL);
   alt_write_word(0xffd07408, PINMUX_RGMII2_USEFPGA_SEL);
   alt_write_word(0xffd0740c, PINMUX_I2C0_USEFPGA_SEL);
   alt_write_word(0xffd07410, PINMUX_I2C1_USEFPGA_SEL);
   alt_write_word(0xffd07414, PINMUX_I2CEMAC0_USEFPGA_SEL);
   alt_write_word(0xffd07418, PINMUX_I2CEMAC1_USEFPGA_SEL);
   alt_write_word(0xffd0741c, PINMUX_I2CEMAC2_USEFPGA_SEL);
   alt_write_word(0xffd07420, PINMUX_NAND_USEFPGA_SEL);
   alt_write_word(0xffd07424, PINMUX_QSPI_USEFPGA_SEL);
   alt_write_word(0xffd07428, PINMUX_SDMMC_USEFPGA_SEL);
   alt_write_word(0xffd0742c, PINMUX_SPIM0_USEFPGA_SEL);
   alt_write_word(0xffd07430, PINMUX_SPIM1_USEFPGA_SEL);
   alt_write_word(0xffd07434, PINMUX_SPIS0_USEFPGA_SEL);
   alt_write_word(0xffd07438, PINMUX_SPIS1_USEFPGA_SEL);
   alt_write_word(0xffd0743c, PINMUX_UART0_USEFPGA_SEL);
   alt_write_word(0xffd07440, PINMUX_UART1_USEFPGA_SEL);

   alt_globaltmr_init();
   alt_globaltmr_start();
}