//
// AIIT 4JG
// UART Communication
// TODO: Add description


// Includes ------------------------------------------------------------------
#include "uart.h"
#include "main.h"
#include "ringbuffer.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "stm32f4xx_hal_uart.h"
#include "stm32f429i_discovery_lcd.h"


// Defines --------------------------------------------------------------------
#define UART_DATA_SIZE		256

// Variables ------------------------------------------------------------------

static UART_HandleTypeDef uartHandle;
static volatile char rxChar = 0;
static bool Flm_User = 0;
static uint32_t Time_Counter = 0;
char Tc_String[32];

RingBuffer_t USART6_Recieve;
uint8_t USART6_Recieve_Data[UART_DATA_SIZE];

// Function Declarations ------------------------------------------------------
static void GpioInit(void);
static void Uart6Init(void);

static void uartSendByte(uint8_t byte);
static void uartSendString(char *buffer);
static void uartSendData(uint8_t *data, unsigned int size);
static int GetUserButtonPressed(void);
void uartSendMsgIfAvailable(RingBuffer_t MsgBuffer);



// Functions ------------------------------------------------------------------
/**
 * Initialization Code for module uart
 * @param None
 * @retval None
 */
void uartInit(void) {
	GpioInit();
	Uart6Init();
	ringBufferInit(&USART6_Recieve, USART6_Recieve_Data, UART_DATA_SIZE);
}

/**
 * Task function of module uart.
 * @param None
 * @retval None
 */
void uartSendMsgIfAvailable(RingBuffer_t MsgBuffer)
{
	if(ringBufferLen(&MsgBuffer) > 0)
	{
		char nextChar = ringBufferGetOne(&MsgBuffer);
		switch(ringBufferGetOne(&MsgBuffer))
		{
		case 13:
			break;

		case 228:
			uartSendByte('a');
			uartSendByte('e');
			break;

		case 246:
			uartSendByte('o');
			uartSendByte('e');
			break;

		case 252:
			uartSendByte('u');
			uartSendByte('e');
			break;

		default:
			uartSendByte(nextChar);
			break;
		}
	}
}
void uartTask(int* timer){
	// TODO: What has to be done in our main loop?

	if(ringBufferLen(&USART6_Recieve) > 0)
	{
		switch(ringBufferGetOne(&USART6_Recieve))
		{
		case 13:
			break;

		case 228:
			printf("ae");
			break;

		case 246:
			printf("oe");
			break;

		case 252:
			printf("ae");
			break;

		default:
			printf("%c",rxChar);
			break;
		}
	}

	if(*timer >= 1000)
	{
		Time_Counter++;
		sprintf(Tc_String, "%li ", Time_Counter);
		uartSendString(Tc_String);
		*timer = 0;
	}

}

/**
   @brief configure the used GPIO pins
   @param None
   @return None
 */
static void GpioInit(void) {
	// configure GPIOs for UART
	__HAL_RCC_GPIOG_CLK_ENABLE();
	GPIO_InitTypeDef pin;
	pin.Alternate = GPIO_AF8_USART6;
	pin.Mode = GPIO_MODE_AF_PP;
	pin.Pull = GPIO_PULLUP;
	pin.Speed = GPIO_SPEED_FAST;
	pin.Pin = GPIO_PIN_9 | GPIO_PIN_14;
	HAL_GPIO_Init(GPIOG, &pin);
}

/**
   @brief configure UART6: 115200 Baud, No Parity, 1 Stop Bit, 8Bit word length
   @param None
   @return None
 */
static void Uart6Init(void) {
	__HAL_RCC_USART6_CLK_ENABLE();
	uartHandle.Instance = USART6;
	uartHandle.Init.BaudRate = 115200;
	uartHandle.Init.Mode = USART_MODE_TX_RX;
	uartHandle.Init.Parity = USART_PARITY_NONE;
	uartHandle.Init.StopBits = USART_STOPBITS_1;
	uartHandle.Init.WordLength = USART_WORDLENGTH_8B;
	uartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uartHandle.Init.OverSampling = UART_OVERSAMPLING_8;
	HAL_UART_Init(&uartHandle);

	__HAL_USART_ENABLE_IT(&uartHandle, USART_IT_RXNE);

	HAL_NVIC_EnableIRQ(USART6_IRQn);


}

/**
   @brief send a byte over UART
   @param byte byte to send
   @return None
 */
static void uartSendByte(uint8_t byte)
{
	HAL_UART_Transmit(&uartHandle, &byte, 1, 5);
}

/**
   @brief send a string over uart
   @param buffer string to send
   @return None
 */
static void uartSendString(char *buffer)
{
	int len = strlen(buffer);
	HAL_UART_Transmit(&uartHandle, (uint8_t*)buffer, len, len*2);
}

/**
   @brief send data array over uart
   @param data array to send
	 @param size size of array
   @return None
 */
static void uartSendData(uint8_t *data, unsigned int size) {
	HAL_UART_Transmit(&uartHandle, data, size, size*2);
}

/**
 * ISR (Interrupt Service Routine) of USART6.
 * Takes received byte from receive buffer and stores it in global
 * array uartData. If a full packet has been received the CRC is checked
 * and either the valid or the invalid packet counter is increased.
 * @param None
 * @retval None
 */
void USART6_IRQHandler(void)
{
	if(__HAL_UART_GET_FLAG(&uartHandle, UART_FLAG_RXNE)) {
		__HAL_UART_CLEAR_FLAG(&uartHandle, UART_FLAG_RXNE);
		uint8_t rx = uartHandle.Instance->DR;
		rxChar = rx;
		ringBufferAppendOne(&USART6_Recieve, rxChar);
	}
}

/**
   @brief Get User button State
   @param None
   @return Button State
 */
static int GetUserButtonPressed(void) {
	return (GPIOA->IDR & 0x0001);
}
