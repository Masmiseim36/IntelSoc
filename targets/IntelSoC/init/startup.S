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
 * $Id: //depot/embedded/rel/15.0/examples/software/Altera-SoCFPGA-HardwareLib-MPL/core/startup.S#1 $
 */

#include "asm.h" 


	EXPORT(reset_clock_manager)
/* Relocate the sdram_applycfg_ocram function to OCRAM and call it */
LABEL(reset_clock_manager)
	/* Put Main PLL and Peripheral PLL in bypass */
	ldr     r0, SOCFPGA_CLKMGR
	mov     r1, #0x4	/* CLKMGR_BYPASS_ADDRESS */
	mov     r2, #0x19	/* CLKMGR_BYPASS_MAIN_PER_PLL_MASK */
	add     r3, r0, r1
	ldr     r4, [r3]
	orr     r5, r4, r2
	str     r5, [r3]
	dsb
	isb
	mov     r1, #0x54	/*CLKMGR_MAINPLLGRP_MAINQSPICLK_ADDRESS */
	mov     r2, #0x3	/*CLKMGR_MAINQSPICLK_RESET_VALUE */
	add     r3, r0, r1
	str     r2, [r3]
	mov     r1, #0x58	/*CLKMGR_MAINPLLGRP_MAINNANDSDMMCCLK_ADDRESS */
	mov     r2, #0x3	/*CLKMGR_MAINNANDSDMMCCLK_RESET_VALUE */
	add     r3, r0, r1
	str     r2, [r3]
	mov     r1, #0x90	/*CLKMGR_PERPLLGRP_PERQSPICLK_ADDRESS */
	mov     r2, #0x1	/*CLKMGR_PERQSPICLK_RESET_VALUE */
	add     r3, r0, r1
	str     r2, [r3]
	mov     r1, #0x94	/*CLKMGR_PERPLLGRP_PERNANDSDMMCCLK_ADDRESS */
	mov     r2, #0x1	/*CLKMGR_PERNANDSDMMCCLK_RESET_VALUE */
	add     r3, r0, r1
	str     r2, [r3]

	/* Disable the RAM boot */
	ldr     r0, SOCFPGA_RSTMGR
	ldr     r1, SYSMGR_WARMRAMGRP_ENABLE
	mov     r2, #0
	str     r2, [r1]

	/* Trigger warm reset to continue boot normally */
	mov     r1, #0x00000004	/*RSTMGR_CTRL_OFFSET */
	add     r2, r0, r1
	mov     r3, #1
	mov     r3, r3, LSL #1	/*RSTMGR_CTRL_SWWARMRSTREQ_LSB */
	str     r3, [r2]

LABEL(reset_clock_manager_loop)
	dsb
	isb
	b	reset_clock_manager_loop

LABEL(SOCFPGA_CLKMGR)
	DCD 	0xffd04000	/* SOCFPGA_CLKMGR_ADDRESS */
LABEL(SOCFPGA_RSTMGR)
	DCD	0xffd05000	/* SOCFPGA_RSTMGR_ADDRESS */
LABEL(SYSMGR_WARMRAMGRP_ENABLE)
	DCD	0xffd080e0	/* CONFIG_SYSMGR_WARMRAMGRP_ENABLE =>  (SOCFPGA_SYSMGR_ADDRESS + 0xe0) */

	EXPORT(reset_clock_manager_size)
LABEL(reset_clock_manager_size)
	DCD	(reset_clock_manager_size-reset_clock_manager)

/*=====  mark end of file*/
    END
