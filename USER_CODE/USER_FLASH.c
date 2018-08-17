#include "USER_FLASH.h"

#define FLASH_DATA_START_PHYSICAL_ADDRESS ((uint32_t)0x004000) /*!< Data EEPROM memory: start address */

void KINO_FLASH_INIT( void )
{
	FLASH_SetLowPowerMode( FLASH_LPMODE_POWERDOWN );
}



