









typedef struct receive_struct {
	
	uint8_t rx_buffer;					// store the byte/char that we just recieved 
	uint8_t rx_string[MAXSTRING];		// 4 byte string of chars built from rx_buffer
	int rx_index;						// index for going through rx_string
	GPIO_InitTypeDef GPIO_InitStruct;
	UART_HandleTypeDef huart1;
	DMA_HandleTypeDef hdma_usart1_rx;

} RX_T;

void init_rx(void);

void init_gpio(RX_T * R);

void init_uart(RX_T * R);

void init_dma(RX_T * R);

void DMA2_Stream2_IRQHandler(RX_T * R);

void HAL_UART_RxCpltCallback(RX_T * R);