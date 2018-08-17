#include "USER_LED.h"
#include "USER_GPIO.h"

/**
 *加载需要显示的颜色
 */
void DISPLAY_COLOR_SELECT( LED_COLORTYPE color )
{
	static LED_COLORTYPE last_color;
	if( last_color != color )
	{
		last_color = color;
		GPIO_WriteHigh( BLU_LED_PORT, BLU_LED_PIN );
		GPIO_WriteHigh( RED_LED_PORT, RED_LED_PIN );
		GPIO_WriteHigh( GRE_LED_PORT, GRE_LED_PIN );
		if( color & RED_LIGHT )
			GPIO_WriteLow( RED_LED_PORT, RED_LED_PIN );
		if( color & GRE_LIGHT )
			GPIO_WriteLow( GRE_LED_PORT, GRE_LED_PIN );
		if( color & BLU_LIGHT )
			GPIO_WriteLow( BLU_LED_PORT, BLU_LED_PIN );
	}
}

void LED_PWM_DUTY_SET( float duty )
{
	if( duty < 1 )
		TIM2_SetCompare3((uint16_t)( KINO_LED_PWM_PERIOD * duty ));
	else
		TIM2_SetCompare3((uint16_t)( KINO_LED_PWM_PERIOD ));
}

void LED_BREATHING( void )
{
	static KINO_DIRECTION dir;
	static float duty;
	if( duty < KINO_LED_PWM_MIN_DUTY )																//设置最低亮度。
	{
		duty = KINO_LED_PWM_MIN_DUTY;
		dir = KINO_UP;
	}
	if( duty > KINO_LED_PWM_MAX_DUTY )															//当大于1时即设置高亮保持时间。
	{
		duty = 1;
		dir = KINO_DOWN;
	}
	if( dir != KINO_UP )
	{
		duty = duty / KINO_LED_BLOOM_BASE;
	}
	else
	{
		duty = duty * KINO_LED_BLOOM_BASE;
	}
	LED_PWM_DUTY_SET( duty );
}

void LED_BREATHING_SWITCH( FunctionalState State )
{
	TIM2_ITConfig( TIM2_IT_CC3, State );										//LED_PWM允许/禁止产生中断，中断后会呼吸效果
}

void LED_SHOW( LED_COLORTYPE COLOR, LED_DISPLAY_MODE_TYPE mode )
{
	static LED_DISPLAY_MODE_TYPE last_mode;
	DISPLAY_COLOR_SELECT( COLOR );
	if( last_mode != mode )
	{
		last_mode = mode;
		switch ( mode )
		{
			case LED_SMASH:
				LED_BREATHING_SWITCH( DISABLE );								
				GPIO_WriteHigh( LED_COM_PORT, LED_COM_PIN );
				TIM2_CCxCmd( LED_CC_CHANNEL, ENABLE );							//LED_PWM输出启用
				break;
			case LED_BREATH:
				LED_BREATHING_SWITCH( ENABLE );								
				GPIO_WriteHigh( LED_COM_PORT, LED_COM_PIN );
				TIM2_CCxCmd( LED_CC_CHANNEL, ENABLE );							//LED_PWM输出启用
				break;
			case LED_ON:
				LED_BREATHING_SWITCH( DISABLE );
				GPIO_WriteLow( LED_COM_PORT, LED_COM_PIN );
				TIM2_CCxCmd( LED_CC_CHANNEL, DISABLE );							//LED_PWM输出停用
				break;
			case LED_OFF:
			default:
				LED_BREATHING_SWITCH( DISABLE );
				GPIO_WriteHigh( LED_COM_PORT, LED_COM_PIN );
				TIM2_CCxCmd( LED_CC_CHANNEL, DISABLE );							//LED_PWM输出停用
				break;
		}
	}
}

