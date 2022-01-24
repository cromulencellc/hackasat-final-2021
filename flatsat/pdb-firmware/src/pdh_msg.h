#ifndef _pdb_msg_h
#define _pdb_msg_h

#include "cfe_sb.h"

typedef struct {
   uint16_t raw;
   // float    converted;
} __attribute__((__packed__)) AnalogData;

typedef struct
{

   uint8_t    Header[CFE_SB_TLM_HDR_SIZE];

   /*
   ** CMDMGR Data
   */
   uint32_t   ValidCmdCnt;
   uint32_t   InvalidCmdCnt;

   /*
   ** PDB Control Data
   */
   uint32_t   CtrlExeCnt;

   /*
   ** PDB 5V Channel State
   */
  uint8_t   ChannelStateSet[OUTPUT_CHANNEL_COUNT];
  uint8_t   ChannelStateRead[OUTPUT_CHANNEL_COUNT];

  uint16_t ChannelAnalogVoltage[7];
  uint16_t ChannelAnalogCurrent[6];
  uint16_t SystemAnalogVoltage[6];
  
} __attribute__((__packed__)) PDB_HkPkt;

#define PDB_TLM_HK_LEN sizeof (PDB_HkPkt)

typedef struct
{

   uint8_t    Header[CFE_SB_TLM_HDR_SIZE];
   uint8_t    ChargeEnabled;
   uint16_t   MaxChargeVoltage;
   uint16_t   MaxChargeCurrent;
   uint16_t   SystemPowerUtilization;
   uint16_t   VBUS;
   uint16_t   VBAT;
   uint16_t   VSYS;
   uint16_t   ActualChargeCurrent;
   uint16_t   ActualDischargeCurrent;
  
} __attribute__((__packed__)) PDB_ChargerPkt;
#define PDB_TLM_CHARGER_LEN sizeof (PDB_ChargerPkt)


typedef struct {

    uint8_t   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8_t   ChannelId;
    uint8_t   State;

}  __attribute__((__packed__)) PDB_OutputChannelCmdPkt;
#define PDB_OUTPUT_CHANNEL_CMD_DATA_LEN  (sizeof(PDB_OutputChannelCmdPkt) - CFE_SB_CMD_HDR_SIZE)



#endif