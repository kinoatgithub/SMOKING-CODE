#include "USER_ADC.h"
#include "stm8s_adc1.h"
#include "USER_HEATER.h"
#include "stm8s_itc.h"

u32 TARGET_POWER;
struct POWER_SHEET_TYPE POWER_SHEET;
BAT_POWER_STATUS_TYPE BAT_POWER_LEVEL;

u16 buffer_avger( void );

#pragma vector = ITC_IRQ_ADC1 + 2
__interrupt void ADC_CONVER_FINISH( void )
{
	u16 x,y;
	/* Read MSB first*/
    x = ADC1->DRH;
    /* Then read LSB */
    y = ADC1->DRL;
	y = ( x << 2 ) + y;
	ADC1_ClearFlag( ADC1_FLAG_EOC );
	POWER_SHEET.vol[ POWER_SHEET.ptr++ % KINO_ADC_DATE_VOLUME ] = y;		
	POWER_SHEET.avg = buffer_avger();
	if( HEATER_MODE == HEATER_PWM )
	{//功率自动调节
		HEATER_POWER_ADJ( TARGET_POWER );
	}
	else
	{//电量判断
		if( POWER_SHEET.avg < KINO_BAT_NO_VOL )
		{
			BAT_POWER_LEVEL = NO_POWER;
		}
		else if( POWER_SHEET.avg < KINO_BAT_LOW_VOL )
		{
			BAT_POWER_LEVEL = LOW_POWER;
		}
		else if( POWER_SHEET.avg < KINO_BAT_MANY_VOL )
		{
			BAT_POWER_LEVEL = MANY_POWER;
		}
		else
		{
			BAT_POWER_LEVEL = FULL_POWER;
		}
	}
	
}

u16 buffer_avger()
{
	static u8	i;
	static u16	sum;
	sum = 0;
	for( i = 0; i < KINO_ADC_DATE_VOLUME; i++ )
	{
		sum += POWER_SHEET.vol[ i ];
	}
	return sum / KINO_ADC_DATE_VOLUME;
}

void KINO_ADC_INIT()
{	
	ADC1->CR1 &= ~ADC1_CR1_CONT;													//单次转换模式
	ADC1->CR1 |= ADC1_PRESSEL_FCPU_D2;											//设置ADC1时钟相对于cpu时钟的分频量
	ADC1->CR2 |= ADC1_ALIGN_LEFT;												//输出结果左对齐
	ADC1_SchmittTriggerConfig( ADC1_SCHMITTTRIG_CHANNEL5, DISABLE );			//关闭该通道的施密特触发器以节能
	ADC1_SchmittTriggerConfig( ADC1_SCHMITTTRIG_CHANNEL6, DISABLE );			//关闭该通道的施密特触发器以节能
	ADC1_ITConfig( ADC1_IT_EOCIE, ENABLE );										//允许产生中断
}

void ADC1_PRE_CONVER( u8 channel )
{//ADC转换准备
	ADC1->CSR |= channel;
}

void BAT_POWER_COLLECT( void )
{
	HEATER_CONTROL( HEATER_OFF );												//关了电热片才能采集电池电量
	ADC1_PRE_CONVER( ADC1_BAT_VOL_CH );											//指定需要用到ADC的通道
	ADC1->CR1 |= ADC1_CR1_ADON;													//开始采样电池电压
}