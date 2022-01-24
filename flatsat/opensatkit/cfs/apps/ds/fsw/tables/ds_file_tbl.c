
/************************************************************************
**
** $Id: ds_file_tbl.c 1.11.1.1 2015/02/28 17:13:55EST sstrege Exp  $
**
**  Copyright � 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** CFS Data Storage (DS) sample destination file table
**
** Note: This source file creates a sample table that defines several
**       data storage destination files using a variety of the options
**       available. None of the file system details (name, size, etc.)
**       are known at this time for the target platform. Therefore,
**       the file pathnames are set to "set_by_cmd_b4_enable" which
**       indicates that pathnames must be set by command before trying
**       to enable any of the destination files. Max file size values
**       should also be modified via command before using this table.
**
**       Obviously, a better solution is to replace this sample table
**       and the sample packet filter table (which references this
**       table) with mission specific versions that define the data
**       storage behavior appropriate for the platform.
**
**       But, as long as the target platform has a file system, the
**       sample data storage tables may be used to demonstrate data
**       storage.
**
** $Log: ds_file_tbl.c  $
** Revision 1.11.1.1 2015/02/28 17:13:55EST sstrege 
** Added copyright information
** Revision 1.11 2012/07/20 16:49:24EDT aschoeni 
** Restored subobject naming (but now as comments)
** Revision 1.10 2012/07/20 16:32:23EDT aschoeni 
** Fixed compiler warnings
** Revision 1.9 2010/11/09 15:10:40EST lwalling 
** Added conditional field initializers for Movename
** Revision 1.8 2010/02/23 11:30:57EST lwalling 
** Change application name from DS_APP to DS per CFS naming convention
** Revision 1.7 2009/10/06 10:36:47EDT lwalling 
** Change filename reference in destination file table header
** Revision 1.6 2009/09/02 15:29:04EDT lwalling 
** Member renamed from ds_file.c to ds_file_tbl.c in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/ds/fsw/tables/project.pj.
** Revision 1.5 2009/09/02 14:29:04ACT lwalling 
** Make table filenames match source filenames
** Revision 1.4 2009/08/27 16:32:29EDT lwalling 
** Member renamed from ds_sample_filetable.c to ds_file.c in project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/ds/fsw/tables/project.pj.
** Revision 1.3 2009/08/27 15:32:29ACT lwalling 
** Updates from source code review
** Revision 1.2 2009/07/15 10:16:20EDT lwalling 
** Include application header file due to changes made to platform configuration header file
** Revision 1.1 2009/06/16 17:06:58EDT lwalling 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/ds/fsw/tables/project.pj
**
*************************************************************************/

#include "cfe.h"
#include "cfe_tbl_filedef.h"
#include "ds_platform_cfg.h"
#include "ds_appdefs.h"
#include "ds_app.h"
#include "ds_msg.h"


/*
** Note: It is suggested that missions pre-define their file table
**       index numbers in a public header file to be included by
**       both the packet filter table source file and the destination
**       file table source file. Common definitions may also be used
**       when creating ground system database entries that require
**       file index numbers for command arguments.
*/
/* Moved definitions to ds_platfororm_cfg.h */


/*
** Sample Destination File Table Data
*/
DS_DestFileTable_t DS_DestFileTable =
{
  /* .Descriptor = */ "OpenSatKit Default",
  /* .File       = */
  {
    /* File Index 00 -- event packets only */
    {
#if (DS_MOVE_FILES == TRUE)
      /* .Movename      = */ DS_EMPTY_STRING,
#endif
      /* .Pathname      = */ "/cf/simsat",
      /* .Basename      = */ "events",
      /* .Extension     = */ ".dat",

      /* .FileNameType  = */ DS_BY_COUNT,
      /* .EnableState   = */ DS_DISABLED,
      /* .MaxFileSize   = */ (1024 * 32),               /* 32 K-bytes */
      /* .MaxFileAge    = */ (60 * 4),                  /* 4 minutes - Keep short to generate multiple files in example run */
      /* .SequenceCount = */ 1000,
    },

    /* File Index 01 -- cFE housekeeping packets */
    {
#if (DS_MOVE_FILES == TRUE)
      /* .Movename      = */ DS_EMPTY_STRING,
#endif
      /* .Pathname      = */ "/cf/simsat",
      /* .Basename      = */ "cfe",
      /* .Extension     = */ ".hk",

      /* .FileNameType  = */ DS_BY_COUNT,
      /* .EnableState   = */ DS_DISABLED,
      /* .MaxFileSize   = */ (1024 * 1024 * 2),         /* 2 M-bytes */
      /* .MaxFileAge    = */ (60 * 60 * 2),             /* 2 hours */
      /* .SequenceCount = */ 4000,
    },

    /* File Index 02 -- application housekeeping packets */
    {
#if (DS_MOVE_FILES == TRUE)
      /* .Movename      = */ DS_EMPTY_STRING,
#endif
      /* .Pathname      = */ "/cf/simsat",
      /* .Basename      = */ "app",
      /* .Extension     = */ ".hk",

      /* .FileNameType  = */ DS_BY_TIME,
      /* .EnableState   = */ DS_DISABLED,
      /* .MaxFileSize   = */ (1024 * 1024 * 2),         /* 2 M-bytes */
      /* .MaxFileAge    = */ (60 * 60 * 2),             /* 2 hours */
      /* .SequenceCount = */ DS_UNUSED,
    },

    /* File Index 03 -- application telemetry packets */
    {
#if (DS_MOVE_FILES == TRUE)
      /* .Movename      = */ DS_EMPTY_STRING,
#endif
      /* .Pathname      = */ "/cf/simsat",
      /* .Basename      = */ "app",
      /* .Extension     = */ ".tlm",

      /* .FileNameType  = */ DS_BY_COUNT,
      /* .EnableState   = */ DS_DISABLED,
      /* .MaxFileSize   = */ (1024 * 1024 * 1024 * 1),  /* 1 G-byte */
      /* .MaxFileAge    = */ (60 * 60 * 2),             /* 2 hours */
      /* .SequenceCount = */ 2000,
    },

    /* File Index 04 -- hardware telemetry packets */
    {
#if (DS_MOVE_FILES == TRUE)
      /* .Movename      = */ DS_EMPTY_STRING,
#endif
      /* .Pathname      = */ "/cf/simsat",
      /* .Basename      = */ "hw",
      /* .Extension     = */ "tlm",

      /* .FileNameType  = */ DS_BY_COUNT,
      /* .EnableState   = */ DS_DISABLED,
      /* .MaxFileSize   = */ (1024 * 1024 * 2),         /* 2 M-bytes */
      /* .MaxFileAge    = */ (60 * 60 * 2),             /* 2 hours */
      /* .SequenceCount = */ 3000,
    },

    /* File Index 05 -- cFE telemetry packets */
    {
#if (DS_MOVE_FILES == TRUE)
      /* .Movename      = */ DS_EMPTY_STRING,
#endif
      /* .Pathname      = */ "/cf/simsat",
      /* .Basename      = */ "cfe",
      /* .Extension     = */ "tlm",

      /* .FileNameType  = */ DS_BY_COUNT,
      /* .EnableState   = */ DS_DISABLED,
      /* .MaxFileSize   = */ (1024 * 1024 * 2),         /* 2 M-bytes */
      /* .MaxFileAge    = */ (60 * 60 * 2),             /* 2 hours */
      /* .SequenceCount = */ 5000,
    },

    /* File Index 06 */
    {
#if (DS_MOVE_FILES == TRUE)
      /* .Movename      = */ DS_EMPTY_STRING,
#endif
      /* .Pathname      = */ "/cf/simsat",
      /* .Basename      = */ "sci_aux",
      /* .Extension     = */ ".dat",

      /* .FileNameType  = */ DS_BY_COUNT,
      /* .EnableState   = */ DS_DISABLED,
      /* .MaxFileSize   = */ (1024 * 32),               /* 32 K-bytes */
      /* .MaxFileAge    = */ (60 * 4),                  /* 4 minutes - Keep short to generate multiple files in example run */
      /* .SequenceCount = */ 1000,
    },
        
    /* File Index 07 */
    {
#if (DS_MOVE_FILES == TRUE)
      /* .Movename      = */ DS_EMPTY_STRING,
#endif
      /* .Pathname      = */ DS_EMPTY_STRING,
      /* .Basename      = */ DS_EMPTY_STRING,
      /* .Extension     = */ DS_EMPTY_STRING,

      /* .FileNameType  = */ DS_UNUSED,
      /* .EnableState   = */ DS_UNUSED,
      /* .MaxFileSize   = */ DS_UNUSED,
      /* .MaxFileAge    = */ DS_UNUSED,
      /* .SequenceCount = */ DS_UNUSED,
    },
    /* File Index 08 */
    {
#if (DS_MOVE_FILES == TRUE)
      /* .Movename      = */ DS_EMPTY_STRING,
#endif
      /* .Pathname      = */ DS_EMPTY_STRING,
      /* .Basename      = */ DS_EMPTY_STRING,
      /* .Extension     = */ DS_EMPTY_STRING,

      /* .FileNameType  = */ DS_UNUSED,
      /* .EnableState   = */ DS_UNUSED,
      /* .MaxFileSize   = */ DS_UNUSED,
      /* .MaxFileAge    = */ DS_UNUSED,
      /* .SequenceCount = */ DS_UNUSED,
    },
    /* File Index 09 */
    {
#if (DS_MOVE_FILES == TRUE)
      /* .Movename      = */ DS_EMPTY_STRING,
#endif
      /* .Pathname      = */ DS_EMPTY_STRING,
      /* .Basename      = */ DS_EMPTY_STRING,
      /* .Extension     = */ DS_EMPTY_STRING,

      /* .FileNameType  = */ DS_UNUSED,
      /* .EnableState   = */ DS_UNUSED,
      /* .MaxFileSize   = */ DS_UNUSED,
      /* .MaxFileAge    = */ DS_UNUSED,
      /* .SequenceCount = */ DS_UNUSED,
    },
    /* File Index 10 */
    {
#if (DS_MOVE_FILES == TRUE)
      /* .Movename      = */ DS_EMPTY_STRING,
#endif
      /* .Pathname      = */ DS_EMPTY_STRING,
      /* .Basename      = */ DS_EMPTY_STRING,
      /* .Extension     = */ DS_EMPTY_STRING,

      /* .FileNameType  = */ DS_UNUSED,
      /* .EnableState   = */ DS_UNUSED,
      /* .MaxFileSize   = */ DS_UNUSED,
      /* .MaxFileAge    = */ DS_UNUSED,
      /* .SequenceCount = */ DS_UNUSED,
    },
    /* File Index 11 */
    {
#if (DS_MOVE_FILES == TRUE)
      /* .Movename      = */ DS_EMPTY_STRING,
#endif
      /* .Pathname      = */ DS_EMPTY_STRING,
      /* .Basename      = */ DS_EMPTY_STRING,
      /* .Extension     = */ DS_EMPTY_STRING,

      /* .FileNameType  = */ DS_UNUSED,
      /* .EnableState   = */ DS_UNUSED,
      /* .MaxFileSize   = */ DS_UNUSED,
      /* .MaxFileAge    = */ DS_UNUSED,
      /* .SequenceCount = */ DS_UNUSED,
    },
    /* File Index 12 */
    {
#if (DS_MOVE_FILES == TRUE)
      /* .Movename      = */ DS_EMPTY_STRING,
#endif
      /* .Pathname      = */ DS_EMPTY_STRING,
      /* .Basename      = */ DS_EMPTY_STRING,
      /* .Extension     = */ DS_EMPTY_STRING,

      /* .FileNameType  = */ DS_UNUSED,
      /* .EnableState   = */ DS_UNUSED,
      /* .MaxFileSize   = */ DS_UNUSED,
      /* .MaxFileAge    = */ DS_UNUSED,
      /* .SequenceCount = */ DS_UNUSED,
    },
    /* File Index 13 */
    {
#if (DS_MOVE_FILES == TRUE)
      /* .Movename      = */ DS_EMPTY_STRING,
#endif
      /* .Pathname      = */ DS_EMPTY_STRING,
      /* .Basename      = */ DS_EMPTY_STRING,
      /* .Extension     = */ DS_EMPTY_STRING,

      /* .FileNameType  = */ DS_UNUSED,
      /* .EnableState   = */ DS_UNUSED,
      /* .MaxFileSize   = */ DS_UNUSED,
      /* .MaxFileAge    = */ DS_UNUSED,
      /* .SequenceCount = */ DS_UNUSED,
    },
    /* File Index 14 */
    {
#if (DS_MOVE_FILES == TRUE)
      /* .Movename      = */ DS_EMPTY_STRING,
#endif
      /* .Pathname      = */ DS_EMPTY_STRING,
      /* .Basename      = */ DS_EMPTY_STRING,
      /* .Extension     = */ DS_EMPTY_STRING,

      /* .FileNameType  = */ DS_UNUSED,
      /* .EnableState   = */ DS_UNUSED,
      /* .MaxFileSize   = */ DS_UNUSED,
      /* .MaxFileAge    = */ DS_UNUSED,
      /* .SequenceCount = */ DS_UNUSED,
    },
    /* File Index 15 */
    {
#if (DS_MOVE_FILES == TRUE)
      /* .Movename      = */ DS_EMPTY_STRING,
#endif
      /* .Pathname      = */ DS_EMPTY_STRING,
      /* .Basename      = */ DS_EMPTY_STRING,
      /* .Extension     = */ DS_EMPTY_STRING,

      /* .FileNameType  = */ DS_UNUSED,
      /* .EnableState   = */ DS_UNUSED,
      /* .MaxFileSize   = */ DS_UNUSED,
      /* .MaxFileAge    = */ DS_UNUSED,
      /* .SequenceCount = */ DS_UNUSED,
    },
  }
};

/*
** Sample Destination File Table Header
*/
CFE_TBL_FILEDEF(DS_DestFileTable, DS.FILE_TBL, DS Destination File Table,ds_file_tbl.tbl)


/************************/
/*  End of File Comment */
/************************/
