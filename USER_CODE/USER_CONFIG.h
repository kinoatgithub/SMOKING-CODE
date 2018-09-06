#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H

#include "stm8s.h"


/*ʱ��������*/
#define KINO_HSI_DIVER					CLK_PRESCALER_HSIDIV8 					//����hsi�ķ�Ƶ�������8��Ƶ

/*��������*/
#define KINO_TIME_TO_SLEEP_S			5										//�����޲���~�������

/*��������У׼����*/
#define KINO_ADJ_TEMP_TIME				10000									//�ϵ����`����Ĵ���ʱ����Դ����ߵ��¶�����
#define KINO_ADJ_BASE_POWER				( 62378UL )								//���Ϊ���ʻ���ֵ
#define KINO_ADJ_FACTOR					100										//��ťadֵ����~��Ϊ���ʵ��ӷ���

/*��ʱ��1����*/
#define KINO_TIM1_PRESCALER				10										//TIM1��Ƶ��
#define KINO_TIM1_PERIOD				100										//TIM1�ж�����

/*��ʱ��2����*/

/*LED����*/
#define KINO_LED_PWM_MIN_DUTY			0.05									//���ú���ģʽ��С�б�ռ�ձ�
#define KINO_LED_PWM_MAX_DUTY			1.5										//���ú���ģʽ����б�ռ�ձȣ�����1��Ϊ1��
#define KINO_LED_PWM_PRESCALER			1       								//���÷�Ƶֵ�����統~��Ϊ8ʱ����ƵֵΪ2^8=256��Ƶ
#define KINO_LED_PWM_PERIOD				1000     								//��������,ÿ~������Ϊһ��pwm����
#define KINO_LED_BLOOM_BASE				1.002									//���ں��������Ƚ��䣬~��Ϊָ����ը�ĵ���.
#define KINO_LED_RED_BRIGHT_ADJ			0.5										//��Ƴ������ȵ���
#define KINO_LED_GRE_BRIGHT_ADJ			0.5										//�̵Ƴ������ȵ���
#define KINO_LED_BLU_BRIGHT_ADJ			0.5										//���Ƴ������ȵ���
#define KINO_LED_BLINK_PERIOD_MS		250										//����˸ʱ��ת���ڵ�λ����
#define KINO_LED_BLINK_TIMES			3										//����˸ʱ�Ĵ���

/*HEATER����*/
#define KINO_HEATER_PWM_PRESCALER		4       								//���÷�Ƶֵ�����統~��Ϊ8ʱ����ƵֵΪ2^8=256��Ƶ
#define KINO_HEATER_PWM_PERIOD			6400     								//��������,ÿ~������Ϊһ��pwm����
#define KINO_TIDE_SAMPLE_TIME			100										//�㹦��ģʽ��ѹ����ʱ������λ����
#define KINO_ADJ_CD						50										//~��KINO_TIDE_SAMPLE_TIME������������ݱ䶯����������
#define KINO_BLOOMBER_FACTOR			2500									//����Ƭ��������ϵ��

/*﮵������*/
#define KINO_BAT_HYS					10										//״̬��仯�����»�����
#define KINO_BAT_NO_VOL					615										//���ÿ���״̬��ѹ���ֵ������Ƶ�~=615ʱ��ص�ѹΪ3.3V
#define KINO_BAT_LOW_VOL				689										//���õ͵���״̬��ѹ���ֵ������Ƶ�~=689ʱ��ص�ѹΪ3.7V
#define KINO_BAT_MANY_VOL				860										//��������״̬��ѹ���ֵ������Ƶ�~=745ʱ��ص�ѹΪ4V

/*ADC����*/
#define KINO_ADC_DATE_VOLUME			8										//ADC���ݻ�������С,ֻ����2���ݴη�����~>64���ۼ������������
#define KINO_ADC_DELAY					80										//heater��ͨ��Դ~��TIM2��ʱ���ں�ADC��ʼ����
#define KINO_ADC_WORKTIME				20										//adc��������ʱ��

/*��ť����*/
#define KINO_BUT_ACT_COUNT				3										//��������������ť~��Ϊ����״̬
#define KINO_BUT_ACT_MS					2000									//��������������ť��~��������ɲż����λ����
#define KINO_BUT_DIS_ACT_MS				200										//���δ���ʱ��������~���룬��ϲ�Ϊһ�Σ���λ���룬
#define KINO_BUT_HOLD_MAX_TIME_MS		3000									//������ťͣ��ʱ������λ����

/*�������*/
#define KINO_MOTO_SHORT_MS				500										//������λ����
#define KINO_MOTO_LONG_MS				1000									//��ﳤ��λ����

/*�����û����Ի�����*/
#define KINO_TOBACCO_MAX_COUNT			80										//��๦�ʵݼ�����80������
#define KINO_TOBACOO_MAX_SUCK_TIMES		10										//ÿ�κ㹦��ģʽ�����~����
#define KINO_USER_SMOKE_TIME_S			200										//�㹦��ʱ����λ��
#define KINO_PRE_HEAT_TIME_MS			500										//����ƬԤ��ʱ����λ����
#define KINO_HEATER_READY_TIME_MS		10000									//�㹦��ģʽ�еȴ�~�����Ĭ���¶��ȶ�Ȼ�������
#define KINO_TIDE_HYSTERESIS_VOLUME		3										//�㹦��ģʽ��ѹ��ʷ��¼����,����~�ε�ѹ���������仯��Ϊ�¼�
#define KINO_ACTIVE_SLOPE				1										//��ѹ�仯��Чб��

/******************************Ԥ����**********************************/
#if(   KINO_HSI_DIVER == CLK_PRESCALER_HSIDIV8 )
	#define KINO_FMASTER ( HSI_VALUE / 8 )
#elif( KINO_HSI_DIVER == CLK_PRESCALER_HSIDIV4 )
	#define KINO_FMASTER ( HSI_VALUE / 4 )
#elif( KINO_HSI_DIVER == CLK_PRESCALER_HSIDIV2 )
	#define KINO_FMASTER ( HSI_VALUE / 2 )
#elif( KINO_HSI_DIVER == CLK_PRESCALER_HSIDIV1 )
	#define KINO_FMASTER ( HSI_VALUE / 1 )
#endif

#define KINO_TIM1_F				(( KINO_FMASTER / KINO_TIM1_PRESCALER ) / KINO_TIM1_PERIOD )	//TIM1_PWM��Ƶ��
#define KINO_TIME_TO_SLEEP		(  KINO_TIM1_F * KINO_TIME_TO_SLEEP_S )							//KINO_TIME_TO_SLEEP_S��ʱ���ڰ���~��TIM2��PWM����
#define KINO_USER_SMOKE_TIME	(  KINO_TIM1_F * KINO_USER_SMOKE_TIME_S )
#define KINO_BUT_HOLD_MAX_TIME	(( KINO_TIM1_F / 2000 ) * KINO_BUT_HOLD_MAX_TIME_MS )
#define KINO_LED_BLINK_PERIOD	(( KINO_TIM1_F / 2000 ) * KINO_LED_BLINK_PERIOD_MS )
#define KINO_PRE_HEAT_TIME		(( KINO_TIM1_F / 2000 ) * KINO_PRE_HEAT_TIME_MS )
#define KINO_BUT_DIS_ACT_TIME	(( KINO_TIM1_F / 2000 ) * KINO_BUT_DIS_ACT_MS )					//KINO_BUT_DIS_ACT_MS ʱ���ڰ���~��TIM2��PWM����
#define KINO_BUT_ACT_TIME		(( KINO_TIM1_F / 2000 ) * KINO_BUT_ACT_MS )						//KINO_BUT_ACT_MS ʱ���ڰ���~��TIM2��PWM����
#define KINO_MOTO_SHORT			(( KINO_TIM1_F / 2000 ) * KINO_MOTO_SHORT_MS )
#define KINO_MOTO_LONG			(( KINO_TIM1_F / 2000 ) * KINO_MOTO_LONG_MS )
#define KINO_HEATER_READY_TIME  (( KINO_TIM1_F / 2000 ) * KINO_HEATER_READY_TIME_MS )



typedef enum { KINO_DOWN, KINO_STAY, KINO_UP } KINO_DIRECTION;

typedef enum
{
	NO_POWER,
	LOW_POWER,
	MANY_POWER,
	FULL_POWER
}BAT_POWER_STATUS_TYPE;

//extern BAT_POWER_STATUS_TYPE BAT_POWER_LEVEL;

typedef enum
{
	WAITING,
	CHARGING,
	MAX_HEATING,
	PWM_HEATING,
	SYS_ADJING
}SYS_STATUS_TYPE;

extern SYS_STATUS_TYPE SYS_STATUS;



#endif