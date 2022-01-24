/************************************************************************
 ** File:
 **   $Id: cs_tablestbl.c 1.1.1.1 2015/03/03 11:58:36EST sstrege Exp  $
 **
 **   Copyright � 2007-2014 United States Government as represented by the 
 **   Administrator of the National Aeronautics and Space Administration. 
 **   All Other Rights Reserved.  
 **
 **   This software was created at NASA's Goddard Space Flight Center.
 **   This software is governed by the NASA Open Source Agreement and may be 
 **   used, distributed and modified only pursuant to the terms of that 
 **   agreement. 
 **
 ** Purpose: 
 **  The CFS Checksum (CS) Application Unit Test Tables Table Definition
 **
 ** Notes:
 **  This is a partially-populated table with duplicate entries.
 **  This table should be located in unit_test\cf\apps\DupEntryNonFullTable
 **
 ** $Log: cs_tablestbl.c  $
 ** Revision 1.1.1.1 2015/03/03 11:58:36EST sstrege 
 ** Added copyright information
 ** Revision 1.1 2011/09/06 15:23:38EDT jmdagost 
 ** Initial revision
 ** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cs/fsw/unit_test/cf/apps/DupEntryNonFullTable/project.pj
 ** Revision 1.1 2010/05/03 17:35:47EDT jmdagost 
 ** Initial revision
 ** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cs/fsw/unit_test/cf/apps/DupEntryNonFullTable/project.pj
 **
 *************************************************************************/

#include "cfe.h"
#include "cs_platform_cfg.h"
#include "cs_msgdefs.h"
#include "cs_tbldefs.h"
#include "cfe_tbl_filedef.h"

static CFE_TBL_FileDef_t CFE_TBL_FileDef =
{
    "CS_TablesTable", "CS.DefTablesTbl", "CS Tables Tbl",
    "cs_tablestbl.tbl", sizeof (CS_Def_Tables_Table_Entry_t) * CS_MAX_NUM_TABLES_TABLE_ENTRIES
};

CS_Def_Tables_Table_Entry_t CS_TablesTable[CS_MAX_NUM_TABLES_TABLE_ENTRIES]= 
{
    /*         State               Name   */
    /*   0 */ { CS_STATE_ENABLED,   "CS.DefAppTbl"             },
    /*   1 */ { CS_STATE_ENABLED,   "CS.DefTablesTbl"          },
    /*   2 */ { CS_STATE_ENABLED,   "CS.DefMemoryTbl"          },
    /*   3 */ { CS_STATE_ENABLED,   "CS.DefEepromTbl"          },
    /*   4 */ { CS_STATE_ENABLED,   "AA.Table1"                },  
    /*   5 */ { CS_STATE_DISABLED,  "BB.Table1"                },  /* Entry disabled, duplicated below */
    /*   6 */ { CS_STATE_ENABLED,   "CC.Table1"                },  /* Entry duplicated below */
    /*   7 */ { CS_STATE_ENABLED,   "DD.Table1"                },  /* Entry duplicated below */
    /*   8 */ { CS_STATE_ENABLED,   "EE.Table1"                },
    /*   9 */ { CS_STATE_ENABLED,   "FF.Table1"                },
    /*  10 */ { CS_STATE_ENABLED,   "GG.Table1"                },  /* Entry duplicated below */
    /*  11 */ { CS_STATE_EMPTY,     ""                         },
    /*  12 */ { CS_STATE_EMPTY,     ""                         },
    /*  13 */ { CS_STATE_ENABLED,   "JJ.Table1"                },
    /*  14 */ { CS_STATE_ENABLED,   "KK.Table1"                },
    /*  15 */ { CS_STATE_EMPTY,     ""                         },
    /*  16 */ { CS_STATE_ENABLED,   "CC.Table1"                },  /* Duplicate entry, marked as enabled */
    /*  17 */ { CS_STATE_EMPTY,     "BB.Table1"                },
    /*  18 */ { CS_STATE_EMPTY,     ""                         },
    /*  19 */ { CS_STATE_EMPTY,     ""                         },  /* Duplicate entry, marked as empty */
    /*  20 */ { CS_STATE_EMPTY,     "GG.Table1"                },  /* Duplicate entry, marked as disabled */
    /*  21 */ { CS_STATE_DISABLED,  "DD.Table1"                },
    /*  22 */ { CS_STATE_DISABLED,  "BB.Table1"                },  /* Duplicate entry, marked as disabled */
    /*  23 */ { CS_STATE_EMPTY,     ""                         },
    
};

/************************/
/*  End of File Comment */
/************************/
