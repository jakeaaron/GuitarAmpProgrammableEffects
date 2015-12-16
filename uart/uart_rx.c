/**
 * @file uart_rx.c
 *
 * @author Jacob Allenwood
 * @date November 29, 2015
 *
 * @brief []
 * @details []
 * 
 */


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


// GLOBAL VARIABLES TO THIS FILE ------------------------- 

static 	__IO ITStatus RX_Complete = RESET;
static UART_HandleTypeDef huart1;
// static DMA_HandleTypeDef hdma_rx;

// static UART_HandleTypeDef UartHandle;
static __IO ITStatus UartReady = RESET;
static DMA_HandleTypeDef hdma_tx;

// -------------------------------------------------------

static void Error_Handler(void);
static size_t mystrlen( const char *str );


RX_T * init_rx(void) {

	int i, j;

	RX_T * R = (RX_T *)malloc(sizeof(RX_T));

	R->rx_buffer = (uint8_t *)malloc(sizeof(uint8_t) * MAXSTRING);
	R->rx_string = (char *)malloc((sizeof(char) * MAXSTRING + 1)); // +1 for null at end for snprintf
	for(i = 0; i < MAXSTRING; i++) R->rx_buffer[i] = 0;
	for(j = 0; j < MAXSTRING + 1; j++) R->rx_string[j] = 0;
	// R->rx_index = 0;

	init_uart();

	// // initialize LED for waiting to receive characters
	// BSP_LED_Init(LED3);
	// BSP_LED_Init(LED6);

	return R;
}


void init_uart(void) {
	// set up UART --------------------------------------------------------------
	huart1.Instance = UART4;	// CANNOT USE USART1 SOMEHOW CONNECTED TO THE USB PORT

	huart1.Init.BaudRate = 600;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.Mode = UART_MODE_TX_RX;

  /* 
   * Note that HAL_UART_Init() calls HAL_UART_MspInit() (The MPU-specific initialization
   * routine).  This function must be re-defined  below in order to associate 
   * the correct DMA stream with the UART.  Similarly, HAL_UART_DeInit() calls
   * HAL_UART_MspDeInit()... which we're on the hook to define.
   */
	if(HAL_UART_Init(&huart1) != HAL_OK) Error_Handler();
	UartReady = SET;
}


// this is automatically called by the HAL_UART_INIT call in the init_uart function
void HAL_UART_MspInit(UART_HandleTypeDef * huart) {
	
	GPIO_InitTypeDef GPIO_InitStruct;
  
	__GPIOC_CLK_ENABLE();	// enable dataport clock
	__UART4_CLK_ENABLE();	// uart clock
	__DMA1_CLK_ENABLE();	// dma clock
  
  	// set up UART4 RX GPIO config (PC11) ---------------------------
 	GPIO_InitStruct.Pin = RXPIN;				// use gpio 11
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;		// alternate push pull mode
	GPIO_InitStruct.Pull = GPIO_NOPULL;			// no pull up or pull down activation
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;	
	GPIO_InitStruct.Alternate = GPIO_AF8_UART4;

	// init rx gpio
	HAL_GPIO_Init(DATAPORT, &GPIO_InitStruct);

	// transmit gpio PC10
	GPIO_InitStruct.Pin = TXPIN;
	GPIO_InitStruct.Alternate = GPIO_AF8_UART4;

	// init tx gpio
	HAL_GPIO_Init(DATAPORT, &GPIO_InitStruct);
  

	/* Configure the DMA handler for Transmission process (DMA1, Stream 4, Channel 4) */
	hdma_tx.Instance                 = DMA1_Stream4;

	hdma_tx.Init.Channel             = DMA_CHANNEL_4;
	hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
	hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
	hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	hdma_tx.Init.Mode                = DMA_NORMAL;
	hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
	hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;
	hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;

	HAL_DMA_Init(&hdma_tx);   

	/* Associate the initialized DMA handle to the the UART handle */
	__HAL_LINKDMA(huart, hdmatx, hdma_tx);


	/* NVIC configuration for DMA transfer complete interrupt (UART4_TX) */
	HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
    
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
  
	static DMA_HandleTypeDef hdma_tx;
	static DMA_HandleTypeDef hdma_rx;

	/* Reset peripherals  */
	__UART4_FORCE_RESET();
	__UART4_RELEASE_RESET();

	HAL_GPIO_DeInit(DATAPORT, RXPIN);
	HAL_GPIO_DeInit(DATAPORT, TXPIN);

	/* De-Initialize the DMA Stream associate to transmission process */
	HAL_DMA_DeInit(&hdma_tx); 
	HAL_DMA_DeInit(&hdma_rx);

	/*##-4- Disable the NVIC for DMA ###########################################*/
	HAL_NVIC_DisableIRQ(DMA1_Stream4_IRQn);

}


// handle DMA interrupt 
void DMA1_Stream4_IRQHandler(void) {

    // handle the dma interrupt request
    HAL_DMA_IRQHandler(huart1.hdmatx);
}

void DMA1_Stream2_IRQHandler(void) {
	HAL_DMA_IRQHandler(huart1.hdmarx);
}

/**
  * @brief  This function handles USARTx interrupt request.
  * @param  None
  * @retval None
  */
void UART4_IRQHandler(void) {
 	HAL_UART_IRQHandler(&huart1);
}


// this is automatically executed at end of recieve process
void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart) {
	// UART_putstr("im in receive callback");
    // set receive complete flag for usart_read
    RX_Complete = SET;
    // UartReady = SET;
}


/* UART  Tx Transfer completed callback
  * @param  UartHandle: UART handle. 
  * @retval None
  * 
  * Getting here is confusing...  
  * The DMA IRQ handler below calls  HAL_DMA_IRQHandler() for the UART Tx DMA
  * HAL_DMA_IRQHandler() calls UART_DMATransmitCplt() on a transmit complete.
  * UART_DMA_TransmitCplt() eventually calls HAL_UART_TxCpltCallback(), which we're
  * allowed to re-define.
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	// UART_putstr("im done transmitting");
  /* Set transmission flag: trasfer complete*/
  UartReady = SET; 
}


// /** talk about the mapping of vals from the gui to this side
//  * @brief [brief description]
//  * @details [long description]
//  * 
//  * @param R [description]
//  */
// void usart_read(RX_T * R) {

// 	// set peripheral to receive
// 	if(HAL_UART_Receive_DMA(&huart1, R->rx_buffer, MAXSTRING) != HAL_OK) { flagerror(DEBUG_ERROR); while(1); } 

// 	UART_putstr("now peripheral is set to receive");

// 	// blink while we are waiting to receive characters
// 	while(RX_Complete != SET) {
//     	BSP_LED_Toggle(LED6);
//     	HAL_Delay(450);	
//     	BSP_LED_Toggle(LED3);
//     	HAL_Delay(450);
// 	}	

// 	UART_putstr("apparently we've received characters now");

//     // rx_string now contains the 4 ints from the rx stream
//     snprintf(R->rx_string, 5, "%d %d %d %d", R->rx_buffer[0], R->rx_buffer[1], R->rx_buffer[2], R->rx_buffer[3]);

// 	// the values received are from the gui and are mapped a certain way to always be a positive (or 0) value
// 	// on this end we do the reverse of what the gui did to get the actual values entered
// 	switch(R->rx_string[0]) {
// 		case 1: // DELAY --------------------------------------------------------
			
// 			// amount of delay in seconds
// 			R->rx_string[1] = R->rx_string[1] / (2.0 * 255.0);	// 8 bit char is max of 255, so a transmitted delay val of 255 will be half a second
// 			// amount of gain of delayed signal
// 			R->rx_string[2] = R->rx_string[2] / 255.0;		// max gain is 1

// 			break;

//  		case 2: // COMPRESSOR ---------------------------------------------------

//  			// threshold level in dB
//  			R->rx_string[1] = R->rx_string[1] - 200;

//  			// compressor ratio
//  			// R->rx_string[2] doesn't need to be mapped to a different value

// 			break;

// 		case 3: // EQ -----------------------------------------------------------

// 			// low band
// 			R->rx_string[1] = R->rx_string[1] - 10;
// 			// mid band
// 			R->rx_string[2] = R->rx_string[2] - 10;
// 			// high band
// 			R->rx_string[3] = R->rx_string[3] - 10;


// 			break;

// 		default:

// 			// invalid effect input
// 			flagerror(DEBUG_ERROR);	// turn on error led
// 			while(1);
// 	}
// }




// void usart_read(RX_T * R) {
// 	if(HAL_UART_Transmit(&huart1, (uint8_t*)aTxBuffer, sizeof(aTxBuffer) / sizeof(uint8_t), 50000) != HAL_OK)
// 	{
// 	Error_Handler();   
// 	}

// 	/* Turn LED3 on: Transfer in transmission process is correct */
// 	/* then Off for next transmission */
// 	BSP_LED_On(LED3);
// 	HAL_Delay(400);
// 	BSP_LED_Off(LED3);

// 	/*##-3- Put UART peripheral in reception process ###########################*/  
// 	if(HAL_UART_Receive(&huart1, (uint8_t*)rxbuf, sizeof(rxbuf) / sizeof(uint8_t), 50000) != HAL_OK)
// 	{
// 	Error_Handler();  
// 	}

// 	UART_putstr("past receive");
// }


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void) {
  BSP_LED_On(ERROR_LED);
  while(1);
}





static size_t mystrlen( const char *str ) 
{
  // This is to avoid loading strlen.o
  // Supposedly, this is the bsd implementation...  really...  I heard it from
  // a guy on the internet.
  const char *s;
  for (s=str; *s; ++s)
    ;
  return(s-str);
}

/* User access routine...  send a character string through the UART */
void UART_putstr(const char *s) {
  
  int len;
  
  /*
   * If the UART is already busy with a transmission, block here until it is complete.
   * Once the UART is free, turn the DMA loose transmitting the new string through
   * the UART
   */
  
  while (UartReady != SET) {}	// Wait for the UART to become available
  UartReady = RESET;		// Now mark the UART as being busy.  When the
				// transmission is complete, the callback routine
				// below will change the flag to SET
  len = mystrlen(s);
  if(HAL_UART_Transmit_DMA(&huart1, (uint8_t*)s, len) != HAL_OK) Error_Handler();
  return;
}








// void HAL_UART_MspInit(UART_HandleTypeDef *huart)
// {  
//   GPIO_InitTypeDef  GPIO_InitStruct;
  
//   /*##-1- Enable peripherals and GPIO Clocks #################################*/
// 	__GPIOC_CLK_ENABLE();	// enable dataport clock
// 	__UART4_CLK_ENABLE();	// uart clock
// 	__DMA1_CLK_ENABLE();	// dma clock
  
//   /*##-2- Configure peripheral GPIO ##########################################*/  
//   /* UART TX GPIO pin configuration  */
//   GPIO_InitStruct.Pin       = TXPIN;
//   GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
//   GPIO_InitStruct.Pull      = GPIO_NOPULL;
//   GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
//   GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
  
//   HAL_GPIO_Init(DATAPORT, &GPIO_InitStruct);
    
//   /* UART RX GPIO pin configuration  */
//   GPIO_InitStruct.Pin = RXPIN;
    
//   HAL_GPIO_Init(DATAPORT, &GPIO_InitStruct);
// }











/**
  * @brief UART MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - DMA configuration for transmission request by peripheral 
  *           - NVIC configuration for DMA interrupt request enable
  * @param huart: UART handle pointer
  * @retval None
  */
// void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
//   static DMA_HandleTypeDef hdma_tx;
//   static DMA_HandleTypeDef hdma_rx;
  
//   GPIO_InitTypeDef  GPIO_InitStruct;
  
//   /*##-1- Enable peripherals and GPIO Clocks #################################*/
//   // /* Enable GPIO TX/RX clock */
//   // USARTx_TX_GPIO_CLK_ENABLE();
//   // USARTx_RX_GPIO_CLK_ENABLE();
//   // /* Enable USART1 clock */
//   // USARTx_CLK_ENABLE(); 
//   // /* Enable DMA1 clock */
//   // DMAx_CLK_ENABLE();   

// 	__GPIOC_CLK_ENABLE();	// enable dataport clock
// 	__UART4_CLK_ENABLE();	// uart clock
// 	__DMA1_CLK_ENABLE();	// dma clock
  
//   /*##-2- Configure peripheral GPIO ##########################################*/  
//   /* UART TX GPIO pin configuration  */
//   GPIO_InitStruct.Pin       = TXPIN;	// pc10
//   GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
//   GPIO_InitStruct.Pull      = GPIO_NOPULL;
//   GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
//   GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
  
//   HAL_GPIO_Init(DATAPORT, &GPIO_InitStruct);
    
//   /* UART RX GPIO pin configuration  */
//   GPIO_InitStruct.Pin = RXPIN;	// pc11
//   GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
    
//   HAL_GPIO_Init(DATAPORT, &GPIO_InitStruct);
    
//   /*##-3- Configure the DMA streams ##########################################*/
//   /* Configure the DMA handler for Transmission process */
//   hdma_tx.Instance                 = DMA1_Stream4;
  
//   hdma_tx.Init.Channel             = DMA_CHANNEL_4;
//   hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
//   hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
//   hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
//   hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//   hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
//   hdma_tx.Init.Mode                = DMA_NORMAL;
//   hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
//   hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
//   hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
//   hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;
//   hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;
  
//   HAL_DMA_Init(&hdma_tx);   
  
//   /* Associate the initialized DMA handle to the the UART handle */
//   __HAL_LINKDMA(huart, hdmatx, hdma_tx);
    
//   /* Configure the DMA handler for Receive process */
//   hdma_rx.Instance                 = DMA1_Stream2;
  
//   hdma_rx.Init.Channel             = DMA_CHANNEL_4;
//   hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
//   hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
//   hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
//   hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//   hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
//   hdma_rx.Init.Mode                = DMA_CIRCULAR;
//   hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
//   hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;         
//   hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
//   hdma_rx.Init.MemBurst            = DMA_MBURST_INC4;
//   hdma_rx.Init.PeriphBurst         = DMA_PBURST_INC4; 

//   HAL_DMA_Init(&hdma_rx);
    
//   /* Associate the initialized DMA handle to the the UART handle */
//   __HAL_LINKDMA(huart, hdmarx, hdma_rx);
    
//   /*##-4- Configure the NVIC for DMA #########################################*/
//   /* NVIC configuration for DMA transfer complete interrupt (TX) */
//   HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 1);
//   HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
    
//   /* NVIC configuration for DMA transfer complete interrupt (RX) */
//   HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 0, 0);   
//   HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
  
//   /* NVIC configuration for USART TC interrupt */
//   HAL_NVIC_SetPriority(UART4_IRQn, 0, 0);
//   HAL_NVIC_EnableIRQ(UART4_IRQn);
// }


























// test with hummels uart transmit ---------------------------------------------------
// static UART_HandleTypeDef UartHandle;
// static __IO ITStatus UartReady = RESET;
// static DMA_HandleTypeDef hdma_tx;


// void init_uart(void) 
// { 
//   /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
//   /* UART4 configured as follow:
//       - Word Length = 8 Bits
//       - Stop Bit = One Stop bit
//       - Parity = None
//       - BaudRate = 9600 baud
//       - Hardware flow control disabled (RTS and CTS signals) 
//       - Transmit only mode */
//   huart1.Instance        = UART4;
//   huart1.Init.BaudRate   = 9600;
//   huart1.Init.WordLength = UART_WORDLENGTH_8B;
//   huart1.Init.StopBits   = UART_STOPBITS_1;
//   huart1.Init.Parity     = UART_PARITY_NONE;
//   huart1.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
//   huart1.Init.Mode       = UART_MODE_TX_RX;


//   /* 
//    * Note that HAL_UART_Init() calls HAL_UART_MspInit() (The MPU-specific initialization
//    * routine).  This function must be re-defined  below in order to associate 
//    * the correct DMA stream with the UART.  Similarly, HAL_UART_DeInit() calls
//    * HAL_UART_MspDeInit()... which we're on the hook to define.
//    */
//   // HAL_UART_DeInit(&UartHandle);
//   if(HAL_UART_Init(&huart1) != HAL_OK);
//   UartReady = SET; 
// }

// static size_t mystrlen( const char *str ) 
// {
//   // This is to avoid loading strlen.o
//   // Supposedly, this is the bsd implementation...  really...  I heard it from
//   // a guy on the internet.
//   const char *s;
//   for (s=str; *s; ++s)
//     ;
//   return(s-str);
// }

// /* User access routine...  send a character string through the UART */
// void UART_putstr(const char *s) {
  
//   // int len;
  
//   // /*
//   //  * If the UART is already busy with a transmission, block here until it is complete.
//   //  * Once the UART is free, turn the DMA loose transmitting the new string through
//   //  * the UART
//   //  */
  
//   // while (UartReady != SET) {}	// Wait for the UART to become available
//   // UartReady = RESET;		// Now mark the UART as being busy.  When the
// 		// 		// transmission is complete, the callback routine
// 		// 		// below will change the flag to SET
//   // len = mystrlen(s);
//   // if(HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)s, len)!= HAL_OK);
//   // return;

//   // 	BSP_LED_Toggle(NORMAL_LED);
//   // 	HAL_Delay(300);
//   // 	BSP_LED_Toggle(NORMAL_LED);
 
// }

// void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
// {
//   // Error_Handler(); 
// }

/* UART  Tx Transfer completed callback
  * @param  UartHandle: UART handle. 
  * @retval None
  * 
  * Getting here is confusing...  
  * The DMA IRQ handler below calls  HAL_DMA_IRQHandler() for the UART Tx DMA
  * HAL_DMA_IRQHandler() calls UART_DMATransmitCplt() on a transmit complete.
  * UART_DMA_TransmitCplt() eventually calls HAL_UART_TxCpltCallback(), which we're
  * allowed to re-define.
  */
// void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
// {
//   /* Set transmission flag: trasfer complete*/
//   UartReady = SET; 
// }

/*
  * @brief UART MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - DMA configuration for transmission request by peripheral 
  *           - NVIC configuration for DMA interrupt request enable
  * 
  * The UART is configured for transmit only, using UART4 which is connected to 
  * GPIO Pin PC10.  We're using DMA1, Stream 4, Channel 4.
  * 
  * @param huart: UART handle pointer
  * @retval None
  */
// void HAL_UART_MspInit(UART_HandleTypeDef *huart)
// {
  
//   GPIO_InitTypeDef  GPIO_InitStruct;
  
//   /* Enable GPIO and periferal clocks.  Need PC10 as the output pin, UART4 and DMA1. */
//   __GPIOC_CLK_ENABLE();		// Need PC10 as an output pin
//   __UART4_CLK_ENABLE();
//   __DMA1_CLK_ENABLE();   
  
//   /*  UART4 TX GPIO pin configuration (PC10) */
//   GPIO_InitStruct.Pin       = GPIO_PIN_10;
//   GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
//   GPIO_InitStruct.Pull      = GPIO_NOPULL;
//   GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
//   GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
  
//   HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
        
//   /* Configure the DMA handler for Transmission process (DMA1, Stream 4, Channel 4) */
//   hdma_tx.Instance                 = DMA1_Stream4;
  
//   hdma_tx.Init.Channel             = DMA_CHANNEL_4;
//   hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
//   hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
//   hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
//   hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//   hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
//   hdma_tx.Init.Mode                = DMA_NORMAL;
//   hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
//   hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
//   hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
//   hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;
//   hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;
  
//   HAL_DMA_Init(&hdma_tx);   
  
//   /* Associate the initialized DMA handle to the the UART handle */
//   __HAL_LINKDMA(huart, hdmatx, hdma_tx);
    
    
//   /* NVIC configuration for DMA transfer complete interrupt (UART4_TX) */
//   HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 1);
//   HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

    
// }

// /**
//   * @brief UART MSP De-Initialization 
//   *        This function frees the hardware resources used in this example:
//   *          - Disable the Peripheral's clock
//   *          - Revert GPIO, DMA and NVIC configuration to their default state
//   * @param huart: UART handle pointer
//   * @retval None
//   */
// void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
// {
  

//   /* Reset peripherals  */
//   __UART4_FORCE_RESET();
//   __UART4_RELEASE_RESET();

//   HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10);
   
//   /* De-Initialize the DMA Stream associate to transmission process */
//   HAL_DMA_DeInit(&hdma_tx); 
  
//   /*##-4- Disable the NVIC for DMA ###########################################*/
//   HAL_NVIC_DisableIRQ(DMA1_Stream4_IRQn);
// }
