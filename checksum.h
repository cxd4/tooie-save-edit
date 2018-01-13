/******************************************************************************\
* Project:  Banjo-Tooie Checksum Validator (MIPS simulation layer)             *
* Authors:  Iconoclast                                                         *
* Release:  2014.03.14                                                         *
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

#ifndef _CHECKSUM_H_
#define _CHECKSUM_H_

#include "eeprom.h"
#include "stdint.h"

enum {
    ZERO = 0,
    AT = 1,
    V0 = 2,
    V1 = 3,
    A0 = 4,
    A1 = 5,
    A2 = 6,
    A3 = 7,
    T0 = 8,
    T1 = 9,
    T2 = 10,
    T3 = 11,
    T4 = 12,
    T5 = 13,
    T6 = 14,
    T7 = 15,
    S0 = 16,
    S1 = 17,
    S2 = 18,
    S3 = 19,
    S4 = 20,
    S5 = 21,
    S6 = 22,
    S7 = 23,
    T8 = 24,
    T9 = 25,
    K0 = 26,
    K1 = 27,
    GP = 28,
    SP = 29,
    S8 = 30,
    RA = 31
};
#define FP      S8

extern char checksum_actual[28];
extern char checksum_stored[28];
static const char digits[16] = {
    '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
};

extern u64 calculate_Banjo_checksum(unsigned int block, int size);
extern int trace_Banjo_checksum(unsigned int block, int size);
#endif
