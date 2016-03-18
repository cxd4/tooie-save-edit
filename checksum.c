/******************************************************************************\
* Project:  Banjo-Tooie Checksum Validator                                     *
* Authors:  bryc, Iconoclast                                                   *
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

/*
 * file created on May 2014 by Iconoclast
 * breakpoint advice on Nemu64 and original Banjo-Kazooie research by bryc
 */

#include "checksum.h"
#include "stdint.h"

static void addiu  (const int rd, const int rs, const u16 immediate);
static void addu   (const int rd, const int rs, const int rt);
static void andi   (const int rd, const int rs, const u16 immediate);
static void bne    (int * branch, const int rs, const int rt);
static void bnez   (int * branch, const int rs);
static void dsll   (const int rd, const int rs, const int sa);
static void dsll32 (const int rd, const int rs, const int sa);
static void dsra32 (const int rd, const int rs, const int sa);
static void dsrl   (const int rd, const int rs, const int sa);
static void dsrl32 (const int rd, const int rs, const int sa);
static void jal    (int * branch);
static void jr     (int * branch);
static void lbu    (const int rd, const int base, const s16 offset);
static void ld     (const int rd, const int base, const s16 offset);
static void lw     (const int rd, const int base, const s16 offset);
static void or     (const int rd, const int rs, const int rt);
static void sd     (const int rs, const int base, const s16 offset);
static void sllv   (const int rd, const int rs, const int rt);
static void sltu   (const int rd, const int rs, const int rt);
static void sra    (const int rd, const int rs, const int sa);
static void sw     (const int rs, const int base, const s16 offset);
static void xor    (const int rd, const int rs, const int rt);

char checksum_actual[28] = "actual:  0x0000000000000000";
char checksum_stored[28] = "stored:  0x0000000000000000";
int trace_Banjo_checksum(unsigned int block, int size)
{
    __W64 actual, stored;

    actual.W = calculate_Banjo_checksum(block, size);
    stored.B[7] = EEPROM[8*block + size + 00];
    stored.B[6] = EEPROM[8*block + size + 01];
    stored.B[5] = EEPROM[8*block + size + 02];
    stored.B[4] = EEPROM[8*block + size + 03];
    stored.B[3] = EEPROM[8*block + size + 04];
    stored.B[2] = EEPROM[8*block + size + 05];
    stored.B[1] = EEPROM[8*block + size + 06];
    stored.B[0] = EEPROM[8*block + size + 07];

    checksum_actual[0xB + 0x0] = digits[(actual.UW & 0xF000000000000000) >> 60];
    checksum_actual[0xB + 0x1] = digits[(actual.UW & 0x0F00000000000000) >> 56];
    checksum_actual[0xB + 0x2] = digits[(actual.UW & 0x00F0000000000000) >> 52];
    checksum_actual[0xB + 0x3] = digits[(actual.UW & 0x000F000000000000) >> 48];
    checksum_actual[0xB + 0x4] = digits[(actual.UW & 0x0000F00000000000) >> 44];
    checksum_actual[0xB + 0x5] = digits[(actual.UW & 0x00000F0000000000) >> 40];
    checksum_actual[0xB + 0x6] = digits[(actual.UW & 0x000000F000000000) >> 36];
    checksum_actual[0xB + 0x7] = digits[(actual.UW & 0x0000000F00000000) >> 32];
    checksum_actual[0xB + 0x8] = digits[(actual.UW & 0x00000000F0000000) >> 28];
    checksum_actual[0xB + 0x9] = digits[(actual.UW & 0x000000000F000000) >> 24];
    checksum_actual[0xB + 0xA] = digits[(actual.UW & 0x0000000000F00000) >> 20];
    checksum_actual[0xB + 0xB] = digits[(actual.UW & 0x00000000000F0000) >> 16];
    checksum_actual[0xB + 0xC] = digits[(actual.UW & 0x000000000000F000) >> 12];
    checksum_actual[0xB + 0xD] = digits[(actual.UW & 0x0000000000000F00) >>  8];
    checksum_actual[0xB + 0xE] = digits[(actual.UW & 0x00000000000000F0) >>  4];
    checksum_actual[0xB + 0xF] = digits[(actual.UW & 0x000000000000000F) >>  0];

    checksum_stored[0xB + 0x0] = digits[(stored.UW & 0xF000000000000000) >> 60];
    checksum_stored[0xB + 0x1] = digits[(stored.UW & 0x0F00000000000000) >> 56];
    checksum_stored[0xB + 0x2] = digits[(stored.UW & 0x00F0000000000000) >> 52];
    checksum_stored[0xB + 0x3] = digits[(stored.UW & 0x000F000000000000) >> 48];
    checksum_stored[0xB + 0x4] = digits[(stored.UW & 0x0000F00000000000) >> 44];
    checksum_stored[0xB + 0x5] = digits[(stored.UW & 0x00000F0000000000) >> 40];
    checksum_stored[0xB + 0x6] = digits[(stored.UW & 0x000000F000000000) >> 36];
    checksum_stored[0xB + 0x7] = digits[(stored.UW & 0x0000000F00000000) >> 32];
    checksum_stored[0xB + 0x8] = digits[(stored.UW & 0x00000000F0000000) >> 28];
    checksum_stored[0xB + 0x9] = digits[(stored.UW & 0x000000000F000000) >> 24];
    checksum_stored[0xB + 0xA] = digits[(stored.UW & 0x0000000000F00000) >> 20];
    checksum_stored[0xB + 0xB] = digits[(stored.UW & 0x00000000000F0000) >> 16];
    checksum_stored[0xB + 0xC] = digits[(stored.UW & 0x000000000000F000) >> 12];
    checksum_stored[0xB + 0xD] = digits[(stored.UW & 0x0000000000000F00) >>  8];
    checksum_stored[0xB + 0xE] = digits[(stored.UW & 0x00000000000000F0) >>  4];
    checksum_stored[0xB + 0xF] = digits[(stored.UW & 0x000000000000000F) >>  0];
    return (actual.W != stored.W);
}

u64 calculate_Banjo_checksum(unsigned int block, int size)
{
    __W64 ret_slot;
    int branch, half_done;
    u32 fake_stack[0x60 / 4] = {
        0x00000000, 0x80082130, 0x80079B78, 0x80082154,
        0x00000001, 0x00000003, 0x00000000, 0x801922F0,
        0x800456E8, 0x00000002, 0xFFFFFFFF, 0x801929C8,
        0x00000002, 0xFFFFFFFF, 0x801927F0, 0x80088708,
        0x80081F3C, 0xC0000000, 0x8F809F47, 0x3108B3C1,
        0x00000000, 0x00000000, 0x801922F0, 0x00000080
    };

    half_done = 0;
/*
 * Fake the RDRAM offsets to target buffered EEPROM.
 */
    GPR[SP].W = (u64)(fake_stack);
    GPR[S0].W = (u64)(EEPROM) + 8*block;
    GPR[S5].W = GPR[S0].W + size;

/*
 * MIPS "re-assembly" of the checksum algorithm:
 */
    or     (S1, ZERO, ZERO);
 /* ... */
    or     (S3, ZERO, ZERO);
    or     (S4, ZERO, ZERO);
 /* ... */
    addiu  (S2, SP, 0x0048);
block_1: /* USA offset:  0x1928A4 */
    lbu    (T8, S0, 0x0000);
    lw     (T5, SP, 0x004C);
    andi   (T9, S1, 0x000F);
    sllv   (T0, T8, T9);
    lw     (T4, SP, 0x0048);
    addu   (T7, T0, T5);
    sra    (T2, T0, 31);
    sltu   (AT, T7, T5);
    addu   (T6, AT, T2);
    addu   (T6, T6, T4);
    sw     (T6, SP, 0x0048);
    sw     (T7, SP, 0x004C);
    jal    (&branch);
    or     (A0, S2, ZERO);
    if (branch != 0)
        goto block_2;
 /* who cares (unconditional jump) */
block_2: /* USA offset:  0x1168AC */
    ld     (A3, A0, 0x0000);
    dsll32 (A2, A3, 31);
    dsll   (A1, A3, 31);
    dsrl   (A2, A2, 31);
    dsrl32 (A1, A1, 0);
    dsll32 (A3, A3, 12);
    or     (A2, A2, A1);
    dsrl32 (A3, A3, 0);
    xor    (A2, A2, A3);
    dsrl   (A3, A2, 20);
    andi   (A3, A3, 0x0FFF);
    xor    (A3, A3, A2);
    dsll32 (V0, A3, 0);
    sd     (A3, A0, 0x0000);
    jr     (&branch);
    dsra32 (V0, V0, 0);
    if (branch != 0)
    {
        if (half_done == 0)
            goto block_3;
        else
            goto block_6;
    }
 /* who cares (unconditional jump) */
block_3: /* USA offset:  0x1928DC */
    addiu  (S0, S0, 0x0001);
    addiu  (S1, S1, 0x0007);
    bne    (&branch, S0, S5);
    xor    (S3, S3, V0);
    if (branch != 0)
        goto block_1;
    half_done = 1;
#ifndef REAL_N64_RDRAM_STATE
    GPR[S0].W = (u64)(EEPROM) + 8*block;
    sd     (S0, SP, 0x0058);
    ld     (A3, SP, 0x0058); /* actually a LW op-code but broke 64-bit ptrs */
#else
    lw     (A3, SP, 0x0058);
#endif
    addiu  (S0, S5, 0xFFFF);
    sltu   (AT, S0, A3);
    bnez   (&branch, AT);
    addiu  (S2, SP, 0x0048);
    if (branch != 0) /* This should never happen. */
        goto block_5;
    addiu  (S5, A3, 0xFFFF);
block_4: /* USA offset:  0x192904 */
    lbu    (T1, S0, 0x0000);
    lw     (T3, SP, 0x004C);
    andi   (T8, S1, 0x000F);
    sllv   (T9, T1, T8);
    lw     (T2, SP, 0x0048);
    addu   (T5, T9, T3);
    sra    (T0, T9, 31);
    sltu   (AT, T5, T3);
    addu   (T4, AT, T0);
    addu   (T4, T4, T2);
    sw     (T4, SP, 0x0048);
    sw     (T5, SP, 0x004C);
    jal    (&branch);
    or     (A0, S2, ZERO);
    if (branch != 0)
        goto block_2;
 /* who cares (unconditional jump) */
block_6: /* USA offset:  0x19293C */
    addiu  (S0, S0, 0xFFFF);
    addiu  (S1, S1, 0x0003);
    bne    (&branch, S0, S5);
    xor    (S4, S4, V0);
    if (branch != 0)
        goto block_4;
    ret_slot.HW[1] = GPR[S3].HW[0];
    ret_slot.HW[0] = GPR[S4].HW[0];
    return (ret_slot.UW);
block_5:
    return (GPR[S3].W & 0x00000000FFFFFFFF);
}

/*
 * annoying loads and stores
 * (annoying because of the RDRAM being faked here)
 */
static void lbu    (const int rd, const int base, const s16 offset)
{
    const u64 addr = GPR[base & 31].UW + offset;

    if ((rd & 31) == ZERO)
        return;
    GPR[rd & 31].W = *(u8 *)(addr);
    return;
}
static void ld     (const int rd, const int base, const s16 offset)
{
    const u64 addr = GPR[base & 31].UW + offset;

    if (addr & 0x00000007)
        return;
    if ((rd & 31) == ZERO)
        return;
    GPR[rd & 31].HW[1] = *(i32 *)(addr + 0);
    GPR[rd & 31].HW[0] = *(i32 *)(addr + 4);
    return;
}
static void lw     (const int rd, const int base, const s16 offset)
{
    const u64 addr = GPR[base & 31].UW + offset;

    if (addr & 0x00000003)
        return;
    if ((rd & 31) == ZERO)
        return;
    GPR[rd & 31].W = *(i32 *)(addr);
    return;
}
static void sd     (const int rs, const int base, const s16 offset)
{
    const u64 addr = GPR[base & 31].UW + offset;

    if (addr & 0x00000007)
        return;
    *(i32 *)(addr + 0) = GPR[rs & 31].HW[1];
    *(i32 *)(addr + 4) = GPR[rs & 31].HW[0];
    return;
}
static void sw     (const int rs, const int base, const s16 offset)
{
    const u64 addr = GPR[base & 31].UW + offset;

    if (addr & 0x00000003)
        return;
    *(i32 *)(addr) = GPR[rs & 31].HW[0];
    return;
}

/*
 * arithmetic and logical operations
 */
static void andi   (const int rd, const int rs, const u16 immediate)
{
    if ((rd & 31) == ZERO)
        return;
    GPR[rd & 31].W = GPR[rs & 31].W & immediate;
    return;
}
static void or     (const int rd, const int rs, const int rt)
{
    if ((rd & 31) == ZERO)
        return;
    GPR[rd & 31].W = GPR[rs & 31].W | GPR[rt & 31].W;
    return;
}
static void xor    (const int rd, const int rs, const int rt)
{
    if ((rd & 31) == ZERO)
        return;
    GPR[rd & 31].W = GPR[rs & 31].W ^ GPR[rt & 31].W;
    return;
}
static void addiu  (const int rd, const int rs, const u16 immediate)
{
    if ((rd & 31) == ZERO)
        return;
#ifndef REAL_N64_RDRAM_STATE
/*
 * We actually need to do this because the only times ADDIU gets used in this
 * checksum algorithm are for offsets relative to N64 RDRAM, which are being
 * faked here in 64-bit-compliant C code and may be 64-bit pointers.  On the
 * real R4300 CPU, we would not say `GPR[rs].W` instead of `GPR[rs].HW[0]`.
 */
    GPR[rd & 31].W = GPR[rs & 31].W + (s16)(immediate);
#else
    GPR[rd & 31].W = GPR[rs & 31].HW[0] + (s16)(immediate);
#endif
    return;
}
static void addu   (const int rd, const int rs, const int rt)
{
    if ((rd & 31) == ZERO)
        return;
    GPR[rd & 31].W = GPR[rs & 31].HW[0] + GPR[rt & 31].HW[0];
    return;
}
static void sltu   (const int rd, const int rs, const int rt)
{
    if ((rd & 31) == ZERO)
        return;
    GPR[rd & 31].W = (GPR[rs & 31].UW < GPR[rt & 31].UW);
    return;
}

/*
 * shift operations
 */
static void sra    (const int rd, const int rs, const int sa)
{
    if ((rd & 31) == ZERO)
        return;
    GPR[rd & 31].W = GPR[rs & 31].SHW[0] >> (sa & 31);
    return;
}
static void sllv   (const int rd, const int rs, const int rt)
{
    if ((rd & 31) == ZERO)
        return;
    GPR[rd & 31].W = GPR[rs & 31].HW[0] << (GPR[rt & 31].HW[0] & 31);
    return;
}
static void dsll   (const int rd, const int rs, const int sa)
{
    if ((rd & 31) == ZERO)
        return;
    GPR[rd & 31].W = GPR[rs & 31].W << (sa & 31);
    return;
}
static void dsrl   (const int rd, const int rs, const int sa)
{
    if ((rd & 31) == ZERO)
        return;
    GPR[rd & 31].W = GPR[rs & 31].UW >> (sa & 31);
    return;
}
static void dsll32 (const int rd, const int rs, const int sa)
{
    if ((rd & 31) == ZERO)
        return;
    GPR[rd & 31].W = GPR[rs & 31].W << ((sa & 31) + 32);
    return;
}
static void dsrl32 (const int rd, const int rs, const int sa)
{
    if ((rd & 31) == ZERO)
        return;
    GPR[rd & 31].W = GPR[rs & 31].UW >> ((sa & 31) + 32);
    return;
}
static void dsra32 (const int rd, const int rs, const int sa)
{
    if ((rd & 31) == ZERO)
        return;
    GPR[rd & 31].W = GPR[rs & 31].SW >> ((sa & 31) + 32);
    return;
}

/*
 * jump and branch operations
 */
static void jal    (int * branch)
{
    *(branch) = 0x003FFFFF & 1;
 /* GPR[RA] = PC; */
    return;
}
static void jr     (int * branch)
{
    *(branch) = 1;
    return;
}
static void bne    (int * branch, const int rs, const int rt)
{
    *(branch) = (GPR[rs & 31].W != GPR[rt & 31].W);
    return;
}
static void bnez   (int * branch, const int rs)
{ /* official MIPS pseudo-op-code */
    bne    (branch, rs, ZERO);
    return;
}
