
/*=================================================================================
 	 	 	 	 	 	 	     	Grupo #1
 	 	 	 	 	 	 Jacobo,Gustavo, Rafael,Julian

 =================================================================================*/

#include "Task1.h"
#include "FreeRTOSConfig.h"
#include "General.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include "semphr.h"
#include "sapi_uart.h"


int main(void){

	boardConfig();

	QmPoolOrMalloc = eUseMalloc ;//eUseQMPool;

	// Timer para task_Medir_Performance
	*_DEMCR = *_DEMCR | 0x01000000;     // enable trace
	*_LAR = 0xC5ACCE55;                // <-- added unlock access to DWT (ITM, etc.)registers
	*_DWT_CTRL |= 1;


	/*=======Config Uart===============================*/
	uartConfig(UART_USB, 115200);

	/*Callback interrupt*/
	uartCallbackSet(UART_USB, UART_RECEIVE, CallbackRx, NULL);

	/*Habilito todas las interrupciones de UART_USB*/
	uartInterrupt(UART_USB, true);

	semaphoreCreateAll();
	QueueCreateAll();
	TaskCreateAll();
	QMPools_inicializar();

	/*Inicializar Driver memoria dinamica*/
	ModuleDinamicMemory_initialize(&ModuleData,50,xQueueGenericSend,xQueueGenericSendFromISR, xQueueReceive,xQueueGenericCreate, Memory_Get_,Memory_Free_, Add_IncommingFrame);

	/* Iniciar scheduler*/
	vTaskStartScheduler();

	while( TRUE ) {
	}

	return 0;
}
