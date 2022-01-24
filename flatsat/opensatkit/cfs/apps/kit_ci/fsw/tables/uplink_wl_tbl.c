/*
** Purpose: Define KIT_CI's list of whitelisted commands
**
**
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
*/

#include "cfe_tbl_filedef.h"
#include "app_cfg.h"
#include "uplink.h"
// #include "mq2sb.h"
// #include "mqtt_msg.h"


/*
** UPLINK WL Table
** 
** Each table entry has the following:
**  Message ID:
*/
UPLINK_WL_TblData UPLINK_WL_Tbl =
{
   {
      {   .MsgId             = 0x1806},
      {   .MsgId             = 0x1826},
      {   .MsgId             = 0x1801},
      {   .MsgId             = 0x1812},
      {   .MsgId             = 0x1803},
      {   .MsgId             = 0x1804},
      {   .MsgId             = 0x1805},
      {   .MsgId             = 0x18B3},
      {   .MsgId             = 0x18B4},
      {   .MsgId             = 0x18B5},
      {   .MsgId             = 0x18B6},
      {   .MsgId             = 0x18B7},
      {   .MsgId             = 0x18B8},
      {   .MsgId             = 0x18B9},
      {   .MsgId             = 0x18BA},
      {   .MsgId             = 0x1FFD},
      {   .MsgId             = 0x18C3},
      {   .MsgId             = 0x18C4},
      {   .MsgId             = 0x18C5},
      {   .MsgId             = 0x18C6},
      {   .MsgId             = 0x18C7},
      {   .MsgId             = 0x18C8},
      {   .MsgId             = 0x18C9},
      {   .MsgId             = 0x18CA},
      {   .MsgId             = 0x1FFC},
      {   .MsgId             = 0x189F},
      {   .MsgId             = 0x18BB},
      {   .MsgId             = 0x188C},
      {   .MsgId             = 0x184C},
      {   .MsgId             = 0x189A},
      {   .MsgId             = 0x18AE},
      {   .MsgId             = 0x18A4},
      {   .MsgId             = 0x18A6},
      {   .MsgId             = 0x1890},
      {   .MsgId             = 0x1888},
      {   .MsgId             = 0x18A9},
      {   .MsgId             = 0x18AB},
      {   .MsgId             = 0x19F0},
      {   .MsgId             = 0x1802},
      {   .MsgId             = 0x1884},
      {   .MsgId             = 0x1895},
      {   .MsgId             = 0x1880},
      {   .MsgId             = 0x1883},
      {   .MsgId             = 0x19DC},
      {   .MsgId             = 0x19DF},
      {   .MsgId             = 0x19F6},
      {   .MsgId             = 0x18FA},
      {   .MsgId             = 0x18FB},
      {   .MsgId             = 0x1882},
      {   .MsgId             = 0x1F50},
      {   .MsgId             = 0x1F52},
      {   .MsgId             = 0x1910},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000},
      {   .MsgId             = 0x0000}
   }
}; 

CFE_TBL_FILEDEF(UPLINK_WL_Tbl,KIT_CI.UPLINK_WL_Tbl,KIT CI Whitlist Table, uplink_wl_tbl.tbl)
