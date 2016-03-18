/******************************************************************************\
* Project:  Banjo-Tooie Save Data Controls                                     *
* Authors:  Iconoclast                                                         *
* Release:  2014.03.22                                                         *
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
#include <stdlib.h>
#include "data.h"
#include "eeprom.h"
#include "stdint.h"

unsigned char * global_data;
unsigned char * game;

static int adjust_screen_offset(int optc, char* optv[]);
static int enable_replay_bosses(int optc, char* optv[]);
static int enable_replay_cinema(int optc, char* optv[]);
static int enable_replay_mini_games(int optc, char* optv[]);
static int have_Cheato_pages(int optc, char* optv[]);
static int have_eggs(int optc, char* optv[]);
static int have_extra_honeycombs(int optc, char* optv[]);
static int have_feathers(int optc, char* optv[]);
static int have_Glowbos(int optc, char* optv[]);
static int have_jiggies(int optc, char* optv[]);
static int have_moves(int optc, char* optv[]);
static int have_musical_notes(int optc, char* optv[]);
static int hex_edit_bit(int optc, char* optv[]);
static int hex_edit_byte(int optc, char* optv[]);
static int Klungo_alerted(int optc, char* optv[]);
static int set_cheat_codes(int optc, char* optv[]);
static int set_speaker_mode(int optc, char* optv[]);
static int silo_shortcut_tunnels(int optc, char* optv[]);
static int widescreen_mode(int optc, char* optv[]);
static int invalid_option(int optc, char* optv[]);

static int (*edit_modes[128])(int, char**) = {
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    enable_replay_bosses, /* -B */
    enable_replay_cinema, /* -C */
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    Klungo_alerted, /* -K */
    invalid_option,
    enable_replay_mini_games, /* -M */
    invalid_option,
    adjust_screen_offset, /* -O */
    invalid_option,
    invalid_option,
    invalid_option,
    set_speaker_mode, /* -S */
    invalid_option,
    invalid_option,
    invalid_option,
    widescreen_mode, /* -W */
    hex_edit_byte, /* -X */
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    set_cheat_codes, /* -c */
    invalid_option,
    have_eggs, /* -e */
    have_feathers, /* -f */
    have_Glowbos, /* -g */
    have_extra_honeycombs, /* -h */
    invalid_option,
    have_jiggies, /* -j */
    invalid_option,
    invalid_option,
    have_moves, /* -m */
    have_musical_notes, /* -n */
    invalid_option,
    have_Cheato_pages, /* -p */
    invalid_option,
    invalid_option,
    silo_shortcut_tunnels, /* -s */
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    hex_edit_bit, /* -x */
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
    invalid_option,
};

int execute_option(int optc, char* optv[])
{
    register int ret_slot;

    if (optv[0][2] != '\0')
    {
        fputs("Optional argument too long.\n", stderr);
        return 1;
    }
    ret_slot = edit_modes[optv[0][1] & 127](optc, optv);
    if (ret_slot != 0)
    {
        fputs("Insufficient arguments for this option.\n", stderr);
        return 1;
    }
    return 0;
}

static int invalid_option(int optc, char* optv[])
{
    fprintf(
        stderr,
        "Unrecognized option:  (optc, optv[0]) = (%i, %s)\n",
        optc,
        optv[0]);
    return 1;
}

static int adjust_screen_offset(int optc, char* optv[])
{
    long x, y;

    if (optc < 3)
        return 1;
    x = strtol(optv[1], NULL, 10);
    y = strtol(optv[2], NULL, 10);
    global_data[0x01] &= ~0xFC;
    global_data[0x02] &= ~0x3F;

/*
 * If the user specifies negative (invalid) coordinate offsets, then I think
 * I prefer to just clamp the underflow up to 0.  Actually, do overflow, too.
 */
    x &= (x < 0) ? 0 : x;
    y &= (y < 0) ? 0 : y;
    x |= (x - 64 < 0) ? x : ~0;
    y |= (y - 64 < 0) ? y : ~0;

    global_data[0x01] |= (x & 077) << 2;
    global_data[0x02] |= (y & 077) >> 0;
    return 0;
}

static int enable_replay_bosses(int optc, char* optv[])
{
    unsigned long flags;

    if (optc < 2)
        return 1;
    global_data[0x03] &= 0x00;
    global_data[0x04] &= 0xE0;
    flags = strtoul(optv[1], NULL, 2);
    global_data[0x03]  = (unsigned char)(flags);
    global_data[0x04] |= (flags >>= 8) & 31;
    return 0;
}

static int enable_replay_cinema(int optc, char* optv[])
{
    unsigned long flags;

    if (optc < 2)
        return 1;
    global_data[0x09] &= 0x03;
    flags = strtoul(optv[1], NULL, 2);
    global_data[0x09] |= (unsigned char)(flags << 2);
    return 0;
}

static int enable_replay_mini_games(int optc, char* optv[])
{
    unsigned long flags;

    if (optc < 2)
        return 1;
    global_data[0x06] &= 0x07;
    global_data[0x07] &= 0x00;
    global_data[0x08] &= 0x00;
    global_data[0x09] &= 0xFC;
    flags = strtoul(optv[1], NULL, 2);
    global_data[0x06] |= (unsigned char)(flags << 3);
    global_data[0x07] |= (unsigned char)(flags >> 5);
    global_data[0x08] |= (unsigned char)(flags >> 13);
    global_data[0x09] |= (flags >>= 21) & 3;
    return 0;
}

static int have_Cheato_pages(int optc, char* optv[])
{
    long source;

    if (optc < 2)
        return 1;
    source = strtol(optv[1], NULL, 10);
    game[0x03F] = (unsigned char)(source >> 8);
    game[0x040] = (unsigned char)(source >> 0);
    return 0;
}

static int have_eggs(int optc, char* optv[])
{
    unsigned long source;

    if (optc < 3)
        return 1;
    source = strtoul(optv[2], NULL, 0);
    switch (optv[1][0] & ~0x20) /* for case-insensitivity */
    {
        case 'B':
        case 'E': /* blue eggs */
            game[0x02B] = (unsigned char)(source >> 8);
            game[0x02C] = (unsigned char)(source >> 0);
            return 0;
        case 'C': /* clockwork */
            game[0x033] = (unsigned char)(source >> 8);
            game[0x034] = (unsigned char)(source >> 0);
            return 0;
        case 'F': /* fire */
            game[0x02D] = (unsigned char)(source >> 8);
            game[0x02E] = (unsigned char)(source >> 0);
            return 0;
        case 'G': /* grenade */
            game[0x031] = (unsigned char)(source >> 8);
            game[0x032] = (unsigned char)(source >> 0);
            return 0;
        case 'I': /* ice */
            game[0x02F] = (unsigned char)(source >> 8);
            game[0x030] = (unsigned char)(source >> 0);
            return 0;
        default:
            return 1;
    }
}

static int have_feathers(int optc, char* optv[])
{
    unsigned long source;

    if (optc < 3)
        return 1;
    source = strtoul(optv[2], NULL, 0);
    switch (optv[1][0] & ~0x20) /* for case-insensitivity */
    {
        case 'G':
            game[0x039] = (unsigned char)(source >> 8);
            game[0x03A] = (unsigned char)(source >> 0);
            return 0;
        case 'R':
            game[0x037] = (unsigned char)(source >> 8);
            game[0x038] = (unsigned char)(source >> 0);
            return 0;
        default:
            return 1;
    }
}

static int have_extra_honeycombs(int optc, char* optv[])
{
    long source;

    if (optc < 2)
        return 1;
    source = strtol(optv[1], NULL, 10);
    game[0x03D] = (unsigned char)(source >> 8);
    game[0x03E] = (unsigned char)(source >> 0);
    return 0;
}

static int have_Glowbos(int optc, char* optv[])
{
    long source;

    if (optc < 2)
        return 1;
    source = strtol(optv[1], NULL, 10);
    game[0x03B] = (unsigned char)(source >> 8);
    game[0x03C] = (unsigned char)(source >> 0);
    return 0;
}

static int have_jiggies(int optc, char* optv[])
{
    int source;

    if (optc < 2)
        return 1;
/*
 * In the future this might take a large bit string signifying exactly which
 * jiggies Banjo should or should not have.
 */

    memset(game + 0xCC, 0x00, 11); /* clearing 88 of the 90 jiggies */
    game[0x0D7] &= 0xFC;
    source = optv[1][0] & 1;
    if (source == 0)
        return 0;
    memset(game + 0xCC, 0xFF, 11);
    game[0x0D7] |= 0x03;
    return 0;
}

static int have_moves(int optc, char* optv[])
{
    int source;

    if (optc < 2)
        return 1;
/*
 * In the future this might take a large bit string signifying exactly which
 * moves Banjo should or should not have, including those from Banjo-Kazooie.
 */
    game[0x09F] &= 0x1F;
    memset(game + 0x0A0, 0x00, 6);
    source = optv[1][0] & 1;
    if (source == 0)
        return 0;
    game[0x09F] |= 0xE0;
    memset(game + 0x0A0, 0xFF, 6);
 /* game[0xA6] |= ~0x00; // not sure if these are a part of the moves */
    return 0;
}

static int have_musical_notes(int optc, char* optv[])
{
    int source;

    if (optc < 2)
        return 1;
/*
 * Since there may be optional, additional parameters in the future, such as
 * which specific world in the game to set the musical notes for, or the
 * specific masking of one, single musical note item in the game rather than
 * all of them, the below bits clearing is organized for future use.
 */

/* Mayahem Temple */
    game[0x10B] &= 0x7F;
    game[0x10C] &= 0x00;
    game[0x10D] &= 0x00;
/* Glitter Gulch Mine */
    game[0x10E] &= 0x00;
    game[0x10F] &= 0x00;
    game[0x110] &= 0xFE;
/* Witchyworld */
    game[0x110] &= 0x01;
    game[0x111] &= 0x00;
    game[0x112] &= 0xFC;
/* Jolly Roger's Lagoon */
    game[0x112] &= 0x03;
    game[0x113] &= 0x00;
    game[0x114] &= 0xF8;
/* Terrydactyland */
    game[0x114] &= 0x07;
    game[0x115] &= 0x00;
    game[0x116] &= 0xF0;
/* Grunty Industries */
    game[0x116] &= 0x0F;
    game[0x117] &= 0x00;
    game[0x118] &= 0xE0;
/* Hailfire Peaks */
    game[0x118] &= 0x1F;
    game[0x119] &= 0x00;
    game[0x11A] &= 0xC0;
/* Cloud Cuckooland */
    game[0x11A] &= 0x3F;
    game[0x11B] &= 0x00;
    game[0x11C] &= 0x80;
/* Isle O' Hags */
    game[0x11C] &= 0x7F;
    game[0x11D] &= 0x00;
    game[0x11E] &= 0x00;

    source = optv[1][0] & 1;
    if (source == 0)
        return 0;
    game[0x10B] |= 0x80;
    memset(game + 0x10C, ~0x00, 19);
    return 0;
}

static int hex_edit_bit(int optc, char* optv[])
{
    unsigned long addr;
    long bit_no;
    int source;

    if (optc < 4)
        return 1;
    addr = strtoul(optv[1], NULL, 16) & 0x000007FF;
    bit_no = optv[2][0] & 07;
    source = optv[3][0] & 1;
    EEPROM[addr] &= ~(0x01 << bit_no);
    EEPROM[addr] |= source << bit_no;
    return 0;
}

static int hex_edit_byte(int optc, char* optv[])
{
    unsigned long addr;
    long source;

    if (optc < 3)
        return 1;
    addr = strtoul(optv[1], NULL, 16) & 0x000007FF;
    source = strtol(optv[2], NULL, 0);
    EEPROM[addr] = (unsigned char)(source);
    return 0;
}

static int Klungo_alerted(int optc, char* optv[])
{
    int source;

    if (optc < 2)
        return 1;
    source = optv[1][0] & 1;
    game[0x089] &= ~(0x01 << 4);
    game[0x089] |= source << 4;
    return 0;
}

static int set_cheat_codes(int optc, char* optv[])
{
    unsigned long flags;

    if (optc < 2)
        return 1;
    game[0x128] &= ~0x0F;
    game[0x129] &= ~0x3F;
    game[0x08B] &= ~(1 << 04); /* all world doors opened up cheat */
    game[0x136] &= ~(1 << 03); /* homing eggs cheat */
    flags = strtoul(optv[1], NULL, 2);
    game[0x128] |= !!(flags & 1<< 0) << (4 + 0);
    game[0x128] |= !!(flags & 1<< 1) << (4 + 1);
    game[0x128] |= !!(flags & 1<< 2) << (4 + 2);
    game[0x128] |= !!(flags & 1<< 3) << (4 + 3);
    game[0x129] |= !!(flags & 1<< 4) << (0 + 0);
    game[0x129] |= !!(flags & 1<< 5) << (0 + 1);
    game[0x129] |= !!(flags & 1<< 6) << (0 + 2);
    game[0x129] |= !!(flags & 1<< 7) << (0 + 3);
    game[0x129] |= !!(flags & 1<< 8) << (0 + 4);
    game[0x129] |= !!(flags & 1<< 9) << (0 + 5);
    game[0x08B] |= !!(flags & 1<<10) << 4;
    game[0x136] |= !!(flags & 1<<11) << 3;
    return 0;
}

static int set_speaker_mode(int optc, char* optv[])
{
    if (optc < 2)
        return 1;
    global_data[0x02] &= ~0xC0;
    switch (optv[1][0] & ~0x20) /* for case-insensitivity */
    {
        case 'M': /* "mono" */
            global_data[0x02] |= 0 << 6;
            return 0;
        case 'S': /* "stereo" */
            global_data[0x02] |= 1 << 6;
            return 0;
        case 'H': /* "headphone" */
            global_data[0x02] |= 2 << 6;
            return 0;
        case 'D': /* "dolby surround" */
            global_data[0x02] |= 3 << 6;
            return 0;
        default:
            return 1;
    }
}

static int silo_shortcut_tunnels(int optc, char* optv[])
{
    unsigned long flags;

    if (optc < 2)
        return 1;
    game[0x0E7] &= ~0xE0;
    game[0x0E8] &= ~0x1F;
    flags = strtoul(optv[1], NULL, 2);
    game[0x0E7] |= !!(flags & 1<<00) << (0 + 5);
    game[0x0E7] |= !!(flags & 1<<01) << (1 + 5);
    game[0x0E7] |= !!(flags & 1<<02) << (2 + 5);
    game[0x0E8] |= !!(flags & 1<<03) << (3 - 3);
    game[0x0E8] |= !!(flags & 1<<04) << (4 - 3);
    game[0x0E8] |= !!(flags & 1<<05) << (5 - 3);
    game[0x0E8] |= !!(flags & 1<<06) << (6 - 3);
    game[0x0E8] |= !!(flags & 1<<07) << (7 - 3);
    return 0;
}

static int widescreen_mode(int optc, char* optv[])
{
    int source;

    if (optc < 2)
        return 1;
    source = optv[1][0] & 1;
    global_data[0x01] &= ~(0x01 << 1);
    global_data[0x01] |= source << 1;
    return 0;
}
