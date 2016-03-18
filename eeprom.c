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

#include <stdio.h>
#include "eeprom.h"

unsigned char EEPROM[EEPROM_BLOCK_SIZE * EEPROM_MAXBLOCKS];

int import_EEPROM(char * filepath)
{
    FILE * stream;
    size_t result;
    int failure;

    stream = fopen(filepath, "rb");
    if (stream == NULL)
    {
        fprintf(stderr, "Failed file read-access on \"%s\".\n", filepath);
        return 8; /* CONT_NO_RESPONSE_ERROR */
    }
    result = fread(EEPROM, EEPROM_BLOCK_SIZE, EEPROM_MAXBLOCKS, stream);
    if (result != EEPROM_MAXBLOCKS)
    {
        int test;

        test = ferror(stream);
        if (test != 0)
            return -1;
        test = feof(stream);
        if (test != 0)
            fprintf(stderr, "Warning:  EEPROM file seems truncated.\n");
        printf(
            "%u of %u bytes read.\n",
            (unsigned)result, EEPROM_BLOCK_SIZE * EEPROM_MAXBLOCKS);
    }
    failure = fclose(stream);
    if (failure != 0)
        fprintf(stderr, "Problem closing the file stream?\n");
    return 0;
}

int export_EEPROM(char * filepath)
{
    FILE * stream;
    size_t result;
    int failure;

    stream = fopen(filepath, "wb");
    if (stream == NULL)
    {
        fprintf(stderr, "Failed file write-access on \"%s\".\n", filepath);
        return 8; /* CONT_NO_RESPONSE_ERROR */
    }
    result = fwrite(EEPROM, EEPROM_BLOCK_SIZE, EEPROM_MAXBLOCKS, stream);
    if (result != EEPROM_MAXBLOCKS)
    {
        int test;

        test = ferror(stream);
        if (test != 0)
            return -1;
        test = feof(stream);
        if (test != 0)
            fprintf(stderr, "Warning:  EEPROM file seems truncated.\n");
        printf(
            "%u of %u bytes written.\n",
            (unsigned)result, EEPROM_BLOCK_SIZE * EEPROM_MAXBLOCKS);
    }
    failure = fclose(stream);
    if (failure != 0)
        fprintf(stderr, "Problem closing the file stream?\n");
    return 0;
}
