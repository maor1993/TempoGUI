#ifndef FLASHSTRCUTURE_H
#define FLASHSTRCUTURE_H

#include <stdint.h>



namespace flash {


/*----------defines------*/

#define FLASH_FONT_SMALL_ADDR	0x002000
#define FLASH_FONT_MEDIUM_ADDR	0x004000
#define FLASH_FONT_LARGE_ADDR	0x006000
#define FLASH_FILE_HEADERS_ADDR 0x008000
#define FLASH_FILES_ADDR		0x100000

#define FLASH_MAX_HEADERS 		200

#define FLASH_MAX_BLOCK			4000
#define FLASH_MAX_FILE_SIZE		0x200000

#define FLASH_HEADER_SIG 		0xfeedabba


#define FLASH_RECORDING_SECTORS 0x200000/0x1000

/*------------------------*/











}




#endif // FLASHSTRCUTURE_H
