


// INCLUDE -----------------------------------------------

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f4xx_hal.h"

#ifdef STM32F407xx
  #include "stm32f4_discovery.h"
#endif /* STM32F407xx */

#ifdef STM32F429xx
  #include "stm32f429i_discovery.h"
  #include "stm32f429i_discovery_lcd.h"
#endif /* STM32F429xx */

// -------------------------------------------------------


// DEFINE ------------------------------------------------

#define BAUDRATE 				9600
#define RXPIN 					GPIO_PIN_7
#define DATAPORT 				GPIOB
#define UART_PRIORITY         	6
#define UART_RX_SUBPRIORITY   	0
#define MAXSTRING          		4 // Biggest string the user will type is 4 bytes

// -------------------------------------------------------





void init_rx(void) {

	int i;

	// set up USART1 RX GPIO config (PB7) ---------------------------
	__GPIOB_CLK_ENABLE();	// enable dataport clock
	__USART1_CLK_ENABLE();	// usart clock
	__DMA2_CLK_ENABLE();	// dma clock

	RX_T * R = (RX_T *)malloc(sizeof(RX_T));

	R->rx_buffer = '\000';
	for(i = 0; i < MAXSTRING; i++) R->rx_string[i] = -1;
	R->rx_index = 0;

	init_gpio(R);
	init_uart(R);
	init_dma(R);

	/* Associate the initialized DMA handle to the the UART handle */
	__HAL_LINKDMA(R->huart, R->hdmarx, R->hdma_usart1_rx);

	// set priority of dma interrupt
	HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, UART_PRIORITY, UART_RX_SUBPRIORITY);
	// enable the dma interrupt
	HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);


	// initialize dma interrupt
	DMA2_Stream2_IRQHandler(R);


	// start DMA
	__HAL_UART_FLUSH_DRREGISTER(&(R->huart1));
	HAL_UART_Receive_DMA(&(R->huart1), &(R->rx_buffer), 1);

}


void init_gpio(RX_T * R) {

	R->GPIO_InitStruct.Pin = RXPIN;					// use gpio 7
	R->GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;		// alternate push pull mode
	R->GPIO_InitStruct.Pull = GPIO_NOPULL;			// no pull up or pull down activation
	R->GPIO_InitStruct.Speed = GPIO_SPEED_FAST;	
	R->GPIO_InitStruct.Alternate = GPIO_AF7_USART1;

	HAL_GPIO_Init(DATAPORT, &(R->GPIO_InitStruct));
}


void init_uart(RX_T * R) {
	// set up UART --------------------------------------------------------------

	R->huart1.Instance = USART1;
	R->huart1.Init.BaudRate = BAUDRATE;
	R->huart1.Init.WordLength = UART_WORDLENGTH_8B;
	R->huart1.Init.StopBits = UART_STOPBITS_1;
	R->huart1.Init.Parity = UART_PARITY_NONE;
	R->huart1.Init.Mode = UART_MODE_RX;
	R->huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	R->huart1.Init.OverSampling = UART_OVERSAMPLING_16;

	HAL_UART_Init(&(R->huart1));
}


void init_dma(RX_T * R) {
	// set up DMA channel for receiving process ---------------------------------

	R->hdma_usart1_rx.Instance = DMA2_Stream2;
	R->hdma_usart1_rx.Init.Channel = DMA_CHANNEL_5;
	R->hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	R->hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
	R->hdma_usart1_rx.Init.MemInc = DMA_MINC_DISABLE;
	R->hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	R->hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	R->hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;	// important so it keeps getting chars until told to stop, otherwise need to config everytime
	R->hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
	R->hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

	HAL_DMA_Init(&(R->hdma_usart1_rx));
}


// set up DMA interrupt
void DMA2_Stream2_IRQHandler(RX_T * R) {

	// clear pending bit
    HAL_NVIC_ClearPendingIRQ(DMA2_Stream2_IRQn);

    // handle the dma interrupt request
    HAL_DMA_IRQHandler(&(R->hdma_usart1_rx));
}


// this is executed at end of recieve process
void HAL_UART_RxCpltCallback(RX_T * R) {

    __HAL_UART_FLUSH_DRREGISTER(&(R->huart1)); // Clear the buffer to prevent overrun

    int i = 0;

    print(&(R->rx_buffer)); // Echo the character that caused this callback so the user can see what they are typing

    if(R->rx_buffer == 8 || R->rx_buffer == 127) {	// backspace or delete
        print(" \b"); // "\b space \b" clears the terminal character. Remember we just echoced a \b so don't need another one here, just space and \b
        R->rx_index--; 

        if(R->rx_index < 0) R->rx_index = 0;

    } else if(R->rx_buffer == '\n' || R->rx_buffer == '\r') { // enter
        executeSerialCommand(R->rx_string);
        R->rx_string[R->rx_index] = -1;
        R->rx_index = 0;

        for(i = 0; i < MAXSTRING; i++) R->rx_string[i] = -1; // Clear the string buffer

    } else {
        R->rx_string[R->rx_index] = R->rx_buffer; 	// Add character to string
        R->rx_index++;

        if(R->rx_index > MAXSTRING) {	// too many characters
            R->rx_index = 0;
            for(i = 0; i < MAXSTRING; i++) R->rx_string[i] = -1; // Clear string buffer
            print("\r\nConsole> ");
        }

    }
}