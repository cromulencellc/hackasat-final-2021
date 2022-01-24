/************************************************************************
** File:
**   $Id: cf_msgids.h 1.5 2015/03/06 14:38:32EST sstrege Exp  $
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
**  The CF Application Message IDs header file
**
** Notes:
**
** $Log: cf_msgids.h  $
** Revision 1.5 2015/03/06 14:38:32EST sstrege 
** Added copyright information
** Revision 1.4 2010/10/20 11:06:35EDT rmcgraw 
** DCR12495:1 Removed unused MsgId CF_DIAG_TLM_MID
** Revision 1.3 2010/03/12 12:14:35EST rmcgraw 
** DCR11510:1 Initial check-in towards CF Version 1000
** Revision 1.2 2009/12/08 09:03:22EST rmcgraw 
** DCR10350:3 Replaced History Tlm with Config tlm
** Revision 1.1 2009/11/24 12:47:35EST rmcgraw 
** Initial revision
** Member added to CFS CF project
**
*************************************************************************/
#ifndef _cf_msgids_h_
#define _cf_msgids_h_

/**************************
** CF Command Message IDs
***************************/

#define CF_CMD_MID                      0x18C3
#define CF_SEND_HK_MID                  0x18C4 
#define CF_WAKE_UP_REQ_CMD_MID          0x18C5
#define CF_SPARE1_CMD_MID               0x18C6
#define CF_SPARE2_CMD_MID               0x18C7
#define CF_SPARE3_CMD_MID               0x18C8
#define CF_SPARE4_CMD_MID               0x18C9
#define CF_SPARE5_CMD_MID               0x18CA
#define CF_INCOMING_PDU_MID             0x1FFC 


/***************************
** CF Telemetry Message IDs
****************************/

#define CF_HK_TLM_MID                   0x08C0
#define CF_TRANS_TLM_MID                0x08C1
#define CF_CONFIG_TLM_MID               0x08C2
#define CF_SPARE0_TLM_MID               0x08C3
#define CF_SPARE1_TLM_MID               0x08C4
#define CF_SPARE2_TLM_MID               0x08C5
#define CF_SPARE3_TLM_MID               0x08C6
#define CF_SPARE4_TLM_MID               0x08C7

/* 
** NOTE: the definition below is NOT used by the code. The code uses the MsgId 
** defined in the CF table. For the purpose of keeping all CF related message
** IDs defined in this file, the CF table should reference this macro 
** definition.
*/
#define CF_SPACE_TO_GND_PDU_MID         0x0FFC

#endif /* _cf_msgids_h_ */

/************************/
/*  End of File Comment */
/************************/
