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

	TargetInterface.message ("## TargetFullName: " + TargetFullName + " - TargetShort: " + TargetShort + " - TargetCore: " + TargetCore);
	
	TargetInterface.setDeviceTypeProperty (TargetShort);



//	if (TargetInterface.implementation() != "j-link")
	{
//		TargetInterface.setDebugInterfaceProperty ("set_adiv5_AHB_ap_num", 0);
//		TargetInterface.setDebugInterfaceProperty ("use_adiv5_AHB", 0, 0x00100000, 0x10000000); // DDR RAM
//		TargetInterface.setDebugInterfaceProperty ("set_adiv5_AHB_ap_num", -1);
		TargetInterface.setDebugInterfaceProperty ("set_adiv5_APB_ap_num", 1);
//		TargetInterface.setDebugInterfaceProperty ("reset_debug_interface_enabled", false);

		TargetInterface.setDebugInterfaceProperty ("component_base",  0x80001000); // ETF
		TargetInterface.setDebugInterfaceProperty ("component_base",  0x80002000); // CTI
		TargetInterface.setDebugInterfaceProperty ("component_base",  0x80003000); // TPIU
		TargetInterface.setDebugInterfaceProperty ("component_base",  0x80004000); // CSTF
		TargetInterface.setDebugInterfaceProperty ("component_base",  0x80005000); // STM
		TargetInterface.setDebugInterfaceProperty ("component_base",  0x80006000); // ETR
		if (TargetCore == "0")
		{
			TargetInterface.setDebugInterfaceProperty ("component_base",  0x80110000); // CPU0
			TargetInterface.setDebugInterfaceProperty ("component_base",  0x80111000); // CPU0_PMU
		}
		else
		{
			TargetInterface.setDebugInterfaceProperty ("component_base",  0x80112000); // CPU1
			TargetInterface.setDebugInterfaceProperty ("component_base",  0x80113000); // CPU1_PMU
		}
		TargetInterface.setDebugInterfaceProperty ("component_base",  0x80118000); // CTI0
		TargetInterface.setDebugInterfaceProperty ("component_base",  0x80119000); // CTI1
		TargetInterface.setDebugInterfaceProperty ("component_base",  0x8011C000); // PTM0
		TargetInterface.setDebugInterfaceProperty ("component_base",  0x8011D000); // PTM1
	}
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
		TargetInterface.resetAndStop (100);
		var i = TargetInterface.executeMRC(MRC(15, 0, 1, 0, 0)); // Read control register
		TargetInterface.executeMCR(MCR(15, 0, 1, 0, 0), i & ~(1<<0|1<<2|1<<12)); // Write control register
		TargetInterface.executeMCR(MCR(15, 0, 7, 5, 0)); // Invalidate ICache
	}
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
		var RSTMGR = 0xFFD05000;
		var RSTMGR_MPUMODRST = RSTMGR + 0x0010;
		AlterRegister (RSTMGR_MPUMODRST, 2, 0); // Enable second core

		InitializeDdrMemory ();
	}
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

		var RSTMGR = 0xFFD05000;
		var RSTMGR_MPUMODRST = RSTMGR + 0x0010;
//		AlterRegister (RSTMGR_MPUMODRST, 2, 0); // Enable second core
	}
	else
	{
		TargetInterface.stop ();
	}
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

function AlterRegister (Addr, Clear, Set)
{
	var temp = TargetInterface.peekUint32 (Addr);
	temp &= ~Clear;
	temp |= Set;
	TargetInterface.pokeUint32 (Addr, temp);
}