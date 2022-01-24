/*
** Purpose: Define Example Table
**
** Notes:
**   1. Use the Singleton design pattern. A pointer to the table object
**      is passed to the constructor and saved for all other operations.
**      This is a table-specific file so it doesn't need to be re-entrant.
**
** License:
**   Template written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/
#ifndef _extbl_
#define _extbl_

/*
** Includes
*/

#include "app_cfg.h"
#include "adcs_hw_cfg.h"

/*
** Macro Definitions
*/

/*
** Event Message IDs
*/

#define ADCS_CONF_TBL_CREATE_FILE_ERR_EID        (ADCS_CONF_TBL_BASE_EID + 0)
#define ADCS_CONF_TBL_LOAD_INDEX_ERR_EID         (ADCS_CONF_TBL_BASE_EID + 1)
#define ADCS_CONF_TBL_LOAD_LINE_ERR_EID          (ADCS_CONF_TBL_BASE_EID + 2)
#define ADCS_CONF_TBL_LOAD_CMD_TYPE_ERR_EID      (ADCS_CONF_TBL_BASE_EID + 3)
#define ADCS_CONF_TBL_LOAD_CMD_PARSE_ERR_EID     (ADCS_CONF_TBL_BASE_EID + 4)
#define ADCS_CONF_TBL_LOAD_CMD_JSON_OPEN_ERR_EID (ADCS_CONF_TBL_BASE_EID + 5)
#define ADCS_CONF_TBL_LOAD_CMD_DBG_EID           (ADCS_CONF_TBL_BASE_EID + 6)

/*
** Type Definitions
*/

#define  ADCS_CONF_TBL_OBJ_KR       0
#define  ADCS_CONF_TBL_OBJ_KP       1
#define  ADCS_CONF_TBL_OBJ_KUNL     2
#define  ADCS_CONF_TBL_OBJ_HCMD_LIM 3
#define  ADCS_CONF_TBL_OBJ_CNT      4

#define ADCS_CONF_TBL_OBJ_NAME_IMU        "imu"
#define ADCS_CONF_TBL_OBJ_NAME_MTR        "mtr"
#define ADCS_CONF_TBL_OBJ_NAME_WHEEL      "wheel"
#define ADCS_CONF_TBL_OBJ_NAME_CONTROLLER "controller"

#define  ADCS_CONF_TBL_OBJ_NAME_KR       "kr"
#define  ADCS_CONF_TBL_OBJ_NAME_KP       "kp"
#define  ADCS_CONF_TBL_OBJ_NAME_KUNL     "kunl"
#define  ADCS_CONF_TBL_OBJ_NAME_HCMD_LIM "hcmd-lim"


/******************************************************************************
** Table -  Local table copy used for table loads
** 
*/
typedef struct {
   char gpiochip[MAX_GPIO_DEVICE_NAME];
   uint8_t linenum;
   uint8_t initialState;
} GpioLine_Struct;

typedef struct {
   char channelName[TORQUEROD_NAME_MAX_CHAR];
   GpioLine_Struct channel_p;
   GpioLine_Struct channel_n;
} MtrChannel_Struct;

typedef struct {
   uint8_t channelCount;
   MtrChannel_Struct mtrChannels[ADCS_MTR_MAX_CHANNELS];
} MtrData_Struct;

typedef struct {
   uint8_t sensorIdx;
   uint8_t sensorId;
   uint32_t updateRate; // usec
   uint8_t sensorDataSize; // bytes
} ImuSensor_Struct;

typedef struct {
   char i2cPath[MAX_DEVICE_NAME_LEN];
   uint8_t i2cAddr;
   uint8_t i2cType;
} I2CData_Struct;

typedef struct {
   char uartPath[MAX_DEVICE_NAME_LEN];
   uint16_t timeout;
   uint16_t baudRate;
} UARTData_Struct;

typedef struct {
   I2CData_Struct i2cConfig;
   GpioLine_Struct imuResetGpio;
   GpioLine_Struct imuIntGpio;
   boolean imuIntEnabled;
   uint8_t sensorCount;
   ImuSensor_Struct imuSensors[ADCS_IMU_NUM_SENSORS];
   uint8_t readTimeoutCnt;
   uint16 pollRateMs;
} ImuData_Struct;

typedef struct {
   I2CData_Struct i2cConfig;
   GpioLine_Struct enableGpio;
} WheelDevice_Struct;

typedef struct {
   uint8_t controlMode;
   uint8_t wheelCount;
   uint16 pollRateMs;
   WheelDevice_Struct wheelData[ADCS_WHEEL_COUNT_MAX];
} WheelData_Struct;

typedef struct {
   char sockAddr[16];
   uint16_t port;
} UDPSocket_Struct;

typedef struct {
   uint8_t useFakeSt;
   uint32_t mode;
   UARTData_Struct uartConfig;
   UDPSocket_Struct fakeStConfig;
} StarTrackerData_Struct;

// _ADCS_CHALLENGE_
typedef struct{
   uint8_t enabled;
   float calibration[3];
} FssData_Struct;

typedef struct {
   char channelName[20];
} XadcChannel_Struct;
typedef struct {
   uint8_t channelCount;
   uint32_t sampleRate;
   char iioDevice[20];
   char iioTrigger[20];
   bool channelHasOffset[XADC_MAX_CHANNEL_COUNT];
   char *channelNames[20];
} XadcData_Struct;

typedef struct {

   float X;
   float Y;
   float Z;

} Kp_Struct;

typedef struct {

   float X;
   float Y;
   float Z;

} Kr_Struct;

typedef struct {

   float Lower;
   float Upper;

} HcmdLim_Struct;

typedef struct {
   /* Attitude Controller */
   Kp_Struct   Kp;
   Kr_Struct   Kr;

   /* Momentum Unload Controller */
   HcmdLim_Struct HcmdLim;
   float          Kunl;
} Controller_Struct;

typedef struct {
   uint8_t HardwareStartCnt;
   uint8_t HardwareIdxToStart[ADCS_HW_DEVICE_CNT_MAX];
   uint16_t ControlIntervalMsec;
   ImuData_Struct Imu;
   MtrData_Struct Mtr;
   WheelData_Struct Wheel;
   XadcData_Struct   Analog;
   StarTrackerData_Struct StarTracker;
   // _ADCS_CHALLENGE_
   FssData_Struct Fss;
   Controller_Struct Controller;
} ADCS_CONF_TBL_Struct;


/*
** Table Owner Callback Functions
*/

/* Return pointer to owner's table data */
typedef const ADCS_CONF_TBL_Struct* (*ADCS_CONF_TBL_GetTblPtr)(void);
            
/* Table Owner's function to load all table data */
typedef boolean (*ADCS_CONF_TBL_LoadTbl)(ADCS_CONF_TBL_Struct* NewTable); 

/* Table Owner's function to load a single table entry */
typedef boolean (*ADCS_CONF_TBL_LoadTblEntry)(uint16 ObjId, void* ObjData); 


typedef struct {

   uint8    LastLoadStatus;
   uint16   AttrErrCnt;
   uint16   DataArrayEntryIdx;
   boolean  Modified[ADCS_CONF_TBL_MAX_ENTRY_ID];

   ADCS_CONF_TBL_Struct Data;

   ADCS_CONF_TBL_GetTblPtr    GetTblPtrFunc;
   ADCS_CONF_TBL_LoadTbl      LoadTblFunc;
   ADCS_CONF_TBL_LoadTblEntry LoadTblEntryFunc; 

} ADCS_CONF_TBL_Class;


/*
** Exported Functions
*/


/******************************************************************************
** Function: ADCS_CONF_TBL_Constructor
**
** Initialize the example table object.
**
** Notes:
**   1. The table values are not populated. This is done when the table is 
**      registered with the table manager.
**
*/
void ADCS_CONF_TBL_Constructor(ADCS_CONF_TBL_Class* TblObj, 
                       ADCS_CONF_TBL_GetTblPtr    GetTblPtrFunc,
                       ADCS_CONF_TBL_LoadTbl      LoadTblFunc, 
                       ADCS_CONF_TBL_LoadTblEntry LoadTblEntryFunc);


/******************************************************************************
** Function: ADCS_CONF_TBL_ResetStatus
**
** Reset counters and status flags to a known reset state.  The behavour of
** the table manager should not be impacted. The intent is to clear counters
** and flags to a known default state for telemetry.
**
*/
void ADCS_CONF_TBL_ResetStatus(void);


/******************************************************************************
** Function: ADCS_CONF_TBL_LoadCmd
**
** Command to load the table.
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean ADCS_CONF_TBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename);


/******************************************************************************
** Function: ADCS_CONF_TBL_DumpCmd
**
** Command to dump the table.
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean ADCS_CONF_TBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename);

#endif /* _adcs_conf_tbl_ */

