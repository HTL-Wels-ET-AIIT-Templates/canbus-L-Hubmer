//
// AIIT 4JG
// UART Communication
//

#ifndef UART_H
#define UART_H
#include "ringbuffer.h"
/* Defines -------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void uartInit(void);
void uartTask(char Sender[8]);
void uartSendMsgIfAvailable(RingBuffer_t* MsgBuffer);

#endif // UART_H
