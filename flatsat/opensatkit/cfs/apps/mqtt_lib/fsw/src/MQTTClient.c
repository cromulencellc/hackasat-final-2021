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
 *   Allan Stockdill-Mander/Ian Craggs - initial API and implementation and/or initial documentation
 *   Ian Craggs - fix for #96 - check rem_len in readPacket
 *   Ian Craggs - add ability to set message handler separately #6
 *******************************************************************************/
#include "MQTTClient.h"

#include <stdio.h>
#include <string.h>

static void NewMessageData(MessageData* md, MQTTString* aTopicName, MQTTMessage* aMessage) {
    md->topicName = aTopicName;
    md->message = aMessage;
}


static int getNextPacketId(MQTTClient *c) {
    return c->next_packetid = (c->next_packetid == MAX_PACKET_ID) ? 1 : c->next_packetid + 1;
}


static int sendPacket(MQTTClient* c, int length, Timer* timer)
{
    int rc = FAILURE;
    int sent = 0;
    MQTT_LOG_DEBUG("Send packet with length %d", length);
    while (sent < length)
    {
        rc = 0;
        MQTT_LOG_DEBUG("Time remaining ms: %d", TimerLeftMS(timer));
        // timeoutms = (timerem <= MQTT_SOCKET_TRANSACTION_TIMEOUT) ? MQTT_SOCKET_TRANSACTION_TIMEOUT: timerem;
        // MQTT_LOG_INFO("Sending packet over socket. Timeout: %d ms", timeoutms);
    
        rc = c->ipstack->mqttwrite(c->ipstack, &c->buf[sent], length-sent, TimerLeftMS(timer));
        // rc = c->ipstack->mqttwrite(c->ipstack, &c->buf[sent], length-sent, timeoutms);
        
        if (rc < 0)  // there was an error writing the data 
        {
            MQTT_LOG_ERROR("sendPacket - there was an error writing the data");
            break;
        } else if (rc == 0 ) {
            MQTT_LOG_DEBUG("write returned 0. Rutroh...");
        }
        else if (rc > 0) {
            sent += rc;
        }
        MQTT_LOG_DEBUG("Socket write rc: %d, sent: %d", rc, sent);
    }
    if (sent == length)
    {
        // printf("sendPacket success sent == length");
        TimerCountdown(&c->last_sent, c->keepAliveInterval); // record the fact that we have successfully sent the packet
        rc = SUCCESS;
    }
    else {
        MQTT_LOG_ERROR("sendPacket failure sent: %d length: %d\n", sent,length);
        rc = FAILURE;
    }
    return rc;
}


void MQTTClientInit(MQTTClient* c, Network* network, unsigned int command_timeout_ms,
		unsigned char* sendbuf, size_t sendbuf_size, unsigned char* readbuf, size_t readbuf_size)
{
    int i;
    c->ipstack = network;

    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
        c->messageHandlers[i].topicFilter = 0;
    c->command_timeout_ms = command_timeout_ms;
    c->buf = sendbuf;
    c->buf_size = sendbuf_size;
    c->readbuf = readbuf;
    c->readbuf_size = readbuf_size;
    c->isconnected = 0;
    c->cleansession = 0;
    c->ping_outstanding = 0;
    c->defaultMessageHandler = NULL;
	c->next_packetid = 1;
    TimerInit(&c->last_sent);
    TimerInit(&c->last_received);
#if defined(MQTT_TASK)
	  MutexInit(&c->mutex);
#endif
}


static int decodePacket(MQTTClient* c, int* value, int timeout)
{
    unsigned char i;
    int multiplier = 1;
    int len = 0;
    const int MAX_NO_OF_REMAINING_LENGTH_BYTES = 4;

    *value = 0;
    do
    {
        int rc = MQTTPACKET_READ_ERROR;

        if (++len > MAX_NO_OF_REMAINING_LENGTH_BYTES)
        {
            rc = MQTTPACKET_READ_ERROR; /* bad data */
            goto exit;
        }
        rc = c->ipstack->mqttread(c->ipstack, &i, 1, timeout);
        if (rc != 1)
            goto exit;
        *value += (i & 127) * multiplier;
        multiplier *= 128;
    } while ((i & 128) != 0);
exit:
    return len;
}


static int readPacket(MQTTClient* c, Timer* timer)
{
    MQTTHeader header = {0};
    int len = 0;
    int rem_len = 0;
    int timeoutms;
    int timerem;

    timerem = TimerLeftMS(timer);
    timeoutms = (timerem <= MQTT_SOCKET_TRANSACTION_TIMEOUT) ? MQTT_SOCKET_TRANSACTION_TIMEOUT : timerem;

    /* 1. read the header byte.  This has the packet type in it */
    int rc = c->ipstack->mqttread(c->ipstack, c->readbuf, 1, timeoutms);
    MQTT_LOG_DEBUG("MQTT Read Return Code: %d", rc);
    if (rc != 1) {
        goto exit;
    }
    len = 1;

    /* 2. read the remaining length.  This is variable in itself */
    timerem = TimerLeftMS(timer);
    timeoutms = (timerem <= MQTT_SOCKET_TRANSACTION_TIMEOUT) ? MQTT_SOCKET_TRANSACTION_TIMEOUT : timerem;
    decodePacket(c, &rem_len, timeoutms);
    len += MQTTPacket_encode(c->readbuf + 1, rem_len); /* put the original remaining length back into the buffer */

    if (rem_len > (c->readbuf_size - len))
    {
        rc = BUFFER_OVERFLOW;
        goto exit;
    }

    /* 3. read the rest of the buffer using a callback to supply the rest of the data */
    timerem = TimerLeftMS(timer);
    timeoutms = (timerem <= MQTT_SOCKET_TRANSACTION_TIMEOUT) ? MQTT_SOCKET_TRANSACTION_TIMEOUT : timerem;
    if (rem_len > 0 && (rc = c->ipstack->mqttread(c->ipstack, c->readbuf + len, rem_len, timerem) != rem_len)) {
        rc = 0;
        goto exit;
    }

    header.byte = c->readbuf[0];
    rc = header.bits.type;
    if (c->keepAliveInterval > 0) {
        // The original logic was flawed and assumed that a ping response
        // had to be processed on the same cycle as a ping was sent out
        // adding some margin to recieve the response from the broker
        TimerCountdown(&c->last_received, c->keepAliveInterval + 5); // record the fact that we have successfully received a packet
    }
exit:
    // printf("readpacket--> (c->last_received)");
    // TimerLeftMSPrint(&c->last_received);
    // if (rc == -1) {
    //     printf("mqttread error\n");
    // }
    return rc;
}


// assume topic filter and name is in correct format
// # can only be at end
// + and # can only be next to separator
static char isTopicMatched(char* topicFilter, MQTTString* topicName)
{
    char* curf = topicFilter;
    char* curn = topicName->lenstring.data;
    char* curn_end = curn + topicName->lenstring.len;

    while (*curf && curn < curn_end)
    {
        if (*curn == '/' && *curf != '/')
            break;
        if (*curf != '+' && *curf != '#' && *curf != *curn)
            break;
        if (*curf == '+')
        {   // skip until we meet the next separator, or end of string
            char* nextpos = curn + 1;
            while (nextpos < curn_end && *nextpos != '/')
                nextpos = ++curn + 1;
        }
        else if (*curf == '#')
            curn = curn_end - 1;    // skip until end of string
        curf++;
        curn++;
    };

    return (curn == curn_end) && (*curf == '\0');
}


int deliverMessage(MQTTClient* c, MQTTString* topicName, MQTTMessage* message)
{
    int i;
    int rc = FAILURE;

    // we have to find the right message handler - indexed by topic
    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
    {
        if (c->messageHandlers[i].topicFilter != 0 && (MQTTPacket_equals(topicName, (char*)c->messageHandlers[i].topicFilter) ||
                isTopicMatched((char*)c->messageHandlers[i].topicFilter, topicName)))
        {
            if (c->messageHandlers[i].fp != NULL)
            {
                MessageData md;
                NewMessageData(&md, topicName, message);
                c->messageHandlers[i].fp(&md);
                rc = SUCCESS;
            }
        }
    }

    if (rc == FAILURE && c->defaultMessageHandler != NULL)
    {
        MessageData md;
        NewMessageData(&md, topicName, message);
        c->defaultMessageHandler(&md);
        rc = SUCCESS;
    }

    return rc;
}


int keepalive(MQTTClient* c)
{
    int rc = SUCCESS;
    // struct timeval current;

    if (c->keepAliveInterval == 0)
        goto exit;


    MQTT_LOG_DEBUG("Running Keepalive. Interval: %d, Ping Outstanding : %d, Last Sent: %d, Last Received: %d", c->keepAliveInterval, c->ping_outstanding, TimerLeftMS(&c->last_sent), TimerLeftMS(&c->last_received));
    // printf("keep alive last sent expired?: %d\n",TimerIsExpired(&c->last_sent));
    // printf("keep alive last recieved expired?: %d\n",TimerIsExpired(&c->last_received));
    if (TimerIsExpired(&c->last_sent) || TimerIsExpired(&c->last_received))
    {
        if (c->ping_outstanding) {
	        // gettimeofday(&current, NULL);
	        // printf("gettimeofday seconds : %lld\tmicro seconds : %ld\n", current.tv_sec, current.tv_usec);
            MQTT_LOG_ERROR("Ping not received in keepalive interval");
            rc = FAILURE; /* PINGRESP not received in keepalive interval */
        }
        else
        {
            MQTT_LOG_DEBUG("Need to send ping");
            Timer timer;
            TimerInit(&timer);
            TimerCountdownMS(&timer, 1000);
            int len = MQTTSerialize_pingreq(c->buf, c->buf_size);
            if (len > 0 && (rc = sendPacket(c, len, &timer)) == SUCCESS) // send the ping packet
                c->ping_outstanding = 1;
                // printf("sent the ping packet\n");
            // gettimeofday(&current, NULL);
	        // printf("ping outstanding set at seconds : %lld\tmicro seconds : %ld\n", current.tv_sec, current.tv_usec);
            // printf("sent ping packet (&c->last_sent)\n");
            // TimerLeftMSPrint(&c->last_sent);
        }
    }

exit:
    return rc;
}


void MQTTCleanSession(MQTTClient* c)
{
    int i = 0;

    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
        c->messageHandlers[i].topicFilter = NULL;
}


void MQTTCloseSession(MQTTClient* c)
{
    MQTT_LOG_WARN("MQTTCloseSession Called with CleanSession = %d",c->cleansession);
    c->ping_outstanding = 0;
    c->isconnected = 0;
    if (c->cleansession)
        MQTTCleanSession(c);
}


int cycle(MQTTClient* c, Timer* timer)
{
    // struct timeval current;
    int len = 0,
        rc = SUCCESS;

    MQTT_LOG_DEBUG("readPacket() with time remaining: %d", TimerLeftMS(timer));
    int packet_type = readPacket(c, timer);     /* read the socket, see what work is due */
    // printf("readPacket packet_type = %d\n", packet_type);

    switch (packet_type)
    {
        default:
            /* no more data to read, unrecoverable. Or read packet fails due to unexpected network error */
            rc = packet_type;
            goto exit;
        case 0: /* timed out reading packet */
            break;
        case CONNACK:
            MQTT_LOG_DEBUG("recieved CONNACK");
            break;
        case PUBACK:
            break;
        case SUBACK:
            break;
        case UNSUBACK:
            // printf("got SUBACK in normal cycle\n");
            break;
        case PUBLISH:
        {
            MQTTString topicName;
            MQTTMessage msg;
            int intQoS;
            msg.payloadlen = 0; /* this is a size_t, but deserialize publish sets this as int */
            if (MQTTDeserialize_publish(&msg.dup, &intQoS, &msg.retained, &msg.id, &topicName,
               (unsigned char**)&msg.payload, (int*)&msg.payloadlen, c->readbuf, c->readbuf_size) != 1) {
                MQTT_LOG_ERROR("MQTT Deserialization Failed: Msg: %s", (char*) msg.payload);
                goto exit;
            
            }
             
            msg.qos = (enum QoS)intQoS;
            deliverMessage(c, &topicName, &msg);
            if (msg.qos != QOS0)
            {
                if (msg.qos == QOS1)
                    len = MQTTSerialize_ack(c->buf, c->buf_size, PUBACK, 0, msg.id);
                else if (msg.qos == QOS2)
                    len = MQTTSerialize_ack(c->buf, c->buf_size, PUBREC, 0, msg.id);
                if (len <= 0)
                    rc = FAILURE;
                else
                    rc = sendPacket(c, len, timer);
                if (rc == FAILURE) {
                    MQTT_LOG_ERROR("PUBLISH failure");
                    goto exit; // there was a problem
                }
            }
            break;
        }
        case PUBREC:
        case PUBREL:
        {
            unsigned short mypacketid;
            unsigned char dup, type;
            if (MQTTDeserialize_ack(&type, &dup, &mypacketid, c->readbuf, c->readbuf_size) != 1)
                rc = FAILURE;
            else if ((len = MQTTSerialize_ack(c->buf, c->buf_size,
                (packet_type == PUBREC) ? PUBREL : PUBCOMP, 0, mypacketid)) <= 0)
                rc = FAILURE;
            else if ((rc = sendPacket(c, len, timer)) != SUCCESS) // send the PUBREL packet
                rc = FAILURE; // there was a problem
            if (rc == FAILURE) {
                MQTT_LOG_ERROR("PUBREC/PUBREL failure");
                goto exit; // there was a problem
            }
            break;
        }

        case PUBCOMP:
            break;
        case PINGRESP:
            c->ping_outstanding = 0;
            // gettimeofday(&current, NULL);
	        // printf("gettimeofday seconds : %lld\tmicro seconds : %ld\n", current.tv_sec, current.tv_usec);
            // printf("received ping response\n");
            break;
    }

    if (keepalive(c) != SUCCESS) {
        //check only keepalive FAILURE status so that previous FAILURE status can be considered as FAULT
        MQTT_LOG_ERROR("keepalive failure");
        rc = FAILURE;
    }

exit:
    if (rc == SUCCESS)
        rc = packet_type;
    else if (c->isconnected)
        MQTTCloseSession(c);
    return rc;
}


int MQTTYield(MQTTClient* c, int timeout_ms)
{
    int rc = SUCCESS;
    Timer timer;

    TimerInit(&timer);
    TimerCountdownMS(&timer, timeout_ms);
    // TimerLeftMS(&timer);

	do
    {
        if (cycle(c, &timer) < 0)
        {
            rc = FAILURE;
            break;
        }
  	} while (!TimerIsExpired(&timer));

    return rc;
}

int MQTTIsConnected(MQTTClient* client)
{
  return client->isconnected;
}

void MQTTRun(void* parm)
{
	Timer timer;
	MQTTClient* c = (MQTTClient*)parm;

	TimerInit(&timer);

	while (1)
	{
#if defined(MQTT_TASK)
		MutexLock(&c->mutex);
#endif
		TimerCountdownMS(&timer, 500); /* Don't wait too long if no traffic is incoming */
		cycle(c, &timer);
#if defined(MQTT_TASK)
		MutexUnlock(&c->mutex);
#endif
	}
}


#if defined(MQTT_TASK)
int MQTTStartTask(MQTTClient* client)
{
	return ThreadStart(&client->thread, &MQTTRun, client);
}
#endif


int waitfor(MQTTClient* c, int packet_type, Timer* timer)
{
    int rc = FAILURE;

    do
    {
        if (TimerIsExpired(timer)) {
            MQTT_LOG_WARN("Timed out in waitfor call...");
            break; // we timed out
        }
        rc = cycle(c, timer);
    }
    while (rc != packet_type && rc >= 0);

    return rc;
}




int MQTTConnectWithResults(MQTTClient* c, MQTTPacket_connectData* options, MQTTConnackData* data)
{
    Timer connect_timer;
    int rc = FAILURE;
    MQTTPacket_connectData default_options = MQTTPacket_connectData_initializer;
    int len = 0;

#if defined(MQTT_TASK)
	  MutexLock(&c->mutex);
#endif

    if (c->isconnected) /* don't send connect packet again if we are already connected */
    {
        // printf("MQTTConnectWithResults - isConnected %d\n",c->isconnected);
        goto exit;
    }

    TimerInit(&connect_timer);
    TimerCountdownMS(&connect_timer, c->command_timeout_ms);

    if (options == 0) {
        options = &default_options; /* set default options if none were supplied */
        MQTT_LOG_WARN("Using default options for MQTT connection");
    }
    c->keepAliveInterval = options->keepAliveInterval;
    MQTT_LOG_DEBUG("Connect with Keep Alive Interval: %d", c->keepAliveInterval);
    c->cleansession = options->cleansession;
    TimerCountdown(&c->last_received, c->keepAliveInterval);
    if ((len = MQTTSerialize_connect(c->buf, c->buf_size, options)) <= 0) {
        MQTT_LOG_ERROR("MQTT Connect Serialize Error");
        goto exit;
    }

    if ((rc = sendPacket(c, len, &connect_timer)) != SUCCESS) { // send the connect packet
        MQTT_LOG_ERROR("MQTT Connect sendPacket error");
        goto exit; // there was a problem
    }


    MQTT_LOG_DEBUG("Sent connect pkt, waiting on ack. Time Remaining %d", TimerLeftMS(&connect_timer));
    // this will be a blocking call, wait for the connack
    if (waitfor(c, CONNACK, &connect_timer) == CONNACK)
    {
        data->rc = 0;
        data->sessionPresent = 0;
        if (MQTTDeserialize_connack(&data->sessionPresent, &data->rc, c->readbuf, c->readbuf_size) == 1) {
            rc = data->rc;
            MQTT_LOG_DEBUG("Received and deserialized CONNACK, rc: %d", rc);
            // printf ("received CONNACK in waitfor\n");
        }
        else
            rc = FAILURE;
    }
    else
        rc = FAILURE;

exit:
    if (rc == SUCCESS)
    {
        c->isconnected = 1;
        c->ping_outstanding = 0;
    }

#if defined(MQTT_TASK)
	  MutexUnlock(&c->mutex);
#endif
    MQTT_LOG_DEBUG("End of MQTTConnectWithResults() rc = %d", rc);
    return rc;
}


int MQTTConnect(MQTTClient* c, MQTTPacket_connectData* options)
{
    MQTTConnackData data = {0, 0};
    return MQTTConnectWithResults(c, options, &data);
}


int MQTTSetMessageHandler(MQTTClient* c, const char* topicFilter, messageHandler messageHandler)
{
    int rc = FAILURE;
    int i = -1;

    /* first check for an existing matching slot */
    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
    {
        if (c->messageHandlers[i].topicFilter != NULL && strcmp(c->messageHandlers[i].topicFilter, topicFilter) == 0)
        {
            if (messageHandler == NULL) /* remove existing */
            {
                c->messageHandlers[i].topicFilter = NULL;
                c->messageHandlers[i].fp = NULL;
            }
            rc = SUCCESS; /* return i when adding new subscription */
            break;
        }
    }
    /* if no existing, look for empty slot (unless we are removing) */
    if (messageHandler != NULL) {
        if (rc == FAILURE)
        {
            for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
            {
                if (c->messageHandlers[i].topicFilter == NULL)
                {
                    rc = SUCCESS;
                    break;
                }
            }
        }
        if (i < MAX_MESSAGE_HANDLERS)
        {
            c->messageHandlers[i].topicFilter = topicFilter;
            c->messageHandlers[i].fp = messageHandler;
        }
    }
    return rc;
}


int MQTTSubscribeWithResults(MQTTClient* c, const char* topicFilter, enum QoS qos,
       messageHandler messageHandler, MQTTSubackData* data)
{
    int rc = FAILURE;
    Timer timer;
    int len = 0;
    MQTTString topic = MQTTString_initializer;
    topic.cstring = (char *)topicFilter;

#if defined(MQTT_TASK)
	  MutexLock(&c->mutex);
#endif
    if (!c->isconnected) {
        MQTT_LOG_ERROR("MQTTSubscribeWithResults - not connected goto exit");
        goto exit;
    }

    TimerInit(&timer);
    TimerCountdownMS(&timer, c->command_timeout_ms);
    // printf("MQTTSubscribeWithResults - timeout %u\n",c->command_timeout_ms);

    len = MQTTSerialize_subscribe(c->buf, c->buf_size, 0, getNextPacketId(c), 1, &topic, (int*)&qos);
    if (len <= 0) {
        MQTT_LOG_ERROR("MQTTSubscribeWithResults - MQTTSerialize_subscribe len < 0\n");
        goto exit;
    }
    if ((rc = sendPacket(c, len, &timer)) != SUCCESS) // send the subscribe packet
    {
        MQTT_LOG_ERROR("MQTTSubscribeWithResults - sendPacket != SUCCESS %d\n",rc);
        goto exit;             // there was a problem
    } 
    MQTT_LOG_DEBUG("Time MS Remaining Post Ack: %d", TimerLeftMS(&timer));
    // else {
    //     printf("MQTTSubscribeWithResults - sendPacket == SUCCESS %d\n",rc);
    // }
    TimerCountdownMS(&timer, c->command_timeout_ms);
    if (waitfor(c, SUBACK, &timer) == SUBACK)      // wait for suback
    {
        // printf("waitfor SUBACK\n");
        int count = 0;
        unsigned short mypacketid;
        data->grantedQoS = QOS0;
        if (MQTTDeserialize_suback(&mypacketid, 1, &count, (int*)&data->grantedQoS, c->readbuf, c->readbuf_size) == 1)
        {
            // printf("MQTTDeserialize_suback == 1\n");
            if (data->grantedQoS != 0x80) {
                // printf("data->grantedQoS != 0x80\n");
                rc = MQTTSetMessageHandler(c, topicFilter, messageHandler);
            } else {
                // printf("data->grantedQoS = 0x%02X\n",data->grantedQoS);
            }
        }
    }
    else {
        MQTT_LOG_DEBUG("Timer MS Remaining SUBACK Fail: %d\n", TimerLeftMS(&timer));
        MQTT_LOG_DEBUG("MQTTSubscribeWithResults - SUBACK failure\n");
        rc = FAILURE;
    }

//    printf("MQTTSubscribeWithResults: After suback rc=%d\n",rc); // dcm

exit:
    if (rc == FAILURE)
        // printf("Time MS Remaining: %d\n", TimerLeftMS(&timer));
        MQTTCloseSession(c);
#if defined(MQTT_TASK)
	  MutexUnlock(&c->mutex);
#endif
    return rc;
}


int MQTTSubscribe(MQTTClient* c, const char* topicFilter, enum QoS qos,
       messageHandler messageHandler)
{
    MQTTSubackData data;
    return MQTTSubscribeWithResults(c, topicFilter, qos, messageHandler, &data);
}


int MQTTUnsubscribe(MQTTClient* c, const char* topicFilter)
{
    int rc = FAILURE;
    Timer timer;
    MQTTString topic = MQTTString_initializer;
    topic.cstring = (char *)topicFilter;
    int len = 0;

#if defined(MQTT_TASK)
	  MutexLock(&c->mutex);
#endif
	  if (!c->isconnected)
		  goto exit;

    TimerInit(&timer);
    TimerCountdownMS(&timer, c->command_timeout_ms);

    if ((len = MQTTSerialize_unsubscribe(c->buf, c->buf_size, 0, getNextPacketId(c), 1, &topic)) <= 0)
        goto exit;
    if ((rc = sendPacket(c, len, &timer)) != SUCCESS) // send the subscribe packet
        goto exit; // there was a problem

    if (waitfor(c, UNSUBACK, &timer) == UNSUBACK)
    {
        unsigned short mypacketid;  // should be the same as the packetid above
        if (MQTTDeserialize_unsuback(&mypacketid, c->readbuf, c->readbuf_size) == 1)
        {
            /* remove the subscription message handler associated with this topic, if there is one */
            MQTTSetMessageHandler(c, topicFilter, NULL);
        }
    }
    else
        rc = FAILURE;

exit:
    if (rc == FAILURE)
        MQTTCloseSession(c);
#if defined(MQTT_TASK)
	  MutexUnlock(&c->mutex);
#endif
    return rc;
}


int MQTTPublish(MQTTClient* c, const char* topicName, MQTTMessage* message)
{
    int rc = FAILURE;
    Timer timer;
    MQTTString topic = MQTTString_initializer;
    topic.cstring = (char *)topicName;
    int len = 0;

#if defined(MQTT_TASK)
	  MutexLock(&c->mutex);
#endif
	  if (!c->isconnected)
		    goto exit;

    TimerInit(&timer);
    TimerCountdownMS(&timer, c->command_timeout_ms);

    if (message->qos == QOS1 || message->qos == QOS2)
        message->id = getNextPacketId(c);

    len = MQTTSerialize_publish(c->buf, c->buf_size, 0, message->qos, message->retained, message->id,
              topic, (unsigned char*)message->payload, message->payloadlen);
    MQTT_LOG_DEBUG("Timer MS Remaining Post Pub: %d", TimerLeftMS(&timer));
    if (len <= 0) {
        MQTT_LOG_ERROR("MQTT Pub Error len < 0");
        goto exit;
    }
    // send the subscribe packet
    if ((rc = sendPacket(c, len, &timer)) != SUCCESS) {
        MQTT_LOG_ERROR("MQTT sendPacket rc = %d", rc);
        goto exit; // there was a problem
    }
    MQTT_LOG_DEBUG("Timer MS Remaining Post Ack: %d\n", TimerLeftMS(&timer));
    if (message->qos == QOS1)
    {
        MQTT_LOG_DEBUG("Timer MS Remaining QOS1: %d", TimerLeftMS(&timer));
        if (waitfor(c, PUBACK, &timer) == PUBACK)
        {
            unsigned short mypacketid;
            unsigned char dup, type;
            if (MQTTDeserialize_ack(&type, &dup, &mypacketid, c->readbuf, c->readbuf_size) != 1) {
                rc = FAILURE;
                MQTT_LOG_ERROR("Could not process MQTT Pub ACK QOS1");
            }
        }
        else {
            rc = FAILURE;
            MQTT_LOG_ERROR("Timed out MQTT Pub ACK QOS1");
        }
        
    }
    else if (message->qos == QOS2)
    {
        MQTT_LOG_DEBUG("Timer MS Remaining QOS2: %d", TimerLeftMS(&timer));
        if (waitfor(c, PUBCOMP, &timer) == PUBCOMP)
        {
            unsigned short mypacketid;
            unsigned char dup, type;
            if (MQTTDeserialize_ack(&type, &dup, &mypacketid, c->readbuf, c->readbuf_size) != 1) {
                rc = FAILURE;
                MQTT_LOG_ERROR("Could not process MQTT Pub ACK QOS2");
            }
        }
        else{
            rc = FAILURE;
            MQTT_LOG_ERROR("Timed out MQTT Pub ACK QOS2");
        }
        
    }

exit:
    // printf("Timer MS Remaining: %d\n", TimerLeftMS(&timer));
    if (rc == FAILURE) {
        MQTT_LOG_ERROR("MQTTPublish failure");
        MQTTCloseSession(c);
    }
#if defined(MQTT_TASK)
	  MutexUnlock(&c->mutex);
#endif
    return rc;
}


int MQTTDisconnect(MQTTClient* c)
{
    int rc = FAILURE;
    Timer timer;     // we might wait for incomplete incoming publishes to complete
    int len = 0;

#if defined(MQTT_TASK)
	MutexLock(&c->mutex);
#endif
    TimerInit(&timer);
    TimerCountdownMS(&timer, c->command_timeout_ms);

	  len = MQTTSerialize_disconnect(c->buf, c->buf_size);
    if (len > 0)
        rc = sendPacket(c, len, &timer);            // send the disconnect packet
    MQTTCloseSession(c);

#if defined(MQTT_TASK)
	  MutexUnlock(&c->mutex);
#endif
    return rc;
}
