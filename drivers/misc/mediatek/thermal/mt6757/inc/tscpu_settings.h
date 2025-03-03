/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef __TSCPU_SETTINGS_H__
#define __TSCPU_SETTINGS_H__

#include "mt_gpufreq.h"

#include <linux/of.h>
#include <linux/of_address.h>

#include "tzcpu_initcfg.h"
#include "clatm_initcfg.h"
#include <mt_eem.h>

/*=============================================================
 * Genernal
 *=============================================================*/
#define MIN(_a_, _b_) ((_a_) > (_b_) ? (_b_) : (_a_))
#define MAX(_a_, _b_) ((_a_) > (_b_) ? (_a_) : (_b_))
#define _BIT_(_bit_)		(unsigned)(1 << (_bit_))
#define _BITMASK_(_bits_)	(((unsigned) -1 >> (31 - ((1) ? _bits_))) & ~((1U << ((0) ? _bits_)) - 1))

#define THERMAL_TPROFILE_INIT() long long thermal_pTime_us, thermal_cTime_us, thermal_diff_us

#define THERMAL_GET_PTIME() {thermal_pTime_us = thermal_get_current_time_us()}

#define THERMAL_GET_CTIME() {thermal_cTime_us = thermal_get_current_time_us()}

#define THERMAL_TIME_TH 3000

#define THERMAL_IS_TOO_LONG()   \
	do {                                    \
		thermal_diff_us = thermal_cTime_us - thermal_pTime_us;	\
		if (thermal_diff_us > THERMAL_TIME_TH) {                \
			pr_warn(TSCPU_LOG_TAG "%s: %llu us\n", __func__, thermal_diff_us); \
		} else if (thermal_diff_us < 0) {	\
			pr_warn(TSCPU_LOG_TAG "Warning: tProfiling uses incorrect %s %d\n", __func__, __LINE__); \
		}	\
	} while (0)
/*=============================================================
 * CONFIG (SW related)
 *=============================================================*/
#define ENALBE_UART_LIMIT		(1)
#define TEMP_EN_UART			(80000)
#define TEMP_DIS_UART			(85000)
#define TEMP_TOLERANCE			(0)

#define ENALBE_SW_FILTER		(0)
#define ATM_USES_PPM	(1)

#define THERMAL_GET_AHB_BUS_CLOCK		    (0)
#define THERMAL_PERFORMANCE_PROFILE         (0)

/* 1: turn on GPIO toggle monitor; 0: turn off*/
#define THERMAL_GPIO_OUT_TOGGLE             (0)

/* 1: turn on adaptive AP cooler; 0: turn off*/
#define CPT_ADAPTIVE_AP_COOLER              (1)

/* 1: turn on supports to MET logging; 0: turn off*/
#define CONFIG_SUPPORT_MET_MTKTSCPU         (0)

/* Thermal controller HW filtering function. Only 1, 2, 4, 8, 16 are valid values,
they means one reading is a avg of X samples*/
#define THERMAL_CONTROLLER_HW_FILTER        (2) /* 1, 2, 4, 8, 16*/

/* 1: turn on thermal controller HW thermal protection; 0: turn off*/
#define THERMAL_CONTROLLER_HW_TP            (1)

/* 1: turn on fast polling in this sw module; 0: turn off*/
#define MTKTSCPU_FAST_POLLING               (1)

#if CPT_ADAPTIVE_AP_COOLER
#define MAX_CPT_ADAPTIVE_COOLERS            (3)

#define THERMAL_HEADROOM                    (0)
#define CONTINUOUS_TM                       (1)
#define DYNAMIC_GET_GPU_POWER			    (1)

/* 1: turn on precise power budgeting; 0: turn off */
#define PRECISE_HYBRID_POWER_BUDGET         (1)

#define PHPB_DEFAULT_ON						(1)
#endif

/* 1: thermal driver fast polling, use hrtimer; 0: turn off*/
/*#define THERMAL_DRV_FAST_POLL_HRTIMER          (1)*/

/* 1: thermal driver update temp to MET directly, use hrtimer; 0: turn off*/
#define THERMAL_DRV_UPDATE_TEMP_DIRECT_TO_MET  (1)

/*	Define this in tscpu_settings.h enables this feature. It polls CPU TS in hrtimer and
	run ATM in RT 98 kthread. This is for Everest only.
 */
/*#define FAST_RESPONSE_ATM					(0)*/
/*=============================================================
 * Chip related
 *=============================================================*/
/**/
#define THERMAL_INFORM_OTP	(0)
#define OTP_HIGH_OFFSET_TEMP	80000
#define OTP_LOW_OFFSET_TEMP	70000
#define OTP_TEMP_TOLERANCE	3000

/*double check*/
#define TS_CONFIGURE     TS_CON1_TM    /* depend on CPU design*/
#define TS_CONFIGURE_P   TS_CON1_P  /* depend on CPU design*/
#define TS_TURN_ON       0xFFFFFFCF /* turn on TS_CON1[5:4] 2'b 00  11001111 -> 0xCF  ~(0x30)*/
#define TS_TURN_OFF      0x00000030 /* turn off thermal*/
/*chip dependent*/
#define ADDRESS_INDEX_0  46  /*0x10206184*/
#define ADDRESS_INDEX_1	 45  /*0x10206180*/
#define ADDRESS_INDEX_2	 47  /*0x10206188*/

#define CLEAR_TEMP 26111

/* TSCON1 bit table */
#define TSCON0_bit_6_7_00 0x00  /* TSCON0[7:6]=2'b00*/
#define TSCON0_bit_6_7_01 0x40  /* TSCON0[7:6]=2'b01*/
#define TSCON0_bit_6_7_10 0x80  /* TSCON0[7:6]=2'b10*/
#define TSCON0_bit_6_7_11 0xc0  /* TSCON0[7:6]=2'b11*/
#define TSCON0_bit_6_7_MASK 0xc0

#define TSCON1_bit_4_5_00 0x00  /* TSCON1[5:4]=2'b00*/
#define TSCON1_bit_4_5_01 0x10  /* TSCON1[5:4]=2'b01*/
#define TSCON1_bit_4_5_10 0x20  /* TSCON1[5:4]=2'b10*/
#define TSCON1_bit_4_5_11 0x30  /* TSCON1[5:4]=2'b11*/
#define TSCON1_bit_4_5_MASK 0x30

#define TSCON1_bit_0_2_000 0x00  /*TSCON1[2:0]=3'b000*/
#define TSCON1_bit_0_2_001 0x01  /*TSCON1[2:0]=3'b001*/
#define TSCON1_bit_0_2_010 0x02  /*TSCON1[2:0]=3'b010*/
#define TSCON1_bit_0_2_011 0x03  /*TSCON1[2:0]=3'b011*/
#define TSCON1_bit_0_2_100 0x04  /*TSCON1[2:0]=3'b100*/
#define TSCON1_bit_0_2_101 0x05  /*TSCON1[2:0]=3'b101*/
#define TSCON1_bit_0_2_110 0x06  /*TSCON1[2:0]=3'b110*/
#define TSCON1_bit_0_2_111 0x07  /*TSCON1[2:0]=3'b111*/
#define TSCON1_bit_0_2_MASK 0x07


/* ADC value to mcu */
/*chip dependent*/
#define TEMPADC_MCU1    ((0x30&TSCON1_bit_4_5_00)|(0x07&TSCON1_bit_0_2_000))
#define TEMPADC_MCU2    ((0x30&TSCON1_bit_4_5_00)|(0x07&TSCON1_bit_0_2_001))
#define TEMPADC_MCU3    ((0x30&TSCON1_bit_4_5_00)|(0x07&TSCON1_bit_0_2_010))
#define TEMPADC_MCU4    ((0x30&TSCON1_bit_4_5_00)|(0x07&TSCON1_bit_0_2_011))
#define TEMPADC_MCU5    ((0x30&TSCON1_bit_4_5_00)|(0x07&TSCON1_bit_0_2_100))
#define TEMPADC_ABB     ((0x30&TSCON1_bit_4_5_01)|(0x07&TSCON1_bit_0_2_000))

#define TS_FILL(n) {#n, n}
#define TS_LEN_ARRAY(name) (sizeof(name)/sizeof(name[0]))
#define MAX_TS_NAME 20

#define CPU_COOLER_NUM 34
#define MTK_TS_CPU_RT                       (0)

#ifdef CONFIG_MTK_RAM_CONSOLE
#define CONFIG_THERMAL_AEE_RR_REC (1)
#else
#define CONFIG_THERMAL_AEE_RR_REC (0)
#endif
/*=============================================================
 *REG ACCESS
 *=============================================================*/

#define thermal_setl(addr, val)     mt_reg_sync_writel(readl(addr) | (val), ((void *)addr))
#define thermal_clrl(addr, val)     mt_reg_sync_writel(readl(addr) & ~(val), ((void *)addr))

#define MTKTSCPU_TEMP_CRIT 120000 /* 120.000 degree Celsius */

#define y_curr_repeat_times 1
#define THERMAL_NAME    "mtk-thermal"

#define TS_MS_TO_NS(x) (x * 1000 * 1000)

#if THERMAL_GET_AHB_BUS_CLOCK
#define THERMAL_MODULE_SW_CG_SET	(therm_clk_infracfg_ao_base + 0x88)
#define THERMAL_MODULE_SW_CG_CLR	(therm_clk_infracfg_ao_base + 0x8C)
#define THERMAL_MODULE_SW_CG_STA	(therm_clk_infracfg_ao_base + 0x94)

#define THERMAL_CG	(therm_clk_infracfg_ao_base + 0x80)
#define THERMAL_DCM	(therm_clk_infracfg_ao_base + 0x70)
#endif
/*=============================================================
 *LOG
 *=============================================================*/
#define TSCPU_LOG_TAG		"[CPU_Thermal]"

#define tscpu_dprintk(fmt, args...)   \
do {                                    \
	if (tscpu_debug_log) {                \
		pr_debug(TSCPU_LOG_TAG fmt, ##args); \
	}                                   \
} while (0)

#define tscpu_printk(fmt, args...) pr_debug(TSCPU_LOG_TAG fmt, ##args)
#define tscpu_warn(fmt, args...)  pr_warn(TSCPU_LOG_TAG fmt, ##args)

/*=============================================================
 * Structures
 *=============================================================*/

/* Align with thermal_sensor_name   struct @ Mt_thermal.h*/
typedef enum thermal_sensor_enum {
	MCU1 = 0,
	MCU2,
	MCU3,
	MCU4,
	MCU5,
	ABB,
	ENUM_MAX,
} ts_e;

typedef struct {
	char ts_name[MAX_TS_NAME];
	ts_e type;
} thermal_sensor_t;

typedef struct {
	thermal_sensor_t ts[ENUM_MAX];
	int ts_number;
} bank_t;

#if (CONFIG_THERMAL_AEE_RR_REC == 1)
enum thermal_state {
	TSCPU_SUSPEND = 0,
	TSCPU_RESUME  = 1,
	TSCPU_NORMAL  = 2,
	TSCPU_INIT    = 3,
	TSCPU_PAUSE   = 4,
	TSCPU_RELEASE = 5
};
enum atm_state {
	ATM_WAKEUP = 0,
	ATM_CPULIMIT  = 1,
	ATM_GPULIMIT  = 2,
	ATM_DONE    = 3,
};
#endif

struct mtk_cpu_power_info {
	unsigned int cpufreq_khz;
	unsigned int cpufreq_ncpu;
	unsigned int cpufreq_power;
};

/*=============================================================
 * Shared variables
 *=============================================================*/
/*In src/mtk_tc.c*/
extern int temp_eUART;
extern int temp_dUART;

extern int tscpu_debug_log;
extern const struct of_device_id mt_thermal_of_match[2];
extern int tscpu_bank_ts[THERMAL_BANK_NUM][ENUM_MAX];
extern bank_t tscpu_g_bank[THERMAL_BANK_NUM];
extern int tscpu_polling_trip_temp1;
extern int tscpu_polling_trip_temp2;
extern int tscpu_polling_factor1;
extern int tscpu_polling_factor2;
#if MTKTSCPU_FAST_POLLING
/* Combined fast_polling_trip_temp and fast_polling_factor,
it means polling_delay will be 1/5 of original interval
after mtktscpu reports > 65C w/o exit point */
extern int fast_polling_trip_temp;
extern int fast_polling_trip_temp_high;
extern int fast_polling_factor;
extern int tscpu_cur_fp_factor;
extern int tscpu_next_fp_factor;
#endif

/*In common/thermal_zones/mtk_ts_cpu.c*/
extern long long thermal_get_current_time_us(void);
extern void tscpu_workqueue_cancel_timer(void);
extern void tscpu_workqueue_start_timer(void);

extern void __iomem  *therm_clk_infracfg_ao_base;
extern int Num_of_GPU_OPP;
extern struct mt_gpufreq_power_table_info *mtk_gpu_power;
extern int tscpu_read_curr_temp;
#if MTKTSCPU_FAST_POLLING
extern int tscpu_cur_fp_factor;
#endif

#if !defined(CONFIG_MTK_CLKMGR)
extern struct clk *therm_main;           /* main clock for Thermal*/
#endif

#if CPT_ADAPTIVE_AP_COOLER
extern int tscpu_g_curr_temp;
extern int tscpu_g_prev_temp;
#if (THERMAL_HEADROOM == 1) || (CONTINUOUS_TM == 1)
extern int bts_cur_temp;	/* in mtk_ts_bts.c */
#endif

#if PRECISE_HYBRID_POWER_BUDGET
/*	tscpu_prev_cpu_temp: previous CPUSYS temperature
	tscpu_curr_cpu_temp: current CPUSYS temperature
	tscpu_prev_gpu_temp: previous GPUSYS temperature
	tscpu_curr_gpu_temp: current GPUSYS temperature
 */
extern int tscpu_prev_cpu_temp, tscpu_prev_gpu_temp;
extern int tscpu_curr_cpu_temp, tscpu_curr_gpu_temp;
#endif

#endif

#ifdef CONFIG_OF
extern u32 thermal_irq_number;
extern void __iomem *thermal_base;
extern void __iomem *auxadc_ts_base;
extern void __iomem *infracfg_ao_base;
extern void __iomem *th_apmixed_base;
extern void __iomem *INFRACFG_AO_base;

extern int thermal_phy_base;
extern int auxadc_ts_phy_base;
extern int apmixed_phy_base;
extern int pericfg_phy_base;
#endif
extern char *adaptive_cooler_name;

/*common/coolers/mtk_cooler_atm.c*/
extern unsigned int adaptive_cpu_power_limit;
extern unsigned int adaptive_gpu_power_limit;
extern int TARGET_TJS[MAX_CPT_ADAPTIVE_COOLERS];
#ifdef FAST_RESPONSE_ATM
extern void atm_cancel_hrtimer(void);
extern void atm_restart_hrtimer(void);
#endif

/*common/coolers/mtk_cooler_dtm.c*/
extern unsigned int static_cpu_power_limit;
extern unsigned int static_gpu_power_limit;
extern int tscpu_cpu_dmips[CPU_COOLER_NUM];
/*=============================================================
 * Shared functions
 *=============================================================*/
/*In common/thermal_zones/mtk_ts_cpu.c*/
extern void thermal_init_interrupt_for_UART(int temp_e, int temp_d);
extern void tscpu_print_all_temperature(int isDprint);
extern void tscpu_update_tempinfo(void);
#if THERMAL_GPIO_OUT_TOGGLE
void tscpu_set_GPIO_toggle_for_monitor(void);
#endif
extern void tscpu_thermal_tempADCPNP(int adc, int order);
extern int tscpu_thermal_ADCValueOfMcu(ts_e type);
extern void tscpu_thermal_enable_all_periodoc_sensing_point(thermal_bank_name bank_num);
extern void tscpu_update_tempinfo(void);
extern int tscpu_max_temperature(void);

/*In src/mtk_tc.c*/
extern int get_io_reg_base(void);
extern void tscpu_config_all_tc_hw_protect(int temperature, int temperature2);
extern void tscpu_reset_thermal(void);
extern void tscpu_thermal_initial_all_bank(void);
extern int tscpu_switch_bank(thermal_bank_name bank);
extern void tscpu_thermal_read_bank_temp(thermal_bank_name bank, ts_e type, int order);
extern void tscpu_thermal_cal_prepare(void);
extern void tscpu_thermal_cal_prepare_2(U32 ret);
extern irqreturn_t tscpu_thermal_all_bank_interrupt_handler(int irq, void *dev_id);
extern int tscpu_thermal_clock_on(void);
extern int tscpu_thermal_clock_off(void);
extern int tscpu_read_temperature_info(struct seq_file *m, void *v);
extern int tscpu_thermal_fast_init(void);
extern int tscpu_get_curr_temp(void);
extern int tscpu_get_curr_max_ts_temp(void);
extern void thermal_get_AHB_clk_info(void);

/*
In drivers/misc/mediatek/gpu/hal/mtk_gpu_utility.c
It's not our api, ask them to provide header file
*/
extern bool mtk_get_gpu_loading(unsigned int *pLoading);
/*
In drivers/misc/mediatek/auxadc/mt_auxadc.c
It's not our api, ask them to provide header file
*/
extern int IMM_IsAdcInitReady(void);
/*aee related*/
#if (CONFIG_THERMAL_AEE_RR_REC == 1)
extern void aee_rr_rec_thermal_temp1(u8 val);
extern void aee_rr_rec_thermal_temp2(u8 val);
extern void aee_rr_rec_thermal_temp3(u8 val);
extern void aee_rr_rec_thermal_temp4(u8 val);
extern void aee_rr_rec_thermal_temp5(u8 val); /* TODO: add ts5 */
extern void aee_rr_rec_thermal_status(u8 val);
extern void aee_rr_rec_thermal_ATM_status(u8 val);
extern void aee_rr_rec_thermal_ktime(u64 val);

extern u8 aee_rr_curr_thermal_temp1(void);
extern u8 aee_rr_curr_thermal_temp2(void);
extern u8 aee_rr_curr_thermal_temp3(void);
extern u8 aee_rr_curr_thermal_temp4(void);
extern u8 aee_rr_curr_thermal_temp5(void); /* TODO: add ts5 */
extern u8 aee_rr_curr_thermal_status(void);
extern u8 aee_rr_curr_thermal_ATM_status(void);
extern u64 aee_rr_curr_thermal_ktime(void);
#endif

/*=============================================================
 * Register macro for internal use
 *=============================================================*/

#if 1
extern void __iomem *thermal_base;
extern void __iomem *auxadc_base;
extern void __iomem *infracfg_ao_base;
extern void __iomem *th_apmixed_base;
extern void __iomem *INFRACFG_AO_base;

#define THERM_CTRL_BASE_2    thermal_base
#define AUXADC_BASE_2        auxadc_ts_base
#define INFRACFG_AO_BASE_2   infracfg_ao_base
#define APMIXED_BASE_2       th_apmixed_base
#else
#include <mach/mt_reg_base.h>
#define AUXADC_BASE_2     AUXADC_BASE
#define THERM_CTRL_BASE_2 THERM_CTRL_BASE
#define PERICFG_BASE_2    PERICFG_BASE
#define APMIXED_BASE_2    APMIXED_BASE
#endif

#define MT6752_EVB_BUILD_PASS /*Jerry fix build error FIX_ME*/

/*******************************************************************************
 * AUXADC Register Definition
 ******************************************************************************/
/*K2 AUXADC_BASE: 0xF1001000 from Vincent Liang 2014.5.8*/

#define AUXADC_CON0_V       (AUXADC_BASE_2 + 0x000)	/*yes, 0x11003000*/
#define AUXADC_CON1_V       (AUXADC_BASE_2 + 0x004)
#define AUXADC_CON1_SET_V   (AUXADC_BASE_2 + 0x008)
#define AUXADC_CON1_CLR_V   (AUXADC_BASE_2 + 0x00C)
#define AUXADC_CON2_V       (AUXADC_BASE_2 + 0x010)
/*#define AUXADC_CON3_V       (AUXADC_BASE_2 + 0x014)*/
#define AUXADC_DAT0_V       (AUXADC_BASE_2 + 0x014)
#define AUXADC_DAT1_V       (AUXADC_BASE_2 + 0x018)
#define AUXADC_DAT2_V       (AUXADC_BASE_2 + 0x01C)
#define AUXADC_DAT3_V       (AUXADC_BASE_2 + 0x020)
#define AUXADC_DAT4_V       (AUXADC_BASE_2 + 0x024)
#define AUXADC_DAT5_V       (AUXADC_BASE_2 + 0x028)
#define AUXADC_DAT6_V       (AUXADC_BASE_2 + 0x02C)
#define AUXADC_DAT7_V       (AUXADC_BASE_2 + 0x030)
#define AUXADC_DAT8_V       (AUXADC_BASE_2 + 0x034)
#define AUXADC_DAT9_V       (AUXADC_BASE_2 + 0x038)
#define AUXADC_DAT10_V       (AUXADC_BASE_2 + 0x03C)
#define AUXADC_DAT11_V       (AUXADC_BASE_2 + 0x040)
#define AUXADC_MISC_V       (AUXADC_BASE_2 + 0x094)

#define AUXADC_CON0_P       (auxadc_ts_phy_base + 0x000)
#define AUXADC_CON1_P       (auxadc_ts_phy_base + 0x004)
#define AUXADC_CON1_SET_P   (auxadc_ts_phy_base + 0x008)
#define AUXADC_CON1_CLR_P   (auxadc_ts_phy_base + 0x00C)
#define AUXADC_CON2_P       (auxadc_ts_phy_base + 0x010)
/*#define AUXADC_CON3_P       (auxadc_ts_phy_base + 0x014)*/
#define AUXADC_DAT0_P       (auxadc_ts_phy_base + 0x014)
#define AUXADC_DAT1_P       (auxadc_ts_phy_base + 0x018)
#define AUXADC_DAT2_P       (auxadc_ts_phy_base + 0x01C)
#define AUXADC_DAT3_P       (auxadc_ts_phy_base + 0x020)
#define AUXADC_DAT4_P       (auxadc_ts_phy_base + 0x024)
#define AUXADC_DAT5_P       (auxadc_ts_phy_base + 0x028)
#define AUXADC_DAT6_P       (auxadc_ts_phy_base + 0x02C)
#define AUXADC_DAT7_P       (auxadc_ts_phy_base + 0x030)
#define AUXADC_DAT8_P       (auxadc_ts_phy_base + 0x034)
#define AUXADC_DAT9_P       (auxadc_ts_phy_base + 0x038)
#define AUXADC_DAT10_P       (auxadc_ts_phy_base + 0x03C)
#define AUXADC_DAT11_P       (auxadc_ts_phy_base + 0x040)

#define AUXADC_MISC_P       (auxadc_ts_phy_base + 0x094)

/*
#define AUXADC_CON0_P       (AUXADC_BASE_2 + 0x000 - 0xE0000000)
#define AUXADC_CON1_P       (AUXADC_BASE_2 + 0x004 - 0xE0000000)
#define AUXADC_CON1_SET_P   (AUXADC_BASE_2 + 0x008 - 0xE0000000)
#define AUXADC_CON1_CLR_P   (AUXADC_BASE_2 + 0x00C - 0xE0000000)
#define AUXADC_CON2_P       (AUXADC_BASE_2 + 0x010 - 0xE0000000)
#define AUXADC_CON3_P       (AUXADC_BASE_2 + 0x014 - 0x30000000)
#define AUXADC_DAT0_P       (AUXADC_BASE_2 + 0x014 - 0xE0000000)
#define AUXADC_DAT1_P       (AUXADC_BASE_2 + 0x018 - 0xE0000000)
#define AUXADC_DAT2_P       (AUXADC_BASE_2 + 0x01C - 0xE0000000)
#define AUXADC_DAT3_P       (AUXADC_BASE_2 + 0x020 - 0xE0000000)
#define AUXADC_DAT4_P       (AUXADC_BASE_2 + 0x024 - 0xE0000000)
#define AUXADC_DAT5_P       (AUXADC_BASE_2 + 0x028 - 0xE0000000)
#define AUXADC_DAT6_P       (AUXADC_BASE_2 + 0x02C - 0xE0000000)
#define AUXADC_DAT7_P       (AUXADC_BASE_2 + 0x030 - 0xE0000000)
#define AUXADC_DAT8_P       (AUXADC_BASE_2 + 0x034 - 0xE0000000)
#define AUXADC_DAT9_P       (AUXADC_BASE_2 + 0x038 - 0xE0000000)
#define AUXADC_DAT10_P       (AUXADC_BASE_2 + 0x03C - 0xE0000000)
#define AUXADC_DAT11_P       (AUXADC_BASE_2 + 0x040 - 0xE0000000)

#define AUXADC_MISC_P       (AUXADC_BASE_2 + 0x094 - 0xE0000000)
*/
/*******************************************************************************
 * Peripheral Configuration Register Definition
 ******************************************************************************/

/*APB Module infracfg_ao*/
/*#define INFRACFG_AO_BASE (0xF0001000)*/
#define INFRA_GLOBALCON_RST_0_SET (INFRACFG_AO_BASE_2 + 0x120) /*yes, 0x10001000*/
#define INFRA_GLOBALCON_RST_0_CLR (INFRACFG_AO_BASE_2 + 0x124) /*yes, 0x10001000*/
#define INFRA_GLOBALCON_RST_0_STA (INFRACFG_AO_BASE_2 + 0x128) /*yes, 0x10001000*/
/*******************************************************************************
 * APMixedSys Configuration Register Definition
 ******************************************************************************/
/*K2 APMIXED_BASE: 0x1000C000 from KJ 2014.5.8 TODO: FIXME*/
#define TS_CON0_TM             (APMIXED_BASE_2 + 0x600) /*yes 0x10209000*/
#define TS_CON1_TM             (APMIXED_BASE_2 + 0x604)
#define TS_CON0_P           (apmixed_phy_base + 0x600)
#define TS_CON1_P           (apmixed_phy_base + 0x604)
/*
#define TS_CON0             (APMIXED_BASE_2 + 0x800)
#define TS_CON1             (APMIXED_BASE_2 + 0x804)
#define TS_CON2             (APMIXED_BASE_2 + 0x808)
#define TS_CON3             (APMIXED_BASE_2 + 0x80C)
*/

/*******************************************************************************
 * Thermal Controller Register Definition
 ******************************************************************************/
#define TEMPMONCTL0         (THERM_CTRL_BASE_2 + 0x000) /*yes 0xF100B000*/
#define TEMPMONCTL1         (THERM_CTRL_BASE_2 + 0x004)
#define TEMPMONCTL2         (THERM_CTRL_BASE_2 + 0x008)
#define TEMPMONINT          (THERM_CTRL_BASE_2 + 0x00C)
#define TEMPMONINTSTS       (THERM_CTRL_BASE_2 + 0x010)
#define TEMPMONIDET0        (THERM_CTRL_BASE_2 + 0x014)
#define TEMPMONIDET1        (THERM_CTRL_BASE_2 + 0x018)
#define TEMPMONIDET2        (THERM_CTRL_BASE_2 + 0x01C)
#define TEMPH2NTHRE         (THERM_CTRL_BASE_2 + 0x024)
#define TEMPHTHRE           (THERM_CTRL_BASE_2 + 0x028)
#define TEMPCTHRE           (THERM_CTRL_BASE_2 + 0x02C)
#define TEMPOFFSETH         (THERM_CTRL_BASE_2 + 0x030)
#define TEMPOFFSETL         (THERM_CTRL_BASE_2 + 0x034)
#define TEMPMSRCTL0         (THERM_CTRL_BASE_2 + 0x038)
#define TEMPMSRCTL1         (THERM_CTRL_BASE_2 + 0x03C)
#define TEMPAHBPOLL         (THERM_CTRL_BASE_2 + 0x040)
#define TEMPAHBTO           (THERM_CTRL_BASE_2 + 0x044)
#define TEMPADCPNP0         (THERM_CTRL_BASE_2 + 0x048)
#define TEMPADCPNP1         (THERM_CTRL_BASE_2 + 0x04C)
#define TEMPADCPNP2         (THERM_CTRL_BASE_2 + 0x050)
#define TEMPADCPNP3         (THERM_CTRL_BASE_2 + 0x0B4)

#define TEMPADCMUX          (THERM_CTRL_BASE_2 + 0x054)
#define TEMPADCEXT          (THERM_CTRL_BASE_2 + 0x058)
#define TEMPADCEXT1         (THERM_CTRL_BASE_2 + 0x05C)
#define TEMPADCEN           (THERM_CTRL_BASE_2 + 0x060)
#define TEMPPNPMUXADDR      (THERM_CTRL_BASE_2 + 0x064)
#define TEMPADCMUXADDR      (THERM_CTRL_BASE_2 + 0x068)
#define TEMPADCEXTADDR      (THERM_CTRL_BASE_2 + 0x06C)
#define TEMPADCEXT1ADDR     (THERM_CTRL_BASE_2 + 0x070)
#define TEMPADCENADDR       (THERM_CTRL_BASE_2 + 0x074)
#define TEMPADCVALIDADDR    (THERM_CTRL_BASE_2 + 0x078)
#define TEMPADCVOLTADDR     (THERM_CTRL_BASE_2 + 0x07C)
#define TEMPRDCTRL          (THERM_CTRL_BASE_2 + 0x080)
#define TEMPADCVALIDMASK    (THERM_CTRL_BASE_2 + 0x084)
#define TEMPADCVOLTAGESHIFT (THERM_CTRL_BASE_2 + 0x088)
#define TEMPADCWRITECTRL    (THERM_CTRL_BASE_2 + 0x08C)
#define TEMPMSR0            (THERM_CTRL_BASE_2 + 0x090)
#define TEMPMSR1            (THERM_CTRL_BASE_2 + 0x094)
#define TEMPMSR2            (THERM_CTRL_BASE_2 + 0x098)
#define TEMPMSR3            (THERM_CTRL_BASE_2 + 0x0B8)

#define TEMPIMMD0           (THERM_CTRL_BASE_2 + 0x0A0)
#define TEMPIMMD1           (THERM_CTRL_BASE_2 + 0x0A4)
#define TEMPIMMD2           (THERM_CTRL_BASE_2 + 0x0A8)
#define TEMPIMMD3           (THERM_CTRL_BASE_2 + 0x0BC)


#define TEMPPROTCTL         (THERM_CTRL_BASE_2 + 0x0C0)
#define TEMPPROTTA          (THERM_CTRL_BASE_2 + 0x0C4)
#define TEMPPROTTB          (THERM_CTRL_BASE_2 + 0x0C8)
#define TEMPPROTTC          (THERM_CTRL_BASE_2 + 0x0CC)

#define TEMPSPARE0          (THERM_CTRL_BASE_2 + 0x0F0)
#define TEMPSPARE1          (THERM_CTRL_BASE_2 + 0x0F4)
#define TEMPSPARE2          (THERM_CTRL_BASE_2 + 0x0F8)
#define TEMPSPARE3          (THERM_CTRL_BASE_2 + 0x0FC)

#define PTPCORESEL          (THERM_CTRL_BASE_2 + 0x400)
#define THERMINTST          (THERM_CTRL_BASE_2 + 0x404)
#define PTPODINTST          (THERM_CTRL_BASE_2 + 0x408)
#define THSTAGE0ST          (THERM_CTRL_BASE_2 + 0x40C)
#define THSTAGE1ST          (THERM_CTRL_BASE_2 + 0x410)
#define THSTAGE2ST          (THERM_CTRL_BASE_2 + 0x414)
#define THAHBST0            (THERM_CTRL_BASE_2 + 0x418)
#define THAHBST1            (THERM_CTRL_BASE_2 + 0x41C) /*Only for DE debug*/
#define PTPSPARE0           (THERM_CTRL_BASE_2 + 0x420)
#define PTPSPARE1           (THERM_CTRL_BASE_2 + 0x424)
#define PTPSPARE2           (THERM_CTRL_BASE_2 + 0x428)
#define PTPSPARE3           (THERM_CTRL_BASE_2 + 0x42C)
#define THSLPEVEB           (THERM_CTRL_BASE_2 + 0x430)


#define PTPSPARE0_P           (thermal_phy_base + 0x420)
#define PTPSPARE1_P           (thermal_phy_base + 0x424)
#define PTPSPARE2_P           (thermal_phy_base + 0x428)
#define PTPSPARE3_P           (thermal_phy_base + 0x42C)

/*******************************************************************************
 * Thermal Controller Register Mask Definition
 ******************************************************************************/
#define THERMAL_ENABLE_SEN0     0x1
#define THERMAL_ENABLE_SEN1     0x2
#define THERMAL_ENABLE_SEN2     0x4
#define THERMAL_MONCTL0_MASK    0x00000007

#define THERMAL_PUNT_MASK       0x00000FFF
#define THERMAL_FSINTVL_MASK    0x03FF0000
#define THERMAL_SPINTVL_MASK    0x000003FF
#define THERMAL_MON_INT_MASK    0x0007FFFF

#define THERMAL_MON_CINTSTS0    0x000001
#define THERMAL_MON_HINTSTS0    0x000002
#define THERMAL_MON_LOINTSTS0   0x000004
#define THERMAL_MON_HOINTSTS0   0x000008
#define THERMAL_MON_NHINTSTS0   0x000010
#define THERMAL_MON_CINTSTS1    0x000020
#define THERMAL_MON_HINTSTS1    0x000040
#define THERMAL_MON_LOINTSTS1   0x000080
#define THERMAL_MON_HOINTSTS1   0x000100
#define THERMAL_MON_NHINTSTS1   0x000200
#define THERMAL_MON_CINTSTS2    0x000400
#define THERMAL_MON_HINTSTS2    0x000800
#define THERMAL_MON_LOINTSTS2   0x001000
#define THERMAL_MON_HOINTSTS2   0x002000
#define THERMAL_MON_NHINTSTS2   0x004000
#define THERMAL_MON_TOINTSTS    0x008000
#define THERMAL_MON_IMMDINTSTS0 0x010000
#define THERMAL_MON_IMMDINTSTS1 0x020000
#define THERMAL_MON_IMMDINTSTS2 0x040000
#define THERMAL_MON_FILTINTSTS0 0x080000
#define THERMAL_MON_FILTINTSTS1 0x100000
#define THERMAL_MON_FILTINTSTS2 0x200000


#define THERMAL_tri_SPM_State0	0x20000000
#define THERMAL_tri_SPM_State1	0x40000000
#define THERMAL_tri_SPM_State2	0x80000000


#define THERMAL_MSRCTL0_MASK    0x00000007
#define THERMAL_MSRCTL1_MASK    0x00000038
#define THERMAL_MSRCTL2_MASK    0x000001C0

#endif	/* __TSCPU_SETTINGS_H__ */
