/*
 * Main File
 *
 * Created on: May 24, 2019
 * TP1 a TP3 Grupo 1 RTOS2
 *      Author: julian,jacobo,rafael,gustavo
 */

#include "Task1.h"
#include "FreeRTOSConfig.h"
#include "General.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include "semphr.h"
#include "sapi_uart.h"

/*==================[TP3]=========================*/
#include "FrameworkEventos.h"
#include "pulsadores.h"
#include "leds.h"
#include "TiempoPulsacion.h"

/*==================[TP3]=========================*/
Modulo_t * ModuloBroadcast;
Modulo_t * ModuloDriverPulsadores;
Modulo_t * ModuloLed;
Modulo_t * ModuloTiempoPulsacion;


// float lala = cyclesCounterToUs(*DWT_CYCCNT);
// uint32_t temp1 =0;

int main(void){

	boardConfig();

	QmPoolOrMalloc = eUseMalloc ;//eUseQMPool;
	// temp1 = configTOTAL_HEAP_SIZE;

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
	// Manejador de eventos TP3 - 19.6.2019
	// Timer requiere freeRTOSConfig.h con Tickhook = 1
	queEventosBaja = xQueueCreate(15, sizeof(Evento_t));

	// Tarea Eventos despues de TaskCreateAll() 19.6.19
	// Requiere llevar configTOTAL_HEAP_SIZE de 8x1024 a 16x1024 en
	// freeRTOSconfg.h - 19.6.2019
	TaskCreateAll();
	// Creo la tarea de baja prioridad - del Framework de AC -2012
	xTaskCreate(
	    		taskDespacharEventos,				/* Pointer to the function that implements the task. */
				"Control",							/* Text name for the task.  This is to facilitate debugging only. */
				5 * configMINIMAL_STACK_SIZE,		/* Stack depth in words. Originally 5 18.6.19 */
				(void*) queEventosBaja,				/* Parametro de la tarea */
				tskIDLE_PRIORITY+1,				    /* Prioridad (era 2 - bajo a 1 19.6 */
				NULL );								/* Task handle. */

	QMPools_inicializar();


	/*Inicializar Driver memoria dinamica*/
	ModuleDinamicMemory_initialize(&ModuleData,50,xQueueGenericSend,xQueueGenericSendFromISR, xQueueReceive,xQueueGenericCreate, Memory_Get_,Memory_Free_, Add_IncommingFrame);

	// Del TP3
    ModuloBroadcast			= RegistrarModulo(ManejadorEventosBroadcast, 			PRIORIDAD_BAJA);
    ModuloDriverPulsadores	= RegistrarModulo(DriverPulsadores, 					PRIORIDAD_BAJA);
    ModuloLed				= RegistrarModulo(DriverLeds, 							PRIORIDAD_BAJA);
    ModuloTiempoPulsacion	= RegistrarModulo(DriverTiempoPulsacion,				PRIORIDAD_BAJA);

    IniciarTodosLosModulos();
    //////////// Manejador de eventos

	/* Iniciar scheduler*/
	vTaskStartScheduler();

	while( TRUE ) {
	}

	return 0;
}
