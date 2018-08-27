#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H

#include "stm8s.h"


/*ʱ��������*/
#define KINO_HSI_DIVER					CLK_PRESCALER_HSIDIV8 					//����hsi�ķ�Ƶ�������8��Ƶ

/*��������*/
#define KINO_TIME_TO_SLEEP_S			5										//�����޲���~�������

/*��������У׼���ô���ʱ��*/
#define KINO_ADJ_TEMP_TIME				2000									//�ϵ����`�ĺ����ʱ����Դ����ߵ��¶�����
#define KINO_ADJ_BASE_POWER				( 178225UL )							//���Ϊ����Ϊ����ֵ
#define KINO_ADJ_FACTOR					400										//��ťadֵ����~��Ϊ���ʵ��ӷ���
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
#define KINO_HEATER_PWM_PRESCALER		7       								//���÷�Ƶֵ�����統~��Ϊ8ʱ����ƵֵΪ2^8=256��Ƶ
#define KINO_HEATER_PWM_PERIOD			400     								//��������,ÿ~������Ϊһ��pwm����

/*﮵������*/
#define KINO_BAT_NO_VOL					615										//���ÿ���״̬��ѹ���ֵ������Ƶ�~=615ʱ��ص�ѹΪ3.3V
#define KINO_BAT_LOW_VOL				689										//���õ͵���״̬��ѹ���ֵ������Ƶ�~=689ʱ��ص�ѹΪ3.7V
#define KINO_BAT_MANY_VOL				745										//��������״̬��ѹ��Сֵ������Ƶ�~=745ʱ��ص�ѹΪ4V

/*ADC����*/
#define KINO_ADC_DATE_VOLUME			8										//ADC���ݻ�������С,ֻ����2���ݴη�����~>64���ۼ������������
#define KINO_ADC_DELAY					5										//heater��ͨ��Դ~��TIM2��ʱ���ں�ADC��ʼ����

/*��ť����*/
#define KINO_BUT_ACT_COUNT				3										//��������������ť~��Ϊ����״̬
#define KINO_BUT_ACT_MS					2000									//��������������ť��~��������ɲż����λ����
#define KINO_BUT_DIS_ACT_MS				200										//���δ���ʱ��������~���룬��ϲ�Ϊһ�Σ���λ���룬
#define KINO_BUT_HOLD_MAX_TIME_MS		3000									//������ťͣ��ʱ�䣬��λ����

/*�������*/
#define KINO_MOTO_SHORT_MS				500										//������λ����
#define KINO_MOTO_LONG_MS				1000									//��ﳤ��λ����

/*�����û����Ի�����*/
#define KINO_TOBACCO_MAX_COUNT			80										//��๦�ʵݼ�����80������
#define KINO_TOBACOO_MAX_SUCK_TIMES		10										//ÿ�κ㹦��ģʽ�����~����
#define KINO_USER_SMOKE_TIME_S			200										//�㹦��ʱ����λ��
#define KINO_PRE_HEAT_TIME_MS			10000									//����ƬԤ��ʱ����λ����
#define KINO_TIDE_HYSTERESIS_VOLUME		3										//�㹦��ģʽ��ѹ��ʷ��¼����,����~�ε�ѹ���������仯��Ϊ�¼�
#define KINO_ACTIVE_SLOPE				2										//��ѹ�仯��Чб��
#define KINO_TIDE_SAMPLE_TIME			200										//�㹦��ģʽ��ѹ����ʱ������λ����

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
#define KINO_BUT_HOLD_MAX_TIME	(( KINO_TIM1_F / 1000 ) * KINO_BUT_HOLD_MAX_TIME_MS )
#define KINO_LED_BLINK_PERIOD	(( KINO_TIM1_F / 1000 ) * KINO_LED_BLINK_PERIOD_MS )
#define KINO_PRE_HEAT_TIME		(( KINO_TIM1_F / 1000 ) * KINO_PRE_HEAT_TIME_MS )
#define KINO_BUT_DIS_ACT_TIME	(( KINO_TIM1_F / 1000 ) * KINO_BUT_DIS_ACT_MS )					//KINO_BUT_DIS_ACT_MS ʱ���ڰ���~��TIM2��PWM����
#define KINO_BUT_ACT_TIME		(( KINO_TIM1_F / 1000 ) * KINO_BUT_ACT_MS )						//KINO_BUT_ACT_MS ʱ���ڰ���~��TIM2��PWM����
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