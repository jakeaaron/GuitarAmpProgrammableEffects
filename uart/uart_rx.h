/**
 * @file uart_rx.c
 *
 * @author Jacob Allenwood
 * @date November 29, 2015
 *
 * @brief 
 * 
 * @details
 * 
 */



// DEFINE ------------------------------------------------

#define BAUDRATE 				9600
#define TXPIN					GPIO_PIN_10
#define RXPIN 					GPIO_PIN_11
#define DATAPORT 				GPIOC
#define UART_PRIORITY         	0
#define UART_RX_SUBPRIORITY   	0
#define MAXSTRING          		4 // Biggest string the user will type is 4 bytes

// -------------------------------------------------------



typedef struct receive_struct {
	
	uint8_t * rx_buffer;
	char * rx_string;	// 4 byte buffer containing the recieve characters. \n is added at the end when using snprintf

} RX_T;


RX_T * init_rx(
	void
	);

void init_uart(void);

void HAL_UART_MspInit(UART_HandleTypeDef * huart);

void HAL_UART_MspDeInit(UART_HandleTypeDef * huart);

void DMA1_Stream2_IRQHandler(void);

void HAL_UART_TxCpltCallback(UART_HandleTypeDef * huart);

void usart_read(RX_T * R);

void UART_putstr(const char *s);


// void init_uart_hums(void);

