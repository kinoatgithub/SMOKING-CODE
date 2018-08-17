#ifndef __USER_FLASH_H
#define __USER_FLASH_H

#include "stm8s.h"
#include "stm8s_flash.h"

#define KINO_HEAT_HPOWER_ADD		( 0X00 + FLASH_DATA_START_PHYSICAL_ADDRESS )//32位的pwm模式最高功率
#define KINO_HEAT_LPOWER_ADD		( 0X04 + FLASH_DATA_START_PHYSICAL_ADDRESS )//32位的pwm模式最低功率
#define KINO_TOBACCO_DOWNCOUNT_ADD	( 0X08 + FLASH_DATA_START_PHYSICAL_ADDRESS )//8位的已吸烟的数量倒数

void KINO_FLASH_INIT( void );

#endif