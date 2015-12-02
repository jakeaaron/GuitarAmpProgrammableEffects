





// DEFINE ------------------------------------------------

#define BAUDRATE 				9600
#define RXPIN 					GPIO_PIN_7
#define DATAPORT 				GPIOB
#define UART_PRIORITY         	6
#define UART_RX_SUBPRIORITY   	0
#define MAXSTRING          		4 // Biggest string the user will type is 4 bytes

// -------------------------------------------------------




typedef struct receive_struct {
	
	uint8_t * rx_buffer;
	char * rx_string;	// 4 byte buffer containing the recieve characters. \n is added at the end when using snprintf
	int rx_index;						// index for going through rx_string

} RX_T;


RX_T * init_rx(void);

void init_uart(void);

void HAL_UART_MspInit(UART_HandleTypeDef * huart);

void HAL_UART_MspDeInit(UART_HandleTypeDef * huart);

void DMA2_Stream2_IRQHandler(void);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart);

void usart_read(RX_T * R);