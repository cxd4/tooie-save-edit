/******************************************************************************\
* Project:  Nintendo 64 EEPROM Access                                          *
* Authors:  Iconoclast                                                         *
* Release:  2014.03.15                                                         *
* License:  CC0 Public Domain Dedication                                       *
*                                                                              *
* To the extent possible under law, the author(s) have dedicated all copyright *
* and related and neighboring rights to this software to the public domain     *
* worldwide.  This software is distributed without any warranty.               *
*                                                                              *
* You should have received a copy of the CC0 Public Domain Dedication along    *
* with this software.                                                          *
* If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.             *
\******************************************************************************/

#ifndef _EEPROM_H_
#define _EEPROM_H_

#define EEPROM_BLOCK_SIZE       8
#ifdef EEPROM_TYPE_4K
#define EEPROM_MAXBLOCKS        64
#else
#define EEPROM_MAXBLOCKS        256
#endif

extern unsigned char EEPROM[EEPROM_BLOCK_SIZE * EEPROM_MAXBLOCKS];

extern int export_EEPROM(char * filepath);
extern int import_EEPROM(char * filepath);

#endif
