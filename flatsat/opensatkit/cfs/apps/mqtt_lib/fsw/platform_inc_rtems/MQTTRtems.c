/*******************************************************************************
 * Copyright (c) 2014, 2017 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander - initial API and implementation and/or initial documentation
 *    Ian Craggs - return codes from linux_read
 *******************************************************************************/

#include "MQTTRtems.h"
#include <errno.h>
#include "mqtt_logger.h"

void TimerInit(Timer* timer)
{
	timer->end_time = (struct timeval){0, 0};
}

char TimerIsExpired(Timer* timer)
{
	struct timeval now, res;

	gettimeofday(&now, NULL);
	// printf("gettimeofday seconds : %lld\tmicro seconds : %d\n", now.tv_sec, now.tv_usec);
	timersub(&timer->end_time, &now, &res);
	char rc = res.tv_sec < 0 || (res.tv_sec == 0 && res.tv_usec <= 0);
	// printf("TimerIsExpired rc = %d\n", rc);
	MQTT_LOG_DEBUG("Timer Expired: %d", rc);
	return rc;
}


void TimerCountdownMS(Timer* timer, unsigned int timeout)
{
	struct timeval now;

	gettimeofday(&now, NULL);
	// printf("gettimeofday seconds : %lld\tmicro seconds : %d\n", now.tv_sec, now.tv_usec);
	struct timeval interval = {timeout / 1000, (timeout % 1000) * 1000};
	timeradd(&now, &interval, &timer->end_time);
}


void TimerCountdown(Timer* timer, unsigned int timeout)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	// printf("gettimeofday seconds : %lld\tmicro seconds : %d\n", now.tv_sec, now.tv_usec);
	struct timeval interval = {timeout, 0};
	timeradd(&now, &interval, &timer->end_time);
}


int TimerLeftMS(Timer* timer)
{
	struct timeval now, res;
	gettimeofday(&now, NULL);
	// printf("gettimeofday seconds : %lld\tmicro seconds : %d\n", now.tv_sec, now.tv_usec);
	timersub(&timer->end_time, &now, &res);
	// printf("left %lld ms\n", (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000);
	return (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000;
}

int TimerLeftMSPrint(Timer* timer)
{
	struct timeval now, res;
	gettimeofday(&now, NULL);
	// printf("gettimeofday seconds : %lld\tmicro seconds : %d\n", now.tv_sec, now.tv_usec);
	timersub(&timer->end_time, &now, &res);
	printf("left %lld ms\n", (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000);
	return (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000;
}


int rtems_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	struct timeval interval = {timeout_ms / 1000, (timeout_ms % 1000) * 1000};
	if (interval.tv_sec < 0 || (interval.tv_sec == 0 && interval.tv_usec <= 0))
	{
		interval.tv_sec = 0;
		interval.tv_usec = 100;
	}

	setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&interval, sizeof(struct timeval));

	int bytes = 0;
	while (bytes < len)
	{
		int rc = recv(n->my_socket, &buffer[bytes], (size_t)(len - bytes), 0);
		if (rc < 0)
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK) {
				MQTT_LOG_ERROR("read error rc = %d timeout = %d, errorno: %d:%s",rc, timeout_ms, errno, strerror(errno));
				bytes = -1;
			}
			break;
		}
		else if (rc == 0)
		{
			bytes = 0;
			break;
		}
		else
			bytes += rc;
	}
	// printf("rtems_read bytes read = %d\n", bytes);
	return bytes;
}


int rtems_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{

	struct timeval interval = {timeout_ms / 1000, (timeout_ms % 1000) * 1000};
	if (interval.tv_sec < 0 || (interval.tv_sec == 0 && interval.tv_usec <= 0))
	{
		interval.tv_sec = 0;
		interval.tv_usec = 100;
	}

	// tv.tv_sec = 0;  /* 30 Secs Timeout */
	// tv.tv_usec = timeout_ms * 1000;  // Not init'ing this can cause strange errors

	setsockopt(n->my_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&interval, sizeof(struct timeval));
	int	rc = write(n->my_socket, buffer, len);

	if (rc < 0) {
		MQTT_LOG_ERROR("write error rc = %d timeout = %d, errorno: %d:%s",rc, timeout_ms, errno, strerror(errno));
	}
	return rc;
}


void NetworkInit(Network* n)
{
	memset(n, 0, sizeof(Network));
	n->my_socket = 0;
	n->mqttread = rtems_read;
	n->mqttwrite = rtems_write;
}


int NetworkConnect(Network* n, char* addr, int port)
{
	// int type = SOCK_STREAM;
	struct sockaddr_in address;
	int rc = -1;

	if ((n->my_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return -1;
    } else {
		rc = 1;
	}

	address.sin_port = htons(port);
	address.sin_family = AF_INET;

	if(inet_pton(AF_INET, addr, &address.sin_addr)<=0) 
	{
		return -1;
	} else {
		rc = 1;
	}

	if (n->my_socket != -1) {
		rc = connect(n->my_socket, (struct sockaddr*)&address, sizeof(address));
		if (rc < 0) {
		}
	} else {
		rc = -1;
	}

	return rc;
}


void NetworkDisconnect(Network* n)
{
	close(n->my_socket);
}
