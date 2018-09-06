#include "USER_HEATER.h"
#include "USER_ADC.h"
#include "USER_FLASH.h"
#include "USER_GPIO.h"
#include "USER_TIMER.h"

HEATER_MODE_TYPE HEATER_MODE;

u32 overshooter_change( u32 data )
{
	static u16 times;
	static u32 min = U32_MAX, max = 0;
	if( times++ < 3 )
		return data;
	if( times <= KINO_ADJ_CD )
	{
		times++;
	}
	else
	{
//		times = KINO_ADJ_CD;
		min = U32_MAX;
		max = 0;
	}
	if( data < min )
	{
		min = data;
		max = min + 1;
		data += KINO_HEATER_PWM_PERIOD / ( times - 2 );
		times = 0;
	}
	else if( max < data )
	{
		max = data;
		min = max - 1;
		data -= KINO_HEATER_PWM_PERIOD / ( times - 2 );
		times = 0;
	}
	return data;
}

void HEATER_POWER_ADJ( u32 power, float vol_ad )
{
	u16 target_count;
	target_count = ( u16 )( ( ( (float)power / vol_ad ) / vol_ad ) * KINO_HEATER_PWM_PERIOD );
	target_count = overshooter_change( target_count );
	if( KINO_HEATER_PWM_PERIOD < target_count )
	{
		target_count = KINO_HEATER_PWM_PERIOD;
	}
	if( target_count < KINO_ADC_DELAY + KINO_ADC_WORKTIME )
	{//预留给ad采样的时间
		target_count = KINO_ADC_DELAY + KINO_ADC_WORKTIME;
	}
	TIM2_SetCompare2( target_count );
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
		HEATER_MODE = mode;
		switch ( mode )
		{
			case HEATER_OFF:
				GPIO_WriteLow(HEATER_PWM_PORT, HEATER_PWM_PIN);
				TIM2_CCxCmd( HEATER_CC_CHANNEL, DISABLE );								//HEATER_PWM输出停用
				break;
			case HEATER_PWM:
				GPIO_WriteLow(HEATER_PWM_PORT, HEATER_PWM_PIN);
				TIM2_CCxCmd( HEATER_CC_CHANNEL, ENABLE );								//HEATER_PWM输出启用
				break;
			case HEATER_ON:
				GPIO_WriteHigh( HEATER_PWM_PORT, HEATER_PWM_PIN );						//
				TIM2_CCxCmd( HEATER_CC_CHANNEL, DISABLE );								//HEATER_PWM输出停用
				break;
			default:
				break;
		}
	}
}
