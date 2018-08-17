#include "stm8s.h"
#include "USER_ADC.h"
#include "stm8s_clk.h"
#include "USER_LED.h"
#include "USER_GPIO.h"
#include "USER_TIMER.h"
#include "USER_HEATER.h"
#include "USER_FLASH.h"

extern u32 TARGET_POWER;

void KINO_SYS_INIT( void )
{
	CLK_HSIPrescalerConfig( KINO_HSI_DIVER );
	KINO_TIM1_INIT();
	KINO_TIM2_INIT();
	KINO_ADC_INIT();
	KINO_FLASH_INIT();
	KINO_GPIO_INIT();
	rim();
}

u8 GET_SUCK_COUNT( FlagStatus status )
{
	static KINO_DIRECTION dir, public_dir;
	static u8 tide_count, suck_count;
	static u16 tide_his, tide_now;
	if( status != SET )
	{
		suck_count = 0;
		public_dir = KINO_DOWN;													//从全功率过度到恒功率，降温降压
	}
	tide_his = tide_now;
	tide_now = POWER_SHEET.avg;
	if( tide_his < tide_now )
	{
		if( KINO_ACTIVE_SLOPE < ( tide_now - tide_his ) )
		{
			if( dir == KINO_UP )
				tide_count++;
			else
				tide_count = 1;
			dir = KINO_UP;
		}
		else
			tide_count = 0;
	}
	else
	{
		if( KINO_ACTIVE_SLOPE < ( tide_his - tide_now ) )
		{
			if( dir == KINO_DOWN )
				tide_count++;
			else
				tide_count = 1;
			dir = KINO_DOWN;
		}
		else
			tide_count = 0;
	}
	if( KINO_TIDE_HYSTERESIS_VOLUME <= tide_count )
	{
		if( public_dir == KINO_DOWN )
		{
			if( dir == KINO_UP )
			{
				suck_count++;
			}
		}
		public_dir = dir;
	}
	return suck_count;
}

void SYS_POWER_INIT()
{
	SYS_ADJ_WINDOW_TIME = KINO_ADJ_TEMP_TIME;
	ADC1_PRE_CONVER( ADC1_R_ADJ_VOL_CH );										//指定需要用到ADC的通道
	if( KINO_TOBACCO_MAX_COUNT < ( * ( u8 * ) KINO_TOBACCO_DOWNCOUNT_ADD ) )	
	{//防止吸烟周期数不在定义域内
		FLASH_Unlock( FLASH_MEMTYPE_DATA );
		FLASH_ProgramByte( KINO_TOBACCO_DOWNCOUNT_ADD, 0 );
		FLASH_Lock( FLASH_MEMTYPE_DATA );
	}
	while( SYS_ADJ_WINDOW_TIME )
	{
		ADC1->CR1 |= ADC1_CR1_ADON;												//开始采样电池电压
		if( GPIO_ReadInputPin( SYS_ADJ_PORT, SYS_ADJ_PIN ) != RESET )
		{
			u8 step = 0;
			TIM2_PWM_MODE_SWITCH( FOR_HEATER );
			HEATER_CONTROL( HEATER_PWM );
			HEATER_MODE = HEATER_PWM;											//借用一下该模式的PWM自动调节资源
			LED_SHOW( RED_LIGHT, LED_ON );
			while( 1 )
			{
				TARGET_POWER = ( KINO_ADJ_BASE_POWER + POWER_SHEET.avg );
				if( GPIO_ReadInputPin( BUTTON_PORT, BUTTON_PIN ) != RESET )
				{
					FLASH_Unlock( FLASH_MEMTYPE_DATA );
					if( step == 0 )
					{
						FLASH_ProgramWord( KINO_HEAT_LPOWER_ADD, TARGET_POWER );
						LED_SHOW( BLU_LIGHT, LED_ON );
						step = 1;
					}
					else
					{
						if( * ( u32 * )KINO_HEAT_LPOWER_ADD < TARGET_POWER )
						{
							FLASH_ProgramWord( KINO_HEAT_HPOWER_ADD, TARGET_POWER );
							break;
						}
						else
						{
							LED_SHOW( RED_LIGHT, LED_ON );
							step = 0;
						}
					}
					FLASH_Lock( FLASH_MEMTYPE_DATA );
				}
			}
		}
	}
}




int main( void )
{
	KINO_SYS_INIT();
	SYS_POWER_INIT();
	while( 1 )
	{
		static u32 temp;
		if( GPIO_ReadInputPin( BUTTON_PORT, BUTTON_PIN ) != RESET )
		{//按按钮状态
			if( BUTTON_HOLD_TIME == 0 )
			{//计时结束了
				MOTO_TIME_COUNT	= KINO_MOTO_SHORT;								//震机
				SYS_STATUS = WAITING;
			}
		}
		
//		if( GPIO_ReadInputPin( CHARGE_SIGNAL_PORT, CHARGE_SIGNAL_PIN ) != RESET )		//debug@kino 取消充电判断，使充电和没充电状态没有区别
//		{//正在充电
//			if( SYS_STATUS != CHARGING )
//			{
//				SYS_STATUS = PRE_CHARGING;
//			}
//		}
		
		switch ( SYS_STATUS )
		{
			case WAITING:
			{//等待状态
				TIM2_PWM_MODE_SWITCH( FOR_LED );								//选择LED_PWM模式
				LED_SHOW( NON_LIGHT, LED_OFF );
				HEATER_CONTROL( HEATER_OFF );
				BAT_POWER_COLLECT();											//收集数据以备用
				if( BUTTON_ACTIVE_FLAG != DISABLE )
				{
					BUTTON_ACTIVE_FLAG = DISABLE;
					if( BAT_POWER_LEVEL != NO_POWER )
					{
						SYS_STATUS = PRE_MAX_HEAT;								//电量足够，允许下一步
					}
					else
					{
						LED_SHOW( RED_LIGHT, LED_OFF );							//选择红灯,闪完后是关闭状态
						LED_BLINK_TIMES =	KINO_LED_BLINK_TIMES * 2 * \
											KINO_LED_BLINK_PERIOD;				//LED闪烁数次
						while( LED_BLINK_TIMES != 0 );
						halt();
					}
				}
				if( SYS_SLEEP_TIME == 0 )
				{
//					halt();														//debug@kino 暂时取消
				}
				break;
			}
			case PRE_CHARGING:
			{//充电准备
				TIM2_PWM_MODE_SWITCH( FOR_LED );
				LED_SHOW( GRE_LIGHT, LED_BREATH );
				HEATER_CONTROL( HEATER_OFF );
				ADC1_PRE_CONVER( ADC1_BAT_VOL_CH );								//指定需要用到ADC的通道
				ADC1->CR1 |= ADC1_CR1_ADON;										//开始采样电池电压
				SYS_STATUS = CHARGING;
				break;
			}
			case CHARGING:
			{//充电中
				ADC1->CR1 |= ADC1_CR1_ADON;										//触发采样电池电压
				if( GPIO_ReadInputPin( CHARGE_SIGNAL_PORT, CHARGE_SIGNAL_PIN ) != RESET )
				{
					if( BAT_POWER_LEVEL == FULL_POWER )
						LED_SHOW( GRE_LIGHT, LED_ON );
					else
						LED_SHOW( GRE_LIGHT, LED_BREATH );
				}
				else
				{
					SYS_STATUS = WAITING;
				}
				break;
			}
			case PRE_MAX_HEAT:
			{//全功率加热准备
				TIM2_PWM_MODE_SWITCH( FOR_LED );								//选择LED_PWM模式
				if( BAT_POWER_LEVEL != LOW_POWER )
				{//电量正常
					LED_SHOW( BLU_LIGHT, LED_SMASH );							//选择蓝灯,频闪
					LED_PWM_DUTY_SET( KINO_LED_BLU_BRIGHT_ADJ );				//设置亮度
				}
				else
				{//低电量状态
					LED_SHOW( RED_LIGHT, LED_BREATH );							//选择蓝灯,呼吸
				}
				MOTO_TIME_COUNT = KINO_MOTO_SHORT;								//马达起振
				MAX_HEAT_TIME_COUNT = KINO_PRE_HEAT_TIME;						//设置预热时长
				HEATER_CONTROL( HEATER_ON );									//开始加热
				SYS_STATUS = MAX_HEATING;
				break;
			}
			case MAX_HEATING:
			{//全功率加热
				if( MAX_HEAT_TIME_COUNT == 0 )
				{//满功率预热时间到，准备跳转到PWM模式
					MOTO_TIME_COUNT = KINO_MOTO_SHORT;
					ADC1_PRE_CONVER( ADC1_BAT_VOL_CH );								//指定需要用到ADC的通道
					TIM2_PWM_MODE_SWITCH( FOR_HEATER );
					HEATER_CONTROL( HEATER_PWM );
					TARGET_POWER = POWER_LEVEL_COUNTDOWN();						//设置目标功率
					if( ( * ( u8 * ) KINO_TOBACCO_DOWNCOUNT_ADD ) != 0 )		//高温计数情况
					{
						LED_SHOW( ALL_LIGHT, LED_ON );
					}
					SMOKING_TIME = KINO_USER_SMOKE_TIME;						//用户吸烟可用时间
					GET_SUCK_COUNT( RESET );									//重置吸烟口数
					SYS_STATUS = PWM_HEATING;
				}
				break;
			}
			case PWM_HEATING:
			{//恒功率加热
				if( BUTTON_ACTIVE_FLAG != DISABLE )
				{//功率切换
					BUTTON_ACTIVE_FLAG = DISABLE;
					FLASH_Unlock( FLASH_MEMTYPE_DATA );
					if( ( * ( u32 * ) KINO_TOBACCO_DOWNCOUNT_ADD ) != 0 )
					{
						FLASH_ProgramByte( KINO_TOBACCO_DOWNCOUNT_ADD, KINO_TOBACCO_MAX_COUNT );
					}
					else
					{
						FLASH_ProgramByte( KINO_TOBACCO_DOWNCOUNT_ADD, 0 );
					}
					FLASH_Lock( FLASH_MEMTYPE_DATA );
					TARGET_POWER = POWER_LEVEL_COUNTDOWN();						//更新目标功率
				}
				if( HEATER_TIDE_TIME_COUNT == 0 )
				{
					HEATER_TIDE_TIME_COUNT = KINO_TIDE_SAMPLE_TIME;
					if( GET_SUCK_COUNT( SET ) < KINO_TOBACOO_MAX_SUCK_TIMES )		
						break;													//还没吸够口数
				}
				else
				{
					if( SMOKING_TIME != 0 )
						break;													//吸烟时长未用完
				}
				HEATER_CONTROL( HEATER_OFF );
				temp =  * ( u8 * ) KINO_TOBACCO_DOWNCOUNT_ADD;
				if( temp != 0 )
				{
					FLASH_Unlock( FLASH_MEMTYPE_DATA );
					FLASH_ProgramByte( KINO_TOBACCO_DOWNCOUNT_ADD, temp - 1 );
					FLASH_Lock( FLASH_MEMTYPE_DATA );
				}
				MOTO_TIME_COUNT = KINO_MOTO_LONG;								//马达起振
				LED_SHOW( NON_LIGHT, LED_OFF );
				SYS_STATUS = WAITING;
				break;
			}
			default :
				SYS_STATUS = WAITING;
				break;
		}
	}
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif