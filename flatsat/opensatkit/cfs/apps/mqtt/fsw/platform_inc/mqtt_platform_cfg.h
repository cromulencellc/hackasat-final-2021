/*
** Purpose: Define platform configurations for the OSK MQTT application
**
** Notes:
**   1. Initial OSK MQTT App based on a January 2021 refactor of Alan Cudmore's
**      MQTT App https://github.com/alanc98/mqtt_app. 
**
** License:
**   Preserved original https://github.com/alanc98/mqtt_app Apache License 2.0.
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _mqtt_platform_cfg_
#define _mqtt_platform_cfg_

/*
** Includes
*/

#include "mqtt_mission_cfg.h"


/******************************************************************************
** Platform Deployment Configurations
*/

#define MQTT_PLATFORM_REV   0
#define MQTT_INI_FILENAME   "/cf/mqtt_ini.json"


/******************************************************************************
** These are frustrating. They're only needed statically because of the table
** decsriptor build process. Converting to a JSON table will remove dependency.
*/

#define MQTT_APP_CFE_NAME         "MQTT"
#define MQTT_MQ2SB_TBL_CFE_NAME   "mq2sb"
#define MQTT_SB2MQ_TBL_CFE_NAME   "sb2mq"

/******************************************************************************
** These will be in a spec file and the toolchain will create these
** definitions.
*/

/*
** mqtt topic string length
*/
#define MQTT_TOPIC_LEN           64
#define MQTT_MQ2SB_TBL_ENTRIES   20 
#define  MQTT_MIN_RUN_LOOP_DELAY_MS    100  /* Minimum command value */
#define  MQTT_MAX_RUN_LOOP_DELAY_MS   1000  /* Maximum command value */


#endif /* _mqtt_platform_cfg_ */
