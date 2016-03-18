/******************************************************************************\
* Project:  Banjo-Tooie Save Data Controls                                     *
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

#ifndef _TOOIE_DATA_
#define _TOOIE_DATA_

#include "eeprom.h"

extern unsigned char * global_data;
extern unsigned char * game;

extern int execute_option(int optc, char* optv[]);

#endif
