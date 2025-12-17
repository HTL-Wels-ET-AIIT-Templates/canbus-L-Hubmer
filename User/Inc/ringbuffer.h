#ifndef __RING_BUFFER_H_
#define __RING_BUFFER_H_

// Uncomment if your platform does not have memcopy implementation.
// #define NO_MEM_COPY

#include <stdint.h>

#ifdef  __cplusplus
extern  "C" {
#endif

/**
 * This is the "context" struct of the ring buffer.
 * The user does not have to care about the content, but every call to a
 * ring buffer function requires a pointer to such a struct.
 * The context struct is initialized with ringBufferInit()
 */
typedef struct {
   uint32_t tail;
   uint32_t head;
   uint32_t sizeMask;
   uint8_t *data;
}RingBuffer_t;

int ringBufferInit(RingBuffer_t *buffer, uint8_t *data, uint32_t len);

uint32_t ringBufferLen(RingBuffer_t *buffer);
uint8_t ringBufferEmpty(RingBuffer_t *buffer);
uint32_t ringBufferLenAvailable(RingBuffer_t *buffer);
uint32_t ringBufferMaxSize(RingBuffer_t *buffer);

void ringBufferAppendOne(RingBuffer_t *buffer, uint8_t data);
void ringBufferAppendMultiple(RingBuffer_t *buffer, uint8_t *data, uint32_t len);

uint8_t ringBufferPeekOne(RingBuffer_t *buffer);
uint8_t ringBufferGetOne(RingBuffer_t *buffer);

void ringBufferGetMultiple(RingBuffer_t *buffer, uint8_t *dst, uint32_t len);
void ringBufferPeekMultiple(RingBuffer_t *buffer, uint8_t *dst, uint32_t len);

void ringBufferDiscardMultiple(RingBuffer_t *buffer, uint32_t len);
void ringBufferClear(RingBuffer_t *buffer);

#ifdef  __cplusplus
      }
#endif

#endif
