#ifndef __USER_ADC_H
#define __USER_ADC_H

#include "stm8s.h"
#include "USER_CONFIG.h"

#define ADC1_BAT_VOL_CH		ADC1_CHANNEL_6
#define ADC1_R_ADJ_VOL_CH	ADC1_CHANNEL_5

typedef struct
{
	u8		ptr;
	u16		vol[KINO_ADC_DATE_VOLUME];
}POWER_SHEET_TYPE;

extern u32 TARGET_POWER;
extern POWER_SHEET_TYPE POWER_SHEET, KNOB;

void KINO_ADC_INIT( void );
float BUFFER_AVG( POWER_SHEET_TYPE* p );
BAT_POWER_STATUS_TYPE BAT_VOL_LEVEL( void );
void ADC1_PRE_CONVER( u8 channel );
void BAT_POWER_COLLECT( void );



#endif