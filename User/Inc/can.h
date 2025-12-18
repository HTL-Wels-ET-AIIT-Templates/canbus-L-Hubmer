#ifndef CAN_H
#define CAN_H

#ifdef __cplusplus
extern "C" {
#endif
#include "ringbuffer.h"

void canInitHardware(void);
void canInit(void);
void canReceiveTask(RingBuffer_t* MsgRecieve);
void canSendBegin(char Sender[8]);
void canSendEnd();
void canSendLetter(char Letter, uint16_t check_number);
void canRecieve();

#ifdef __cplusplus
}
#endif

#endif // CAN_H
