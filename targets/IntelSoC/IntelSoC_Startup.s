/*****************************************************************************
  Exception handlers and startup code for an CycloneV target.
  
  Copyright (c) 2021 by Markus Klein

  Copyright (c) 2016 by Michael Fischer (www.emb4fun.de).
  Based on the original Zynq_7000 source from Rowley, therefore:

  Copyright (c) 2014 Rowley Associates Limited.

  This file may be distributed under the terms of the License Agreement
  provided with this software.

  THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *****************************************************************************/

/*****************************************************************************
 *                           Preprocessor Definitions
 *                           ------------------------
 *
 *
 * __NO_SYSTEM_INIT
 *
 *   If defined, the SystemInit() function will NOT be called. By default SystemInit()
 *   is called after reset to enable the clocks and memories to be initialised
 *   prior to any C startup initialisation.
 *
 * NO_CACHE_ENABLE
 * 
 *   When specified the MMU is not enabled.
 *
 * NO_ICACHE_ENABLE
 *
 *   If not defined (and NO_CACHE_ENABLE not defined), the I cache is enabled.
 *
 * __NO_RUNFAST_MODE
 *
 *   If defined do NOT turn on flush-to-zero and default NaN modes.
 *
 *
 *****************************************************************************/

  .section .vectors, "ax"
  .code 32
  .align 0
  .global _vectors
  
/*****************************************************************************
 *                                                                           *
 * Exception Vectors                                                         *
 *                                                                           *
 *****************************************************************************/
_vectors:
  ldr pc, [pc, #reset_handler_address - . - 8]  /* reset */
  ldr pc, [pc, #undef_handler_address - . - 8]  /* undefined instruction */
  ldr pc, [pc, #swi_handler_address - . - 8]    /* swi handler */
  ldr pc, [pc, #pabort_handler_address - . - 8] /* abort prefetch */
  ldr pc, [pc, #dabort_handler_address - . - 8] /* abort data */
  nop
  ldr pc, [pc, #irq_handler_address - . - 8]    /* irq */
  ldr pc, [pc, #fiq_handler_address - . - 8]    /* fiq */

reset_handler_address:
  .word reset_handler
undef_handler_address:
  .word undef_handler
swi_handler_address:
  .word swi_handler
pabort_handler_address:
  .word pabort_handler
dabort_handler_address:
  .word dabort_handler
reserved_handler_address:
   .word ReservedHandler
irq_handler_address:
  .word irq_handler
fiq_handler_address:
  .word fiq_handler

#ifndef NO_CACHE_ENABLE
  .section .mmu, "ax"
  .balign 0x4000, 0xff
mmu_page_table:
  .space 0x4000
#endif

/******************************************************************************
 *                                                                            *
 * Default exception handlers                                                 *
 *                                                                            *
 ******************************************************************************/
ReservedHandler:
   b  ReservedHandler


/******************************************************************************
 *                                                                            *
 * Reset Handler                                                              *
 *                                                                            *
 ******************************************************************************/
  .section .init, "ax"
  .code 32
  .align 4
  .global reset_handler
reset_handler:
#ifndef __NO_SYSTEM_INIT
  /* Set the stack pointer for temporary use. The stack will be set later to the correct value. */
  ldr   sp, =__stack_end__
  bl SystemInit
#endif

  /* Set the vector base address */
  ldr r0, =_vectors
  mcr p15, 0, r0, c12, c0, 0

#ifndef NO_CACHE_ENABLE
  /* Set the translation table base address */
  ldr r0, =mmu_page_table
  mcr p15, 0, r0, c2, c0, 0          /* Write to TTB register */

  /* Setup the domain access control so accesses are not checked */
  ldr r0, =0xFFFFFFFF
  mcr p15, 0, r0, c3, c0, 0          /* Write to domain access control register */

  /* Create translation table */
  ldr r0, =mmu_page_table
  bl libarm_mmu_flat_initialise_level_1_table

  /* Make the DDR cacheable */
  ldr r0, =mmu_page_table
  ldr r1, =__DDR_segment_start__ 
  ldr r2, =__DDR_segment_end__
  sub r2, r2, r1
  cmp r2, #0x00100000
  movle r2, #0x00100000
  bl libarm_mmu_flat_set_level_1_cacheable_write_back_region

  /* Enable the MMU and caches */
  mrc p15, 0, r0, c1, c0, 0          /* Read MMU control register */
  orr r0, r0, #0x00001000            /* Enable ICache */
  orr r0, r0, #0x00000007            /* Enable DCache, MMU and alignment fault */
  mcr p15, 0, r0, c1, c0, 0          /* Write MMU control register */
  nop
  nop
#elif !defined(NO_ICACHE_ENABLE)
  mrc p15, 0, r0, c1, c0, 0          /* Read MMU control register */
  orr r0, r0, #0x00001000            /* Enable ICache */ 
  mcr p15, 0, r0, c1, c0, 0          /* Write MMU control register */
  nop
  nop
#endif

#if !defined(__SOFTFP__)
  // enable cp11 and cp10
  mrc p15, #0x00, r0, c1, c0, #0x02  /* Read CP Access register */
  orr r0, r0, #0x00F00000            /* Enable full access to NEON/VFP (Coprocessors 10 and 11) */
  mcr p15, #0x00, r0, c1, c0, #0x02  /* Write CP Access register */
  // enable VFP
  mov r0, #0x40000000                /* Switch on the VFP and NEON hardware */
  fmxr fpexc, r0                     /* Set EN bit in FPEXC */
#ifndef __NO_RUNFAST_MODE
  nop
  nop
  nop  
  vmrs r0, fpscr
  orrs r0, r0, #(0x3 << 24) // FZ and DN
  vmsr fpscr, r0
#endif
#endif

  b _start
  bx lr

/******************************************************************************
 *                                                                            *
 * Default exception handlers                                                 *
 * These are declared weak symbols so they can be redefined in user code.     * 
 *                                                                            *
 ******************************************************************************/

undef_handler:
  b .  /* Endless loop */

swi_handler:
  b .  /* Endless loop */

pabort_handler:
  b .  /* Endless loop */

dabort_handler:
  b .  /* Endless loop */

  .global irq_handler
  .type irq_handler, function
irq_handler:
  stmfd sp!, {r0}                // Save R0 on the Stack
  mrs r0, spsr                   // Get the Saved Program Status Registers
  tst r0, #0x80
  ldmfd sp!, {r0}                // Restore the saved R0 register from the stack
  subnes pc, lr, #4
  stmfd sp!, {r0-r4, r12, lr}    // Push working registers.
#if !defined(__SOFTFP__)
  vpush {d0-d7}                  // Push floating point registers
#endif
  bl IRQ_Handler
#if !defined(__SOFTFP__)
  vpop {d0-d7}                   // Pop floating point registers
#endif
  ldmfd sp!, {r0-r4, r12, lr}    // Pop working registers.
  subs pc, lr, #4                // LR offset to return from this exception: -4
fiq_handler:
  b .  /* Endless loop */

  .weak undef_handler, swi_handler, pabort_handler, dabort_handler, irq_handler, fiq_handler


  .type libarm_mmu_flat_set_level_1_cacheable_write_back_region, function
libarm_mmu_flat_set_level_1_cacheable_write_back_region:
  add r0, r0, r1, lsr #18
  mov r1, r2, lsr #20
1:
  ldr r2, [r0]
  orr r2, r2, #0x0000000C /* set cacheable bit and write back bit */
  str r2, [r0], #4
  subs r1, r1, #1
  bne 1b
  bx lr
  
/*** EOF ***/

