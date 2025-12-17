/**
 ******************************************************************************
 * @file           : can.c
 * @brief          : CAN handling functions
 * @author: L. Hubmer
 ******************************************************************************
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "main.h"
#include "stm32f429i_discovery_lcd.h"
#include "tempsensor.h"

/* Private define ------------------------------------------------------------*/
#define CAN1_CLOCK_PRESCALER  16   // Adjust for correct bitrate

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef canHandle;

/* TX/RX buffers */
static CAN_TxHeaderTypeDef txHeader;
static uint8_t  txData[8];
static uint32_t txMailbox;

static CAN_RxHeaderTypeDef rxHeader;
static uint8_t  rxData[8];

/* Private function prototypes -----------------------------------------------*/
static void initGpio(void);
static void initCanPeripheral(void);

/**
 * Initialize GPIO and CAN peripheral
 */
void canInitHardware(void) {
	initGpio();
	initCanPeripheral();
}

/**
 * CAN initialization (including display)
 */
void canInit(void) {

	canInitHardware();

	LCD_SetFont(&Font12);
	LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
	LCD_SetPrintPosition(3,1);
	printf("CAN1: Send-Recv");

	LCD_SetColors(LCD_COLOR_GREEN, LCD_COLOR_BLACK);
	LCD_SetPrintPosition(5,1);
	printf("Send-Cnt:");
	LCD_SetPrintPosition(5,15);
	printf("%5d", 0);

	LCD_SetPrintPosition(7,1);
	printf("Recv-Cnt:");
	LCD_SetPrintPosition(7,15);
	printf("%5d", 0);

	LCD_SetPrintPosition(9,1);
	printf("Send-Data:");

	LCD_SetPrintPosition(15,1);
	printf("Recv-Data:");

	LCD_SetPrintPosition(30,1);
	printf("Bit-Timing-Register: 0x%lx", CAN1->BTR);

	/* ----------------------
	 * Initialize DS18B20
	 * --------------------- */
	//   ds18b20_init();
	//   ds18b20_startMeasure();
}

/**
 * Task: Send a CAN message every cycle
 */
void canSendTask(void) {
	static unsigned int sendCnt = 0;
	char Name[8] = "NAME";

	/* Read temperature from DS18B20 (°C × 100) */
	int16_t tempScaled = 0x5432; //ds18b20_readTemp();
	//ds18b20_startMeasure();     // Start next measurement
	/* Prepare CAN header */
	txHeader.StdId = 4;
	txHeader.IDE   = CAN_ID_STD;
	txHeader.RTR   = CAN_RTR_DATA;
	txHeader.DLC   = 8;

	/* Data payload (temperature) */
	txData[0] = Name[0];      //
	txData[1] = Name[1];   //
	txData[2] = Name[2];      // Low Byte
	txData[3] = Name[3]; // High Byte
	txData[4] = Name[4];      //
	txData[5] = Name[5];   //
	txData[6] = Name[6];      // Low Byte
	txData[7] = Name[7];

	/* Send CAN frame */
	if (HAL_CAN_AddTxMessage(&canHandle, &txHeader, txData, &txMailbox) == HAL_OK) {
		sendCnt++;

		/* Display send counter */
		LCD_SetColors(LCD_COLOR_GREEN, LCD_COLOR_BLACK);
		LCD_SetPrintPosition(5,15);
		printf("%5d", sendCnt);

		/* Display temperature sent */
		LCD_SetPrintPosition(9,1);
		printf("Send-Data: %02X %02X %02X %02X ",txData[0], txData[1], txData[2], txData[3]);
	}
}

void canSendLetter(void) {
	static unsigned int sendCnt = 0;
	char Letter = 'c';

	/* Prepare CAN header */
	txHeader.StdId = 0x1001;
	txHeader.IDE   = CAN_ID_STD;
	txHeader.RTR   = CAN_RTR_DATA;
	txHeader.DLC   = 1;

	/* Data payload (temperature) */
	txData[0] = Letter;      //


	/* Send CAN frame */
	if (HAL_CAN_AddTxMessage(&canHandle, &txHeader, txData, &txMailbox) == HAL_OK) {
		sendCnt++;

		/* Display send counter */
		LCD_SetColors(LCD_COLOR_GREEN, LCD_COLOR_BLACK);
		LCD_SetPrintPosition(5,15);
		printf("%5d", sendCnt);

		/* Display temperature sent */
		LCD_SetPrintPosition(9,1);
		printf("Send-Data: %c",txData[0]);
	}
}
/**
 * Task: Receive CAN messages if available
 */
void canReceiveTask(void) {
	static unsigned int recvCnt = 0;

	/* Check for RX pending */
	if (HAL_CAN_GetRxFifoFillLevel(&canHandle, CAN_RX_FIFO0) == 0)
		return;

	/* Read message */
	if (HAL_CAN_GetRxMessage(&canHandle, CAN_RX_FIFO0, &rxHeader, rxData) != HAL_OK)
		return;

	recvCnt++;

	/* Extract temperature */
	int16_t temp = (rxData[1] << 8) | rxData[2];
	int16_t Head = rxHeader.StdId;

	/* Update LCD */
	LCD_SetColors(LCD_COLOR_GREEN, LCD_COLOR_BLACK);
	LCD_SetPrintPosition(7,15);
	printf("%5d", recvCnt);

	LCD_SetPrintPosition(15,1);
	printf("Recv-Data: %02X %02X %02X %02X ",rxData[0], rxData[1], rxData[2], rxData[3]);
	LCD_SetPrintPosition(16,1);
	printf("Recv-Head: 0x%04X ",Head);
}

/*
 * Initialize GPIO pins PB8 (RX) and PB9 (TX)
 */
static void initGpio(void) {
	GPIO_InitTypeDef canPins;

	__HAL_RCC_GPIOB_CLK_ENABLE();

	canPins.Alternate = GPIO_AF9_CAN1;
	canPins.Mode = GPIO_MODE_AF_OD;
	canPins.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	canPins.Pull = GPIO_PULLUP;
	canPins.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOB, &canPins);
}

/**
 * Initialize CAN peripheral
 */
static void initCanPeripheral(void) {

	CAN_FilterTypeDef canFilter;

	__HAL_RCC_CAN1_CLK_ENABLE();

	canHandle.Instance = CAN1;
	canHandle.Init.TimeTriggeredMode = DISABLE;
	canHandle.Init.AutoBusOff = DISABLE;
	canHandle.Init.AutoWakeUp = DISABLE;
	canHandle.Init.AutoRetransmission = ENABLE;
	canHandle.Init.ReceiveFifoLocked = DISABLE;
	canHandle.Init.TransmitFifoPriority = DISABLE;
	canHandle.Init.Mode = CAN_MODE_LOOPBACK;
	canHandle.Init.SyncJumpWidth = CAN_SJW_1TQ;

	canHandle.Init.TimeSeg1 = CAN_BS1_15TQ;
	canHandle.Init.TimeSeg2 = CAN_BS2_6TQ;
	canHandle.Init.Prescaler = CAN1_CLOCK_PRESCALER;

	if (HAL_CAN_Init(&canHandle) != HAL_OK)
		Error_Handler();

	/* Accept all messages */
	canFilter.FilterBank = 0;
	canFilter.FilterMode = CAN_FILTERMODE_IDMASK;
	canFilter.FilterScale = CAN_FILTERSCALE_32BIT;
	canFilter.FilterIdHigh = 0x0000;
	canFilter.FilterIdLow = 0x0000;
	canFilter.FilterMaskIdHigh = 0x0000;
	canFilter.FilterMaskIdLow = 0x0000;
	canFilter.FilterFIFOAssignment = CAN_RX_FIFO0;
	canFilter.FilterActivation = ENABLE;
	canFilter.SlaveStartFilterBank = 14;

	if (HAL_CAN_ConfigFilter(&canHandle, &canFilter) != HAL_OK)
		Error_Handler();

	if (HAL_CAN_Start(&canHandle) != HAL_OK)
		Error_Handler();
}

/**
 * CAN RX interrupt handler
 */
void CAN1_RX0_IRQHandler(void) {
	HAL_CAN_IRQHandler(&canHandle);
}

/**
 * FIFO0 message pending callback (unused)
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	// handled in main loop
}
