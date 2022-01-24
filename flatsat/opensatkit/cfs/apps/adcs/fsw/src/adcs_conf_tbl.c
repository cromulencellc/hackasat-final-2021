/*
** Purpose: Implement example table.
**
** Notes:
**   None
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

/*
** Include Files:
*/

#include <string.h>
#include <stdio.h>
#include "adcs_conf_tbl.h"
#include "adcs_logger.h"
/*
** Type Definitions
*/


/*
** Global File Data
*/

static ADCS_CONF_TBL_Class* AdcsConfTbl = NULL;

/*
** Local File Function Prototypes
*/
void ADCS_CONF_TBL_SetDefault(void);
void ADCS_CONF_TBL_SetDefaultWithSt(void);
void ADCS_CONF_TBL_SetDefaultDevBoard(void);

/******************************************************************************
** Function: ADCS_CONF_TBL_Constructor
**
** Notes:
**    1. This must be called prior to any other functions
**
*/
void ADCS_CONF_TBL_Constructor(ADCS_CONF_TBL_Class* ObjPtr,
                       ADCS_CONF_TBL_GetTblPtr    GetTblPtrFunc,
                       ADCS_CONF_TBL_LoadTbl      LoadTblFunc, 
                       ADCS_CONF_TBL_LoadTblEntry LoadTblEntryFunc)
{

   AdcsConfTbl = ObjPtr;

   CFE_PSP_MemSet(AdcsConfTbl, 0, sizeof(ADCS_CONF_TBL_Class));

   AdcsConfTbl->GetTblPtrFunc    = GetTblPtrFunc;
   AdcsConfTbl->LoadTblFunc      = LoadTblFunc;
   AdcsConfTbl->LoadTblEntryFunc = LoadTblEntryFunc; 

} /* End ADCS_CONF_TBL_Constructor() */


/******************************************************************************
** Function: ADCS_CONF_TBL_ResetStatus
**
*/
void ADCS_CONF_TBL_ResetStatus(void)
{

   int Entry;

   AdcsConfTbl->LastLoadStatus    = TBLMGR_STATUS_UNDEF;
   AdcsConfTbl->AttrErrCnt        = 0;
   AdcsConfTbl->DataArrayEntryIdx = 0;
   
   for (Entry=0; Entry < ADCS_CONF_TBL_MAX_ENTRY_ID; Entry++) AdcsConfTbl->Modified[Entry] = FALSE;
   
 
} /* End ADCS_CONF_TBL_ResetStatus() */


/******************************************************************************
** Function: ADCS_CONF_TBL_LoadCmd
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
*/
boolean ADCS_CONF_TBL_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename)
{

   // int Entry;
   
   ADCS_LOG_INFO("ADCS_CONF_TBL_LoadCmd() Entry");

   /*
   ** Set all data and flags to zero. If a table replace is commanded and
   ** all of the data is not defined the zeroes will be copied into the table. 
   ** Real flight code would validate all data is loaded for a replace.
   */
   
   CFE_PSP_MemSet(&(AdcsConfTbl->Data), 0, sizeof(ADCS_CONF_TBL_Struct));  /* Wouldn't do in flight but helps debug prototype */
   
   ADCS_CONF_TBL_ResetStatus();  /* Reset status helps isolate errors if they occur */
   ADCS_CONF_TBL_SetDefault(); 
   
   int error = 0;

   if (!error) {
      OS_printf("JSON deserialization successful\n");

      CFE_EVS_SendEvent(ADCS_CONF_TBL_LOAD_CMD_DBG_EID,CFE_EVS_DEBUG,"ADCS_CONF_TBL: Successfully prepared file %s\n", Filename);
  
      AdcsConfTbl->DataArrayEntryIdx = 0;

      if (AdcsConfTbl->DataArrayEntryIdx > 0) {

         
         if (LoadType == TBLMGR_LOAD_TBL_REPLACE) {
         
            AdcsConfTbl->LastLoadStatus = ((AdcsConfTbl->LoadTblFunc)(&(AdcsConfTbl->Data)) == TRUE) ? TBLMGR_STATUS_VALID : TBLMGR_STATUS_INVALID;

         } /* End if replace entire table */
         else if (LoadType == TBLMGR_LOAD_TBL_UPDATE) {
         
            AdcsConfTbl->LastLoadStatus = TBLMGR_STATUS_VALID;

         } /* End if update individual records */
         else {
            CFE_EVS_SendEvent(ADCS_CONF_TBL_LOAD_CMD_TYPE_ERR_EID,CFE_EVS_ERROR,"ADCS_CONF_TBL: Invalid table command load type %d",LoadType);
         }

      } /* End if successful parse */
      else {
         CFE_EVS_SendEvent(ADCS_CONF_TBL_LOAD_CMD_PARSE_ERR_EID,CFE_EVS_ERROR,"ADCS_CONF_TBL: Table Parsing failure for file %s",Filename);
      } 
   } /* End if valid file */
   else {
      OS_printf("JSON deserialization error\n");
      CFE_EVS_SendEvent(ADCS_CONF_TBL_LOAD_CMD_JSON_OPEN_ERR_EID,CFE_EVS_ERROR,"ADCS_CONF_TBL: Table open failure for file %s.",
                       Filename);
   }
    
   return (AdcsConfTbl->LastLoadStatus == TBLMGR_STATUS_VALID);

} /* End of ExTBL_LoadCmd() */


/******************************************************************************
** Function: ADCS_CONF_TBL_DumpCmd
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
**  3. DumpType is unused.
**  4. File is formatted so it can be used as a load file. It does not follow
**     the cFE table file format. 
**  5. Creates a new dump file, overwriting anything that may have existed
**     previously
*/
boolean ADCS_CONF_TBL_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename)
{

   // Not implementented

   return TRUE;
   
} /* End of ADCS_CONF_TBL_DumpCmd() */

/******************************************************************************
** Function: ADCS_CONF_TBL_SetDefault
**
*/
#ifdef _FLATSAT_HARDWARE_FSW_
void ADCS_CONF_TBL_SetDefault(void)
{
   OS_printf("Load ADCS Conf Tbl Default Values\n");
   ADCS_CONF_TBL_Struct TblData = {
      .ControlIntervalMsec = 500,
      .HardwareStartCnt = 4,
      .HardwareIdxToStart = {ADCS_HW_DEVICE_IMU_IDX, ADCS_HW_DEVICE_XADC_IDX, ADCS_HW_DEVICE_MTR_IDX, ADCS_HW_DEVICE_RW_IDX },
      .Imu = {
         .i2cConfig = {
            .i2cPath = "/dev/i2c-1",
            .i2cAddr = BNO08X_I2CADDR_DEFAULT,
            .i2cType = AXI_I2C
         },
         .imuIntGpio = {
            .gpiochip = "gpiochip3",
            .linenum = 0,
            .initialState = 1
         }, 
         .imuResetGpio = {
            .gpiochip = "gpiochip3",
            .linenum = 1,
            .initialState = 1
         },
         .sensorCount = 4,
         .imuSensors = {
            {  
               .sensorIdx = 0,
               .sensorId = SH2_GYROSCOPE_CALIBRATED, 
               .updateRate = ADCS_IMU_UPDATE_RATE,
               .sensorDataSize = sizeof(sh2_Gyroscope_t)
            },
            {
               .sensorIdx = 1,
               .sensorId = SH2_ACCELEROMETER, 
               .updateRate = ADCS_IMU_UPDATE_RATE,
               .sensorDataSize = sizeof(sh2_Accelerometer_t)
            },
            {
               .sensorIdx = 2,
               .sensorId = SH2_MAGNETIC_FIELD_CALIBRATED, 
               .updateRate = ADCS_IMU_UPDATE_RATE,
               .sensorDataSize = sizeof(sh2_MagneticField_t)
            },
            {
               .sensorIdx = 3,
               .sensorId = SH2_GAME_ROTATION_VECTOR, 
               .updateRate = ADCS_IMU_UPDATE_RATE,
               .sensorDataSize = sizeof(sh2_RotationVectorWAcc_t)
            },

         },
         .imuIntEnabled = false,
         .pollRateMs = ADCS_IMU_POLL_RATE_MS,
         .readTimeoutCnt = ADCS_IMU_READ_TIMEOUT_CNT,
      },
      .Mtr = {
         .channelCount = 2,
         .mtrChannels = {
            {
               .channelName = "trod_x",
               .channel_p = {
                  .gpiochip = "gpiochip2",
                  .linenum = 0,
                  .initialState = 0
               },
               .channel_n = {
                  .gpiochip = "gpiochip2",
                  .linenum = 1,
                  .initialState = 0
               }
            },
            {
               .channelName = "trod_y",
               .channel_p = {
                  .gpiochip = "gpiochip2",
                  .linenum = 2,
                  .initialState = 0
               },
               .channel_n = {
                  .gpiochip = "gpiochip2",
                  .linenum = 3,
                  .initialState = 0
               }
            }
         }
      },
      .Wheel = {
         .controlMode = WHEEL_MODE_USE_SPEED_CONTROL,
         .pollRateMs = ADCS_WHEEL_POLL_RATE_MS,
         .wheelCount = 1,
         .wheelData = {
            {
               .i2cConfig = {
                  .i2cPath = "/dev/i2c-0",
                  .i2cAddr = CUBEWHEEL_I2CADDR,
                  .i2cType = AXI_I2C
               },
               .enableGpio = {
                  .gpiochip = "gpiochip4",
                  .linenum = 0,
                  .initialState = 0
               }
            }
         }
      },
      .Analog = {
         .iioDevice = "iio:device1",
         .iioTrigger = "xadc1-samplerate",
         .sampleRate = XADC_SAMPLE_RATE,
         .channelCount = 8,
         .channelNames = {
            "voltage14", 
            "voltage12",
            "voltage9",
            "voltage10",
            "voltage11",
            "voltage8",
            "voltage13",
            "temp0"
         },
         .channelHasOffset = {
            false, false, false, false, false, false, false, true
         }
      },
      .Controller = {
         .Kp = {
            .X = 2.759831372549e+01,
            .Y = 2.117500000000e+01,
            .Z = 3.835272549020e+01
         },
         .Kr = {
            .X = 3.863763921569e+02,
            .Y = 2.964500000000e+02,
            .Z =  5.369381568628e+02
         },
         .Kunl = 1.000000000000e+06,
         .HcmdLim = {
            .Lower = -0.9,
            .Upper = 0.9
         }
      }
   };
   
   CFE_PSP_MemCpy(&AdcsConfTbl->Data, &TblData, sizeof(ADCS_CONF_TBL_Struct));
 
} /* End ADCS_CONF_TBL_SetDefault() */

#endif

#ifdef _FLATSAT_SIM_FSW_


void ADCS_CONF_TBL_SetDefault(void)
{
   OS_printf("Load ADCS Conf Tbl Default Values\n");
   static const ADCS_CONF_TBL_Struct TblData = {
      .ControlIntervalMsec = 200,
      .HardwareStartCnt = 1,
      .HardwareIdxToStart = {ADCS_HW_DEVICE_XADC_IDX},
      .Analog = {
         .iioDevice = "iio:device1",
         .iioTrigger = "xadc1-samplerate",
         .sampleRate = XADC_SAMPLE_RATE,
         .channelCount = 3,
         .channelNames = {
            "voltage8",
            "voltage13",
            "temp0"
         },
         .channelHasOffset = {
            false, false, true
         }
      },
   };
   CFE_PSP_MemCpy(&AdcsConfTbl->Data, &TblData, sizeof(ADCS_CONF_TBL_Struct));
 
} /* End ADCS_CONF_TBL_SetDefault() */

#endif
/******************************************************************************
** Function: ADCS_CONF_TBL_SetDefault
**
*/
#ifdef _FLATSAT_HARDWARE_FSW
void ADCS_CONF_TBL_SetDefaultWithSt(void)
{
   OS_printf("Load ADCS Conf Tbl Default Values\n");
   ADCS_CONF_TBL_SetDefault();
   int HardwareStartCnt = 5;
   uint8_t HardwareIdxToStart[HardwareStartCnt];
   HardwareIdxToStart[0] = ADCS_HW_DEVICE_IMU_IDX;
   HardwareIdxToStart[1] = ADCS_HW_DEVICE_XADC_IDX;
   HardwareIdxToStart[2] = ADCS_HW_DEVICE_MTR_IDX;
   HardwareIdxToStart[3] = ADCS_HW_DEVICE_RW_IDX;
   HardwareIdxToStart[4] = ADCS_HW_DEVICE_ST_IDX;
   // Update to Include ST
   AdcsConfTbl->Data.HardwareStartCnt = HardwareStartCnt;
   CFE_PSP_MemCpy(&AdcsConfTbl->Data.HardwareIdxToStart, &HardwareIdxToStart, sizeof(HardwareIdxToStart));
 
} /* End ADCS_CONF_TBL_SetDefault() */
#endif


/* end of file */
