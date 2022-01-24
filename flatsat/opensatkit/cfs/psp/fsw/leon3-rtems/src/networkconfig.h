/*
 * Network configuration
 * 
 ************************************************************
 * EDIT THIS FILE TO REFLECT YOUR NETWORK CONFIGURATION     *
 * BEFORE RUNNING ANY RTEMS PROGRAMS WHICH USE THE NETWORK! * 
 ************************************************************
 *
 *
 * The GRETH and SMC driver add them self to the interface list below,
 * they lookup MAC and IP addresses automatically from the 
 * interface_configs[] array below.
 */

#ifndef _RTEMS_NETWORKCONFIG_H_
#define _RTEMS_NETWORKCONFIG_H_

#include <grlib/network_interface_add.h>
#include "network_includes.h"

#include "cfe_mission_cfg.h"

/* #define RTEMS_USE_BOOTP */

/* #include <bsp.h> */


#ifdef RTEMS_USE_LOOPBACK 
/*
 * Loopback interface
 */
extern void rtems_bsdnet_loopattach();
static struct rtems_bsdnet_ifconfig loopback_config = {
	"lo0",				/* name */
	rtems_bsdnet_loopattach,	/* attach function */

	NULL,				/* link to next interface */

	"127.0.0.1",			/* IP address */
	"255.0.0.0",			/* IP net mask */
};
#endif


/*
 * Network configuration
 */
struct rtems_bsdnet_config rtems_bsdnet_config = {
#ifdef RTEMS_USE_LOOPBACK 
	&loopback_config,		/* link to next interface */
#else
	NULL,				/* No more interfaces */
#endif

#if (defined (RTEMS_USE_BOOTP))
	rtems_bsdnet_do_bootp,
#else
	NULL,
#endif

	.network_task_priority = 15,			/* Default network task priority */
	.mbuf_bytecount = 256*1024,		/* Default mbuf capacity */
	.mbuf_cluster_bytecount = 512*1024,		/* Default mbuf cluster capacity */

#if (!defined (RTEMS_USE_BOOTP))
	.hostname = "rtems_host",		/* Host name */
	.domainname = "localnet",		/* Domain name */
	.gateway = "192.168.1.1",		/* Gateway */
	.log_host = "192.168.1.1",		/* Log host */
	.name_server = {"192.168.1.1" },	/* Name server(s) */
	.ntp_server = {"192.168.1.1" },	/* NTP server(s) */

#endif /* !RTEMS_USE_BOOTP */
	// .sb_efficiency = 2, 											/* sb_efficiency (2 = default value) */
	// .udp_tx_buf_size = 72 * 1024, 									/* UDP TX Buffer Size (9216 bytes = default) */
	// .udp_rx_buf_size = 80 * (1024 + sizeof(struct sockaddr_in)),	/* UDP RX Buffer Size (40 * (1024 + sizeof(struct sockaddr_in)) */
	// .tcp_tx_buf_size = 32 * 1024,									/* TCP TX Buffer Size (16 * 1024 bytes = default) */
	// .tcp_rx_buf_size = 32 * 1024,									/* TCP RX Buffer Size (16 * 1024 bytes = default) */

};

/* Table used by network interfaces that register themselves using the
 * network_interface_add routine. From this table the IP address, netmask 
 * and Ethernet MAC address of an interface is taken.
 *
 * The network_interface_add routine puts the interface into the
 * rtems_bsnet_config.ifconfig list.
 *
 * Set IP Address and Netmask to NULL to select BOOTP.
 */
struct ethernet_config interface_configs[] =
{
// #if TEAM_NUM == 0
// 	{ "192.168.100.67", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x70}}
#if TEAM_NUM == 1
	{ "192.168.101.67", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x71}}
#elif TEAM_NUM == 2
	{ "192.168.102.67", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x72}}
#elif TEAM_NUM == 3
	{ "192.168.103.67", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x73}}
#elif TEAM_NUM == 4
	{ "192.168.104.67", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x74}}
#elif TEAM_NUM == 5
	{ "192.168.105.67", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x75}}
#elif TEAM_NUM == 6
	{ "192.168.106.67", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x76}}
#elif TEAM_NUM == 7
	{ "192.168.107.67", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x77}}
#elif TEAM_NUM == 8
	{ "192.168.108.67", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x78}}
#elif TEAM_NUM == 9
	{ "192.168.109.67", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x79}}
#elif TEAM_NUM == 10
	{ "192.168.110.67", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x7A}}
#elif TEAM_NUM == 11
	{ "192.168.111.67", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x7B}}
#elif TEAM_NUM == 12
	{ "192.168.112.67", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x7C}}
#elif TEAM_NUM == 13
	{ "192.168.113.67", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x7D}}
#elif TEAM_NUM == 14
	{ "192.168.114.67", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x7F}}
#elif TEAM_NUM == 15
	{ "192.168.115.67", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x80}}
#else
	{ "192.168.100.67", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x70}}
#endif	//if TEAM_NUM == 1
};

#define INTERFACE_CONFIG_CNT (sizeof(interface_configs)/sizeof(struct ethernet_config) - 1)

/*
 * For TFTP test application
 */
#if (defined (RTEMS_USE_BOOTP))
#define RTEMS_TFTP_TEST_HOST_NAME "BOOTP_HOST"
#define RTEMS_TFTP_TEST_FILE_NAME "BOOTP_FILE"
#else
#define RTEMS_TFTP_TEST_HOST_NAME "XXX.YYY.ZZZ.XYZ"
#define RTEMS_TFTP_TEST_FILE_NAME "tftptest"
#endif

#endif /* _RTEMS_NETWORKCONFIG_H_ */
