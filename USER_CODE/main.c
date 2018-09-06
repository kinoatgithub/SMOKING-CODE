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
	static u16 now_data, his_data, diff;
	static KINO_DIRECTION public_dir, now_dir, his_dir;
	static u8 tide_count, suck_count;
	if( status != SET )
	{
		public_dir = KINO_UP;													//升温升压
		suck_count = 0;
	}
	his_dir = now_dir;
	his_data = now_data;
	now_data = ( u16 ) BUFFER_AVG( &POWER_SHEET );
	if( his_data < now_data )
	{
		diff = now_data - his_data;
		now_dir = KINO_UP;
	}
	else
	{
		diff = his_data - now_data;
		now_dir = KINO_DOWN;
	}
	if( KINO_ACTIVE_SLOPE < diff )
	{
		if( now_dir == his_dir )
		{
			tide_count++;
		}
		else
		{
			tide_count = 1;
		}
	}
	else
	{
		tide_count = 0;
	}
	if( KINO_TIDE_HYSTERESIS_VOLUME <= tide_count )
	{
		if( public_dir == KINO_DOWN )
		{
			if( now_dir == KINO_UP )
			{
				suck_count++;
			}
		}
		public_dir = now_dir;
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
		if( ! ( BUTTON_PORT->IDR & BUTTON_PIN ) )
		{
			BitAction step = RESET;
			SYS_STATUS = SYS_ADJING;
			TIM2_PWM_MODE_SWITCH( FOR_HEATER );
			HEATER_CONTROL( HEATER_PWM );
			LED_SHOW( RED_LIGHT, LED_ON );
			ADC1->CR1 |= ADC1_CR1_ADON;											//预启动
			while( ! ( BUTTON_PORT->IDR & BUTTON_PIN ) );
			while( 1 )
			{
				if( HEATER_TIDE_TIME_COUNT == 0 )
				{//隔一段时间采集一次电热片电压变化并改变占空比
					TARGET_POWER = ((( u32 ) BUFFER_AVG( &KNOB ) ) * KINO_ADJ_FACTOR ) + KINO_ADJ_BASE_POWER ;
					HEATER_POWER_ADJ( TARGET_POWER, BUFFER_AVG( &POWER_SHEET ) );
					HEATER_TIDE_TIME_COUNT = KINO_TIDE_SAMPLE_TIME;
				}
				if( !( BUTTON_PORT->IDR & BUTTON_PIN ) )
				{
					while( ! ( BUTTON_PORT->IDR & BUTTON_PIN ) );
					FLASH_Unlock( FLASH_MEMTYPE_DATA );
					if( step == 0 )
					{
						FLASH_ProgramWord( KINO_HEAT_LPOWER_ADD, TARGET_POWER );
						LED_SHOW( BLU_LIGHT, LED_ON );
						step = SET;
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
							step = RESET;
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
	BUTTON_ACTIVE_FLAG = DISABLE;												//取消调节期間的觸發狀態
	while( 1 )
	{
		static u32 temp;
		if( !( BUTTON_PORT->IDR & BUTTON_PIN ) )
		{//按按钮状态
			if( BUTTON_HOLD_TIME == 0 )
			{//长按后强制停机
				if( SYS_STATUS != WAITING )
				{
					MOTO_TIME_COUNT	= KINO_MOTO_SHORT;							//震机
					SYS_STATUS = WAITING;
				}
			}
		}
		if( CHARGE_SIGNAL_PORT->IDR & CHARGE_SIGNAL_PIN )
		{
			TIM2_PWM_MODE_SWITCH( FOR_LED );
			LED_SHOW( GRE_LIGHT, LED_BREATH );
			HEATER_CONTROL( HEATER_OFF );
			ADC1_PRE_CONVER( ADC1_BAT_VOL_CH );									//指定需要用到ADC的通道
			ADC1->CR1 |= ADC1_CR1_ADON;											//开始采样电池电压
			SYS_STATUS = CHARGING;
		}
		switch ( SYS_STATUS )
		{
			case WAITING:
			{//等待状态
				TIM2_PWM_MODE_SWITCH( FOR_LED );								//选择LED_PWM模式
				HEATER_CONTROL( HEATER_OFF );
				BAT_POWER_COLLECT();											//收集数据以备用
				switch ( BAT_VOL_LEVEL() )										//没事就更新一下电量，免得电量卡在了回滞区间后显示错误电量
				{
					case NO_POWER:
					{
						LED_SHOW( RED_LIGHT, LED_SMASH );
						LED_PWM_DUTY_SET( KINO_LED_RED_BRIGHT_ADJ );			//设置亮度
						break;
					}
					case LOW_POWER:
					{
						LED_SHOW( (LED_COLORTYPE)( RED_LIGHT | GRE_LIGHT ), LED_SMASH );
						LED_PWM_DUTY_SET( KINO_LED_RED_BRIGHT_ADJ );			//设置亮度
						break;
					}
					default:
					{
						LED_SHOW( GRE_LIGHT, LED_SMASH );
						LED_PWM_DUTY_SET( KINO_LED_GRE_BRIGHT_ADJ );			//设置亮度
						break;
					}
				}
				if( SYS_SLEEP_TIME == 0 )
				{
					LED_SHOW( NON_LIGHT, LED_OFF );
					halt();
				}
				if( BUTTON_ACTIVE_FLAG != DISABLE )
				{
					BUTTON_ACTIVE_FLAG = DISABLE;
					if( BAT_VOL_LEVEL() != NO_POWER )
					{//电量足够，允许下一步
						MOTO_TIME_COUNT = KINO_MOTO_SHORT;						//马达起振
						MAX_HEAT_TIME_COUNT = KINO_PRE_HEAT_TIME;				//设置预热时长
						HEATER_CONTROL( HEATER_ON );							//开始加热
						LED_SHOW( BLU_LIGHT, LED_ON );
						SYS_STATUS = MAX_HEATING;
					}
					else
					{
						LED_SHOW( ALL_LIGHT, LED_OFF );							//选择红灯,闪完后是关闭状态
						LED_BLINK_TIMES = KINO_LED_BLINK_TIMES * 2 * \
										  KINO_LED_BLINK_PERIOD;				//LED闪烁数次
						while( LED_BLINK_TIMES != 0 );
						halt();
					}
				}
				break;
			}
			case CHARGING:
			{//充电中
				while( CHARGE_SIGNAL_PORT->IDR & CHARGE_SIGNAL_PIN )
				{//正在连接电源状态
					HEATER_CONTROL( HEATER_OFF );
					if( BAT_VOL_LEVEL() == FULL_POWER )
						LED_SHOW( GRE_LIGHT, LED_ON );
					else
						LED_SHOW( GRE_LIGHT, LED_BREATH );
					ADC1->CR1 |= ADC1_CR1_ADON;									//触发采样电池电压
				}
				BUTTON_ACTIVE_FLAG = DISABLE;									//取消充電期間的觸發狀態
				SYS_STATUS = WAITING;
				break;
			}
			case MAX_HEATING:
			{//全功率加热
				if( MAX_HEAT_TIME_COUNT == 0 )
				{//满功率预热时间到，准备跳转到PWM模式
					ADC1_PRE_CONVER( ADC1_BAT_VOL_CH );							//指定需要用到ADC的通道
					TIM2_PWM_MODE_SWITCH( FOR_HEATER );
					HEATER_CONTROL( HEATER_PWM );
					TARGET_POWER = POWER_LEVEL_COUNTDOWN();						//设置目标功率
					SMOKING_TIME = KINO_USER_SMOKE_TIME;						//用户吸烟可用时间
					GET_SUCK_COUNT( RESET );									//重置吸烟口数
					MAX_HEAT_TIME_COUNT = KINO_HEATER_READY_TIME;				//延时后震机
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
						FLASH_ProgramByte( KINO_TOBACCO_DOWNCOUNT_ADD, 0 );
						LED_SHOW( BLU_LIGHT, LED_ON );
					}
					else
					{
						FLASH_ProgramByte( KINO_TOBACCO_DOWNCOUNT_ADD, KINO_TOBACCO_MAX_COUNT );
						LED_SHOW( ALL_LIGHT, LED_ON );
					}
					FLASH_Lock( FLASH_MEMTYPE_DATA );
					TARGET_POWER = POWER_LEVEL_COUNTDOWN();						//更新目标功率
				}
				if( MAX_HEAT_TIME_COUNT <= KINO_MOTO_SHORT )
				{
					if( MAX_HEAT_TIME_COUNT == 0 )
						MOTO_TIME_COUNT = 1;
					else
						MOTO_TIME_COUNT	= KINO_MOTO_SHORT;						//震机
				}
				if( HEATER_TIDE_TIME_COUNT == 0 )
				{//隔一段时间采集一次电热片电压变化并改变占空比
					HEATER_TIDE_TIME_COUNT = KINO_TIDE_SAMPLE_TIME;
					HEATER_POWER_ADJ( TARGET_POWER, BUFFER_AVG( &POWER_SHEET ) );//功率自动调节
					if( GET_SUCK_COUNT( SET ) < KINO_TOBACOO_MAX_SUCK_TIMES )
					{																//还没吸够口数
						if( SMOKING_TIME != 0 )
						{															//吸烟时长未用完
							break;
						}
					}
				}
				else
				{
					break;
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
				SYS_SLEEP_TIME = KINO_TIME_TO_SLEEP;
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