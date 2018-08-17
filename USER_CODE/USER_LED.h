#ifndef __USER_LED_H
#define __USER_LED_H

#include "stm8s.h"
#include "USER_CONFIG.h"

/******************************************************************************
 *TOOL_DEFINE
 */
typedef enum
{
  NON_LIGHT     =       0x00,
  RED_LIGHT     =       0x01,
  GRE_LIGHT     =       0x02,
  BLU_LIGHT     =       0x04,
  ALL_LIGHT		=		0x0F
}LED_COLORTYPE;

typedef enum
{
	LED_OFF		=		0x00,
	LED_SMASH	=		0x01,
	LED_BREATH	=		0x02,
	LED_ON		=		0x03
}LED_DISPLAY_MODE_TYPE;

void LED_BREATHING( void );
void LED_SHOW( LED_COLORTYPE COLOR, LED_DISPLAY_MODE_TYPE mode );
void LED_PWM_DUTY_SET( float duty );

#endif