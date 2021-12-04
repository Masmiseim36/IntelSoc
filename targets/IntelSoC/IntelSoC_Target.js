/*****************************************************************************
 * Modified work Copyright (C) 2019-2021 Markus Klein                        *
 *                                                                           *
 * This file may be distributed under the terms of the License Agreement     *
 * provided with this software.                                              *
 *                                                                           *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE   *
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. *
 *                                                                           *
 *****************************************************************************/

// Don't do any register access in this function.
// Use GetPartName() for this.
function Connect ()
{
	if (TargetInterface.implementation() != "j-link")
	{
		TargetInterface.setDebugInterfaceProperty ("set_adiv5_AHB_ap_num", 0);
//		TargetInterface.setDebugInterfaceProperty ("use_adiv5_AHB", 0, 0x00100000, 0x10000000); // DDR RAM
	}
}

// This function is used to return the controller type as a string
// we use it also to do some initializations as this function is called right before
// writing the code to the controller
function GetPartName ()
{
	TargetInterface.message ("## GetPartName");
}

function MatchPartName (name)
{
	var partName = GetPartName ();

	if (partName == "")
		return false;

	return partName.substring (0, 6) == name.substring (0, 6);
}

function Reset ()
{
	TargetInterface.message ("## Reset");
	TargetInterface.resetAndStop (1000);
}

function LoadBegin ()
{
	TargetInterface.message ("## call LoadBegin");
	InitializeDdrMemory ();
}

function LoadEnd ()
{
	TargetInterface.message ("## call LoadEnd");
	var cpsr = TargetInterface.getRegister ("cpsr");
	if ((cpsr != 0xFFFFFFFF) && ((cpsr & (1 << 5)) == (1 << 5)))
	{
		TargetInterface.message ("## CPU is in Thumb Mode. Switch to ARM Mode");
		cpsr -= (1 << 5);
		TargetInterface.setRegister ("cpsr", cpsr);
	}

	TargetInterface.setRegister ("pc", 0x00100004);
}

function InitializeDdrMemory ()
{
	if (TargetInterface.implementation() == "crossworks_simulator")
	{
		return;
	}

	TargetInterface.message ("## load initialization App");
	TargetInterface.pokeBinary (0xFFFF0000, "$(TargetsDir)/IntelSoC/init/ARM Release/Init.bin");
	TargetInterface.message ("## start initialization App");
	var ret = TargetInterface.runFromAddress (0xFFFF0000, 10000);
	TargetInterface.message ("## initialization done: " + ret);
	TargetInterface.stop ();
}


var sdr                               = 0xFFC20000;
var sdr_ctrlgrp_ctrlcfg               = sdr + 0x00005000;
var sdr_ctrlgrp_dramtiming1           = sdr + 0x00005004;
var sdr_ctrlgrp_dramtiming2           = sdr + 0x00005008;
var sdr_ctrlgrp_dramtiming3           = sdr + 0x0000500C;
var sdr_ctrlgrp_dramtiming4           = sdr + 0x00005010;
var sdr_ctrlgrp_lowpwrtiming          = sdr + 0x00005014;
var sdr_ctrlgrp_dramodt               = sdr + 0x00005018;
var sdr_ctrlgrp_dramaddrw             = sdr + 0x0000502C;
var sdr_ctrlgrp_dramifwidth           = sdr + 0x00005030;
var sdr_ctrlgrp_dramdevwidth          = sdr + 0x00005034;
var sdr_ctrlgrp_dramsts               = sdr + 0x00005038;
var sdr_ctrlgrp_dramintr              = sdr + 0x0000503C;
var sdr_ctrlgrp_sbecount              = sdr + 0x00005040;
var sdr_ctrlgrp_dbecount              = sdr + 0x00005044;
var sdr_ctrlgrp_erraddr               = sdr + 0x00005048;
var sdr_ctrlgrp_dropcount             = sdr + 0x0000504C;
var sdr_ctrlgrp_dropaddr              = sdr + 0x00005050;
var sdr_ctrlgrp_lowpwreq              = sdr + 0x00005054;
var sdr_ctrlgrp_lowpwrack             = sdr + 0x00005058;
var sdr_ctrlgrp_staticcfg             = sdr + 0x0000505C;
var sdr_ctrlgrp_ctrlwidth             = sdr + 0x00005060;
var sdr_ctrlgrp_portcfg               = sdr + 0x0000507C;
var sdr_ctrlgrp_fpgaportrst           = sdr + 0x00005080;
var sdr_ctrlgrp_protportdefault       = sdr + 0x0000508C;
var sdr_ctrlgrp_protruleaddr          = sdr + 0x00005090;
var sdr_ctrlgrp_protruleid            = sdr + 0x00005094;
var sdr_ctrlgrp_protruledata          = sdr + 0x00005098;
var sdr_ctrlgrp_protrulerdwr          = sdr + 0x0000509C;
var sdr_ctrlgrp_qoslowpri             = sdr + 0x000050A0;
var sdr_ctrlgrp_qoshighpri            = sdr + 0x000050A4;
var sdr_ctrlgrp_qospriorityen         = sdr + 0x000050A8;
var sdr_ctrlgrp_mppriority            = sdr + 0x000050AC;
var sdr_ctrlgrp_mpweight_mpweight_0_4 = sdr + 0x000050B0;
var sdr_ctrlgrp_mpweight_mpweight_1_4 = sdr + 0x000050B4;
var sdr_ctrlgrp_mpweight_mpweight_2_4 = sdr + 0x000050B8;
var sdr_ctrlgrp_mpweight_mpweight_3_4 = sdr + 0x000050BC;
var sdr_ctrlgrp_remappriority         = sdr + 0x000050E0;

var osc1timer0                  = 0xFFD00000;
var osc1timer0_timer1loadcount  = osc1timer0 + 0x00000000;
var osc1timer0_timer1currentval = osc1timer0 + 0x00000004;
var osc1timer0_timer1controlreg = osc1timer0 + 0x00000008;
var osc1timer1                  = 0xFFD01000;
var osc1timer1_timer1loadcount  = osc1timer1 + 0x00000000;
var osc1timer1_timer1currentval = osc1timer1 + 0x00000004;
var osc1timer1_timer1controlreg = osc1timer1 + 0x00000008;

var l4wd0     = 0xFFD02000;
var l4wd0_crr = l4wd0 + 0xC;

var sysmgr                   = 0xFFD08000;
var sysmgr_bootinfo          = sysmgr + 0x00000014;
var sysmgr_frzctrl_vioctrl_0 = sysmgr + 0x00000040;
var sysmgr_frzctrl_vioctrl_1 = sysmgr + 0x00000044;
var sysmgr_frzctrl_vioctrl_2 = sysmgr + 0x00000048;
var sysmgr_frzctrl_hioctrl   = sysmgr + 0x00000050;
var sysmgr_frzctrl_src       = sysmgr + 0x00000054;
var sysmgr_pinmuxgrp_EMACIO0 = sysmgr + 0x00000400;

var rstmgr           = 0xFFD05000;
var rstmgr_stat      = rstmgr + 0x00000000;
var rstmgr_ctrl      = rstmgr + 0x00000004;
var rstmgr_permodrst = rstmgr + 0x00000014;

var clkmgr                           = 0xFFD04000;
var clkmgr_ctrl                      = clkmgr + 0x00000000;
var clkmgr_bypass                    = clkmgr + 0x00000004;
var clkmgr_inter                     = clkmgr + 0x00000008;
var clkmgr_intren                    = clkmgr + 0x0000000C;
var clkmgr_dbctrl                    = clkmgr + 0x00000010;
var clkmgr_stat                      = clkmgr + 0x00000014;
var clkmgr_mainpllgrp_vco            = clkmgr + 0x00000040;
var clkmgr_mainpllgrp_misc           = clkmgr + 0x00000044;
var clkmgr_mainpllgrp_mpuclk         = clkmgr + 0x00000048;
var clkmgr_mainpllgrp_mainclk        = clkmgr + 0x0000004C;
var clkmgr_mainpllgrp_dbgatclk       = clkmgr + 0x00000050;
var clkmgr_mainpllgrp_mainqspiclk    = clkmgr + 0x00000054;
var clkmgr_mainpllgrp_cfgs2fuser0clk = clkmgr + 0x0000005C;
var clkmgr_mainpllgrp_en             = clkmgr + 0x00000060;
var clkmgr_mainpllgrp_maindiv        = clkmgr + 0x00000064;
var clkmgr_mainpllgrp_dbgdiv         = clkmgr + 0x00000068;
var clkmgr_mainpllgrp_tracediv       = clkmgr + 0x0000006C;
var clkmgr_mainpllgrp_l4src          = clkmgr + 0x00000070;
var clkmgr_perpllgrp_vco             = clkmgr + 0x00000080;
var clkmgr_perpllgrp_misc            = clkmgr + 0x00000084;
var clkmgr_perpllgrp_emac0clk        = clkmgr + 0x00000088;
var clkmgr_perpllgrp_emac1clk        = clkmgr + 0x0000008C;
var clkmgr_perpllgrp_perqspiclk      = clkmgr + 0x00000090;
var clkmgr_perpllgrp_pernandsdmmcclk = clkmgr + 0x00000094;
var clkmgr_perpllgrp_perbaseclk      = clkmgr + 0x00000098;
var clkmgr_perpllgrp_s2fuser1clk     = clkmgr + 0x0000009C;
var clkmgr_perpllgrp_en              = clkmgr + 0x000000A0;
var clkmgr_perpllgrp_div             = clkmgr + 0x000000A4;
var clkmgr_perpllgrp_gpiodiv         = clkmgr + 0x000000A8;
var clkmgr_perpllgrp_src             = clkmgr + 0x000000AC;
var clkmgr_sdrpllgrp_vco             = clkmgr + 0x000000C0;
var clkmgr_sdrpllgrp_ctrl			 = clkmgr + 0x000000C4;
var clkmgr_sdrpllgrp_ddrdqsclk		 = clkmgr + 0x000000C8;
var clkmgr_sdrpllgrp_ddr2xdqsclk	 = clkmgr + 0x000000CC;
var clkmgr_sdrpllgrp_ddrdqclk		 = clkmgr + 0x000000D0;
var clkmgr_sdrpllgrp_s2fuser2clk	 = clkmgr + 0x000000D4;
var clkmgr_sdrpllgrp_en              = clkmgr + 0x000000D8;
var clkmgr_alteragrp_mpu             = clkmgr + 0xE0;
var clkmgr_alteragrp_main            = clkmgr + 0xE4;

var PinInitTable =
[
	0, /* EMACIO0 - Unused */
	2, /* EMACIO1 - USB */
	2, /* EMACIO2 - USB */
	2, /* EMACIO3 - USB */
	2, /* EMACIO4 - USB */
	2, /* EMACIO5 - USB */
	2, /* EMACIO6 - USB */
	2, /* EMACIO7 - USB */
	2, /* EMACIO8 - USB */
	0, /* EMACIO9 - Unused */
	2, /* EMACIO10 - USB */
	2, /* EMACIO11 - USB */
	2, /* EMACIO12 - USB */
	2, /* EMACIO13 - USB */
	0, /* EMACIO14 - N/A */
	0, /* EMACIO15 - N/A */
	0, /* EMACIO16 - N/A */
	0, /* EMACIO17 - N/A */
	0, /* EMACIO18 - N/A */
	0, /* EMACIO19 - N/A */
	3, /* FLASHIO0 - SDMMC */
	3, /* FLASHIO1 - SDMMC */
	3, /* FLASHIO2 - SDMMC */
	3, /* FLASHIO3 - SDMMC */
	0, /* FLASHIO4 - SDMMC */
	0, /* FLASHIO5 - SDMMC */
	0, /* FLASHIO6 - SDMMC */
	0, /* FLASHIO7 - SDMMC */
	0, /* FLASHIO8 - SDMMC */
	3, /* FLASHIO9 - SDMMC */
	3, /* FLASHIO10 - SDMMC */
	3, /* FLASHIO11 - SDMMC */
	3, /* GENERALIO0 - TRACE */
	3, /* GENERALIO1 - TRACE */
	3, /* GENERALIO2 - TRACE */
	3, /* GENERALIO3 - TRACE  */
	3, /* GENERALIO4 - TRACE  */
	3, /* GENERALIO5 - TRACE  */
	3, /* GENERALIO6 - TRACE  */
	3, /* GENERALIO7 - TRACE  */
	3, /* GENERALIO8 - TRACE  */
	0, /* GENERALIO9 - N/A */
	0, /* GENERALIO10 - N/A */
	0, /* GENERALIO11 - N/A */
	0, /* GENERALIO12 - N/A */
	0, /* GENERALIO13 - N/A */
	0, /* GENERALIO14 - N/A */
	3, /* GENERALIO15 - I2C0 */
	3, /* GENERALIO16 - I2C0 */
	2, /* GENERALIO17 - UART0 */
	2, /* GENERALIO18 - UART0 */
	0, /* GENERALIO19 - N/A */
	0, /* GENERALIO20 - N/A */
	0, /* GENERALIO21 - N/A */
	0, /* GENERALIO22 - N/A */
	0, /* GENERALIO23 - N/A */
	0, /* GENERALIO24 - N/A */
	0, /* GENERALIO25 - N/A */
	0, /* GENERALIO26 - N/A */
	0, /* GENERALIO27 - N/A */
	0, /* GENERALIO28 - N/A */
	0, /* GENERALIO29 - N/A */
	0, /* GENERALIO30 - N/A */
	0, /* GENERALIO31 - N/A */
	2, /* MIXED1IO0 - EMAC */
	2, /* MIXED1IO1 - EMAC */
	2, /* MIXED1IO2 - EMAC */
	2, /* MIXED1IO3 - EMAC */
	2, /* MIXED1IO4 - EMAC */
	2, /* MIXED1IO5 - EMAC */
	2, /* MIXED1IO6 - EMAC */
	2, /* MIXED1IO7 - EMAC */
	2, /* MIXED1IO8 - EMAC */
	2, /* MIXED1IO9 - EMAC */
	2, /* MIXED1IO10 - EMAC */
	2, /* MIXED1IO11 - EMAC */
	2, /* MIXED1IO12 - EMAC */
	2, /* MIXED1IO13 - EMAC */
	0, /* MIXED1IO14 - Unused */
	3, /* MIXED1IO15 - QSPI */
	3, /* MIXED1IO16 - QSPI */
	3, /* MIXED1IO17 - QSPI */
	3, /* MIXED1IO18 - QSPI */
	3, /* MIXED1IO19 - QSPI */
	3, /* MIXED1IO20 - QSPI */
	0, /* MIXED1IO21 - GPIO */
	0, /* MIXED2IO0 - N/A */
	0, /* MIXED2IO1 - N/A */
	0, /* MIXED2IO2 - N/A */
	0, /* MIXED2IO3 - N/A */
	0, /* MIXED2IO4 - N/A */
	0, /* MIXED2IO5 - N/A */
	0, /* MIXED2IO6 - N/A */
	0, /* MIXED2IO7 - N/A */
	0, /* GPLINMUX48 */
	0, /* GPLINMUX49 */
	0, /* GPLINMUX50 */
	0, /* GPLINMUX51 */
	0, /* GPLINMUX52 */
	0, /* GPLINMUX53 */
	0, /* GPLINMUX54 */
	0, /* GPLINMUX55 */
	0, /* GPLINMUX56 */
	0, /* GPLINMUX57 */
	0, /* GPLINMUX58 */
	0, /* GPLINMUX59 */
	0, /* GPLINMUX60 */
	0, /* GPLINMUX61 */
	0, /* GPLINMUX62 */
	0, /* GPLINMUX63 */
	0, /* GPLINMUX64 */
	0, /* GPLINMUX65 */
	0, /* GPLINMUX66 */
	0, /* GPLINMUX67 */
	0, /* GPLINMUX68 */
	0, /* GPLINMUX69 */
	0, /* GPLINMUX70 */
	1, /* GPLMUX0 */
	1, /* GPLMUX1 */
	1, /* GPLMUX2 */
	1, /* GPLMUX3 */
	1, /* GPLMUX4 */
	1, /* GPLMUX5 */
	1, /* GPLMUX6 */
	1, /* GPLMUX7 */
	1, /* GPLMUX8 */
	1, /* GPLMUX9 */
	1, /* GPLMUX10 */
	1, /* GPLMUX11 */
	1, /* GPLMUX12 */
	1, /* GPLMUX13 */
	1, /* GPLMUX14 */
	1, /* GPLMUX15 */
	1, /* GPLMUX16 */
	1, /* GPLMUX17 */
	1, /* GPLMUX18 */
	1, /* GPLMUX19 */
	1, /* GPLMUX20 */
	1, /* GPLMUX21 */
	1, /* GPLMUX22 */
	1, /* GPLMUX23 */
	1, /* GPLMUX24 */
	1, /* GPLMUX25 */
	1, /* GPLMUX26 */
	1, /* GPLMUX27 */
	1, /* GPLMUX28 */
	1, /* GPLMUX29 */
	1, /* GPLMUX30 */
	1, /* GPLMUX31 */
	1, /* GPLMUX32 */
	1, /* GPLMUX33 */
	1, /* GPLMUX34 */
	1, /* GPLMUX35 */
	1, /* GPLMUX36 */
	1, /* GPLMUX37 */
	1, /* GPLMUX38 */
	1, /* GPLMUX39 */
	1, /* GPLMUX40 */
	1, /* GPLMUX41 */
	1, /* GPLMUX42 */
	1, /* GPLMUX43 */
	1, /* GPLMUX44 */
	1, /* GPLMUX45 */
	1, /* GPLMUX46 */
	1, /* GPLMUX47 */
	1, /* GPLMUX48 */
	1, /* GPLMUX49 */
	1, /* GPLMUX50 */
	1, /* GPLMUX51 */
	1, /* GPLMUX52 */
	1, /* GPLMUX53 */
	1, /* GPLMUX54 */
	1, /* GPLMUX55 */
	1, /* GPLMUX56 */
	1, /* GPLMUX57 */
	1, /* GPLMUX58 */
	1, /* GPLMUX59 */
	1, /* GPLMUX60 */
	1, /* GPLMUX61 */
	1, /* GPLMUX62 */
	1, /* GPLMUX63 */
	1, /* GPLMUX64 */
	1, /* GPLMUX65 */
	1, /* GPLMUX66 */
	1, /* GPLMUX67 */
	1, /* GPLMUX68 */
	1, /* GPLMUX69 */
	1, /* GPLMUX70 */
	0, /* NANDUSEFPGA */
	0, /* UART0USEFPGA */
	0, /* RGMII1USEFPGA */
	0, /* SPIS0USEFPGA */
	0, /* CAN0USEFPGA */
	0, /* I2C0USEFPGA */
	0, /* SDMMCUSEFPGA */
	0, /* QSPIUSEFPGA */
	0, /* SPIS1USEFPGA */
	0, /* RGMII0USEFPGA */
	0, /* UART1USEFPGA */
	0, /* CAN1USEFPGA */
	0, /* USB1USEFPGA */
	0, /* I2C3USEFPGA */
	0, /* I2C2USEFPGA */
	0, /* I2C1USEFPGA */
	0, /* SPIM1USEFPGA */
	0, /* USB0USEFPGA */
	0 /* SPIM0USEFPGA */
];


function AlterRegister (Addr, Clear, Set)
{
	var temp = TargetInterface.peekUint32 (Addr);
	temp &= ~Clear;
	temp |= Set;
	TargetInterface.pokeUint32 (Addr, temp);
}


function Watchdog0Reset ()
{
	TargetInterface.pokeUint32 (l4wd0_crr, 0x76);
}

function frzctrl_freeze (channel)
{
	TargetInterface.pokeUint32 (sysmgr_frzctrl_src, 0);
	var frzctrl_vioctrl = 0;
	switch (channel)
	{
		case 0:
			frzctrl_vioctrl = sysmgr_frzctrl_vioctrl_0;
			break;
		case 1:
			frzctrl_vioctrl = sysmgr_frzctrl_vioctrl_1;
			break;
		case 2:
			frzctrl_vioctrl = sysmgr_frzctrl_vioctrl_2;
			break;
		default:
			return -1;
	}

	// Assert active low enrnsl, plniotri and niotri signals 
	AlterRegister (frzctrl_vioctrl, 0x00000010 | 0x00000008 | 0x00000004, 0);

	// Assert active low bhniotri signal and de-assert active high
	AlterRegister (frzctrl_vioctrl, 0x00000002 | 0x00000001, 0);

	// Insert short delay to allow freeze signal propagation
	TargetInterface.delay (1);
}

function frzctrl_thaw (channel)
{
	TargetInterface.pokeUint32 (sysmgr_frzctrl_src, 0);
	var frzctrl_vioctrl = 0;
	switch (channel)
	{
		case 0:
			frzctrl_vioctrl = sysmgr_frzctrl_vioctrl_0;
			break;
		case 1:
			frzctrl_vioctrl = sysmgr_frzctrl_vioctrl_1;
			break;
		case 2:
			frzctrl_vioctrl = sysmgr_frzctrl_vioctrl_2;
			break;
		default:
			return -1;
	}

	// Assert active low bhniotri signal and de-assert active high csrdone
	AlterRegister (frzctrl_vioctrl, 0, 0x00000002 | 0x00000001);

	// Note: Delay for 20ns at min
	// de-assert active low plniotri and niotri signals
	AlterRegister (frzctrl_vioctrl, 0, 0x00000008 | 0x00000004);

	// Note: Delay for 20ns at min
	// de-assert active low enrnsl signal
	AlterRegister (frzctrl_vioctrl, 0, 0x00000010);

	// Insert short delay to allow freeze signal propagation
	TargetInterface.delay (1);
}

function frzctrl_freeze_ch3 ()
{
	TargetInterface.pokeUint32 (sysmgr_frzctrl_src, 0);
	// Assert active low enrnsl, plniotri and niotri signals 
	AlterRegister (sysmgr_frzctrl_hioctrl, 0x00000010 | 0x00000008 | 0x00000004, 0);

	// assert active low bhniotri, de-assert active high csrdone and assert active high frzreg and nfrzdrv signals
	AlterRegister (sysmgr_frzctrl_hioctrl, 0x00000002 | 0x00000001, 0x00000080 | 0x00000040);

	// assert active high reinit signal and de-assert active high pllbiasen signals
	AlterRegister (sysmgr_frzctrl_hioctrl, 0x00000100, 0x00000020);

	// Insert short delay to allow freeze signal propagation
	TargetInterface.delay (1);
}

function frzctrl_thaw_ch3 ()
{
	TargetInterface.pokeUint32 (sysmgr_frzctrl_src, 0);

	//de-assert active high reinit signal
	AlterRegister (sysmgr_frzctrl_hioctrl, 0x00000020, 0);

	// Note: Delay for 40ns at min
	// assert active high pllbiasen signals
	AlterRegister (sysmgr_frzctrl_hioctrl, 0, 0x00000100);

	// Delay 1000 intosc. intosc is based on eosc1
	// start_l4_timer (ALT_OSC1TMR0_OFST, 0xFFFFFFFF, 0x03);
	TargetInterface.pokeUint32 (osc1timer0_timer1controlreg, 0);	// disable
	TargetInterface.pokeUint32 (osc1timer0_timer1loadcount,  0xFFFFFFFF);  // Load value
	TargetInterface.pokeUint32 (osc1timer0_timer1controlreg, 0x3);	// enable, re-start mode
	var startCount = TargetInterface.peekUint32 (osc1timer0_timer1currentval); // read current count
	startCount += 33;
	var now = 0;
	do
	{
		now = TargetInterface.peekUint32 (osc1timer0_timer1currentval); // read current count
	}
	while (startCount >= now);

	// de-assert active low bhniotri signal, assert active high
	// csrdone and nfrzdrv signals
	AlterRegister (sysmgr_frzctrl_hioctrl, 0x00000040, 0x00000002 | 0x00000001);

	// Delay 33 intosc
	// reset_timer_count (ALT_OSC1TMR0_OFST);
	// start_count = get_timer_count (0, ALT_OSC1TMR0_OFST);
	TargetInterface.pokeUint32 (osc1timer0_timer1loadcount,  0xFFFFFFFF);  // Load value --> reset_count
	startCount = TargetInterface.peekUint32 (osc1timer0_timer1currentval); // read current count
	startCount += 33;
	do
	{
		now = TargetInterface.peekUint32 (osc1timer0_timer1currentval); // read current count
	}
	while (startCount >= now);

	// Stop L4 timer
	// disable_l4_timer(ALT_OSC1TMR0_OFST, 0);
	TargetInterface.pokeUint32 (osc1timer0_timer1controlreg, 0);	// disable

	// de-assert active low plniotri and niotri signals
	AlterRegister (sysmgr_frzctrl_hioctrl, 0, 0x00000008 | 0x00000004);

	// Note: Delay for 40ns at min
	// de-assert active high frzreg signal
	AlterRegister (sysmgr_frzctrl_hioctrl, 0x00000080, 0);

	// Note: Delay for 40ns at min
	// de-assert active low enrnsl signal
	AlterRegister (sysmgr_frzctrl_hioctrl, 0, 0x00000010);

	// Insert short delay to allow freeze signal propagation
	TargetInterface.delay (1);
}

function Initialize ()
{
	Watchdog0Reset ();
	// ensure all IO banks are in frozen state via Freeze Controller
	frzctrl_freeze (0);
	frzctrl_freeze (1);
	frzctrl_freeze (2);
	frzctrl_freeze_ch3 ();

	// Assert reset to peripherals except L4wd0 and all bridges
	TargetInterface.pokeUint32 (rstmgr_permodrst, 0xffffffbf);

	// Enable and init OSC1 Timer0 (required during PLL-config)
	AlterRegister (rstmgr_permodrst, 0x00000100, 0);
	// release reset bit
	AlterRegister (osc1timer0_timer1controlreg, 0, 0x00000002);
	TargetInterface.pokeUint32 (osc1timer0_timer1loadcount, 0xFFFFFFFF); // set loadreg value to max
	AlterRegister (osc1timer0_timer1controlreg, 0, 0x00000001); // start timer

	Watchdog0Reset ();

	// Set up the PLL RAM Boot fix, if needed
	var bootInfo = argetInterface.peekUint32 (sysmgr_bootinfo);
	if ((bootInfo & 0x00000018) == 0)
	{
		// ToDo
		// Copy the ramboot program at offset 60 KiB in the on-chip RAM.
	}


	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Clock initialize
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	TargetInterface.message ("## Start clock initialization");
	// Re-configuration of all PLLs
	// fogbugz 61158 - we need to disable nandclk and then do another apb access before disabling gatting off the rest of the periperal clocks.
	var val = TargetInterface.peekUint32 (clkmgr_perpllgrp_en);
	val &= 0xfffffbff;
	TargetInterface.pokeUint32 (clkmgr_perpllgrp_en, 0, val)

	// DO NOT GATE OFF DEBUG CLOCKS
	TargetInterface.pokeUint32 (clkmgr_mainpllgrp_en, 0x02F4);
	TargetInterface.pokeUint32 (clkmgr_sdrpllgrp_en, 0);

	// Put all plls in bypass
	TargetInterface.pokeUint32 (clkmgr_bypass, 11);

	var stat = 0;
	do
	{
		stat = TargetInterface.peekUint32 (clkmgr_stat);
	}
	while (stat & 1); // wait while bussy

	TargetInterface.pokeUint32 (clkmgr_mainpllgrp_vco, 0x0001000D);
	TargetInterface.pokeUint32 (clkmgr_perpllgrp_vco,  0x0001000D);
	TargetInterface.pokeUint32 (clkmgr_sdrpllgrp_vco,  0x0001000D);

	// Fogbugz 60434 - the clocks to the flash devices and the L4_MAIN clocks can glitch when coming out of safe mode if their source values are different from their reset value.
    // So the trick it to put them back to their reset state, and change input after exiting safe mode but before ungating the clocks.
	TargetInterface.pokeUint32 (clkmgr_perpllgrp_src,    21);
	TargetInterface.pokeUint32 (clkmgr_mainpllgrp_l4src, 0);

	// Now deassert BG PWR DN and set VCO reg with values from passdown file.
	TargetInterface.pokeUint32 (clkmgr_mainpllgrp_vco, 0x800001F8);
	TargetInterface.pokeUint32 (clkmgr_perpllgrp_vco,  192);
	TargetInterface.pokeUint32 (clkmgr_sdrpllgrp_vco,  64);

	var start = TargetInterface.peekUint32 (osc1timer0_timer1currentval);

	TargetInterface.pokeUint32 (clkmgr_mainpllgrp_mpuclk, 0);
	TargetInterface.pokeUint32 (clkmgr_mainpllgrp_mainclk, 0);

	TargetInterface.pokeUint32 (clkmgr_alteragrp_mpu, 1);           // re-configuring fixed divider for faster MPU clock
	TargetInterface.pokeUint32 (clkmgr_mainpllgrp_dbgatclk, 0);     // main for dbg cnt
	TargetInterface.pokeUint32 (clkmgr_alteragrp_main, 0);          // re-configuring the fixed divider due to different main VCO
	TargetInterface.pokeUint32 (clkmgr_mainpllgrp_cfgs2fuser0clk, 15); // main for cfgs2fuser0clk cnt
	TargetInterface.pokeUint32 (clkmgr_perpllgrp_emac0clk, 0x01FF); // Peri emac0 cnt
	TargetInterface.pokeUint32 (clkmgr_perpllgrp_emac1clk, 3);      // Peri emac1 cnt
	TargetInterface.pokeUint32 (clkmgr_mainpllgrp_mainqspiclk, 3);  // Peri QSPI div
	TargetInterface.pokeUint32 (clkmgr_perpllgrp_perqspiclk, 0x1FF);

	TargetInterface.pokeUint32 (clkmgr_perpllgrp_pernandsdmmcclk, 4); // Peri pernandsdmmcclk cnt
	TargetInterface.pokeUint32 (clkmgr_perpllgrp_perbaseclk,      4); // Peri perbaseclk cnt
	TargetInterface.pokeUint32 (clkmgr_perpllgrp_s2fuser1clk,   511); // Peri s2fuser1clk cnt

	// 7 us must have elapsed before we can enable vco's

	// Enable vco's
	TargetInterface.pokeUint32 (clkmgr_mainpllgrp_vco, 0x01FA); // main pll vco is typically 800 MHz.
	TargetInterface.pokeUint32 (clkmgr_perpllgrp_vco,  0x013A); // periferal pll typically 1000 MHz.
	TargetInterface.pokeUint32 (clkmgr_sdrpllgrp_vco,  250);    // sdram pll vco 1062.5 MHz.

	// setup dividers while plls are locking set L3 MP and L3 SP to divide by 2 from main_clk, down to 400 MHz from the 800 MHz main_clk
	TargetInterface.pokeUint32 (clkmgr_mainpllgrp_maindiv, 149);
	TargetInterface.pokeUint32 (clkmgr_mainpllgrp_dbgdiv,    4);
	TargetInterface.pokeUint32 (clkmgr_mainpllgrp_tracediv,  0);

	// set L4 MP/SP, can0, and can1, etc.. div from perif-base_clock
	TargetInterface.pokeUint32 (clkmgr_perpllgrp_div,     0x0900);
	TargetInterface.pokeUint32 (clkmgr_perpllgrp_gpiodiv, 0x1869);

	var LockedMask = 0x1C0;
	var Counter = 0;
	do
	{
		stat = TargetInterface.peekUint32 (clkmgr_inter);
		Counter++;
	}
	while ((stat & LockedMask) == LockedMask && Counter < 20); // wait while bussy

	// write the sdram clock counters before toggling outreset all (without phase info)
	TargetInterface.pokeUint32 (clkmgr_sdrpllgrp_ddrdqsclk,   1);
	TargetInterface.pokeUint32 (clkmgr_sdrpllgrp_ddr2xdqsclk, 0);
	TargetInterface.pokeUint32 (clkmgr_sdrpllgrp_ddrdqclk,    1);
	TargetInterface.pokeUint32 (clkmgr_sdrpllgrp_s2fuser2clk, 5);

	// after locking, but before taking out of bypass assert/deassert outresetall
	AlterRegister (clkmgr_mainpllgrp_vco, 0, 0x01000000); // assert main outresetall
	AlterRegister (clkmgr_perpllgrp_vco,  0, 0x01000000); // assert pheriph outresetall
	AlterRegister (clkmgr_sdrpllgrp_vco,  0, 0x01000000); // assert sdram outresetall

	AlterRegister (clkmgr_mainpllgrp_vco, 0x01000000, 0); // deassert main outresetall
	AlterRegister (clkmgr_perpllgrp_vco,  0x01000000, 0); // deassert pheriph outresetall
	AlterRegister (clkmgr_sdrpllgrp_vco,  0x01000000, 0); // deassert sdram outresetall

	// now that we've toggled outreset all, all the clocks are aligned nicely; so we can change any phase.
	TargetInterface.pokeUint32 (clkmgr_sdrpllgrp_ddrdqsclk,    1); // re-write the sdram clock counters with phase info
	TargetInterface.pokeUint32 (clkmgr_sdrpllgrp_ddr2xdqsclk,  0); // SDRAM DDR2XDQSCLK 1062.5 MHz
	TargetInterface.pokeUint32 (clkmgr_sdrpllgrp_ddrdqclk, 0x0801);
	TargetInterface.pokeUint32 (clkmgr_sdrpllgrp_s2fuser2clk,  5); // divide by six to get 177.08 MHz

	// Take all three PLLs out of bypass when safe mode is cleared.
	TargetInterface.pokeUint32 (clkmgr_bypass, 0);

	do
	{
		stat = TargetInterface.peekUint32 (clkmgr_stat);
	}
	while (stat & 1); // wait while bussy

	// clear safe mode.. W1C write 1 to clear
	AlterRegister (clkmgr_ctrl, 0, 1);
	do
	{
		stat = TargetInterface.peekUint32 (clkmgr_stat);
	}
	while (stat & 1); // wait while bussy

	// now that safe mode is clear with clocks gated it safe to change the source mux for the flashes the the L4_MAIN
	TargetInterface.pokeUint32 (clkmgr_perpllgrp_src,   26);
	TargetInterface.pokeUint32 (clkmgr_mainpllgrp_l4src, 3);

	// Now ungate non-hw-managed clocks
	TargetInterface.pokeUint32 (clkmgr_mainpllgrp_en, 0xFFFFFFFF);
	TargetInterface.pokeUint32 (clkmgr_perpllgrp_en,  0xFFFFFFFF);
	TargetInterface.pokeUint32 (clkmgr_sdrpllgrp_en,  0xFFFFFFFF);

	Watchdog0Reset ();
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// init Scan Manager to configure HPS IOCSR
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	TargetInterface.message ("## Start init scan manager initialization");
	// ToDo


	Watchdog0Reset ();
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// config pin mux via system manager
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	TargetInterface.message ("## Start pin mux initialization");
	var Pin = 0;
	while (Pin < 207)
	{
		TargetInterface.pokeUint32 (sysmgr_pinmuxgrp_EMACIO0 + (i*4), PinInitTable[i]);
		Pin++;
	}

	frzctrl_thaw (0);
	frzctrl_thaw (1);
	frzctrl_thaw (2);
	frzctrl_thaw_ch3 ();

	Watchdog0Reset ();
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// SDRAM init
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	TargetInterface.message ("## Start SDRAM initialization");

	// Take the SDR peripheral out of reset.
	AlterRegister (rstmgr_permodrst, 0x20000000, 0);

	TargetInterface.pokeUint32 (sdr_ctrlgrp_ctrlcfg,               0x000a8842);
	TargetInterface.pokeUint32 (sdr_ctrlgrp_dramtiming1,           0x7848ce07);
	TargetInterface.pokeUint32 (sdr_ctrlgrp_dramtiming2,           0x08cccc30);
	TargetInterface.pokeUint32 (sdr_ctrlgrp_dramtiming3,           0x002228f3);
	TargetInterface.pokeUint32 (sdr_ctrlgrp_dramtiming4,           0x00000e00);
	TargetInterface.pokeUint32 (sdr_ctrlgrp_lowpwrtiming,          0x00000000);
	TargetInterface.pokeUint32 (sdr_ctrlgrp_dramaddrw,             0x00002dea);
	TargetInterface.pokeUint32 (sdr_ctrlgrp_dramifwidth,           0x00000020);
	TargetInterface.pokeUint32 (sdr_ctrlgrp_dramdevwidth,          0x00000008);
	TargetInterface.pokeUint32 (sdr_ctrlgrp_dramintr,              0x00000000);
	TargetInterface.pokeUint32 (sdr_ctrlgrp_staticcfg,             0x00000002);
	TargetInterface.pokeUint32 (sdr_ctrlgrp_ctrlwidth,             0x00000002);
	TargetInterface.pokeUint32 (sdr_ctrlgrp_portcfg,               0x00000000);
	TargetInterface.pokeUint32 (sdr + 0x5088,                      0x00000000);  // sdr + 0x5088 --> 0xffc25088
	TargetInterface.pokeUint32 (sdr_ctrlgrp_mppriority,            0x3ffd1088);
	TargetInterface.pokeUint32 (sdr_ctrlgrp_mpweight_mpweight_0_4, 0x21084210);
	TargetInterface.pokeUint32 (sdr_ctrlgrp_mpweight_mpweight_1_4, 0x8081ef84);
	TargetInterface.pokeUint32 (sdr_ctrlgrp_mpweight_mpweight_2_4, 0x00000000);
	TargetInterface.pokeUint32 (sdr_ctrlgrp_mpweight_mpweight_3_4, 0x0000f800);
	TargetInterface.pokeUint32 (sdr + 0x50C0,                      0x20820820); // MPPACING_MPPACING_0
	TargetInterface.pokeUint32 (sdr + 0x50C4,                      0x00000000); // MPPACING_MPPACING_1
	TargetInterface.pokeUint32 (sdr + 0x50C8,                      0x41041041); // MPPACING_MPPACING_2
	TargetInterface.pokeUint32 (sdr + 0x50CC,                      0x00410410); // MPPACING_MPPACING_3
	TargetInterface.pokeUint32 (sdr + 0x50D0,                      0x01010101); // MPTHRESHOLDRST_MPTHRESHOLDRST_0
	TargetInterface.pokeUint32 (sdr + 0x50D4,                      0x01010101); // MPTHRESHOLDRST_MPTHRESHOLDRST_1
	TargetInterface.pokeUint32 (sdr + 0x50D8,                      0x00000101); // MPTHRESHOLDRST_MPTHRESHOLDRST_2
	TargetInterface.pokeUint32 (sdr + 0x5150,                      0x00000200); // PHYCTRL_PHYCTRL_0
	TargetInterface.pokeUint32 (sdr + 0x5064,                      0x00044555); // CPORTWIDTH_CPORTWIDTH
	TargetInterface.pokeUint32 (sdr + 0x5068,                      0x2c011000); // CPORTWMAP_CPORTWMAP
	TargetInterface.pokeUint32 (sdr + 0x506C,                      0x00b00088); // CPORTRMAP_CPORTRMAP
	TargetInterface.pokeUint32 (sdr + 0x5070,                      0x00760210); // RFIFOCMAP_RFIFOCMAP
	TargetInterface.pokeUint32 (sdr + 0x5074,                      0x00980543); // WFIFOCMAP_WFIFOCMAP
	TargetInterface.pokeUint32 (sdr + 0x5078,                      0x0005a56a); // CPORTRDWR_CPORTRDWR
	TargetInterface.pokeUint32 (sdr_ctrlgrp_dramodt,               0x00000001);
	TargetInterface.pokeUint32 (sdr_ctrlgrp_staticcfg,             0x0000000a);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// SDRAM calibration
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	TargetInterface.message ("## Start SDRAM calibration");
	// initialize reg file:
	TargetInterface.pokeUint32 (0xffc24800, 0x55550496);
	TargetInterface.pokeUint32 (0xffc24804, 0x00000000);
	TargetInterface.pokeUint32 (0xffc24808, 0x00000000);
	TargetInterface.pokeUint32 (0xffc2480c, 0x00000000);
	TargetInterface.pokeUint32 (0xffc24810, 0x00000000);
	TargetInterface.pokeUint32 (0xffc24814, 0x00000000);
	TargetInterface.pokeUint32 (0xffc24818, 0x00000000);
	// initialize hps phy:
	TargetInterface.pokeUint32 (0xffc25150, 0x01d4c256);
	TargetInterface.pokeUint32 (0xffc25154, 0xa0064000);
	TargetInterface.pokeUint32 (0xffc25158, 0x00000000);
	// scc_mgr_initialize
	var i=0;
	while (i<16)
	{
		TargetInterface.pokeUint32 (0xffc20000 + 0x0b00 + (i<<2), 0); // Clearing SCC RFILE
		i++
	}
	// initialize tracking:
	TargetInterface.pokeUint32 (0xffc2481c, 0x0000000c);
	TargetInterface.pokeUint32 (0xffc24820, 0x00001d4c);
	TargetInterface.pokeUint32 (0xffc24824, 0x000a0064);
	TargetInterface.pokeUint32 (0xffc24828, 0xf30e0a04);
	TargetInterface.pokeUint32 (0xffc2482c, 0x000f7d12);
	TargetInterface.pokeUint32 (0xffc24830, 0x00000004);
	TargetInterface.pokeUint32 (0xffc24834, 0x140003e8);
}
