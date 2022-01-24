
/*
 * Filename: uttools.c
 *
 *  Copyright � 2007-2014 United States Government as represented by the 
 *  Administrator of the National Aeronautics and Space Administration. 
 *  All Other Rights Reserved.  
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *  This software is governed by the NASA Open Source Agreement and may be 
 *  used, distributed and modified only pursuant to the terms of that 
 *  agreement. 
 *
 * Purpose: This file contains functions to implement a set of tools for use in unit testing.
 *
 */

/*
 * Includes
 */

#include "common_types.h"
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

/*
 * Function Definitions
 */

boolean UtMem2BinFile(void *Memory, uint8 *Filename, uint32 Length)
{
    FILE   *fp;

    if ((fp = fopen(Filename, "w"))) {
        fwrite(Memory, Length, 1, fp);
        fclose(fp);
        return(TRUE);
    }
    else {
        printf("UtMem2BinFile: Error Opening File: %s, %s\n", Filename, strerror(errno));
        return(FALSE);
    }
}

boolean UtBinFile2Mem(void *Memory, uint8 *Filename, uint32 Length)
{
    FILE   *fp;

    if ((fp = fopen(Filename, "r"))) {     
        fread(Memory, Length, 1, fp);
        fclose(fp);
        return(TRUE);
    }
    else {
        printf("UtBinFile2Mem: Error Opening File: %s, %s\n", Filename, strerror(errno));
        return(FALSE);
    }
}

boolean UtMem2HexFile(void *Memory, uint8 *Filename, uint32 Length)
{
    FILE        *fp;
    uint32       i;
    uint32       j;

    if ((fp = fopen(Filename, "w"))) {

        for (i=0; i < Length; i+=16) {
            fprintf(fp, "   %06lX: ", i);
            for (j=0; j < 16; j++) {
                if ((i+j) < Length)
                    fprintf(fp, "%02X ", ((uint8 *)Memory)[i+j]);
                else
                    fprintf(fp, "   ");
            }
            fprintf(fp, " ");
            for (j=0; j < 16; j++) {
                if ((i+j) < Length)
                    fprintf(fp, "%c", isprint(((uint8 *)Memory)[i+j]) ? ((uint8 *)Memory)[i+j] : '.');
            }
            fprintf(fp, "\n");
        }
        fclose(fp);
        return(TRUE);
    }
    else {
        printf("UtMem2HexFile: Error Opening File: %s, %s\n", Filename, strerror(errno));
        return(FALSE);
    }
}

void UtMemFill(void *Memory, uint32 Length)
{
    uint32 i;
    uint8  *Byte_ptr = Memory;

    for(i=0; i < Length; i++) {
        Byte_ptr[i] = i;
    }
}

void UtPrintf(char *Spec, ...)
{
#ifdef UT_VERBOSE
    va_list         Args;
    static char     Text[256];

    va_start(Args, Spec);
    vsprintf(Text, Spec, Args);
    va_end(Args);

    printf("%s", Text);
#else
    (void)Spec;
#endif
}

char *UtSprintf(char *Spec, ...)
{
    va_list         Args;
    static char     Text[10][256];
    static uint32   TextIndex = 0;

    if (TextIndex >= 10) TextIndex = 0;

    va_start(Args, Spec);
    vsprintf(Text[TextIndex], Spec, Args);
    va_end(Args);

    return(Text[TextIndex++]);
}

void UtPrintx(void *Memory, uint32 Length)
{
    uint32 i;
    uint8  *Byte_ptr = Memory;

    for (i=0; i < Length; i++) {
        UtPrintf("%02X ", Byte_ptr[i]);
    }
    UtPrintf("\n");
}

boolean UtMemCmpValue(void *Memory, uint8 Value, uint32 Length)
{
    uint32 i;
    uint8  *Byte_ptr = Memory;

    for (i=0; i < Length; i++) {
        if (Byte_ptr[i] != Value) {
            return(FALSE);
        }
    }
    return (TRUE);
}

boolean UtMemCmpCount(void *Memory, uint32 Length)
{
    uint32 i;
    uint8  *Byte_ptr = Memory;

    for (i=0; i < Length; i++) {
        if (Byte_ptr[i] != (i & 0xFF)) {
           return(FALSE);
        }
    }
    return (TRUE);
}

boolean UtMem2BinFileCmp(void *Memory, uint8 *Filename)
{
    FILE   *fp;
    uint8  *MemByte = Memory;
    int     FileByte;
    boolean Success;
    uint32  i;

    Success = TRUE;
    if ((fp = fopen(Filename, "r"))) {

        for (i=0; (FileByte = fgetc(fp)) != EOF; i++) {
            if (MemByte[i] != FileByte) {
                Success = FALSE;
                printf("UtMem2BinFileCmp: Miscompare in file: %s, byte offset: %lu, expected: %u, found: %u\n", Filename, i, MemByte[i], FileByte);
                break;
            }            
        }
        fclose(fp);
    }
    else {
        Success = FALSE;
        printf("UtMem2BinFileCmp: Error Opening File: %s, %s\n", Filename, strerror(errno));
    }

    return(Success);
}

