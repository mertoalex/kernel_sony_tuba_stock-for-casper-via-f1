/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program
 * If not, see <http://www.gnu.org/licenses/>.
 */
/*******************************************************************************
 *
 * Filename:
 * ---------
 *  mt_sco_afe_connection.c
 *
 * Project:
 * --------
 *   MT6797  Audio Driver Kernel Function
 *
 * Description:
 * ------------
 *   Audio register
 *
 * Author:
 * -------
 * Chipeng Chang
 *
 *------------------------------------------------------------------------------
 *
 *******************************************************************************/


/*****************************************************************************
 *                     C O M P I L E R   F L A G S
 *****************************************************************************/


/*****************************************************************************
 *                E X T E R N A L   R E F E R E N C E S
 *****************************************************************************/

#include "AudDrv_Common.h"
#include "AudDrv_Def.h"
#include "AudDrv_Afe.h"
#include "AudDrv_Ana.h"
#include "AudDrv_Clk.h"
#include "mt_soc_afe_connection.h"
#include "mt_soc_afe_control.h"
#include "mt_soc_digital_type.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/completion.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/vmalloc.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/sched.h>
#include <linux/wakelock.h>
#include <linux/semaphore.h>
#include <linux/jiffies.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/mutex.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/div64.h>

/* mutex lock */
static DEFINE_MUTEX(afe_connection_mutex);

/**
* connection of register
*/
const uint32 mConnectionReg[Soc_Aud_InterConnectionOutput_Num_Output] = {
	AFE_CONN0, AFE_CONN1, AFE_CONN2, AFE_CONN3, AFE_CONN4,
	AFE_CONN5, AFE_CONN6, AFE_CONN7, AFE_CONN8, AFE_CONN9,
	AFE_CONN10, AFE_CONN11, AFE_CONN12, AFE_CONN13, AFE_CONN14,
	AFE_CONN15, AFE_CONN16, AFE_CONN17, AFE_CONN18, AFE_CONN19,
	AFE_CONN20, AFE_CONN21, AFE_CONN22, AFE_CONN23, AFE_CONN24,
	AFE_CONN25, AFE_CONN26, AFE_CONN27, AFE_CONN28, AFE_CONN29,
	AFE_CONN30, AFE_CONN31, AFE_CONN32, AFE_CONN33
	};

/**
* connection state of register
*/
static char mConnectionState[Soc_Aud_InterConnectionInput_Num_Input]
	[Soc_Aud_InterConnectionOutput_Num_Output] = { {0} };

typedef bool (*connection_function)(uint32);

bool SetDl1ToI2s0(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I05,
			Soc_Aud_InterConnectionOutput_O00);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I06,
			Soc_Aud_InterConnectionOutput_O01);
	return true;
}

bool SetDl1ToI2s1Dac(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I05,
			Soc_Aud_InterConnectionOutput_O03);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I06,
			Soc_Aud_InterConnectionOutput_O04);
	return true;
}

bool SetI2s2Adc2ToVulData2(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I23,
			Soc_Aud_InterConnectionOutput_O21);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I24,
			Soc_Aud_InterConnectionOutput_O22);
	return true;
}

bool SetI2s2AdcToVul(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I03,
			Soc_Aud_InterConnectionOutput_O09);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I04,
			Soc_Aud_InterConnectionOutput_O10);
	return true;
}

bool SetDl1ToI2s1Dac2(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I05,
			Soc_Aud_InterConnectionOutput_O28);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I06,
			Soc_Aud_InterConnectionOutput_O29);
	return true;
}

bool SetDl1ToAwb(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I05,
			Soc_Aud_InterConnectionOutput_O05);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I06,
			Soc_Aud_InterConnectionOutput_O06);
	return true;
}

bool SetDl2ToAwb(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I07,
			Soc_Aud_InterConnectionOutput_O05);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I08,
			Soc_Aud_InterConnectionOutput_O06);
	return true;
}

bool SetDl1ToDaiBtOut(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I05,
			Soc_Aud_InterConnectionOutput_O02);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I06,
			Soc_Aud_InterConnectionOutput_O02);
	return true;
}

bool SetModem1InCh1ToI2s3(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I09,
			Soc_Aud_InterConnectionOutput_O00);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I09,
			Soc_Aud_InterConnectionOutput_O01);
	return true;
}

bool SetModem2InCh1ToI2s3(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I14,
			Soc_Aud_InterConnectionOutput_O00);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I14,
			Soc_Aud_InterConnectionOutput_O01);
	return true;
}

bool SetI2s0Ch2ToModem1OutCh4(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I01,
			Soc_Aud_InterConnectionOutput_O27);
	return true;
}

bool SetI2s0Ch2ToModem2OutCh4(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I01,
			Soc_Aud_InterConnectionOutput_O24);
	return true;
}

bool SetDl2ToI2s1Dac(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I07,
			Soc_Aud_InterConnectionOutput_O03);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I08,
			Soc_Aud_InterConnectionOutput_O04);
	return true;
}

bool SetDl2ToI2s1Dac2(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I07,
			Soc_Aud_InterConnectionOutput_O28);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I08,
			Soc_Aud_InterConnectionOutput_O29);
	return true;
}

bool SetDl2ToVul(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I07,
			Soc_Aud_InterConnectionOutput_O09);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I08,
			Soc_Aud_InterConnectionOutput_O10);
	return true;
}

bool SetI2s0ToHwGain1Out(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I00,
			Soc_Aud_InterConnectionOutput_O13);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I01,
			Soc_Aud_InterConnectionOutput_O14);
	return true;
}

bool SetConnsysToHwGain1Out(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I25,
			Soc_Aud_InterConnectionOutput_O13);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I26,
			Soc_Aud_InterConnectionOutput_O14);
	return true;
}

bool SetConnsysToAwb(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I25,
			Soc_Aud_InterConnectionOutput_O05);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I26,
			Soc_Aud_InterConnectionOutput_O06);
	return true;
}

bool SetHwGain1InToI2s1Dac(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I10,
			Soc_Aud_InterConnectionOutput_O03);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I11,
			Soc_Aud_InterConnectionOutput_O04);
	return true;
}

bool SetHwGain1InToI2s1Dac2(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I10,
			Soc_Aud_InterConnectionOutput_O28);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I11,
			Soc_Aud_InterConnectionOutput_O29);
	return true;
}

bool SetHwGain1InToI2s3(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I10,
			Soc_Aud_InterConnectionOutput_O00);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I11,
			Soc_Aud_InterConnectionOutput_O01);
	return true;
}

bool SetI2s0ToAwb(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I00,
			Soc_Aud_InterConnectionOutput_O05);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I01,
			Soc_Aud_InterConnectionOutput_O06);
	return true;
}

bool SetModem2InCh1ToModemDai(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I14,
			Soc_Aud_InterConnectionOutput_O12);
	return true;
}

bool SetModem1InCh1ToModemDai(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I09,
			Soc_Aud_InterConnectionOutput_O12);
	return true;
}

bool SetModem2InCh1ToI2s1Dac2(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I14,
			Soc_Aud_InterConnectionOutput_O28);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I14,
			Soc_Aud_InterConnectionOutput_O29);
	return true;
}

bool SetModem2InCh2ToI2s1Dac2(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I21,
			Soc_Aud_InterConnectionOutput_O28);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I21,
			Soc_Aud_InterConnectionOutput_O29);
	return true;
}

bool SetModem1InCh1ToI2s1Dac2(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I09,
			Soc_Aud_InterConnectionOutput_O28);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I09,
			Soc_Aud_InterConnectionOutput_O29);
	return true;
}

bool SetModem1InCh2ToI2s1Dac2(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I22,
			Soc_Aud_InterConnectionOutput_O28);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I22,
			Soc_Aud_InterConnectionOutput_O29);
	return true;
}

bool SetDl1Ch1ToModem2OutCh4(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I05,
			Soc_Aud_InterConnectionOutput_O24);
	return true;
}

bool SetDl1ToHwGain1Out(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I15,
			Soc_Aud_InterConnectionOutput_O13);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I16,
			Soc_Aud_InterConnectionOutput_O14);
	return true;
}

bool SetMrgI2sInToHwGain1Out(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I15,
			Soc_Aud_InterConnectionOutput_O13);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I16,
			Soc_Aud_InterConnectionOutput_O14);
	return true;
}

bool SetMrgI2sInToAwb(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I15,
			Soc_Aud_InterConnectionOutput_O05);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I16,
			Soc_Aud_InterConnectionOutput_O06);
	return true;
}

bool SetI2s2AdcToI2s3(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I03,
			Soc_Aud_InterConnectionOutput_O00);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I04,
			Soc_Aud_InterConnectionOutput_O01);
	return true;
}

bool SetI2s2AdcToI2s1Dac(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I03,
			Soc_Aud_InterConnectionOutput_O03);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I04,
			Soc_Aud_InterConnectionOutput_O04);
	return true;
}

bool SetI2s2AdcToI2s1Dac2(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I03,
			Soc_Aud_InterConnectionOutput_O28);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I04,
			Soc_Aud_InterConnectionOutput_O29);
	return true;
}

bool SetI2s2AdcCh1ToI2s3(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I03,
			Soc_Aud_InterConnectionOutput_O00);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I03,
			Soc_Aud_InterConnectionOutput_O01);
	return true;
}

bool SetI2s2AdcCh1ToI2s1Dac(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I03,
			Soc_Aud_InterConnectionOutput_O03);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I03,
			Soc_Aud_InterConnectionOutput_O04);
	return true;
}

bool SetI2s2AdcCh1ToI2s1Dac2(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I03,
			Soc_Aud_InterConnectionOutput_O28);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I03,
			Soc_Aud_InterConnectionOutput_O29);
	return true;
}

bool SetI2s2AdcToModem2Out(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I03,
			Soc_Aud_InterConnectionOutput_O17);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I04,
			Soc_Aud_InterConnectionOutput_O18);
	return true;
}

bool SetModem2InCh1ToI2s1Dac(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I14,
			Soc_Aud_InterConnectionOutput_O03);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I14,
			Soc_Aud_InterConnectionOutput_O04);
	return true;
}

bool SetDaiBtInToModem2Out(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I02,
			Soc_Aud_InterConnectionOutput_O17);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I02,
			Soc_Aud_InterConnectionOutput_O18);
	return true;
}

bool SetModem2InCh1ToDaiBtOut(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I14,
			Soc_Aud_InterConnectionOutput_O02);
	return true;
}

bool SetI2s2AdcToModem1Out(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I03,
			Soc_Aud_InterConnectionOutput_O07);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I04,
			Soc_Aud_InterConnectionOutput_O08);
	return true;
}

bool SetModem1InCh1ToI2s1Dac(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I09,
			Soc_Aud_InterConnectionOutput_O03);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I09,
			Soc_Aud_InterConnectionOutput_O04);
	return true;
}

bool SetDaiBtInToModem1Out(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I02,
			Soc_Aud_InterConnectionOutput_O07);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I02,
			Soc_Aud_InterConnectionOutput_O08);
	return true;
}

bool SetModem1InCh1ToDaiBtOut(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I09,
			Soc_Aud_InterConnectionOutput_O02);
	return true;
}

bool SetModem2InCh1ToAwbCh1(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I14,
			Soc_Aud_InterConnectionOutput_O05);
	return true;
}

bool SetModem1InCh1ToAwbCh1(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I09,
			Soc_Aud_InterConnectionOutput_O05);
	return true;
}

bool SetI2s0ToVul(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I00,
			Soc_Aud_InterConnectionOutput_O09);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I01,
			Soc_Aud_InterConnectionOutput_O10);
	return true;
}

bool SetDl1ToMrgI2sOut(uint32 ConnectionState)
{
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I05,
			Soc_Aud_InterConnectionOutput_O00);
	SetConnectionState(ConnectionState, Soc_Aud_InterConnectionInput_I06,
			Soc_Aud_InterConnectionOutput_O01);
	return true;
}

typedef struct connection_link_t {
	uint32 input;
	uint32 output;
	connection_function connectionFunction;
} connection_link_t;

static const connection_link_t mConnectionLink[] = {
	{Soc_Aud_AFE_IO_Block_MEM_DL1, Soc_Aud_AFE_IO_Block_I2S3, SetDl1ToI2s0},
	{Soc_Aud_AFE_IO_Block_MEM_DL1, Soc_Aud_AFE_IO_Block_I2S1_DAC, SetDl1ToI2s1Dac},
	{Soc_Aud_AFE_IO_Block_I2S2_ADC_2, Soc_Aud_AFE_IO_Block_MEM_VUL_DATA2, SetI2s2Adc2ToVulData2},
	{Soc_Aud_AFE_IO_Block_I2S2_ADC, Soc_Aud_AFE_IO_Block_MEM_VUL, SetI2s2AdcToVul},
	{Soc_Aud_AFE_IO_Block_MEM_DL1, Soc_Aud_AFE_IO_Block_I2S1_DAC_2, SetDl1ToI2s1Dac2},
	{Soc_Aud_AFE_IO_Block_MEM_DL1, Soc_Aud_AFE_IO_Block_MEM_AWB, SetDl1ToAwb},
	{Soc_Aud_AFE_IO_Block_MEM_DL2, Soc_Aud_AFE_IO_Block_MEM_AWB, SetDl2ToAwb},
	{Soc_Aud_AFE_IO_Block_MEM_DL1, Soc_Aud_AFE_IO_Block_DAI_BT_OUT, SetDl1ToDaiBtOut},
	{Soc_Aud_AFE_IO_Block_MODEM_PCM_1_I_CH1, Soc_Aud_AFE_IO_Block_I2S3, SetModem1InCh1ToI2s3},
	{Soc_Aud_AFE_IO_Block_MODEM_PCM_2_I_CH1, Soc_Aud_AFE_IO_Block_I2S3, SetModem2InCh1ToI2s3},
	{Soc_Aud_AFE_IO_Block_I2S0_CH2, Soc_Aud_AFE_IO_Block_MODEM_PCM_1_O_CH4, SetI2s0Ch2ToModem1OutCh4},
	{Soc_Aud_AFE_IO_Block_I2S0_CH2, Soc_Aud_AFE_IO_Block_MODEM_PCM_2_O_CH4, SetI2s0Ch2ToModem2OutCh4},
	{Soc_Aud_AFE_IO_Block_MEM_DL2, Soc_Aud_AFE_IO_Block_I2S1_DAC, SetDl2ToI2s1Dac},
	{Soc_Aud_AFE_IO_Block_MEM_DL2, Soc_Aud_AFE_IO_Block_I2S1_DAC_2, SetDl2ToI2s1Dac2},
	{Soc_Aud_AFE_IO_Block_MEM_DL2, Soc_Aud_AFE_IO_Block_MEM_VUL, SetDl2ToVul},
	{Soc_Aud_AFE_IO_Block_I2S0, Soc_Aud_AFE_IO_Block_HW_GAIN1_OUT, SetI2s0ToHwGain1Out},
	{Soc_Aud_AFE_IO_Block_I2S_CONNSYS, Soc_Aud_AFE_IO_Block_HW_GAIN1_OUT, SetConnsysToHwGain1Out},
	{Soc_Aud_AFE_IO_Block_HW_GAIN1_IN, Soc_Aud_AFE_IO_Block_I2S1_DAC, SetHwGain1InToI2s1Dac},
	{Soc_Aud_AFE_IO_Block_HW_GAIN1_IN, Soc_Aud_AFE_IO_Block_I2S1_DAC_2, SetHwGain1InToI2s1Dac2},
	{Soc_Aud_AFE_IO_Block_HW_GAIN1_IN, Soc_Aud_AFE_IO_Block_I2S3, SetHwGain1InToI2s3},
	{Soc_Aud_AFE_IO_Block_I2S0, Soc_Aud_AFE_IO_Block_MEM_AWB, SetI2s0ToAwb},
	{Soc_Aud_AFE_IO_Block_I2S_CONNSYS, Soc_Aud_AFE_IO_Block_MEM_AWB, SetConnsysToAwb},
	{Soc_Aud_AFE_IO_Block_MODEM_PCM_2_I_CH1, Soc_Aud_AFE_IO_Block_MEM_MOD_DAI, SetModem2InCh1ToModemDai},
	{Soc_Aud_AFE_IO_Block_MODEM_PCM_1_I_CH1, Soc_Aud_AFE_IO_Block_MEM_MOD_DAI, SetModem1InCh1ToModemDai},
	{Soc_Aud_AFE_IO_Block_MODEM_PCM_2_I_CH1, Soc_Aud_AFE_IO_Block_I2S1_DAC_2, SetModem2InCh1ToI2s1Dac2},
	{Soc_Aud_AFE_IO_Block_MODEM_PCM_2_I_CH2, Soc_Aud_AFE_IO_Block_I2S1_DAC_2, SetModem2InCh2ToI2s1Dac2},
	{Soc_Aud_AFE_IO_Block_MODEM_PCM_1_I_CH1, Soc_Aud_AFE_IO_Block_I2S1_DAC_2, SetModem1InCh1ToI2s1Dac2},
	{Soc_Aud_AFE_IO_Block_MODEM_PCM_1_I_CH2, Soc_Aud_AFE_IO_Block_I2S1_DAC_2, SetModem1InCh2ToI2s1Dac2},
	{Soc_Aud_AFE_IO_Block_MEM_DL1_CH1, Soc_Aud_AFE_IO_Block_MODEM_PCM_2_O_CH4, SetDl1Ch1ToModem2OutCh4},
	{Soc_Aud_AFE_IO_Block_MEM_DL1, Soc_Aud_AFE_IO_Block_HW_GAIN1_OUT, SetDl1ToHwGain1Out},
	{Soc_Aud_AFE_IO_Block_MRG_I2S_IN, Soc_Aud_AFE_IO_Block_HW_GAIN1_OUT, SetMrgI2sInToHwGain1Out},
	{Soc_Aud_AFE_IO_Block_MRG_I2S_IN, Soc_Aud_AFE_IO_Block_MEM_AWB, SetMrgI2sInToAwb},
	{Soc_Aud_AFE_IO_Block_I2S2_ADC, Soc_Aud_AFE_IO_Block_I2S3, SetI2s2AdcToI2s3},
	{Soc_Aud_AFE_IO_Block_I2S2_ADC, Soc_Aud_AFE_IO_Block_I2S1_DAC, SetI2s2AdcToI2s1Dac},
	{Soc_Aud_AFE_IO_Block_I2S2_ADC, Soc_Aud_AFE_IO_Block_I2S1_DAC_2, SetI2s2AdcToI2s1Dac2},
	{Soc_Aud_AFE_IO_Block_I2S2_ADC_CH1, Soc_Aud_AFE_IO_Block_I2S3, SetI2s2AdcCh1ToI2s3},
	{Soc_Aud_AFE_IO_Block_I2S2_ADC_CH1, Soc_Aud_AFE_IO_Block_I2S1_DAC, SetI2s2AdcCh1ToI2s1Dac},
	{Soc_Aud_AFE_IO_Block_I2S2_ADC_CH1, Soc_Aud_AFE_IO_Block_I2S1_DAC_2, SetI2s2AdcCh1ToI2s1Dac2},
	{Soc_Aud_AFE_IO_Block_I2S2_ADC, Soc_Aud_AFE_IO_Block_MODEM_PCM_2_O, SetI2s2AdcToModem2Out},
	{Soc_Aud_AFE_IO_Block_MODEM_PCM_2_I_CH1, Soc_Aud_AFE_IO_Block_I2S1_DAC, SetModem2InCh1ToI2s1Dac},
	{Soc_Aud_AFE_IO_Block_DAI_BT_IN, Soc_Aud_AFE_IO_Block_MODEM_PCM_2_O, SetDaiBtInToModem2Out},
	{Soc_Aud_AFE_IO_Block_MODEM_PCM_2_I_CH1, Soc_Aud_AFE_IO_Block_DAI_BT_OUT, SetModem2InCh1ToDaiBtOut},
	{Soc_Aud_AFE_IO_Block_I2S2_ADC, Soc_Aud_AFE_IO_Block_MODEM_PCM_1_O, SetI2s2AdcToModem1Out},
	{Soc_Aud_AFE_IO_Block_MODEM_PCM_1_I_CH1, Soc_Aud_AFE_IO_Block_I2S1_DAC, SetModem1InCh1ToI2s1Dac},
	{Soc_Aud_AFE_IO_Block_DAI_BT_IN, Soc_Aud_AFE_IO_Block_MODEM_PCM_1_O, SetDaiBtInToModem1Out},
	{Soc_Aud_AFE_IO_Block_MODEM_PCM_1_I_CH1, Soc_Aud_AFE_IO_Block_DAI_BT_OUT, SetModem1InCh1ToDaiBtOut},
	{Soc_Aud_AFE_IO_Block_MODEM_PCM_2_I_CH1, Soc_Aud_AFE_IO_Block_MEM_AWB_CH1, SetModem2InCh1ToAwbCh1},
	{Soc_Aud_AFE_IO_Block_MODEM_PCM_1_I_CH1, Soc_Aud_AFE_IO_Block_MEM_AWB_CH1, SetModem1InCh1ToAwbCh1},
	{Soc_Aud_AFE_IO_Block_I2S0, Soc_Aud_AFE_IO_Block_MEM_VUL, SetI2s0ToVul},
	{Soc_Aud_AFE_IO_Block_MEM_DL1, Soc_Aud_AFE_IO_Block_MRG_I2S_OUT, SetDl1ToMrgI2sOut}
};
static const int CONNECTION_LINK_NUM = sizeof(mConnectionLink) / sizeof(mConnectionLink[0]);

static bool CheckBitsandReg(short regaddr, char bits)
{
	if (regaddr <= 0 || bits < 0) {
		pr_debug("regaddr = %x bits = %d\n", regaddr, bits);
		return false;
	}
	return true;
}

uint32 GetConnectionShiftReg(uint32 Output)
{
	if (Soc_Aud_InterConnectionOutput_O32 > Output)
		return AFE_CONN_RS;
	else
		return AFE_CONN_RS1;
}

uint32 GetConnectionShiftOffset(uint32 Output)
{
	if (Soc_Aud_InterConnectionOutput_O32 > Output)
		return Output;
	else
		return (Output - Soc_Aud_InterConnectionOutput_O32);
}

bool SetConnectionState(uint32 ConnectionState, uint32 Input, uint32 Output)
{
	/* printk("SetinputConnection ConnectionState = %d
	Input = %d Output = %d\n", ConnectionState, Input, Output); */
	int connectReg = 0;

	switch (ConnectionState) {
	case Soc_Aud_InterCon_DisConnect:
	{
		/* printk("nConnectionState = %d\n", ConnectionState); */
		if ((mConnectionState[Input][Output] & Soc_Aud_InterCon_Connection)
			== Soc_Aud_InterCon_Connection) {

			/* here to disconnect connect bits */
			connectReg = mConnectionReg[Output];
			if (CheckBitsandReg(connectReg, Input)) {
				Afe_Set_Reg(connectReg, 0, 1 << Input);
				mConnectionState[Input][Output] &= ~(Soc_Aud_InterCon_Connection);
			}
		}
		if ((mConnectionState[Input][Output] & Soc_Aud_InterCon_ConnectionShift)
			== Soc_Aud_InterCon_ConnectionShift) {

			/* here to disconnect connect shift bits */
			if (CheckBitsandReg(AFE_CONN_RS, Input)) {
				Afe_Set_Reg(AFE_CONN_RS, 0, 1 << Input);
				mConnectionState[Input][Output] &= ~(Soc_Aud_InterCon_ConnectionShift);
			}
		}
		break;
	}
	case Soc_Aud_InterCon_Connection:
	{
		/* printk("nConnectionState = %d\n", ConnectionState); */
		connectReg = mConnectionReg[Output];
		if (CheckBitsandReg(connectReg, Input)) {
			Afe_Set_Reg(connectReg, 1 << Input, 1 << Input);
			mConnectionState[Input][Output] |= Soc_Aud_InterCon_Connection;
		}
		break;
	}
	case Soc_Aud_InterCon_ConnectionShift:
	{
		/* printk("nConnectionState = %d\n", ConnectionState); */
		uint32 shiftReg = GetConnectionShiftReg(Output);
		uint32 shiftOffset = GetConnectionShiftOffset(Output);

		if (CheckBitsandReg(shiftReg, Input)) {
			Afe_Set_Reg(shiftReg, 1 << shiftOffset, 1 << shiftOffset);
			mConnectionState[Input][Output] |= Soc_Aud_InterCon_ConnectionShift;
		}
		break;
	}
	default:
		pr_err("no this state ConnectionState = %d\n", ConnectionState);
		break;
	}

	return true;
}
EXPORT_SYMBOL(SetConnectionState);

connection_function GetConnectionFunction(uint32 Aud_block_In, uint32 Aud_block_Out)
{
	connection_function connectionFunction = 0;
	int i = 0;

	for (i = 0; i < CONNECTION_LINK_NUM; i++) {
		if ((mConnectionLink[i].input == Aud_block_In) &&
				(mConnectionLink[i].output == Aud_block_Out)) {
			return mConnectionLink[i].connectionFunction;
		}
	}
	return connectionFunction;
}

bool SetIntfConnectionState(uint32 ConnectionState, uint32 Aud_block_In, uint32 Aud_block_Out)
{
	bool ret = false;
	connection_function connectionFunction = GetConnectionFunction(Aud_block_In, Aud_block_Out);

	if (0 == connectionFunction) {
		pr_warn("no this connection function\n");
		return ret;
	}
	return connectionFunction(ConnectionState);
}
EXPORT_SYMBOL(SetIntfConnectionState);

bool SetIntfConnectionFormat(uint32 ConnectionFormat, uint32 Aud_block)
{
	switch (Aud_block) {
	case Soc_Aud_AFE_IO_Block_I2S3:
	{
		SetoutputConnectionFormat(ConnectionFormat,
				Soc_Aud_InterConnectionOutput_O00);
		SetoutputConnectionFormat(ConnectionFormat,
				Soc_Aud_InterConnectionOutput_O01);
		break;
	}
	case Soc_Aud_AFE_IO_Block_I2S1_DAC:
	{
		SetoutputConnectionFormat(ConnectionFormat,
				Soc_Aud_InterConnectionOutput_O03);
		SetoutputConnectionFormat(ConnectionFormat,
				Soc_Aud_InterConnectionOutput_O04);
		break;
	}
	case Soc_Aud_AFE_IO_Block_MEM_VUL:
	{
		SetoutputConnectionFormat(ConnectionFormat,
				Soc_Aud_InterConnectionOutput_O09);
		SetoutputConnectionFormat(ConnectionFormat,
				Soc_Aud_InterConnectionOutput_O10);
		break;
	}
	case Soc_Aud_AFE_IO_Block_MEM_VUL_DATA2:
	{
		SetoutputConnectionFormat(ConnectionFormat,
				Soc_Aud_InterConnectionOutput_O21);
		SetoutputConnectionFormat(ConnectionFormat,
				Soc_Aud_InterConnectionOutput_O22);
		break;
	}
	case Soc_Aud_AFE_IO_Block_DAI_BT_OUT:
	{
		SetoutputConnectionFormat(ConnectionFormat,
				Soc_Aud_InterConnectionOutput_O02);
		break;
	}
	case Soc_Aud_AFE_IO_Block_I2S1_DAC_2:
	{
		SetoutputConnectionFormat(ConnectionFormat,
				Soc_Aud_InterConnectionOutput_O28);
		SetoutputConnectionFormat(ConnectionFormat,
				Soc_Aud_InterConnectionOutput_O29);
		break;
	}
	case Soc_Aud_AFE_IO_Block_MEM_MOD_DAI:
	{
		SetoutputConnectionFormat(ConnectionFormat,
				Soc_Aud_InterConnectionOutput_O12);
		break;
	}
	case Soc_Aud_AFE_IO_Block_MEM_AWB:
	{
		SetoutputConnectionFormat(ConnectionFormat,
				Soc_Aud_InterConnectionOutput_O05);
		SetoutputConnectionFormat(ConnectionFormat,
				Soc_Aud_InterConnectionOutput_O06);
		break;
	}
	case Soc_Aud_AFE_IO_Block_MRG_I2S_OUT:
	{
		SetoutputConnectionFormat(ConnectionFormat,
				Soc_Aud_InterConnectionOutput_O00);
		SetoutputConnectionFormat(ConnectionFormat,
				Soc_Aud_InterConnectionOutput_O01);
		break;
	}
	default:
		pr_warn("no this Aud_block = %d\n", Aud_block);
		break;
	}
	return true;
}
EXPORT_SYMBOL(SetIntfConnectionFormat);

