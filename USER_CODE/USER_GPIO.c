#include "USER_GPIO.h"
#include "USER_TIMER.h"
#include "USER_CONFIG.h"

u16 BUTTON_TRIGER_TIMES;
SYS_STATUS_TYPE SYS_STATUS;
FunctionalState BUTTON_ACTIVE_FLAG;

#pragma vector = ITC_IRQ_PORTC + 2
__interrupt void GPIOC_BUTTON_IRQ( void )										//debug@kino BUTTON_TRIGER_TIMES添加自加的地方
{
	static u16 ptr;																//对于此函数外部来说ptr指向下一个将被填充的buffer				//@kino debug
	static u16 BUTTON_TIME_BUFF[ KINO_BUT_BUF_NUM ];
	SYS_SLEEP_TIME = KINO_TIME_TO_SLEEP;										//点一下玩一年
	BUTTON_HOLD_TIME = KINO_BUT_HOLD_MAX_TIME;
	if( BUTTON_TRIGER_TIMES - BUTTON_TIME_BUFF[( ptr - 1 ) % KINO_BUT_BUF_NUM] \
		> KINO_BUT_DIS_ACT_TIME )
	{//两次触发时间间隔没有粘连
		BUTTON_TIME_BUFF[ ptr++ % KINO_BUT_BUF_NUM ] = BUTTON_TRIGER_TIMES;
	}
	else
	{
		BUTTON_TIME_BUFF[( ptr - 1 ) % KINO_BUT_BUF_NUM ] = BUTTON_TRIGER_TIMES;
	}
	if(( BUTTON_TIME_BUFF[ ( ptr - 1 ) % KINO_BUT_BUF_NUM ] - \
		 BUTTON_TIME_BUFF[ ( ptr - KINO_BUT_ACT_COUNT ) % \
		 KINO_BUT_BUF_NUM ] ) < KINO_BUT_ACT_TIME )
	{//检测到在规定时间内有效连续触发按钮，按钮激活成功
		for( ptr = KINO_BUT_BUF_NUM; ptr != 0; ptr-- )
			BUTTON_TIME_BUFF[ ptr - 1 ] = 0;
		BUTTON_TRIGER_TIMES = 0;
		BUTTON_ACTIVE_FLAG = ENABLE;
	}
}

#pragma vector = ITC_IRQ_PORTD + 2
__interrupt void GPIOD_CHARGE_IRQ( void )
{
	SYS_STATUS = PRE_CHARGING;
}

void gpio_interrupt_init( void )
{
//	ITC_SetSoftwarePriority( ITC_IRQ_PORTC, ITC_PRIORITYLEVEL_3 );				//设置触发沿前必须先把该中断顶到最高级-中文参考手册10.9.3
	EXTI->CR1 |= 0x10;															//设置GPIOC为仅上升沿触发
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
	GPIO_Init( HEATER_PWM_PORT, HEATER_PWM_PIN, GPIO_MODE_OUT_PP_LOW_FAST );
	
	/*GPO*/
	GPIO_Init( RED_LED_PORT, RED_LED_PIN, GPIO_MODE_OUT_OD_HIZ_SLOW );
	GPIO_Init( GRE_LED_PORT, GRE_LED_PIN, GPIO_MODE_OUT_OD_HIZ_SLOW );
	GPIO_Init( BLU_LED_PORT, BLU_LED_PIN, GPIO_MODE_OUT_OD_HIZ_SLOW );
	GPIO_Init( BLU_LED_PORT, BLU_LED_PIN, GPIO_MODE_OUT_OD_HIZ_SLOW );
	GPIO_Init( MOTO_PORT, MOTO_PIN, GPIO_MODE_OUT_PP_LOW_SLOW );
	
	/*INTERRUPT_INPUT*/
	GPIO_Init( BUTTON_PORT, BUTTON_PIN, GPIO_MODE_IN_FL_IT );
	GPIO_Init( CHARGE_SIGNAL_PORT, CHARGE_SIGNAL_PIN, GPIO_MODE_IN_FL_IT );
	
	/*NORMAL_INPUT*/
	GPIO_Init( SYS_ADJ_PORT, SYS_ADJ_PIN, GPIO_MODE_IN_FL_NO_IT );
	
	/*ADC*/
	GPIO_Init( BAT_VOL_PORT, BAT_VOL_PIN, GPIO_MODE_IN_FL_NO_IT );
	GPIO_Init( R_ADJ_VOL_PORT, R_ADJ_VOL_PIN, GPIO_MODE_IN_FL_NO_IT );	
}






