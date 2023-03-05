/*****************************************************************************
 * Copyright (C) 2019-2022 Markus Klein                                      *
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
	var TargetFullName = TargetInterface.getProjectProperty ("Target");
	var TargetShort    = TargetFullName.substring (0, TargetFullName.length-2);
	var TargetCore     = TargetFullName.substring (TargetFullName.length-1);

	TargetInterface.message ("## Connect");
	TargetInterface.message ("## TargetFullName: " + TargetFullName + " - TargetShort: " + TargetShort + " - TargetCore: " + TargetCore);

	TargetInterface.setDeviceTypeProperty (TargetShort);


	if ((TargetInterface.implementation() != "j-link") && (TargetShort == "Arria 10"))
	{
		// TargetInterface.selectDevice(irPre, irPost, drPre, drPost) sets the instruction and data register
		// (number of devices) pre and post bits.

		// "Devices before" refers to the number of JTAG devices that the TDI signal has to pass through in the
		// daisy-chain before reaching the target device. Similarly, "devices after" is the number of devices that
		// the signal has to pass through after the target device before reaching the JTAG-TDO pin.

		// Instruction bits "before" and "after" refers to the total sum of all JTAG devices instruction register
		// lengths, which are connected before and after the target device in the daisy-chain.
		TargetInterface.selectDevice (0, 10, 0, 1);
	}

	TargetInterface.setDebugInterfaceProperty ("set_adiv5_APB_ap_num", 1);

	TargetInterface.setDebugInterfaceProperty ("component_base",  0x80001000); // ETF
	TargetInterface.setDebugInterfaceProperty ("component_base",  0x80002000); // CTI
	TargetInterface.setDebugInterfaceProperty ("component_base",  0x80003000); // TPIU
	TargetInterface.setDebugInterfaceProperty ("component_base",  0x80004000); // CSTF
	TargetInterface.setDebugInterfaceProperty ("component_base",  0x80005000); // STM
	TargetInterface.setDebugInterfaceProperty ("component_base",  0x80006000); // ETR
//	TargetInterface.setDebugInterfaceProperty ("component_base",  0x80007000); // FPGA-CTI
//	TargetInterface.setDebugInterfaceProperty ("component_base",  0x80080000); // FPGA-ROM
//	TargetInterface.setDebugInterfaceProperty ("component_base",  0x80100000); // A9 ROM
	if (TargetCore == "0")
	{
		TargetInterface.setDebugInterfaceProperty ("component_base",  0x80110000); // CPU0 Debug
		TargetInterface.setDebugInterfaceProperty ("component_base",  0x80111000); // CPU0_PMU --> Performance Monitoring Unit
	}
	else
	{
		TargetInterface.setDebugInterfaceProperty ("component_base",  0x80112000); // CPU1 Debug
		TargetInterface.setDebugInterfaceProperty ("component_base",  0x80113000); // CPU1_PMU --> Performance Monitoring Unit
		
	}
	TargetInterface.setDebugInterfaceProperty ("component_base",  0x80118000); // CTI0 --> Cross-Trigger Interface 0
	TargetInterface.setDebugInterfaceProperty ("component_base",  0x80119000); // CTI1 --> Cross-Trigger Interface 1
	TargetInterface.setDebugInterfaceProperty ("component_base",  0x8011C000); // PTM0 --> Program Trace Macrocell 0
	TargetInterface.setDebugInterfaceProperty ("component_base",  0x8011D000); // PTM1 --> Program Trace Macrocell 1

	TargetInterface.message ("## Connect - done");
}

// This function is used to return the controller type as a string
// we use it also to do some initializations as this function is called right before
// writing the code to the controller
function GetPartName ()
{
	TargetInterface.message ("## GetPartName");

	return "";
}

function MatchPartName (name)
{
	var partName = GetPartName ();

	if (partName == "")
		return false;

	return partName.substring (0, 6) == name.substring (0, 6);
}

function MRC(CPnumber, opcode1, CRn, CRm, opcode2)
{
	return 0xEE100010 | (opcode1 << 21) | (CRn << 16) | (CPnumber << 8) | (opcode2 << 5) | CRm;
}

function MCR(CPnumber, opcode1, CRn, CRm, opcode2)
{
	return 0xEE000010 | (opcode1 << 21) | (CRn << 16) | (CPnumber << 8) | (opcode2 << 5) | CRm;
}

function Reset ()
{
	TargetInterface.message ("## Reset");

	var TargetFullName = TargetInterface.getProjectProperty ("Target");
	var TargetShort    = TargetFullName.substring (0, TargetFullName.length-2);
	var TargetCore     = TargetFullName.substring (TargetFullName.length-1);

	TargetInterface.message ("## TargetFullName: " + TargetFullName + " - TargetShort: " + TargetShort + " - TargetCore: " + TargetCore);

	if (TargetCore == "0")
	{
		TargetInterface.setNSRST(0);
		TargetInterface.resetAndStop (100);
		TargetInterface.setNSRST(1); 
		var i = TargetInterface.executeMRC(MRC(15, 0, 1, 0, 0));                  // Read control register
		TargetInterface.executeMCR(MCR(15, 0, 1, 0, 0), i & ~(1<<0|1<<2|1<<12));  // Write control register
		TargetInterface.executeMCR(MCR(15, 0, 7, 5, 0));                          // Invalidate ICache

		EnableSecondCore (TargetShort);
	}
	else
	{
//		TargetInterface.stop(1000);
	}

	TargetInterface.message ("## Reset - done");
}

function EnableSecondCore (TargetShort)
{
	TargetInterface.message ("## Enable second core on " + TargetShort);
	var RSTMGR = 0xFFD05000;
	var RSTMGR_MPUMODRST = RSTMGR;
	if (TargetShort == "Cyclone V")
		RSTMGR_MPUMODRST += 0x0010;
	else if (TargetShort == "Arria 10")
	{
		// Set an endless loop to the start address of the second core
		TargetInterface.pokeUint32 (0, 0xE320F000); // -->    nop
		TargetInterface.pokeUint32 (4, 0xE320F003); // -->    wfi
		TargetInterface.pokeUint32 (8, 0xEAFBFF52); // -->    b 0x00000000
		TargetInterface.pokeUint32 (0xFFE00000, 0xE320F000); // -->    nop
		TargetInterface.pokeUint32 (0xFFE00004, 0xE320F003); // -->    wfi
		TargetInterface.pokeUint32 (0xFFE00008, 0xEAFBFF52); // -->    b 0x00000000
		RSTMGR_MPUMODRST += 0x0020;
	}
	
	AlterRegister (RSTMGR_MPUMODRST, 2, 0);
}

function DisableWatchdogs_CycloneV ()
{
	var MPU    = 0xFFFEC000;
	var WDOG_TIMER = MPU + 0x620;

	// put L4 watchdog modules into system manager reset: 
	AlterRegister (RSTMGR + 0x14, 0, 0xc0);			// RSTMGR_PERMODRST
	// using the system manager bit to reset the ARM watchdog timer resets *both* ARM watchdog timers,
	// which is often not advisable, so we reset the local ARM watchdog timer manually:

	// first, stop the ARM watchdog timer & disable interrupt: 
	AlterRegister (WDOG_TIMER + 0x8, 5, 0);	// WDOG_CTRL
	// reset load and counter register:
	TargetInterface.pokeUint32 (WDOG_TIMER + 0x0, 0);	// WDOG_LOAD
	// clear any pending reset and interrupt status
	TargetInterface.pokeUint32 (WDOG_TIMER + 0x10, 1);	// WDOG_RSTSTAT
	TargetInterface.pokeUint32 (WDOG_TIMER + 0xC, 1);	// WDOG_INTSTAT
	// return ARM watchdog timer to (initial) general-purpose timer mode
	while (TargetInterface.peekUint32 (WDOG_TIMER + 0x8) & 0x00000008) // WDOG_CTRL
	{
		TargetInterface.pokeUint32 (WDOG_TIMER + 0x14, 0x12345678); // WDOG_DISABLE - Set Watchdog disable value 0
		TargetInterface.pokeUint32 (WDOG_TIMER + 0x14, 0x87654321); // WDOG_DISABLE - Set Watchdog disable value 1
	}

	// now write zeros to the control register significant bitfields
	AlterRegister (WDOG_TIMER + 0x8, 0x0000ff0f, 0);	// WDOG_CTRL
}

function DisableWatchdogs_Arria10 ()
{
	var MPU    = 0xFFFFC000;
	var WDOG_TIMER = MPU + 0x620;

	// put L4 watchdog modules into system manager reset: 
	AlterRegister (RSTMGR + 0x28, 0, 3);		// RSTMGR_PERMODRST
	// using the system manager bit to reset the ARM watchdog timer resets *both* ARM watchdog timers,
	// which is often not advisable, so we reset the local ARM watchdog timer manually:

	// first, stop the ARM watchdog timer & disable interrupt: 
	AlterRegister (WDOG_TIMER + 0x8, 5, 0);	// WDOG_CTRL
	// reset load and counter register:
	TargetInterface.pokeUint32 (WDOG_TIMER + 0x0, 0);	// WDOG_LOAD   - 0xffffc620
	// clear any pending reset and interrupt status
	TargetInterface.pokeUint32 (WDOG_TIMER + 0x10, 1);	// WDOG_RSTSTAT
	TargetInterface.pokeUint32 (WDOG_TIMER + 0xC, 1);	// WDOG_INTSTAT
	// return ARM watchdog timer to (initial) general-purpose timer mode
	while (TargetInterface.peekUint32 (WDOG_TIMER + 0x8) & 0x00000008) // WDOG_CTRL
	{
		TargetInterface.pokeUint32 (WDOG_TIMER + 0x14, 0x12345678); // WDOG_DISABLE - Set Watchdog disable value 0
		TargetInterface.pokeUint32 (WDOG_TIMER + 0x14, 0x87654321); // WDOG_DISABLE - Set Watchdog disable value 1
	}

	// now write zeros to the control register significant bitfields
	AlterRegister (WDOG_TIMER + 0x8, 0x0000ff0f, 0);	// WDOG_CTRL
}

function LoadBegin ()
{
	TargetInterface.message ("## call LoadBegin");
	var TargetFullName = TargetInterface.getProjectProperty ("Target");
	var TargetShort    = TargetFullName.substring (0, TargetFullName.length-2);
	var TargetCore     = TargetFullName.substring (TargetFullName.length-1);

	TargetInterface.message ("## TargetFullName: " + TargetFullName + " - TargetShort: " + TargetShort + " - TargetCore: " + TargetCore);
	if (TargetCore == "0")
	{
		InitializeDdrMemory ();
		EnableSecondCore (TargetShort);
	}
	TargetInterface.message ("## call LoadBegin - done");
}

function LoadEnd ()
{
	TargetInterface.message ("## call LoadEnd");
	var TargetFullName = TargetInterface.getProjectProperty ("Target");
	var TargetShort    = TargetFullName.substring (0, TargetFullName.length-2);
	var TargetCore     = TargetFullName.substring (TargetFullName.length-1);
	TargetInterface.message ("## TargetFullName: " + TargetFullName + " - TargetShort: " + TargetShort + " - TargetCore: " + TargetCore);

	if (TargetCore == "0")
	{
		var cpsr = TargetInterface.getRegister ("cpsr");
		if ((cpsr != 0xFFFFFFFF) && ((cpsr & (1 << 5)) == (1 << 5)))
		{
			TargetInterface.message ("## CPU is in Thumb Mode. Switch to ARM Mode");
			cpsr -= (1 << 5);
			TargetInterface.setRegister ("cpsr", cpsr);
		}

//		EnableSecondCore (TargetShort);
	}
	else
	{
		TargetInterface.stop ();
	}

	TargetInterface.message ("## call LoadEnd - done");
}

function InitializeDdrMemory ()
{
	var TargetFullName = TargetInterface.getProjectProperty ("Target");
	var TargetShort    = TargetFullName.substring (0, TargetFullName.length-2);
	var TargetCore     = TargetFullName.substring (TargetFullName.length-1);
	var ret;
	if (TargetInterface.implementation() == "crossworks_simulator")
	{
		return;
	}

	TargetInterface.message ("## load initialization App");
	if (TargetShort == "Cyclone V")
	{
		TargetInterface.pokeBinary (0xFFFF0000, "$(TargetsDir)/IntelSoC/init/bin/Init CycloneV Release/Init.bin");
		TargetInterface.message ("## start initialization App");
		ret = TargetInterface.runFromAddress (0xFFFF0000, 10000);
	}
	else if( TargetShort == "Arria 10")
	{
//		TargetInterface.pokeBinary (0xFFE00000, "Add the Arria-Loader here");
		TargetInterface.message ("## start initialization App");
		ret = TargetInterface.runFromAddress (0xFFE00000, 10000);
	}
	TargetInterface.message ("## initialization done: " + ret);
	TargetInterface.stop ();
}

function AlterRegister (Addr, Clear, Set)
{
	var temp = TargetInterface.peekUint32 (Addr);
	temp &= ~Clear;
	temp |= Set;
	TargetInterface.pokeUint32 (Addr, temp);
}