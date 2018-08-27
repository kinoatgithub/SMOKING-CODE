#ifndef __USER_ADC_H
#define __USER_ADC_H

#include "stm8s.h"
#include "USER_CONFIG.h"

#define ADC1_BAT_VOL_CH		ADC1_CHANNEL_6
#define ADC1_R_ADJ_VOL_CH	ADC1_CHANNEL_5

struct POWER_SHEET_TYPE
{
	u8		ptr;
	u16		avg;
	u16		vol[KINO_ADC_DATE_VOLUME];
};

extern u32 TARGET_POWER;
extern struct POWER_SHEET_TYPE POWER_SHEET;

void KINO_ADC_INIT();
void ADC1_PRE_CONVER( u8 channel );
void BAT_POWER_COLLECT( void );



#endif