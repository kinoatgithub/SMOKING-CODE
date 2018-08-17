#ifndef __USER_HEATER_H
#define __USER_HEATER_H

#include "stm8s.h"
#include "USER_CONFIG.h"

typedef enum
{
	HEATER_OFF	=	0x00,
	HEATER_PWM	=	0x01,
	HEATER_ON	=	0x02
}HEATER_MODE_TYPE;

extern HEATER_MODE_TYPE HEATER_MODE;

void HEATER_POWER_ADJ( u32 power );
void HEATER_CONTROL( HEATER_MODE_TYPE mode );
u32 POWER_LEVEL_COUNTDOWN( void );



#endif