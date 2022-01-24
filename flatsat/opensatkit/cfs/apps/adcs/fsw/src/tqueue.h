/* 
** Purpose: Defines a helper queue class to coordinate between UART and CI/TO messages
** 
** Notes: 
**  1. Avoids need for Semaphores by design: Producer and consumer use different pointers
**     to the same QUEUE. Producer only touches the Tail pointer, Consumer only reads the
**     the Tail pointer (it could possibly miss the newest bytes, but next pass will get 
**     those) and advances the head pointer. Additionally, if the Head catches up to the tail
**     (they point to the same byte) the Pull stops, meaning there's no chance the byte could
**     be read & written at the same time
**  2. The QUEUE maintains invalid data regions as NULLs, this is for convenience.
*/ 


#ifndef _TQUEUE_
#define _TQUEUE_

#include "app_cfg.h"
#include <common_types.h>

#define TQUEUE_BASENAME "tqueue"

typedef struct {
    uint8 *buffer; // Backing Buffer
    uint8 *head;   // Read Ptr
    uint8 *tail;   // Write Ptr
    uint32 length; // Backing Buffer Size
    uint32 mutex;
} TQUEUE_t, *TQUEUE_Ptr_t ; 


/******************************************************************************
** Function: QUEUE_Setup
** Setup QUEUE with backing memory
*/
void TQUEUE_Setup(TQUEUE_Ptr_t q, uint8 *QUEUE, uint32 length, const char* queueName);

/******************************************************************************
** Function: QUEUE_Push 
** Add bytes to the end of the QUEUE
*/
void TQUEUE_Push(TQUEUE_Ptr_t q, const uint8 *bytes, uint32 length);

/******************************************************************************
** Function: QUEUE_Pop
** Remove up to the request amount of bytes from the start of the QUEUE 
*/
uint32 TQUEUE_Pop(TQUEUE_Ptr_t q, uint8 *out_bytes, uint32 length);


#endif // _circular_QUEUE_ 