#include "USER_GPIO.h"
#include "USER_TIMER.h"
#include "USER_CONFIG.h"

u16 BUTTON_TRIGER_TIMES;
SYS_STATUS_TYPE SYS_STATUS;
FunctionalState BUTTON_ACTIVE_FLAG;

#pragma vector = ITC_IRQ_PORTC + 2
__interrupt void GPIOC_BUTTON_IRQ( void )
{
	static u8 ptr;																//对于此函数外部来说ptr指向下一个将被填充的buffer
	static u8 i;
	static u16 buff[ KINO_BUT_ACT_COUNT ];
	static u16 time;
	BUTTON_HOLD_TIME = KINO_BUT_HOLD_MAX_TIME;
	SYS_SLEEP_TIME = KINO_TIME_TO_SLEEP;										//点玩年
	time = BUTTON_TRIGER_TIMES;													//固定时间以防被篡改
	if( time != 0 )
	{//已经开始记时
		if( KINO_BUT_DIS_ACT_TIME < ( time - buff[ ptr - 1 ] ) )
		{//两次按键触发没有粘连
			buff[ ptr++ ] = time;
		}
		if( KINO_BUT_ACT_COUNT == ptr )
		{//达到触发次数
			if( KINO_BUT_ACT_MS < buff[ KINO_BUT_ACT_COUNT - 1 ] )
			{//连击超时
				ptr--;
				BUTTON_TRIGER_TIMES -= buff[ 1 ];
				for( i = 1; i < KINO_BUT_ACT_COUNT; i++ )
				{
					buff[ i - 1 ] = buff[ i ] - buff[ 1 ];
				}
			}
			else
			{//连击有效
				ptr = 0;
				BUTTON_TRIGER_TIMES = 0;
				BUTTON_ACTIVE_FLAG = ENABLE;
			}
		}
	}
	else
	{//初始化
		BUTTON_TRIGER_TIMES = 1;
		buff[ 0 ] = 0;
		ptr = 1;
	}
}

#pragma vector = ITC_IRQ_PORTD + 2
__interrupt void GPIOD_CHARGE_IRQ( void )
{
	SYS_SLEEP_TIME = KINO_TIME_TO_SLEEP;										//点玩年
	SYS_STATUS = WAITING;													
}

void gpio_interrupt_init( void )
{
//	ITC_SetSoftwarePriority( ITC_IRQ_PORTC, ITC_PRIORITYLEVEL_3 );				//设置触发沿前必须先把该中断顶到最高级-中文参考手册10.9.3
	EXTI->CR1 |= 0x20;															//设置GPIOC为仅下降沿触发
//	ITC_SetSoftwarePriority( ITC_IRQ_PORTC, ITC_PRIORITYLEVEL_2 );				//恢复优先级
//	ITC_SetSoftwarePriority( ITC_IRQ_PORTD, ITC_PRIORITYLEVEL_3 );				//设置触发沿前必须先把该中断顶到最高级-中文参考手册10.9.3
	EXTI->CR1 |= 0x40;															//设置GPIOD为仅上升沿触发
//	ITC_SetSoftwarePriority( ITC_IRQ_PORTD, ITC_PRIORITYLEVEL_2 );				//恢复优先级
}

void KINO_GPIO_INIT( void )
{
	gpio_interrupt_init();
	
	/*PWM_OUTPUT*/
	GPIO_Init( LED_COM_PORT, LED_COM_PIN, GPIO_MODE_OUT_PP_HIGH_FAST );
	GPIO_Init( HEATER_PWM_PORT, HEATER_PWM_PIN, GPIO_MODE_OUT_PP_LOW_SLOW );
	
	/*GPO*/
	GPIO_Init( RED_LED_PORT, RED_LED_PIN, GPIO_MODE_OUT_OD_HIZ_SLOW );
	GPIO_Init( GRE_LED_PORT, GRE_LED_PIN, GPIO_MODE_OUT_OD_HIZ_SLOW );
	GPIO_Init( BLU_LED_PORT, BLU_LED_PIN, GPIO_MODE_OUT_OD_HIZ_SLOW );
	GPIO_Init( MOTO_PORT, MOTO_PIN, GPIO_MODE_OUT_PP_LOW_SLOW );
	
	/*INTERRUPT_INPUT*/
	GPIO_Init( BUTTON_PORT, BUTTON_PIN, GPIO_MODE_IN_PU_IT );
	GPIO_Init( CHARGE_SIGNAL_PORT, CHARGE_SIGNAL_PIN, GPIO_MODE_IN_FL_IT );
	
	/*NORMAL_INPUT*/
	GPIO_Init( SYS_ADJ_PORT, SYS_ADJ_PIN, GPIO_MODE_IN_PU_NO_IT );
	
	/*ADC*/
	GPIO_Init( BAT_VOL_PORT, BAT_VOL_PIN, GPIO_MODE_IN_FL_NO_IT );
	GPIO_Init( R_ADJ_VOL_PORT, R_ADJ_VOL_PIN, GPIO_MODE_IN_FL_NO_IT );	
}






