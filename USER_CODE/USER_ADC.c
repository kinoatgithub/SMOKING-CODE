#include "USER_ADC.h"
#include "stm8s_adc1.h"
#include "USER_HEATER.h"
#include "stm8s_itc.h"
#include "USER_GPIO.h"

u32 TARGET_POWER;
POWER_SHEET_TYPE POWER_SHEET, KNOB;
BAT_POWER_STATUS_TYPE BAT_POWER_LEVEL;

float BUFFER_AVG( POWER_SHEET_TYPE* p )
{
	u8	i;
	u16	sum = 0;
	for( i = 0; i < KINO_ADC_DATE_VOLUME; i++ )
	{
		sum += p->vol[ i ];
	}
	return ( ( float ) sum ) / KINO_ADC_DATE_VOLUME;
}

BAT_POWER_STATUS_TYPE BAT_VOL_LEVEL( void )
{
	u16 vol;
	static BAT_POWER_STATUS_TYPE bat_status = MANY_POWER;
	vol = ( u16 ) BUFFER_AVG( &POWER_SHEET );
	if( vol < KINO_BAT_LOW_VOL )
	{
		if( vol < KINO_BAT_NO_VOL )
		{
			if( vol < ( KINO_BAT_NO_VOL - KINO_BAT_HYS ) )
			{
				bat_status = NO_POWER;
			}
		}
		else
		{
			if( vol < ( KINO_BAT_LOW_VOL - KINO_BAT_HYS ) )
			{
				bat_status = LOW_POWER;
			}
		}
	}
	else
	{
		if( vol < KINO_BAT_MANY_VOL )
		{
			if( vol < ( KINO_BAT_MANY_VOL - KINO_BAT_HYS ) )
			{
				bat_status = MANY_POWER;
			}
		}
		else
		{
			bat_status = FULL_POWER;
		}
	}
	return bat_status;
}

#pragma vector = ITC_IRQ_ADC1 + 2
__interrupt void ADC_CONVER_FINISH( void )
{
	static bool step;
	u8 drh, drl;
	drh = ADC1->DRH;															//Read DRH first
	drl = ADC1->DRL;															//Then read DRL
	ADC1_ClearFlag( ADC1_FLAG_EOC );
	if( SYS_STATUS == SYS_ADJING )
	{
		step = ( bool ) ! step;
		if( step != TRUE )
		{
			KNOB.vol[ KNOB.ptr++ % KINO_ADC_DATE_VOLUME ] = ( drh << 2 ) + drl;
			ADC1_PRE_CONVER( ADC1_BAT_VOL_CH );									//下个周期采集电热片ad
			return;
		}
		else
		{
			ADC1_PRE_CONVER( ADC1_R_ADJ_VOL_CH );								//下个周期采集可调电阻ad
		}
	}
	POWER_SHEET.vol[ POWER_SHEET.ptr++ % KINO_ADC_DATE_VOLUME ] = ( drh << 2 ) + drl;	
}

void KINO_ADC_INIT()
{	
	ADC1->CR1 &= ~ADC1_CR1_CONT;												//激活ADC
	ADC1->CR1 |= ADC1_PRESSEL_FCPU_D2;											//设置ADC1时钟相对于cpu时钟的分频量
	ADC1->CR2 |= ADC1_ALIGN_LEFT;												//输出结果左对齐
	ADC1->TDRL &= ~( BAT_VOL_PIN | R_ADJ_VOL_PIN );								//关闭ad通道的施密特触发器以提高精度
	ADC1->TDRL |= BAT_VOL_PIN | R_ADJ_VOL_PIN;									//关闭ad通道的施密特触发器以提高精度
	ADC1_ITConfig( ADC1_IT_EOCIE, ENABLE );										//允许产生中断
}

void ADC1_PRE_CONVER( u8 channel )
{//ADC转换前指定通道
	ADC1->CSR &= (uint8_t)(~ADC1_CSR_CH);
	ADC1->CSR |= (uint8_t)channel;
}

void BAT_POWER_COLLECT( void )
{
	HEATER_CONTROL( HEATER_OFF );												//关了电热片才能采集电池电量
	ADC1_PRE_CONVER( ADC1_BAT_VOL_CH );											//指定需要用到ADC的通道
	ADC1->CR1 |= ADC1_CR1_ADON;													//开始采样电池电压
}