#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H

#include "stm8s.h"


/*时钟树设置*/
#define KINO_HSI_DIVER					CLK_PRESCALER_HSIDIV8 					//设置hsi的分频量，最高8分频

/*休眠设置*/
#define KINO_TIME_TO_SLEEP_S			5										//设置无操作~秒后休眠

/*出厂功率校准设置窗口时间*/
#define KINO_ADJ_TEMP_TIME				2000									//上电后将有`的毫秒的时间可以触发高低温度设置
#define KINO_ADJ_BASE_POWER				( 178225UL )							//设计为功率为基础值
#define KINO_ADJ_FACTOR					400										//旋钮ad值乘于~作为功率叠加分量
/*定时器1设置*/
#define KINO_TIM1_PRESCALER				10										//TIM1分频量
#define KINO_TIM1_PERIOD				100										//TIM1中断周期

/*定时器2设置*/

/*LED设置*/
#define KINO_LED_PWM_MIN_DUTY			0.05									//设置呼吸模式最小切变占空比
#define KINO_LED_PWM_MAX_DUTY			1.5										//设置呼吸模式最大切变占空比，大于1则为1；
#define KINO_LED_PWM_PRESCALER			1       								//设置分频值，比如当~设为8时，分频值为2^8=256分频
#define KINO_LED_PWM_PERIOD				1000     								//周期设置,每~个计数为一个pwm周期
#define KINO_LED_BLOOM_BASE				1.002									//用于呼吸灯亮度渐变，~作为指数爆炸的底数.
#define KINO_LED_RED_BRIGHT_ADJ			0.5										//红灯常亮亮度调节
#define KINO_LED_GRE_BRIGHT_ADJ			0.5										//绿灯常亮亮度调节
#define KINO_LED_BLU_BRIGHT_ADJ			0.5										//蓝灯常亮亮度调节
#define KINO_LED_BLINK_PERIOD_MS		250										//灯闪烁时反转周期单位毫秒
#define KINO_LED_BLINK_TIMES			3										//灯闪烁时的次数

/*HEATER设置*/
#define KINO_HEATER_PWM_PRESCALER		7       								//设置分频值，比如当~设为8时，分频值为2^8=256分频
#define KINO_HEATER_PWM_PERIOD			400     								//周期设置,每~个计数为一个pwm周期

/*锂电池设置*/
#define KINO_BAT_NO_VOL					615										//设置亏电状态电压最大值，按设计当~=615时电池电压为3.3V
#define KINO_BAT_LOW_VOL				689										//设置低电量状态电压最大值，按设计当~=689时电池电压为3.7V
#define KINO_BAT_MANY_VOL				745										//设置满电状态电压最小值，按设计当~=745时电池电压为4V

/*ADC设置*/
#define KINO_ADC_DATE_VOLUME			8										//ADC数据缓冲区大小,只能是2的幂次方个，~>64则累加器有溢出风险
#define KINO_ADC_DELAY					5										//heater接通电源~个TIM2计时周期后ADC开始采样

/*按钮设置*/
#define KINO_BUT_ACT_COUNT				3										//设置连续触发按钮~次为激活状态
#define KINO_BUT_ACT_MS					2000									//设置连续触发按钮在~毫秒内完成才激活，单位毫秒
#define KINO_BUT_DIS_ACT_MS				200										//两次触发时间间隔少于~毫秒，则合并为一次，单位毫秒，
#define KINO_BUT_HOLD_MAX_TIME_MS		3000									//长按按钮停机时间，单位毫秒

/*马达设置*/
#define KINO_MOTO_SHORT_MS				500										//马达短震单位毫秒
#define KINO_MOTO_LONG_MS				1000									//马达长震单位毫秒

/*吸烟用户个性化设置*/
#define KINO_TOBACCO_MAX_COUNT			80										//最多功率递减倒数80次吸烟
#define KINO_TOBACOO_MAX_SUCK_TIMES		10										//每次恒功率模式最多吸~口烟
#define KINO_USER_SMOKE_TIME_S			200										//恒功率时长单位秒
#define KINO_PRE_HEAT_TIME_MS			10000									//电热片预热时长单位毫秒
#define KINO_TIDE_HYSTERESIS_VOLUME		3										//恒功率模式电压历史纪录容量,连续~次电压单方向超量变化作为事件
#define KINO_ACTIVE_SLOPE				2										//电压变化有效斜率
#define KINO_TIDE_SAMPLE_TIME			200										//恒功率模式电压采样时间间隔单位毫秒

/******************************预处理**********************************/
#if(   KINO_HSI_DIVER == CLK_PRESCALER_HSIDIV8 )
	#define KINO_FMASTER ( HSI_VALUE / 8 )
#elif( KINO_HSI_DIVER == CLK_PRESCALER_HSIDIV4 )
	#define KINO_FMASTER ( HSI_VALUE / 4 )
#elif( KINO_HSI_DIVER == CLK_PRESCALER_HSIDIV2 )
	#define KINO_FMASTER ( HSI_VALUE / 2 )
#elif( KINO_HSI_DIVER == CLK_PRESCALER_HSIDIV1 )
	#define KINO_FMASTER ( HSI_VALUE / 1 )
#endif

#define KINO_TIM1_F				(( KINO_FMASTER / KINO_TIM1_PRESCALER ) / KINO_TIM1_PERIOD )	//TIM1_PWM的频率
#define KINO_TIME_TO_SLEEP		(  KINO_TIM1_F * KINO_TIME_TO_SLEEP_S )							//KINO_TIME_TO_SLEEP_S的时长内包含~个TIM2的PWM周期
#define KINO_USER_SMOKE_TIME	(  KINO_TIM1_F * KINO_USER_SMOKE_TIME_S )
#define KINO_BUT_HOLD_MAX_TIME	(( KINO_TIM1_F / 1000 ) * KINO_BUT_HOLD_MAX_TIME_MS )
#define KINO_LED_BLINK_PERIOD	(( KINO_TIM1_F / 1000 ) * KINO_LED_BLINK_PERIOD_MS )
#define KINO_PRE_HEAT_TIME		(( KINO_TIM1_F / 1000 ) * KINO_PRE_HEAT_TIME_MS )
#define KINO_BUT_DIS_ACT_TIME	(( KINO_TIM1_F / 1000 ) * KINO_BUT_DIS_ACT_MS )					//KINO_BUT_DIS_ACT_MS 时长内包含~个TIM2的PWM周期
#define KINO_BUT_ACT_TIME		(( KINO_TIM1_F / 1000 ) * KINO_BUT_ACT_MS )						//KINO_BUT_ACT_MS 时长内包含~个TIM2的PWM周期
#define KINO_MOTO_SHORT			(( KINO_TIM1_F / 1000 ) * KINO_MOTO_SHORT_MS )
#define KINO_MOTO_LONG			(( KINO_TIM1_F / 1000 ) * KINO_MOTO_LONG_MS )



typedef enum { KINO_DOWN = 0, KINO_UP = !KINO_DOWN } KINO_DIRECTION;

typedef enum
{
	NO_POWER	=	0x00,
	LOW_POWER	=	0x01,
	MANY_POWER	=	0x02,
	FULL_POWER	=	0x03
}BAT_POWER_STATUS_TYPE;

extern BAT_POWER_STATUS_TYPE BAT_POWER_LEVEL;

typedef enum
{
	WAITING			=	0x00,
	PRE_CHARGING	=	0x01,
	CHARGING		=	0x02,
	PRE_MAX_HEAT	=	0x03,
	MAX_HEATING		=	0x04,
	PWM_HEATING		=	0x05
}SYS_STATUS_TYPE;

extern SYS_STATUS_TYPE SYS_STATUS;



#endif