/*
** Purpose: Network interface abstractions file transfer application. 
**
** Notes:
**   None
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/
#ifndef _netif_
#define _netif_

/*
** Includes
*/

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include "common_types.h"
#include "network_includes.h"
#include "cfe.h"
#include "app_cfg.h"

#define NETIF_IP_STR_LEN  16

/*
** Event Message IDs
*/

#define NETIF_BIND_SOCKET_ERR_EID    ( NETIF_BASE_EID +  0)
#define NETIF_CREATE_SOCKET_ERR_EID  ( NETIF_BASE_EID +  1)
#define NETIF_RECV_DBG_EID           ( NETIF_BASE_EID +  2)
#define NETIF_RECV_ERR_EID           ( NETIF_BASE_EID +  3)
#define NETIF_SEND_DBG_EID           ( NETIF_BASE_EID +  4)
#define NETIF_SEND_ERR_EID           ( NETIF_BASE_EID +  5)

/*
** Type Definitions
*/

/******************************************************************************
** NETIF Class
*/

typedef struct {

   char    IpAddrStr[16]; /* IPv4 only */
   int16   ServerPort;
   int     SocketId;		  /* Socket descriptor  */
   struct  sockaddr_in  ServerSocketAddr;  /* Created by Constructor() */
   struct  sockaddr_in  ClientSocketAddr;  /* Created during transfer  */
   struct  sockaddr_in  XferSocketAddr;
   boolean ClientSocketAddrCreated;

} NETIF_Class;


/******************************************************************************
** Command Functions
*/

typedef struct
{

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   int16   ServerPort;
   char    IpAddrStr[NETIF_IP_STR_LEN];
   
}  OS_PACK NETIF_InitSocketCmdParam;
#define NETIF_INIT_SOCKET_CMD_DATA_LEN  (sizeof(NETIF_InitSocketCmdParam) - CFE_SB_CMD_HDR_SIZE)

/*
** Exported Functions
*/

/******************************************************************************
** Function: NETIF_Constructor
**
** Construct a local NETIF object.
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void NETIF_Constructor(NETIF_Class*  NetIfPtr, char* DefIpAddrStr, int16 DefServerPort);


/******************************************************************************
** Function: NETIF_Destructor
**
** This function should be called when the app is terminated. This should
** never occur but if it does this will close the network socket.
**
*/
void NETIF_Destructor();


/******************************************************************************
** Function: NETIF_ClearClient
**
*/
void NETIF_ClearClient();


/******************************************************************************
** Function: NETIF_RcvFrom
**
**
*/
int32 NETIF_RcvFrom(const uint8 NetIFid, void *BufPtr, const uint16 BufSize, boolean ServerListen);


/******************************************************************************
** Function: NETIF_SendTo
**
**
*/
int32 NETIF_SendTo (const uint8 NetIFid, const uint8 *BufPtr, uint16 len); 

/******************************************************************************
** Function: NETIF_InitSocketCmd
**
** Notes:
**   1. Must match CMDMGR_CmdFuncPtr function signature
*/
boolean NETIF_InitSocketCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);

#endif /* _netif_ */
