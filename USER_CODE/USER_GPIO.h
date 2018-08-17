#ifndef __USER_GPIO_H
#define __USER_GPIO_H

#include "stm8s_gpio.h"
#include "USER_CONFIG.h"


/******************************************************************************
 *PORT_DEFINE
 */
#define LED_COM_PORT            GPIOA
#define LED_COM_PIN             GPIO_PIN_3              //LED PWM �������ƶ�

#define GRE_LED_PORT            GPIOB
#define GRE_LED_PIN             GPIO_PIN_4              //��ɫLEDʹ�ܶ�

#define BLU_LED_PORT            GPIOB
#define BLU_LED_PIN             GPIO_PIN_5              //��ɫLEDʹ�ܶ�

#define RED_LED_PORT            GPIOC
#define RED_LED_PIN             GPIO_PIN_3              //��ɫLEDʹ�ܶ�

#define BUTTON_PORT             GPIOC
#define BUTTON_PIN              GPIO_PIN_5              //��ť�����

#define MOTO_PORT               GPIOC
#define MOTO_PIN                GPIO_PIN_6              //��������ƶ�

#define SYS_ADJ_PORT            GPIOC
#define SYS_ADJ_PIN             GPIO_PIN_7              //ϵͳ���ڴ�����

#define CHARGE_SIGNAL_PORT      GPIOD
#define CHARGE_SIGNAL_PIN       GPIO_PIN_2              //����ź������

#define HEATER_PWM_PORT         GPIOD
#define HEATER_PWM_PIN          GPIO_PIN_4              //����Ƭ���ƶ�

#define BAT_VOL_PORT            GPIOD
#define BAT_VOL_PIN             GPIO_PIN_5              //��ص�ѹ����

#define R_ADJ_VOL_PORT          GPIOD
#define R_ADJ_VOL_PIN           GPIO_PIN_6              //�¶����ö�

	 
/******************************************************************************
 *OUTPUT_CHANNEL_DEFINE
 */
#define HEATER_CC_CHANNEL		TIM2_CHANNEL_1
#define ADC_TRI_CC_CHANNEL		TIM2_CHANNEL_2
#define LED_CC_CHANNEL			TIM2_CHANNEL_3

extern FunctionalState BUTTON_ACTIVE_FLAG;
extern u16 BUTTON_TRIGER_TIMES;
extern SYS_STATUS_TYPE SYS_STATUS;

void KINO_GPIO_INIT( void );

#endif