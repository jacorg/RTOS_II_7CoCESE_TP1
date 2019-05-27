
// Includes de FreeRTOS
#include "../../freeRTOS2_01/inc/FreeRTOSConfig.h"
#include "../../freeRTOS2_01/inc/General.h"
#include "../../freeRTOS2_01/inc/Task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include "semphr.h"
#include "sapi_uart.h"




int main(void){

	boardConfig();
	/*=======Config Uart===============================*/
	uartConfig(UART_USB, 115200);
	/*Callback interrupt*/
	uartCallbackSet(UART_USB, UART_RECEIVE, CallbackRx, NULL);
	/*Habilito todas las interrupciones de UART_USB*/
	uartInterrupt(UART_USB, true);

	semaphoreCreateAll();
	QueueCreateAll();
	TaskCreateAll();

	/*Inicializar Driver memoria dinamica*/
	ModuleDinamicMemory_initialize(&ModuleData,50,xQueueGenericSend,xQueueGenericSendFromISR, xQueueReceive,xQueueGenericCreate, pvPortMalloc, vPortFree);

	/* Iniciar scheduler*/
	vTaskStartScheduler();

	while( TRUE ) {
	}

	return 0;
}
