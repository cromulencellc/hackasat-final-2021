#ifndef _adcs_hw_h_
#define _adcs_hw_h_

#include "app_cfg.h"
#include "adcs_io_lib.h"
#include "adcs_logger.h"
#include "adcs_hw_cfg.h"

/*
** Event Message IDs
*/
#define ADCS_HW_DEBUG_EID               (ADCS_HW_BASE_EID +  0)
#define ADCS_HW_ERROR_EID               (ADCS_HW_BASE_EID +  1)
#define ADCS_HW_IMU_DEBUG_EID           (ADCS_HW_BASE_EID +  2)
#define ADCS_HW_IMU_ERROR_EID           (ADCS_HW_BASE_EID +  3)
#define ADCS_HW_IMU_CHILD_TASK_EID      (ADCS_HW_BASE_EID +  4)
#define ADCS_HW_MTR_DEBUG_EID           (ADCS_HW_BASE_EID +  5)
#define ADCS_HW_MTR_ERROR_EID           (ADCS_HW_BASE_EID +  6)
#define ADCS_HW_RW_DEBUG_EID            (ADCS_HW_BASE_EID +  7)
#define ADCS_HW_RW_ERROR_EID            (ADCS_HW_BASE_EID +  8)
#define ADCS_HW_RW_CHILD_TASK_EID       (ADCS_HW_BASE_EID +  9)
#define ADCS_HW_XADC_DEBUG_EID          (ADCS_HW_BASE_EID +  10)
#define ADCS_HW_XADC_ERROR_EID          (ADCS_HW_BASE_EID +  11)
#define ADCS_HW_ST_DEBUG_EID            (ADCS_HW_BASE_EID +  12)
#define ADCS_HW_ST_ERROR_EID            (ADCS_HW_BASE_EID +  13)

// _ADCS_CHALLENGE_
#define ADCS_HW_FSS_DEBUG_EID           (ADCS_HW_BASE_EID +  14)
#define ADCS_HW_FSS_ERROR_EID           (ADCS_HW_BASE_EID +  15)

#define ADCS_HW_EXEC_FAILURE -1
#define ADCS_HW_EXEC_SUCCESS CFE_SUCCESS
#define ADCS_HW_ALL 99
#define ADCS_HW_TLM_ENABLE 100
#define ADCS_HW_LOOP_DELAY 50
/************************/
/** HELPER TYPES       **/
/************************/

typedef enum {
    HW_UNINITIALIZED,  // Default state
    HW_INITIALIZED,    // Set after hw init function is run
    HW_RUNNING,        // Set after hw start function is run
    HW_STOPPED,        // Set after hw stop function is run
    HW_ERROR           // Set on ERROR (must be cleared)
} ADCS_HW_STATUS_t;

// typedef enum {
//     HW_ALL = 99,
//     HW_IMU,
//     HW_MTR,
//     HW_ST,
//     HW_RW,
//     HW_CSS
// } ADCS_HW_COMPONENT_t;

typedef enum {
    HW_STOP,
    HW_START,
    HW_INIT,
    HW_RESET,
    HW_RESET_STATUS
} ADCS_HW_CMD_t;


/*
** ADCS Device HAL
*/
typedef struct {
    uint8_t                 Status;
    uint8_t                 Initialized;
    uint8_t                 ErrorFlag;
    uint16_t                DeviceResetCnt;
    uint32_t                DeviceUpdateCnt;
} OS_PACK ADCS_HW_DeviceTlmCommon_t;

typedef struct ADCS_HW_Device_s ADCS_HW_Device_t;
typedef int32_t (*ADCS_HW_Device_FuncPtr) (ADCS_HW_Device_t *DevicePtr);
typedef void* ADCS_HW_DeviceClassPtr;
typedef void* ADCS_HW_DeviceTlmClassPtr;
typedef void* ADCS_HW_DeviceConfTblPtr;


struct ADCS_HW_Device_s {
    ADCS_HW_STATUS_t            Status;
    bool                        Initialized;
    bool                        TlmEnabled;
    uint16_t                    DeviceResetCnt;
    uint32_t                    DeviceUpdateCnt;
    uint32_t                    TlmPktCnt;
    ADCS_HW_DeviceClassPtr      DeviceClassPtr;
    ADCS_HW_DeviceTlmClassPtr   DeviceTlmPktPtr;
    ADCS_HW_DeviceConfTblPtr    ConfTblPtr;

    // Pointer to function to init device
    // State based guard code handled by hal caller functions
    // Return ADCS_HW_EXEC_SUCCESS upon successful run, ADCS_HW_EXEC_FAILURE upon failure
    int32_t (*init)(ADCS_HW_Device_t *self);

    // Pointer to function to start device
        // State based guard code handled by hal caller functions
    // Return ADCS_HW_EXEC_SUCCESS upon successful run, ADCS_HW_EXEC_FAILURE upon failure
    int32_t (*start)(ADCS_HW_Device_t *self);

    // Pointer to function to stop device
    // State based guard code handled by hal caller functions
    // Return ADCS_HW_EXEC_SUCCESS upon successful run, ADCS_HW_EXEC_FAILURE upon failure
    int32_t (*stop)(ADCS_HW_Device_t *self);

    // Pointer to function to perform device hardware reset
    // State based guard code handled by hal caller functions
    // Return ADCS_HW_EXEC_SUCCESS upon successful run, ADCS_HW_EXEC_FAILURE upon failure
    int32_t (*reset)(ADCS_HW_Device_t *self);

    // Pointer to function to perform device status (counters) reset
    // State based guard code handled by hal caller functions
    int32_t (*reset_status)(ADCS_HW_Device_t *self);

    // Pointer to function to update device telmetry
    // State based guard code handled by hal caller functions
    // Return ADCS_HW_EXEC_SUCCESS upon successful run, ADCS_HW_EXEC_FAILURE upon failure
    int32_t (*tlm_update)(ADCS_HW_Device_t *self);

    // Pointer to function to run device. Gets called every execution loop
    // State based guard code handled by hal caller functions
    // Return ADCS_HW_EXEC_SUCCESS upon successful run, ADCS_HW_EXEC_FAILURE upon failure
    int32_t (*update)(ADCS_HW_Device_t *self);

};

/******************************************************************************
** ADCS HW MGR Class
*/
typedef struct {
    uint32_t                TaskExeCnt;
    uint32_t                DeviceCnt;
    ADCS_HW_Device_t        Devices[ADCS_HW_DEVICE_CNT_MAX];

    // ADCS_IMU_Class IMU;
    // ADCS_HW_IMU_TlmSensorPkt IMU_Tlm;
} ADCS_HW_MGR_Class;


/************************/
/** Exported Functions **/
/************************/

void ADCS_HW_MGR_Constructor(ADCS_HW_MGR_Class*  ADCSHwMgrObj);
void ADCS_HW_MGR_AddDevice(unsigned int deviceMgrIndex, ADCS_HW_DeviceClassPtr deviceClass, ADCS_HW_DeviceTlmClassPtr deviceTlmClass,
    ADCS_HW_Device_FuncPtr initFuncPtr, ADCS_HW_Device_FuncPtr startFuncPtr, ADCS_HW_Device_FuncPtr stopFuncPtr,
    ADCS_HW_Device_FuncPtr resetFuncPtr, ADCS_HW_Device_FuncPtr resetStatusFuncPtr, ADCS_HW_Device_FuncPtr tlmUpdateFuncPtr,
    ADCS_HW_Device_FuncPtr updateFuncPtr, ADCS_HW_DeviceConfTblPtr confTblPtr);
void ADCS_HW_MGR_Update(void);
int32_t ADCS_HW_MGR_UpdateHardware(void);
int32_t ADCS_HW_MGR_UpdateTlm(void);
int32_t ADCS_HW_MGR_UpdateCommonTlm(ADCS_HW_DeviceTlmCommon_t* tlm, ADCS_HW_Device_t* Device);
int32_t ADCS_HW_MGR_InitHardware(uint8_t component);
int32_t ADCS_HW_MGR_StartHardware(uint8_t component);
int32_t ADCS_HW_MGR_StopHardware(uint8_t component);
int32_t ADCS_HW_MGR_ResetHardware(uint8_t component);
int32_t ADCS_HW_MGR_ResetStatus(uint8_t component);

#endif /* _adcs_hw_h_ */