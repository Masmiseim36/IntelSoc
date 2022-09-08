// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2016-2017 Intel Corporation
 */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include "socal/socal.h"
#include "alt_globaltmr.h"
#include "alt_clock_manager.h"
#include "alt_watchdog.h"
#include "alt_generalpurpose_io.h"
#include "alt_qspi.h"
#include "clock_manager_arria10.h"

typedef uint32_t u32;

static void writel(u32 src, u32 addr)
{
  alt_write_word(addr, src);
};

static uint32_t readl(u32 addr)
{
  return  alt_read_word(addr); 
}

static void clrbits_le32 (u32 addr, u32 clear)
{
  alt_write_word(addr, (alt_read_word(addr) & ~clear));
}

static void setbits_le32 (u32 addr, u32 set)
{
alt_write_word(addr, (alt_read_word(addr) | set));
}
/*!
 * Delay function
 *
 * \param      us delay interval
 */
ALT_STATUS_CODE delay_us(uint32_t us)
{
    ALT_STATUS_CODE status = ALT_E_SUCCESS;
    
    uint64_t start_time = alt_globaltmr_get64();
    uint32_t timer_prescaler = alt_globaltmr_prescaler_get() + 1;
    uint64_t end_time;
    alt_freq_t timer_clock;

    status = alt_clk_freq_get(ALT_CLK_MPU_PERIPH, &timer_clock);
    end_time = start_time + us * ((timer_clock / timer_prescaler) / 1000000);

    while(alt_globaltmr_get64() < end_time)
    {
      ;
    }
    
    return status;
}

static uint32_t socfpga_get_clkmgr_addr(void)
{
	return 0xffd04000;
}
static uint32_t socfpga_get_rstmgr_addr(void)
{
	return 0xffd05000;
}

static uint32_t socfpga_get_sysmgr_addr(void)
{
	return 0xffd06000;
}

static void udelay(uint32_t t){
    for(uint32_t i = 0; i < t; i++)
    {
      for(uint32_t m=0; m<0x7F; m++);   
    }
 }

static void mdelay(uint32_t t){
  udelay(t*1000);
}

/* Disable the watchdog (toggle reset to watchdog) */
static void socfpga_watchdog_disable(void)
{
	/* assert reset for watchdog */
	setbits_le32(socfpga_get_rstmgr_addr() + RSTMGR_A10_PER1MODRST,
		     ALT_RSTMGR_PER1MODRST_WD0_SET_MSK);

	setbits_le32(socfpga_get_rstmgr_addr() + RSTMGR_A10_PER1MODRST,
		     ALT_RSTMGR_PER1MODRST_WD1_SET_MSK);
}

 
static void cm_wait_for_lock(u32 mask)
{
	u32 inter_val;
	u32 retry = 0;
	do {
		inter_val = readl(socfpga_get_clkmgr_addr() +
				  CLKMGR_STAT) & mask;
          	/* Wait for stable lock */
		if (inter_val == mask)
			retry++;
		else
			retry = 0;
		if (retry >= 10)
			break;
	} while (1);
}

static int cm_wait_for_fsm(void)
{
	//return wait_for_bit_le32((const void *)(socfpga_get_clkmgr_addr() + CLKMGR_STAT), CLKMGR_STAT_BUSY, false, 20000, false);

          while((alt_read_word(socfpga_get_clkmgr_addr() + CLKMGR_STAT) & CLKMGR_STAT_BUSY) == CLKMGR_STAT_BUSY);
          
          return 0;
}

static u32 eosc1_hz = EOSC1_CLK_HZ;
static u32 cb_intosc_hz = CB_INTOSC_LS_CLK_HZ;
static u32 f2s_free_hz = F2H_FREE_CLK_HZ;

struct mainpll_cfg {
	u32 vco0_psrc;
	u32 vco1_denom;
	u32 vco1_numer;
	u32 mpuclk;
	u32 mpuclk_cnt;
	u32 mpuclk_src;
	u32 nocclk;
	u32 nocclk_cnt;
	u32 nocclk_src;
	u32 cntr2clk_cnt;
	u32 cntr3clk_cnt;
	u32 cntr4clk_cnt;
	u32 cntr5clk_cnt;
	u32 cntr6clk_cnt;
	u32 cntr7clk_cnt;
	u32 cntr7clk_src;
	u32 cntr8clk_cnt;
	u32 cntr9clk_cnt;
	u32 cntr9clk_src;
	u32 cntr15clk_cnt;
	u32 nocdiv_l4mainclk;
	u32 nocdiv_l4mpclk;
	u32 nocdiv_l4spclk;
	u32 nocdiv_csatclk;
	u32 nocdiv_cstraceclk;
	u32 nocdiv_cspdbclk;
};

struct perpll_cfg {
	u32 vco0_psrc;
	u32 vco1_denom;
	u32 vco1_numer;
	u32 cntr2clk_cnt;
	u32 cntr2clk_src;
	u32 cntr3clk_cnt;
	u32 cntr3clk_src;
	u32 cntr4clk_cnt;
	u32 cntr4clk_src;
	u32 cntr5clk_cnt;
	u32 cntr5clk_src;
	u32 cntr6clk_cnt;
	u32 cntr6clk_src;
	u32 cntr7clk_cnt;
	u32 cntr8clk_cnt;
	u32 cntr8clk_src;
	u32 cntr9clk_cnt;
	u32 cntr9clk_src;
	u32 emacctl_emac0sel;
	u32 emacctl_emac1sel;
	u32 emacctl_emac2sel;
	u32 gpiodiv_gpiodbclk;
};


/*
 * OCRAM firewall
 */
struct socfpga_noc_fw_ocram {
	u32 enable;
	u32 enable_set;
	u32 enable_clear;
	u32 region0;
	u32 region1;
	u32 region2;
	u32 region3;
	u32 region4;
	u32 region5;
};


/* calculate the intended main VCO frequency based on handoff */
static unsigned int cm_calc_handoff_main_vco_clk_hz
					(struct mainpll_cfg *main_cfg)
{
	unsigned int clk_hz;

	/* Check main VCO clock source: eosc, intosc or f2s? */
	switch (main_cfg->vco0_psrc) {
	case CLKMGR_MAINPLL_VCO0_PSRC_EOSC:
		clk_hz = eosc1_hz;
		break;
	case CLKMGR_MAINPLL_VCO0_PSRC_E_INTOSC:
		clk_hz = cb_intosc_hz;
		break;
	case CLKMGR_MAINPLL_VCO0_PSRC_F2S:
		clk_hz = f2s_free_hz;
		break;
	default:
		return 0;
	}

	/* calculate the VCO frequency */
	clk_hz /= 1 + main_cfg->vco1_denom;
	clk_hz *= 1 + main_cfg->vco1_numer;

	return clk_hz;
}

/* calculate the intended periph VCO frequency based on handoff */
static unsigned int cm_calc_handoff_periph_vco_clk_hz(
		struct mainpll_cfg *main_cfg, struct perpll_cfg *per_cfg)
{
	unsigned int clk_hz;

	/* Check periph VCO clock source: eosc, intosc, f2s or mainpll? */
	switch (per_cfg->vco0_psrc) {
	case CLKMGR_PERPLL_VCO0_PSRC_EOSC:
		clk_hz = eosc1_hz;
		break;
	case CLKMGR_PERPLL_VCO0_PSRC_E_INTOSC:
		clk_hz = cb_intosc_hz;
		break;
	case CLKMGR_PERPLL_VCO0_PSRC_F2S:
		clk_hz = f2s_free_hz;
		break;
	case CLKMGR_PERPLL_VCO0_PSRC_MAIN:
		clk_hz = cm_calc_handoff_main_vco_clk_hz(main_cfg);
		clk_hz /= main_cfg->cntr15clk_cnt;
		break;
	default:
		return 0;
	}

	/* calculate the VCO frequency */
	clk_hz /= 1 + per_cfg->vco1_denom;
	clk_hz *= 1 + per_cfg->vco1_numer;

	return clk_hz;
}

/* calculate the intended MPU clock frequency based on handoff */
static unsigned int cm_calc_handoff_mpu_clk_hz(struct mainpll_cfg *main_cfg,
					       struct perpll_cfg *per_cfg)
{
	unsigned int clk_hz;

	/* Check MPU clock source: main, periph, osc1, intosc or f2s? */
	switch (main_cfg->mpuclk_src) {
	case CLKMGR_MAINPLL_MPUCLK_SRC_MAIN:
		clk_hz = cm_calc_handoff_main_vco_clk_hz(main_cfg);
		clk_hz /= (main_cfg->mpuclk & CLKMGR_MAINPLL_MPUCLK_CNT_MSK)
			   + 1;
		break;
	case CLKMGR_MAINPLL_MPUCLK_SRC_PERI:
		clk_hz = cm_calc_handoff_periph_vco_clk_hz(main_cfg, per_cfg);
		clk_hz /= ((main_cfg->mpuclk >>
			   CLKMGR_MAINPLL_MPUCLK_PERICNT_LSB) &
			   CLKMGR_MAINPLL_MPUCLK_CNT_MSK) + 1;
		break;
	case CLKMGR_MAINPLL_MPUCLK_SRC_OSC1:
		clk_hz = eosc1_hz;
		break;
	case CLKMGR_MAINPLL_MPUCLK_SRC_INTOSC:
		clk_hz = cb_intosc_hz;
		break;
	case CLKMGR_MAINPLL_MPUCLK_SRC_FPGA:
		clk_hz = f2s_free_hz;
		break;
	default:
		return 0;
	}

	clk_hz /= main_cfg->mpuclk_cnt + 1;
	return clk_hz;
}

/* calculate the intended NOC clock frequency based on handoff */
static unsigned int cm_calc_handoff_noc_clk_hz(struct mainpll_cfg *main_cfg,
					       struct perpll_cfg *per_cfg)
{
	unsigned int clk_hz;

	/* Check MPU clock source: main, periph, osc1, intosc or f2s? */
	switch (main_cfg->nocclk_src) {
	case CLKMGR_MAINPLL_NOCCLK_SRC_MAIN:
		clk_hz = cm_calc_handoff_main_vco_clk_hz(main_cfg);
		clk_hz /= (main_cfg->nocclk & CLKMGR_MAINPLL_NOCCLK_CNT_MSK)
			 + 1;
		break;
	case CLKMGR_MAINPLL_NOCCLK_SRC_PERI:
		clk_hz = cm_calc_handoff_periph_vco_clk_hz(main_cfg, per_cfg);
		clk_hz /= ((main_cfg->nocclk >>
			   CLKMGR_MAINPLL_NOCCLK_PERICNT_LSB) &
			   CLKMGR_MAINPLL_NOCCLK_CNT_MSK) + 1;
		break;
	case CLKMGR_MAINPLL_NOCCLK_SRC_OSC1:
		clk_hz = eosc1_hz;
		break;
	case CLKMGR_MAINPLL_NOCCLK_SRC_INTOSC:
		clk_hz = cb_intosc_hz;
		break;
	case CLKMGR_MAINPLL_NOCCLK_SRC_FPGA:
		clk_hz = f2s_free_hz;
		break;
	default:
		return 0;
	}

	clk_hz /= main_cfg->nocclk_cnt + 1;
	return clk_hz;
}

/* return 1 if PLL ramp is required */
static int cm_is_pll_ramp_required(int main0periph1,
				   struct mainpll_cfg *main_cfg,
				   struct perpll_cfg *per_cfg)
{
	/* Check for main PLL */
	if (main0periph1 == 0) {
		/*
		 * PLL ramp is not required if both MPU clock and NOC clock are
		 * not sourced from main PLL
		 */
		if (main_cfg->mpuclk_src != CLKMGR_MAINPLL_MPUCLK_SRC_MAIN &&
		    main_cfg->nocclk_src != CLKMGR_MAINPLL_NOCCLK_SRC_MAIN)
			return 0;

		/*
		 * PLL ramp is required if MPU clock is sourced from main PLL
		 * and MPU clock is over 900MHz (as advised by HW team)
		 */
		if (main_cfg->mpuclk_src == CLKMGR_MAINPLL_MPUCLK_SRC_MAIN &&
		    (cm_calc_handoff_mpu_clk_hz(main_cfg, per_cfg) >
		     CLKMGR_PLL_RAMP_MPUCLK_THRESHOLD_HZ))
			return 1;

		/*
		 * PLL ramp is required if NOC clock is sourced from main PLL
		 * and NOC clock is over 300MHz (as advised by HW team)
		 */
		if (main_cfg->nocclk_src == CLKMGR_MAINPLL_NOCCLK_SRC_MAIN &&
		    (cm_calc_handoff_noc_clk_hz(main_cfg, per_cfg) >
		     CLKMGR_PLL_RAMP_NOCCLK_THRESHOLD_HZ))
			return 2;

	} else if (main0periph1 == 1) {
		/*
		 * PLL ramp is not required if both MPU clock and NOC clock are
		 * not sourced from periph PLL
		 */
		if (main_cfg->mpuclk_src != CLKMGR_MAINPLL_MPUCLK_SRC_PERI &&
		    main_cfg->nocclk_src != CLKMGR_MAINPLL_NOCCLK_SRC_PERI)
			return 0;

		/*
		 * PLL ramp is required if MPU clock are source from periph PLL
		 * and MPU clock is over 900MHz (as advised by HW team)
		 */
		if (main_cfg->mpuclk_src == CLKMGR_MAINPLL_MPUCLK_SRC_PERI &&
		    (cm_calc_handoff_mpu_clk_hz(main_cfg, per_cfg) >
		     CLKMGR_PLL_RAMP_MPUCLK_THRESHOLD_HZ))
			return 1;

		/*
		 * PLL ramp is required if NOC clock are source from periph PLL
		 * and NOC clock is over 300MHz (as advised by HW team)
		 */
		if (main_cfg->nocclk_src == CLKMGR_MAINPLL_NOCCLK_SRC_PERI &&
		    (cm_calc_handoff_noc_clk_hz(main_cfg, per_cfg) >
		     CLKMGR_PLL_RAMP_NOCCLK_THRESHOLD_HZ))
			return 2;
	}

	return 0;
}

static u32 cm_calculate_numer(struct mainpll_cfg *main_cfg,
			      struct perpll_cfg *per_cfg,
			      u32 safe_hz, u32 clk_hz)
{
	u32 cnt;
	u32 clk;
	u32 shift;
	u32 mask;
	u32 denom;

	if (main_cfg->mpuclk_src == CLKMGR_MAINPLL_MPUCLK_SRC_MAIN) {
		cnt = main_cfg->mpuclk_cnt;
		clk = main_cfg->mpuclk;
		shift = 0;
		mask = CLKMGR_MAINPLL_MPUCLK_CNT_MSK;
		denom = main_cfg->vco1_denom;
	} else if (main_cfg->nocclk_src == CLKMGR_MAINPLL_NOCCLK_SRC_MAIN) {
		cnt = main_cfg->nocclk_cnt;
		clk = main_cfg->nocclk;
		shift = 0;
		mask = CLKMGR_MAINPLL_NOCCLK_CNT_MSK;
		denom = main_cfg->vco1_denom;
	} else if (main_cfg->mpuclk_src == CLKMGR_MAINPLL_MPUCLK_SRC_PERI) {
		cnt = main_cfg->mpuclk_cnt;
		clk = main_cfg->mpuclk;
		shift = CLKMGR_MAINPLL_MPUCLK_PERICNT_LSB;
		mask = CLKMGR_MAINPLL_MPUCLK_CNT_MSK;
		denom = per_cfg->vco1_denom;
	} else if (main_cfg->nocclk_src == CLKMGR_MAINPLL_NOCCLK_SRC_PERI) {
		cnt = main_cfg->nocclk_cnt;
		clk = main_cfg->nocclk;
		shift = CLKMGR_MAINPLL_NOCCLK_PERICNT_LSB;
		mask = CLKMGR_MAINPLL_NOCCLK_CNT_MSK;
		denom = per_cfg->vco1_denom;
	} else {
		return 0;
	}

	return (safe_hz / clk_hz) * (cnt + 1) * (((clk >> shift) & mask) + 1) *
		(1 + denom) - 1;
}

/*
 * Calculate the new PLL numerator which is based on existing DTS hand off and
 * intended safe frequency (safe_hz). Note that PLL ramp is only modifying the
 * numerator while maintaining denominator as denominator will influence the
 * jitter condition. Please refer A10 HPS TRM for the jitter guide. Note final
 * value for numerator is minus with 1 to cater our register value
 * representation.
 */
static unsigned int cm_calc_safe_pll_numer(int main0periph1,
					   struct mainpll_cfg *main_cfg,
					   struct perpll_cfg *per_cfg,
					   unsigned int safe_hz)
{
	unsigned int clk_hz = 0;

	/* Check for main PLL */
	if (main0periph1 == 0) {
		/* Check main VCO clock source: eosc, intosc or f2s? */
		switch (main_cfg->vco0_psrc) {
		case CLKMGR_MAINPLL_VCO0_PSRC_EOSC:
			clk_hz = eosc1_hz;
			break;
		case CLKMGR_MAINPLL_VCO0_PSRC_E_INTOSC:
			clk_hz = cb_intosc_hz;
			break;
		case CLKMGR_MAINPLL_VCO0_PSRC_F2S:
			clk_hz = f2s_free_hz;
			break;
		default:
			return 0;
		}
	} else if (main0periph1 == 1) {
		/* Check periph VCO clock source: eosc, intosc, f2s, mainpll */
		switch (per_cfg->vco0_psrc) {
		case CLKMGR_PERPLL_VCO0_PSRC_EOSC:
			clk_hz = eosc1_hz;
			break;
		case CLKMGR_PERPLL_VCO0_PSRC_E_INTOSC:
			clk_hz = cb_intosc_hz;
			break;
		case CLKMGR_PERPLL_VCO0_PSRC_F2S:
			clk_hz = f2s_free_hz;
			break;
		case CLKMGR_PERPLL_VCO0_PSRC_MAIN:
			clk_hz = cm_calc_handoff_main_vco_clk_hz(main_cfg);
			clk_hz /= main_cfg->cntr15clk_cnt;
			break;
		default:
			return 0;
		}
	} else {
		return 0;
	}

	return cm_calculate_numer(main_cfg, per_cfg, safe_hz, clk_hz);
}

/* ramping the main PLL to final value */
static void cm_pll_ramp_main(struct mainpll_cfg *main_cfg,
			     struct perpll_cfg *per_cfg,
			     unsigned int pll_ramp_main_hz)
{
	unsigned int clk_hz = 0, clk_incr_hz = 0, clk_final_hz = 0;

	/* find out the increment value */
	if (main_cfg->mpuclk_src == CLKMGR_MAINPLL_MPUCLK_SRC_MAIN) {
		clk_incr_hz = CLKMGR_PLL_RAMP_MPUCLK_INCREMENT_HZ;
		clk_final_hz = cm_calc_handoff_mpu_clk_hz(main_cfg, per_cfg);
	} else if (main_cfg->nocclk_src == CLKMGR_MAINPLL_NOCCLK_SRC_MAIN) {
		clk_incr_hz = CLKMGR_PLL_RAMP_NOCCLK_INCREMENT_HZ;
		clk_final_hz = cm_calc_handoff_noc_clk_hz(main_cfg, per_cfg);
	}

	/* execute the ramping here */
	for (clk_hz = pll_ramp_main_hz + clk_incr_hz;
	     clk_hz < clk_final_hz; clk_hz += clk_incr_hz) {
		writel((main_cfg->vco1_denom <<
			CLKMGR_MAINPLL_VCO1_DENOM_LSB) |
			cm_calc_safe_pll_numer(0, main_cfg, per_cfg, clk_hz),
			socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_VCO1);
		mdelay(1);
		cm_wait_for_lock(LOCKED_MASK);
	}
	writel((main_cfg->vco1_denom << CLKMGR_MAINPLL_VCO1_DENOM_LSB) |
		main_cfg->vco1_numer,
		socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_VCO1);
	mdelay(1);
	cm_wait_for_lock(LOCKED_MASK);
}

/* ramping the periph PLL to final value */
static void cm_pll_ramp_periph(struct mainpll_cfg *main_cfg,
			       struct perpll_cfg *per_cfg,
			       unsigned int pll_ramp_periph_hz)
{
	unsigned int clk_hz = 0, clk_incr_hz = 0, clk_final_hz = 0;

	/* find out the increment value */
	if (main_cfg->mpuclk_src == CLKMGR_MAINPLL_MPUCLK_SRC_PERI) {
		clk_incr_hz = CLKMGR_PLL_RAMP_MPUCLK_INCREMENT_HZ;
		clk_final_hz = cm_calc_handoff_mpu_clk_hz(main_cfg, per_cfg);
	} else if (main_cfg->nocclk_src == CLKMGR_MAINPLL_NOCCLK_SRC_PERI) {
		clk_incr_hz = CLKMGR_PLL_RAMP_NOCCLK_INCREMENT_HZ;
		clk_final_hz = cm_calc_handoff_noc_clk_hz(main_cfg, per_cfg);
	}
	/* execute the ramping here */
	for (clk_hz = pll_ramp_periph_hz + clk_incr_hz;
	     clk_hz < clk_final_hz; clk_hz += clk_incr_hz) {
		writel((per_cfg->vco1_denom <<
			      CLKMGR_PERPLL_VCO1_DENOM_LSB) |
			      cm_calc_safe_pll_numer(1, main_cfg, per_cfg,
						     clk_hz),
			      socfpga_get_clkmgr_addr() +
			      CLKMGR_A10_PERPLL_VCO1);
		mdelay(1);
		cm_wait_for_lock(LOCKED_MASK);
	}
	writel((per_cfg->vco1_denom << CLKMGR_PERPLL_VCO1_DENOM_LSB) |
		      per_cfg->vco1_numer,
		      socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_VCO1);
	mdelay(1);
	cm_wait_for_lock(LOCKED_MASK);
}

/*
 * Setup clocks while making no assumptions of the
 * previous state of the clocks.
 *
 * Start by being paranoid and gate all sw managed clocks
 *
 * Put all plls in bypass
 *
 * Put all plls VCO registers back to reset value (bgpwr dwn).
 *
 * Put peripheral and main pll src to reset value to avoid glitch.
 *
 * Delay 5 us.
 *
 * Deassert bg pwr dn and set numerator and denominator
 *
 * Start 7 us timer.
 *
 * set internal dividers
 *
 * Wait for 7 us timer.
 *
 * Enable plls
 *
 * Set external dividers while plls are locking
 *
 * Wait for pll lock
 *
 * Assert/deassert outreset all.
 *
 * Take all pll's out of bypass
 *
 * Clear safe mode
 *
 * set source main and peripheral clocks
 *
 * Ungate clocks
 */

static int cm_full_cfg(struct mainpll_cfg *main_cfg, struct perpll_cfg *per_cfg)
{
	unsigned int pll_ramp_main_hz = 0, pll_ramp_periph_hz = 0,
		ramp_required;

	/* gate off all mainpll clock excpet HW managed clock */
	writel(CLKMGR_MAINPLL_EN_S2FUSER0CLKEN_SET_MSK |
		CLKMGR_MAINPLL_EN_HMCPLLREFCLKEN_SET_MSK,
		socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_ENR);

	/* now we can gate off the rest of the peripheral clocks */
	writel(0, socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_EN);

	/* Put all plls in external bypass */
	writel(CLKMGR_MAINPLL_BYPASS_RESET,
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_BYPASSS);
	writel(CLKMGR_PERPLL_BYPASS_RESET,
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_BYPASSS);

	/*
	 * Put all plls VCO registers back to reset value.
	 * Some code might have messed with them. At same time set the
	 * desired clock source
	 */
	writel(CLKMGR_MAINPLL_VCO0_RESET |
	       CLKMGR_MAINPLL_VCO0_REGEXTSEL_SET_MSK |
	       (main_cfg->vco0_psrc << CLKMGR_MAINPLL_VCO0_PSRC_LSB),
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_VCO0);

	writel(CLKMGR_PERPLL_VCO0_RESET |
	       CLKMGR_PERPLL_VCO0_REGEXTSEL_SET_MSK |
	       (per_cfg->vco0_psrc << CLKMGR_PERPLL_VCO0_PSRC_LSB),
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_VCO0);

	writel(CLKMGR_MAINPLL_VCO1_RESET,
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_VCO1);
	writel(CLKMGR_PERPLL_VCO1_RESET,
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_VCO1);

	/* clear the interrupt register status register */
	writel(CLKMGR_CLKMGR_INTR_MAINPLLLOST_SET_MSK |
		CLKMGR_CLKMGR_INTR_PERPLLLOST_SET_MSK |
		CLKMGR_CLKMGR_INTR_MAINPLLRFSLIP_SET_MSK |
		CLKMGR_CLKMGR_INTR_PERPLLRFSLIP_SET_MSK |
		CLKMGR_CLKMGR_INTR_MAINPLLFBSLIP_SET_MSK |
		CLKMGR_CLKMGR_INTR_PERPLLFBSLIP_SET_MSK |
		CLKMGR_CLKMGR_INTR_MAINPLLACHIEVED_SET_MSK |
		CLKMGR_CLKMGR_INTR_PERPLLACHIEVED_SET_MSK,
		socfpga_get_clkmgr_addr() + CLKMGR_A10_INTR);

	/* Program VCO Numerator and Denominator for main PLL */
	ramp_required = cm_is_pll_ramp_required(0, main_cfg, per_cfg);
	if (ramp_required) {
		/* set main PLL to safe starting threshold frequency */
		if (ramp_required == 1)
			pll_ramp_main_hz = CLKMGR_PLL_RAMP_MPUCLK_THRESHOLD_HZ;
		else if (ramp_required == 2)
			pll_ramp_main_hz = CLKMGR_PLL_RAMP_NOCCLK_THRESHOLD_HZ;

		writel((main_cfg->vco1_denom <<
			CLKMGR_MAINPLL_VCO1_DENOM_LSB) |
			cm_calc_safe_pll_numer(0, main_cfg, per_cfg,
					       pll_ramp_main_hz),
			socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_VCO1);
	} else
		writel((main_cfg->vco1_denom <<
		       CLKMGR_MAINPLL_VCO1_DENOM_LSB) |
		       main_cfg->vco1_numer,
		       socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_VCO1);

	/* Program VCO Numerator and Denominator for periph PLL */
	ramp_required = cm_is_pll_ramp_required(1, main_cfg, per_cfg);
	if (ramp_required) {
		/* set periph PLL to safe starting threshold frequency */
		if (ramp_required == 1)
			pll_ramp_periph_hz =
				CLKMGR_PLL_RAMP_MPUCLK_THRESHOLD_HZ;
		else if (ramp_required == 2)
			pll_ramp_periph_hz =
				CLKMGR_PLL_RAMP_NOCCLK_THRESHOLD_HZ;

		writel((per_cfg->vco1_denom <<
			CLKMGR_PERPLL_VCO1_DENOM_LSB) |
			cm_calc_safe_pll_numer(1, main_cfg, per_cfg,
					       pll_ramp_periph_hz),
			socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_VCO1);
	} else
		writel((per_cfg->vco1_denom <<
			CLKMGR_PERPLL_VCO1_DENOM_LSB) |
			per_cfg->vco1_numer,
			socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_VCO1);

	/* Wait for at least 5 us */
	udelay(5);

	/* Now deassert BGPWRDN and PWRDN */
	clrbits_le32(socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_VCO0,
		     CLKMGR_MAINPLL_VCO0_BGPWRDN_SET_MSK |
		     CLKMGR_MAINPLL_VCO0_PWRDN_SET_MSK);
	clrbits_le32(socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_VCO0,
		     CLKMGR_PERPLL_VCO0_BGPWRDN_SET_MSK |
		     CLKMGR_PERPLL_VCO0_PWRDN_SET_MSK);

	/* Wait for at least 7 us */
	udelay(7);

	/* enable the VCO and disable the external regulator to PLL */
	writel((readl(socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_VCO0) &
		~CLKMGR_MAINPLL_VCO0_REGEXTSEL_SET_MSK) |
		CLKMGR_MAINPLL_VCO0_EN_SET_MSK,
		socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_VCO0);
	writel((readl(socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_VCO0) &
		~CLKMGR_PERPLL_VCO0_REGEXTSEL_SET_MSK) |
		CLKMGR_PERPLL_VCO0_EN_SET_MSK,
		socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_VCO0);

	/* setup all the main PLL counter and clock source */
	writel(main_cfg->nocclk,
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_ALTR_NOCCLK);
	writel(main_cfg->mpuclk,
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_ALTR_MPUCLK);

	/* main_emaca_clk divider */
	writel(main_cfg->cntr2clk_cnt,
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_CNTR2CLK);
	/* main_emacb_clk divider */
	writel(main_cfg->cntr3clk_cnt,
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_CNTR3CLK);
	/* main_emac_ptp_clk divider */
	writel(main_cfg->cntr4clk_cnt,
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_CNTR4CLK);
	/* main_gpio_db_clk divider */
	writel(main_cfg->cntr5clk_cnt,
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_CNTR5CLK);
	/* main_sdmmc_clk divider */
	writel(main_cfg->cntr6clk_cnt,
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_CNTR6CLK);
	/* main_s2f_user0_clk divider */
	writel(main_cfg->cntr7clk_cnt |
	       (main_cfg->cntr7clk_src << CLKMGR_MAINPLL_CNTR7CLK_SRC_LSB),
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_CNTR7CLK);
	/* main_s2f_user1_clk divider */
	writel(main_cfg->cntr8clk_cnt,
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_CNTR8CLK);
	/* main_hmc_pll_clk divider */
	writel(main_cfg->cntr9clk_cnt |
	       (main_cfg->cntr9clk_src << CLKMGR_MAINPLL_CNTR9CLK_SRC_LSB),
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_CNTR9CLK);
	/* main_periph_ref_clk divider */
	writel(main_cfg->cntr15clk_cnt,
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_CNTR15CLK);

	/* setup all the peripheral PLL counter and clock source */
	/* peri_emaca_clk divider */
	writel(per_cfg->cntr2clk_cnt |
	       (per_cfg->cntr2clk_src << CLKMGR_PERPLL_CNTR2CLK_SRC_LSB),
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_CNTR2CLK);
	/* peri_emacb_clk divider */
	writel(per_cfg->cntr3clk_cnt |
	       (per_cfg->cntr3clk_src << CLKMGR_PERPLL_CNTR3CLK_SRC_LSB),
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_CNTR3CLK);
	/* peri_emac_ptp_clk divider */
	writel(per_cfg->cntr4clk_cnt |
	       (per_cfg->cntr4clk_src << CLKMGR_PERPLL_CNTR4CLK_SRC_LSB),
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_CNTR4CLK);
	/* peri_gpio_db_clk divider */
	writel(per_cfg->cntr5clk_cnt |
	       (per_cfg->cntr5clk_src << CLKMGR_PERPLL_CNTR5CLK_SRC_LSB),
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_CNTR5CLK);
	/* peri_sdmmc_clk divider */
	writel(per_cfg->cntr6clk_cnt |
	       (per_cfg->cntr6clk_src << CLKMGR_PERPLL_CNTR6CLK_SRC_LSB),
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_CNTR6CLK);
	/* peri_s2f_user0_clk divider */
	writel(per_cfg->cntr7clk_cnt,
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_CNTR7CLK);
	/* peri_s2f_user1_clk divider */
	writel(per_cfg->cntr8clk_cnt |
	       (per_cfg->cntr8clk_src << CLKMGR_PERPLL_CNTR8CLK_SRC_LSB),
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_CNTR8CLK);
	/* peri_hmc_pll_clk divider */
	writel(per_cfg->cntr9clk_cnt,
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_CNTR9CLK);

	/* setup all the external PLL counter */
	/* mpu wrapper / external divider */
	writel(main_cfg->mpuclk_cnt |
	       (main_cfg->mpuclk_src << CLKMGR_MAINPLL_MPUCLK_SRC_LSB),
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_MPUCLK);
	/* NOC wrapper / external divider */
	writel(main_cfg->nocclk_cnt |
	       (main_cfg->nocclk_src << CLKMGR_MAINPLL_NOCCLK_SRC_LSB),
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_NOCCLK);
	/* NOC subclock divider such as l4 */
	writel(main_cfg->nocdiv_l4mainclk |
	       (main_cfg->nocdiv_l4mpclk <<
		CLKMGR_MAINPLL_NOCDIV_L4MPCLK_LSB) |
	       (main_cfg->nocdiv_l4spclk <<
		CLKMGR_MAINPLL_NOCDIV_L4SPCLK_LSB) |
	       (main_cfg->nocdiv_csatclk <<
		CLKMGR_MAINPLL_NOCDIV_CSATCLK_LSB) |
	       (main_cfg->nocdiv_cstraceclk <<
		CLKMGR_MAINPLL_NOCDIV_CSTRACECLK_LSB) |
	       (main_cfg->nocdiv_cspdbclk <<
		CLKMGR_MAINPLL_NOCDIV_CSPDBGCLK_LSB),
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_NOCDIV);
	/* gpio_db external divider */
	writel(per_cfg->gpiodiv_gpiodbclk,
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_GPIOFIV);

	/* setup the EMAC clock mux select */
	writel((per_cfg->emacctl_emac0sel <<
		CLKMGR_PERPLL_EMACCTL_EMAC0SEL_LSB) |
	       (per_cfg->emacctl_emac1sel <<
		CLKMGR_PERPLL_EMACCTL_EMAC1SEL_LSB) |
	       (per_cfg->emacctl_emac2sel <<
		CLKMGR_PERPLL_EMACCTL_EMAC2SEL_LSB),
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_EMACCTL);

	/* at this stage, check for PLL lock status */
	cm_wait_for_lock(LOCKED_MASK);

	/*
	 * after locking, but before taking out of bypass,
	 * assert/deassert outresetall
	 */
	/* assert mainpll outresetall */
	setbits_le32(socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_VCO0,
		     CLKMGR_MAINPLL_VCO0_OUTRSTALL_SET_MSK);
	/* assert perpll outresetall */
	setbits_le32(socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_VCO0,
		     CLKMGR_PERPLL_VCO0_OUTRSTALL_SET_MSK);
	/* de-assert mainpll outresetall */
	clrbits_le32(socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_VCO0,
		     CLKMGR_MAINPLL_VCO0_OUTRSTALL_SET_MSK);
	/* de-assert perpll outresetall */
	clrbits_le32(socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_VCO0,
		     CLKMGR_PERPLL_VCO0_OUTRSTALL_SET_MSK);

	/* Take all PLLs out of bypass when boot mode is cleared. */
	/* release mainpll from bypass */
	writel(CLKMGR_MAINPLL_BYPASS_RESET,
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_BYPASSR);
	/* wait till Clock Manager is not busy */
	cm_wait_for_fsm();

	/* release perpll from bypass */
	writel(CLKMGR_PERPLL_BYPASS_RESET,
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_BYPASSR);
	/* wait till Clock Manager is not busy */
	cm_wait_for_fsm();

	/* clear boot mode */
	clrbits_le32(socfpga_get_clkmgr_addr() + CLKMGR_A10_CTRL,
		     CLKMGR_CLKMGR_CTL_BOOTMOD_SET_MSK);
	/* wait till Clock Manager is not busy */
	cm_wait_for_fsm();

	/* At here, we need to ramp to final value if needed */
	if (pll_ramp_main_hz != 0)
		cm_pll_ramp_main(main_cfg, per_cfg, pll_ramp_main_hz);
	if (pll_ramp_periph_hz != 0)
		cm_pll_ramp_periph(main_cfg, per_cfg, pll_ramp_periph_hz);

	/* Now ungate non-hw-managed clocks */
	writel(CLKMGR_MAINPLL_EN_S2FUSER0CLKEN_SET_MSK |
	       CLKMGR_MAINPLL_EN_HMCPLLREFCLKEN_SET_MSK,
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_MAINPLL_ENS);
	writel(CLKMGR_PERPLL_EN_RESET,
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_PERPLL_ENS);

	/* Clear the loss lock and slip bits as they might set during
	clock reconfiguration */
	writel(CLKMGR_CLKMGR_INTR_MAINPLLLOST_SET_MSK |
	       CLKMGR_CLKMGR_INTR_PERPLLLOST_SET_MSK |
	       CLKMGR_CLKMGR_INTR_MAINPLLRFSLIP_SET_MSK |
	       CLKMGR_CLKMGR_INTR_PERPLLRFSLIP_SET_MSK |
	       CLKMGR_CLKMGR_INTR_MAINPLLFBSLIP_SET_MSK |
	       CLKMGR_CLKMGR_INTR_PERPLLFBSLIP_SET_MSK,
	       socfpga_get_clkmgr_addr() + CLKMGR_A10_INTR);

	return 0;
}

static void  cm_use_intosc(void)
{
	setbits_le32(socfpga_get_clkmgr_addr() + CLKMGR_A10_CTRL,
		     CLKMGR_CLKMGR_CTL_BOOTCLK_INTOSC_SET_MSK);
}

int cm_basic_init()
{
    struct mainpll_cfg main_cfg;
    struct perpll_cfg per_cfg;
    

    
    main_cfg.vco0_psrc            =  MAINPLLGRP_VCO0_PSRC           ;
    main_cfg.vco1_denom           =  MAINPLLGRP_VCO1_DENOM          ;
    main_cfg.vco1_numer           =  MAINPLLGRP_VCO1_NUMER          ;
    main_cfg.mpuclk               =  ALTERAGRP_MPUCLK;              ;
    main_cfg.mpuclk_cnt           =  MAINPLLGRP_MPUCLK_CNT          ;
    main_cfg.mpuclk_src           =  MAINPLLGRP_MPUCLK_SRC          ;
    main_cfg.nocclk               =  ALTERAGRP_NOCCLK               ;
    main_cfg.nocclk_cnt           =  MAINPLLGRP_NOCCLK_CNT          ;
    main_cfg.nocclk_src           =  MAINPLLGRP_NOCCLK_SRC          ;
    main_cfg.cntr2clk_cnt         =  MAINPLLGRP_CNTR2CLK_CNT        ;
    main_cfg.cntr3clk_cnt         =  MAINPLLGRP_CNTR3CLK_CNT        ;
    main_cfg.cntr4clk_cnt         =  MAINPLLGRP_CNTR4CLK_CNT        ;
    main_cfg.cntr5clk_cnt         =  MAINPLLGRP_CNTR5CLK_CNT        ;
    main_cfg.cntr6clk_cnt         =  MAINPLLGRP_CNTR6CLK_CNT        ;
    main_cfg.cntr7clk_cnt         =  MAINPLLGRP_CNTR7CLK_CNT        ;
    main_cfg.cntr7clk_src         =  MAINPLLGRP_CNTR7CLK_SRC        ;
    main_cfg.cntr8clk_cnt         =  MAINPLLGRP_CNTR8CLK_CNT        ;
    main_cfg.cntr9clk_cnt         =  MAINPLLGRP_CNTR9CLK_CNT        ;
    main_cfg.cntr9clk_src         =  MAINPLLGRP_CNTR9CLK_SRC        ;
    main_cfg.cntr15clk_cnt        =  MAINPLLGRP_CNTR15CLK_CNT       ;
    main_cfg.nocdiv_l4mainclk     =  MAINPLLGRP_NOCDIV_L4MAINCLK    ;
    main_cfg.nocdiv_l4mpclk       =  MAINPLLGRP_NOCDIV_L4MPCLK      ;
    main_cfg.nocdiv_l4spclk       =  MAINPLLGRP_NOCDIV_L4SPCLK      ;
    main_cfg.nocdiv_csatclk       =  MAINPLLGRP_NOCDIV_CSATCLK      ;
    main_cfg.nocdiv_cstraceclk    =  MAINPLLGRP_NOCDIV_CSTRACECLK   ;
    main_cfg.nocdiv_cspdbclk      = MAINPLLGRP_NOCDIV_CSPDBGCLK     ;
    

    per_cfg.vco0_psrc          = PERPLLGRP_VCO0_PSRC         ;           
    per_cfg.vco1_denom         = PERPLLGRP_VCO1_DENOM        ;          
    per_cfg.vco1_numer         = PERPLLGRP_VCO1_NUMER        ;          
    per_cfg.cntr2clk_cnt       = PERPLLGRP_CNTR2CLK_CNT      ;        
    per_cfg.cntr2clk_src       = PERPLLGRP_CNTR2CLK_SRC      ;    
    per_cfg.cntr3clk_cnt       = PERPLLGRP_CNTR3CLK_CNT      ;        
    per_cfg.cntr3clk_src       = PERPLLGRP_CNTR3CLK_SRC      ;        
    per_cfg.cntr4clk_cnt       = PERPLLGRP_CNTR4CLK_CNT      ;        
    per_cfg.cntr4clk_src       = PERPLLGRP_CNTR4CLK_SRC      ;        
    per_cfg.cntr5clk_cnt       = PERPLLGRP_CNTR5CLK_CNT      ;        
    per_cfg.cntr5clk_src       = PERPLLGRP_CNTR5CLK_SRC      ;        
    per_cfg.cntr6clk_cnt       = PERPLLGRP_CNTR6CLK_CNT      ;        
    per_cfg.cntr6clk_src       = PERPLLGRP_CNTR6CLK_SRC      ;        
    per_cfg.cntr7clk_cnt       = PERPLLGRP_CNTR7CLK_CNT      ;        
    per_cfg.cntr8clk_cnt       = PERPLLGRP_CNTR8CLK_CNT      ;        
    per_cfg.cntr8clk_src       = PERPLLGRP_CNTR8CLK_SRC      ;        
    per_cfg.cntr9clk_cnt       = PERPLLGRP_CNTR9CLK_CNT      ;        
//	per_cfg.cntr9clk_src       =  
    per_cfg.emacctl_emac0sel   =  PERPLLGRP_EMACCTL_EMAC0SEL ;  
    per_cfg.emacctl_emac1sel   =  PERPLLGRP_EMACCTL_EMAC1SEL ;  
    per_cfg.emacctl_emac2sel   =  PERPLLGRP_EMACCTL_EMAC2SEL ;  
    per_cfg.gpiodiv_gpiodbclk  =  PERPLLGRP_GPIODIV_GPIODBCLK; 
    
    socfpga_watchdog_disable();
    cm_use_intosc();
    
    return cm_full_cfg(&main_cfg, &per_cfg);
}    

// static const struct socfpga_noc_fw_ocram *noc_fw_ocram_base = (void *)SOCFPGA_SDR_FIREWALL_OCRAM_ADDRESS;
/*
+ * This function initializes security policies to be consistent across
+ * all logic units in the Arria 10.
+ *
+ * The idea is to set all security policies to be normal, nonsecure
+ * for all units.
+ */
void socfpga_init_security_policies(void)
{
	/* Put OCRAM in non-secure */
	//writel(0x003f0000, &noc_fw_ocram_base->region0);
	//writel(0x1, &noc_fw_ocram_base->enable);

	/* Put DDR in non-secure */
	writel(0xffff0000, SOCFPGA_SDR_FIREWALL_L3_ADDRESS + 0xc);
	writel(0x1, SOCFPGA_SDR_FIREWALL_L3_ADDRESS);

	/* Enable priviledged and non-priviledged access to L4 peripherals */
	writel(~0, SOCFPGA_NOC_L4_PRIV_FLT_OFST);

	/* Enable secure and non-secure transactions to bridges */
	writel(~0, SOCFPGA_NOC_FW_H2F_SCR_OFST);
	writel(~0, SOCFPGA_NOC_FW_H2F_SCR_OFST + 4);

	writel(0x0007FFFF, socfpga_get_sysmgr_addr() + SYSMGR_A10_ECC_INTMASK_SET);
}






