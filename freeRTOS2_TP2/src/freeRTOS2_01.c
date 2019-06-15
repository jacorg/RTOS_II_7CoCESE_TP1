

#include "Task1.h"
#include "FreeRTOSConfig.h"
#include "General.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include "semphr.h"
#include "sapi_uart.h"


//float lala = cyclesCounterToUs(*DWT_CYCCNT);

int main(void){

	boardConfig();

	QmPoolOrMalloc = eUseMalloc ;//eUseQMPool;

	// Timer para task_Medir_Performance

	if(cyclesCounterInit(EDU_CIAA_NXP_CLOCK_SPEED) ) {
		cyclesCounterReset();
		/* TODO: Inicializo ok*/
		}


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
