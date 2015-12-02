


// INCLUDE -----------------------------------------------

#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"

#include "arm_math.h"
#include "ece486.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "uart_rx.h"

// -------------------------------------------------------


static __IO ITStatus RX_Complete = RESET;
static GPIO_InitTypeDef GPIO_InitStruct;
static UART_HandleTypeDef huart1;
static DMA_HandleTypeDef hdma_usart1_rx;



RX_T * init_rx(void) {

	int i, j;

	RX_T * R = (RX_T *)malloc(sizeof(RX_T));

	R->rx_buffer = (uint8_t *)malloc(sizeof(uint8_t) * MAXSTRING);
	R->rx_string = (char *)malloc((sizeof(char) * MAXSTRING) + 1);
	for(i = 0; i < MAXSTRING; i++) R->rx_buffer[i] = 0;
	for(j = 0; j < MAXSTRING + 1; j++) R->rx_string[j] = 0;
	R->rx_index = 0;

	init_uart();

	// start DMA
	__HAL_UART_FLUSH_DRREGISTER(&huart1);
	HAL_UART_Receive_DMA(&huart1, R->rx_buffer, 4);	// 

	return R;

}


void init_uart(void) {
	// set up UART --------------------------------------------------------------
	huart1.Instance = USART1;
	huart1.Init.BaudRate = BAUDRATE;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;

	if(HAL_UART_Init(&huart1) != HAL_OK) flagerror(DEBUG_ERROR);
}


// this is automatically called by the HAL_UART_INIT call in the init_uart function
void HAL_UART_MspInit(UART_HandleTypeDef * huart) {
  
	__GPIOB_CLK_ENABLE();	// enable dataport clock
	__USART1_CLK_ENABLE();	// usart clock
	__DMA2_CLK_ENABLE();	// dma clock
  
  	// set up USART1 RX GPIO config (PB7) ---------------------------
 	GPIO_InitStruct.Pin = RXPIN;				// use gpio 7
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;		// alternate push pull mode
	GPIO_InitStruct.Pull = GPIO_NOPULL;			// no pull up or pull down activation
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;	
	GPIO_InitStruct.Alternate = GPIO_AF7_USART1;

	HAL_GPIO_Init(DATAPORT, &GPIO_InitStruct);
  
	// set up DMA channel for receiving process ---------------------------------
	hdma_usart1_rx.Instance = DMA2_Stream2;
	hdma_usart1_rx.Init.Channel = DMA_CHANNEL_5;
	hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_usart1_rx.Init.MemInc = DMA_MINC_DISABLE;
	hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;	// important so it keeps getting chars until told to stop, otherwise need to config everytime
	hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
	hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

	HAL_DMA_Init(&hdma_usart1_rx);
        
	// Associate the initialized DMA handle to the the UART handle
	// this is kinda strange, not actually a function call, but its a macro
	// (UART_HandleTypeDef * DIFFERENT THEN THE ONE DEFINED UP TOP FOR THE HAL_RECEIVE, dma field in uart struct, DMA_HandleTypeDef)
	__HAL_LINKDMA(huart, hdmarx, hdma_usart1_rx);

	// set priority of dma interrupt
	HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, UART_PRIORITY, UART_RX_SUBPRIORITY);
	// enable the dma interrupt
	HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
    
}


/**
  * @brief UART MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO, DMA and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef * huart) {
  
  /* Reset peripherals  */
  __USART1_FORCE_RESET();
  __USART1_RELEASE_RESET();

  HAL_GPIO_DeInit(DATAPORT, RXPIN);
   
  /* De-Initialize the DMA Stream associate to transmission process */
  HAL_DMA_DeInit(&hdma_usart1_rx); 
  
  /*##-4- Disable the NVIC for DMA ###########################################*/
  HAL_NVIC_DisableIRQ(DMA2_Stream2_IRQn);
}


// handle DMA interrupt 
void DMA2_Stream2_IRQHandler(void) {

	// clear pending bit
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream2_IRQn);

    // handle the dma interrupt request
    HAL_DMA_IRQHandler(&hdma_usart1_rx);
}


// this is automatically executed at end of recieve process
void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart) {

    // set transmit complete flag for usart_read
    RX_Complete = SET;
}


void usart_read(RX_T * R) {

	while(RX_Complete != SET) {}	// wait until recieve is complete

    // rx_string now contains the 4 ints from the rx stream
    snprintf(R->rx_string, 4, "%d %d %d %d", R->rx_buffer[0], R->rx_buffer[1], R->rx_buffer[2], R->rx_buffer[3]);

	switch(R->rx_string[0]) {
		case 1: // DELAY --------------------------------------------------------
			
			// amount of delay in seconds
			R->rx_string[1] = R->rx_string[1] / (2 * 255);	// 8 bit char is max of 255, so a transmitted delay val of 255 will be half a second
			// amount of gain of delayed signal
			R->rx_string[2] = R->rx_string[2] / 255;		// max gain is 1

			break;

 		case 2: // COMPRESSOR ---------------------------------------------------



			break;

		case 3: // EQ -----------------------------------------------------------



			break;

		default:

			// invalid effect input
			flagerror(DEBUG_ERROR);	// turn on error led
			while(1);
	}
}

