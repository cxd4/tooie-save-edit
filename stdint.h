/******************************************************************************\
* Project:  Standard Integer Types                                             *
* Authors:  Iconoclast                                                         *
* Release:  2014.03.24                                                         *
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

#ifndef _STDINT_H_
#define _STDINT_H_

/*
 * Microsoft LLP64 system not compliant with ANSI C89
 */
#ifdef _MSC_VER
typedef __int64                 i64;
typedef signed __int64          s64;
typedef unsigned __int64        u64;
typedef __int32                 i32;
typedef signed __int32          s32;
typedef unsigned __int32        u32;
#else
typedef long                    i64;
typedef signed long             s64;
typedef unsigned long           u64;
typedef int                     i32;
typedef signed int              s32;
typedef unsigned int            u32;
#endif

/*
 * If a short integer is not 16 bits, nothing is, and the above #errors.
 */
typedef short                   i16;
typedef signed short            s16;
typedef unsigned short          u16;
typedef char                    i8;
typedef signed char             s8;
typedef unsigned char           u8;

typedef union {
    i8 B[2];
    s8 SB[2];
    u8 UB[2];
    i16 W;
    s16 SW;
    u16 UW;
} __W16;

typedef union {
    i8 B[4];
    s8 SB[4];
    u8 UB[4];
    i16 HW[2];
    s16 SHW[2];
    u16 UHW[2];
    i32 W;
    s32 SW;
    u32 UW;
} __W32;

typedef union {
    i8 B[8];
    s8 SB[8];
    u8 UB[8];
    i16 FW[4];
    s16 SFW[4];
    u16 UFW[4];
    i32 HW[2];
    s32 SHW[2];
    u32 UHW[2];
    i64 W;
    s64 SW;
    u64 UW;
} __W64;

#endif
