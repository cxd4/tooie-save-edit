/******************************************************************************\
* Project:  Banjo-Tooie EEPROM Editor                                          *
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

#include <memory.h>
#include <stdio.h>
#include "checksum.h"
#include "data.h"
#include "eeprom.h"
#include "stdint.h"

static unsigned char * file[4] = {
    EEPROM + 0x100, EEPROM + 0x2C0, EEPROM + 0x480, EEPROM + 0x640
};

static void update_checksum(unsigned char * dst, unsigned int block, int size);

int main(int argc, char* argv[])
{
    int mismatch;
    int optc;
    register int i;

    if (argc < 2)
    {
        fprintf(stderr, "Correct syntax:  %s <filepath> [options]\n", argv[0]);
        fgetc(stdin);
        return 1;
    }
    i = import_EEPROM(argv[1]);
    if (i != 0)
    {
        fputs("Unable to import EEPROM.\n", stderr);
        return 1;
    }

    if (EEPROM[0x080] == 0x08)
        global_data = EEPROM + 0x080;
    else
    {
        if (EEPROM[0x000] != 0x08)
            printf("Warning:  %s\n", "Failed magic number tests.");
        global_data = EEPROM + 0x000;
    }

    game = file[3]; /* default (Rareware adds new games bottom-up.) */
    for (i = 0; i < 3; i++)
        if (file[i][0x00A] == 0x00)
            continue;
    i -= i >> 2; /* i = (i >= 4) ? 3 : i; */
    game = file[i];
/*
 * When updating an existing game save, Rareware likes to zero all 64 bits
 * at the start of the game save block and transfer the new data elsewhere.
 *
 * As if that wasn't enough to throw us off, they also like to alternate and
 * switch which copy is the real game save and which one they corrupted.
 * It can break save editors AND your view of save updates in a hex editor.
 *
 * IF, due to hacks, there is a tie in conditions, or two save blocks both
 * qualify as fully valid game data for the save file, it should be
 * remembered that the ROM defaults to the bottom-most data in EEPROM depths.
 */
    if (file[0][0x00A] != 0x00)
    {
        if (file[0][0x00A] == file[3][0x00A])
            game = (*(i64 *)(file[0] + 0x000) == 0) ? file[3] : file[0];
        else if (file[0][0x00A] == file[2][0x00A])
            game = (*(i64 *)(file[0] + 0x000) == 0) ? file[2] : file[0];
        else if (file[0][0x00A] == file[1][0x00A])
            game = (*(i64 *)(file[0] + 0x000) == 0) ? file[1] : file[0];
    }
    if (file[1][0x00A] != 0x00)
    {
        if (file[1][0x00A] == file[3][0x00A])
            game = (*(i64 *)(file[1] + 0x000) == 0) ? file[3] : file[1];
        else if (file[1][0x00A] == file[2][0x00A])
            game = (*(i64 *)(file[1] + 0x000) == 0) ? file[2] : file[1];
    }
    if (file[2][0x00A] != 0x00)
        if (file[2][0x00A] == file[3][0x00A])
            game = (*(i64 *)(file[2] + 0x000) == 0) ? file[3] : file[2];

    mismatch = trace_Banjo_checksum((unsigned)(global_data - EEPROM)/8, 120);
    if (mismatch != 0)
        printf(
            "Warning:  %s.\n%s\n%s\n\n",
            "Overwriting global data checksum mismatch.",
        checksum_stored, checksum_actual);
    mismatch = trace_Banjo_checksum((unsigned)(game - EEPROM)/8, 440);
    if (mismatch != 0)
        printf(
            "Warning:  %s.\n%s\n%s\n\n",
            "Overwriting save file checksum mismatch.",
        checksum_stored, checksum_actual);

    for (optc = 0; argc > 0; --argc, ++optc)
    {
        if (argv[optc][0] != '-')
            continue;
        execute_option(argc, &argv[optc]);
    }
/*
 * Fill in the magic numbers so the ROM recognizes the data as valid.
 * Wouldn't want to edit deleted or wiped parts of the EEPROM, would we?
 */
    global_data[0x00] = 0x08;
    game[0x000] = 'K'; game[0x001] = 'H'; game[0x002] = 'J'; game[0x003] = 'C';
    game[0x004] = 0x01; game[0x005] = 0x02; game[0x006] = 0x1E;
 /* doesn't seem to care what game[0x007] is set to */
    game[0x008] = 0x02;
 /* game[0x009] = 0x01; */
    if (game[0x00A] == 0x00)
    {
        printf("Warning:  %s\n", "Saved minimalist game data to a new slot.\n");
        game[0x00A] = 0x01; /* If no data was present, default to file 1. */
    }
 /* game[0x00B] = 0x03; */
 /* game[0x00C] = 0x1C; */

    update_checksum(
        global_data + 0x78, (unsigned)(global_data - EEPROM)/8, 120);
    update_checksum(
        game + 0x1B8, (unsigned)(game - EEPROM)/8, 440);

    i = export_EEPROM(argv[1]);
    if (i != 0)
    {
        fputs("Unable to export EEPROM.\n", stderr);
        return 1;
    }
    return 0;
}

static void update_checksum(unsigned char * dst, unsigned int block, int size)
{
    __W64 checksum;

    checksum.UW = calculate_Banjo_checksum(block, size);
    dst[0] = checksum.UB[7];
    dst[1] = checksum.UB[6];
    dst[2] = checksum.UB[5];
    dst[3] = checksum.UB[4];
    dst[4] = checksum.UB[3];
    dst[5] = checksum.UB[2];
    dst[6] = checksum.UB[1];
    dst[7] = checksum.UB[0];
    return;
}
