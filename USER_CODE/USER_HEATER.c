#include "USER_HEATER.h"
#include "USER_ADC.h"
#include "USER_FLASH.h"
#include "USER_GPIO.h"

HEATER_MODE_TYPE HEATER_MODE;

void HEATER_PWM_DUTY_SET( float duty )
{
	if( duty < 1 )
		TIM2_SetCompare2((uint16_t)( KINO_HEATER_PWM_PERIOD * duty ));
	else
		TIM2_SetCompare2((uint16_t)( KINO_HEATER_PWM_PERIOD + 1 ));
}

void HEATER_POWER_ADJ( u32 power )
{
	float target_duty;
	target_duty = ( (float)power / POWER_SHEET.avg ) / POWER_SHEET.avg;
	HEATER_PWM_DUTY_SET( target_duty );
}

u32 POWER_LEVEL_COUNTDOWN( void )												//根据吸烟倒计次数，得到相应的功率
{
	u8 down_count;
	u32 max_power, min_power;
	max_power = * ( u32 * ) KINO_HEAT_HPOWER_ADD;
	min_power = * ( u32 * ) KINO_HEAT_LPOWER_ADD;
	down_count = * ( u8 * ) KINO_TOBACCO_DOWNCOUNT_ADD;
	if( max_power <= min_power )
	{
		return KINO_ADJ_BASE_POWER;
	}
	return	(((	
		 	max_power - min_power ) *	\
			down_count ) / 				\
			KINO_TOBACCO_MAX_COUNT ) + 	\
			min_power;
}

void HEATER_CONTROL( HEATER_MODE_TYPE mode )
{
	static HEATER_MODE_TYPE last_mode;
	if( last_mode != mode )
	{
		last_mode = mode;
		switch ( mode )
		{
			case HEATER_OFF:
				HEATER_MODE = HEATER_OFF;
				GPIO_WriteLow(HEATER_PWM_PORT, HEATER_PWM_PIN);
				TIM2_CCxCmd( HEATER_CC_CHANNEL, DISABLE );								//HEATER_PWM输出停用
				break;
			case HEATER_PWM:
				HEATER_MODE = HEATER_PWM;
				GPIO_WriteLow(HEATER_PWM_PORT, HEATER_PWM_PIN);
				TIM2_CCxCmd( HEATER_CC_CHANNEL, ENABLE );								//HEATER_PWM输出启用
				break;
			case HEATER_ON:
				HEATER_MODE = HEATER_ON;
				GPIO_WriteHigh( HEATER_PWM_PORT, HEATER_PWM_PIN );						//
				TIM2_CCxCmd( HEATER_CC_CHANNEL, DISABLE );								//HEATER_PWM输出停用
				break;
			default:
				break;
		}
	}
}
