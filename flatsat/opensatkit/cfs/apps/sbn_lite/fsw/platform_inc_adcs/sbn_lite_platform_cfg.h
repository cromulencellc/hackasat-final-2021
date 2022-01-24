/*
** Purpose: Define platform configurations for the SBN-lite
**          application
**
**
*/

#ifndef _sbn_lite_platform_cfg_
#define _sbn_lite_platform_cfg_

/*
** Includes
*/

#include "sbn_lite_mission_cfg.h"
#include "sbn_lite_msgids.h"
#include "sbn_lite_perfids.h"

/******************************************************************************
** SBN-lite Application Macros
*/

#define  SBN_LITE_STARTUP_SYNCH_TIMEOUT  10000  /* Timeout in milliseconds */

#define  SBN_LITE_RUN_LOOP_DELAY_MS        150  /* Default delay in milliseconds */
#define  SBN_LITE_MIN_RUN_LOOP_DELAY_MS    100  /* Minimum command value */
#define  SBN_LITE_MAX_RUN_LOOP_DELAY_MS  10000  /* Maximum command value */

#if TEAM_NUM == 1
	#define  SBN_LITE_TX_PEER_ADDR      "192.168.101.67"  //address of peer
#elif TEAM_NUM == 2
	#define  SBN_LITE_TX_PEER_ADDR      "192.168.102.67"  //address of peer
#elif TEAM_NUM == 3
	#define  SBN_LITE_TX_PEER_ADDR      "192.168.103.67"  //address of peer
#elif TEAM_NUM == 4
	#define  SBN_LITE_TX_PEER_ADDR      "192.168.104.67"  //address of peer
#elif TEAM_NUM == 5
	#define  SBN_LITE_TX_PEER_ADDR      "192.168.105.67"  //address of peer
#elif TEAM_NUM == 6
	#define  SBN_LITE_TX_PEER_ADDR      "192.168.106.67"  //address of peer
#elif TEAM_NUM == 7
	#define  SBN_LITE_TX_PEER_ADDR      "192.168.107.67"  //address of peer
#elif TEAM_NUM == 8
    #define  SBN_LITE_TX_PEER_ADDR      "192.168.108.67"  //address of peer
#elif TEAM_NUM == 9
	#define  SBN_LITE_TX_PEER_ADDR      "192.168.109.67"  //address of peer
#elif TEAM_NUM == 10
	#define  SBN_LITE_TX_PEER_ADDR      "192.168.110.67"  //address of peer
#elif TEAM_NUM == 11
	#define  SBN_LITE_TX_PEER_ADDR      "192.168.111.67"  //address of peer
#elif TEAM_NUM == 12
	#define  SBN_LITE_TX_PEER_ADDR      "192.168.112.67"  //address of peer
#elif TEAM_NUM == 13
	#define  SBN_LITE_TX_PEER_ADDR      "192.168.113.67"  //address of peer
#elif TEAM_NUM == 14
	#define  SBN_LITE_TX_PEER_ADDR      "192.168.114.67"  //address of peer
#elif TEAM_NUM == 15
	#define  SBN_LITE_TX_PEER_ADDR      "192.168.115.67"  //address of peer
#else
    #define  SBN_LITE_TX_PEER_ADDR      "192.168.3.67"  //address of peer
#endif	//if TEAM_NUM == 1

#define  SBN_LITE_TX_PORT           4322
#define  SBN_LITE_RX_PORT           4321

// This packet table defines which SB messages are sent over the udp socket
// Similar to the remap table in SBN.  Only packets in this table are sent
// Uses the same format as the KIT_TO packet table
#define  SBN_LITE_DEF_PKTTBL_FILE_NAME "/cf/sbn_pkt_tbl.json"

#endif /* _sbn_lite_platform_cfg_ */
