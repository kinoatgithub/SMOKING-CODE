#include "USER_TIMER.h"
#include "stm8s_tim1.h"
#include "stm8s_tim2.h"
#include "USER_CONFIG.h"
#include "USER_LED.h"
#include "stm8s_itc.h"
#include "USER_GPIO.h"

u32	HEATER_TIDE_TIME_COUNT, \
	BUTTON_HOLD_TIME, \
	LED_BLINK_TIMES, \
	MOTO_TIME_COUNT, \
	MAX_HEAT_TIME_COUNT, \
	SMOKING_TIME, \
	SYS_ADJ_WINDOW_TIME, \
	SYS_SLEEP_TIME;

#pragma vector = ITC_IRQ_TIM1_OVF + 2
__interrupt void TIM1_OVF_IRQ( void )
{
	TIM1_ClearFlag( TIM1_FLAG_UPDATE );
	if( BUTTON_HOLD_TIME != 0 )
	{
		BUTTON_HOLD_TIME--;
	}
	if( LED_BLINK_TIMES != 0 )
	{
		if( ( LED_BLINK_TIMES % KINO_LED_BLINK_PERIOD ) == 0 )
		{
			GPIO_WriteReverse( LED_COM_PORT, LED_COM_PIN );
		}
		LED_BLINK_TIMES--;
	}
	if( MOTO_TIME_COUNT != 0 )
	{
		MOTO_TIME_COUNT--;
		if( MOTO_TIME_COUNT == 0 )
		{
			GPIO_WriteLow( MOTO_PORT, MOTO_PIN );
		}
		else
		{
			GPIO_WriteHigh( MOTO_PORT, MOTO_PIN );
		}
	}
	else
	{
		
	}
	if( MAX_HEAT_TIME_COUNT != 0 )
	{
		MAX_HEAT_TIME_COUNT--;
	}
	if( SMOKING_TIME != 0 )
	{
		SMOKING_TIME--;
	}
	if( HEATER_TIDE_TIME_COUNT != 0 )
	{
		HEATER_TIDE_TIME_COUNT--;
	}
	if( SYS_ADJ_WINDOW_TIME != 0 )
	{
		SYS_ADJ_WINDOW_TIME--;
	}
	if( SYS_SLEEP_TIME != 0 )
	{
		SYS_SLEEP_TIME--;
	}
	if( BUTTON_TRIGER_TIMES != 0 )
	{
		BUTTON_TRIGER_TIMES++;
		if( KINO_BUT_ACT_MS < BUTTON_TRIGER_TIMES )
		{
			BUTTON_TRIGER_TIMES = 0;
		}
	}
}

#pragma vector = ITC_IRQ_TIM2_CAPCOM + 2
__interrupt void TIM2_CAPCOM_IRQ( void )
{
	if( TIM2_GetITStatus( TIM2_IT_CC1 ) != RESET )
	{//激活adc
		ADC1->CR1 |= ADC1_CR1_ADON;												//这步应该是第二次置位ADON，第一次置位ADON用于唤醒ADC
		TIM2->SR1 &= ~TIM2_IT_CC1;
	}
	if( TIM2_GetITStatus( TIM2_IT_CC3 ) != RESET )
	{//调节占空比
		LED_BREATHING();
		TIM2->SR1 &= ~TIM2_IT_CC3;
	}
}

void KINO_PWM_CONFIG( u8 prescaler, u16 period )
{
	TIM2->PSCR	= (uint8_t)prescaler;											//加载分频值
	TIM2_SetAutoreload( period );												//设置周期
//	TIM2_OC3PreloadConfig( ENABLE );											//试试什么用
}

void TIM2_PWM_MODE_SWITCH( HEATER_OR_LED_PWM_MODE mode )
{
	if( mode != FOR_LED )
	{
		TIM2_ITConfig( TIM2_IT_CC3, DISABLE );									//电热片工作时禁止LED_PWM中断
		TIM2_ITConfig( TIM2_IT_CC1, DISABLE );									//先关闭后调整，以免卡在中断出不来
		KINO_PWM_CONFIG( KINO_HEATER_PWM_PRESCALER, KINO_HEATER_PWM_PERIOD );
		TIM2->EGR	= (uint8_t)TIM2_PSCRELOADMODE_IMMEDIATE;					//立即更新TIM2的分频值
		TIM2_CCxCmd( LED_CC_CHANNEL, DISABLE );									//当HEATER_PWM工作时，LED_PWM输出停用
		TIM2_CCxCmd( HEATER_CC_CHANNEL, ENABLE );								//允许HEATER_PWM输出，
		TIM2_ITConfig( TIM2_IT_CC1, ENABLE );									//该中断用于当电热片工作一段固定时间后激活ADC采集电热片电压
	}
	else
	{
		TIM2_ITConfig( TIM2_IT_CC3, DISABLE );									//禁止LED_PWM中断,停止LED呼吸
		TIM2_ITConfig( TIM2_IT_CC1, DISABLE );									//该中断用于当电热片工作一段固定时间后激活ADC采集电热片电压
		KINO_PWM_CONFIG( KINO_LED_PWM_PRESCALER, KINO_LED_PWM_PERIOD );
		TIM2->EGR	= (uint8_t)TIM2_PSCRELOADMODE_IMMEDIATE;					//立即更新TIM2的分频值
		TIM2_CCxCmd( HEATER_CC_CHANNEL, DISABLE );								//当LED_PWM工作时，HEATER_PWM不允许输出
		TIM2_CCxCmd( LED_CC_CHANNEL, ENABLE );									//LED_PWM通道允许输出
	}
}

void KINO_TIM2_INIT( void )
{    
	TIM2_CCxCmd( ADC_TRI_CC_CHANNEL, DISABLE );									//触发ADC用，不允许输出PWM；
	TIM2_OC1Init(//ADC触发通道
					TIM2_OCMODE_PWM2, 											//PWM2 模式
					TIM2_OUTPUTSTATE_DISABLE, 									//暂时禁止pwm输出，这只是公共部分初始化
					0,					 										//设置初始占空比为0%，以防故障
					TIM2_OCPOLARITY_HIGH 										//定义逻辑1为高电平
				);
	TIM2_OC2Init(//电热片通道
					TIM2_OCMODE_PWM2, 											//PWM2 模式
					TIM2_OUTPUTSTATE_DISABLE, 									//暂时禁止pwm输出，这只是公共部分初始化
					0,					 										//设置初始占空比为0%，
					TIM2_OCPOLARITY_LOW 										//定义逻辑1为低电平
				);	
	TIM2_OC3Init(//LED通道
					TIM2_OCMODE_PWM2, 											//PWM2 模式
					TIM2_OUTPUTSTATE_DISABLE, 									//暂时禁止pwm输出，这只是公共部分初始化
					0,					 										//设置初始占空比为0%，以防故障
					TIM2_OCPOLARITY_HIGH 										//定义逻辑1为高电平
				);
	TIM2_ITConfig( TIM2_IT_CC2, DISABLE );										//没想到加热通道PWM捕获中断有什么用
	TIM2_SetCompare1( (uint16_t)KINO_ADC_DELAY );								//设置ADC采样时机
	ITC->ISPR5 &= ~0x03;
	ITC->ISPR5 |= 0x01;															//设置tim2的优先级为1
	TIM2_Cmd( ENABLE );
}

void KINO_TIM1_INIT( void )
{
	TIM1_Cmd( ENABLE );
	TIM1_CounterModeConfig( TIM1_COUNTERMODE_UP );								//向上计数
	TIM1_SetAutoreload( KINO_TIM1_PERIOD );										//中断周期
	TIM1_PrescalerConfig( KINO_TIM1_PRESCALER, TIM1_PSCRELOADMODE_UPDATE );		//立即更新分频量
	TIM1_ITConfig( TIM1_IT_UPDATE, ENABLE );
}




