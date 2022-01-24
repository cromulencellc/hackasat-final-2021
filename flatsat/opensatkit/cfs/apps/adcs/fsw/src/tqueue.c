#include "tqueue.h"

void TQUEUE_Setup(TQUEUE_Ptr_t q, uint8 *buffer, uint32 length, const char* queueName)
{   
    q->buffer = buffer;
    q->length = length;
    q->head   = buffer;
    q->tail   = buffer;
    OS_MutSemCreate(&q->mutex, queueName, 0);
}

void TQUEUE_Push(TQUEUE_Ptr_t q, const uint8 *bytes, uint32 length) 
{   
    uint32 ii = 0;
    OS_MutSemTake(q->mutex);
    for (ii = 0; ii < length; ii++)
    {
        *q->tail = bytes[ii];
        q->tail++;

        if ((intptr_t)(q->tail - q->buffer) >= q->length)
        {
            q->tail = q->buffer;
        }
    }
    OS_MutSemGive(q->mutex);
}

uint32 TQUEUE_Pop(TQUEUE_Ptr_t q, uint8 *out_bytes, uint32 length) 
{
    uint32 ii = 0;
    OS_MutSemTake(q->mutex);
    for (ii = 0; ii < length; ii++)
    {
        if (q->head == q->tail)
        {
            /* We're caught up */
            break;
        }
        out_bytes[ii] = *q->head;
        *q->head = 0;
        q->head++;
        if ((intptr_t)(q->head - q->buffer) >= q->length)
        {
            q->head = q->buffer;
        }
    }
    OS_MutSemTake(q->mutex);
    return ii;
}
