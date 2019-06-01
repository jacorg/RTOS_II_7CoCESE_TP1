/*
 * Task.h
 *
 *  Created on: May 19, 2019
 *      Author: julian
 */

#ifndef EXAMPLES_C_SAPI_RTOS_FREERTOS_STATIC_MEM_FREERTOS_01_BLINKY_INC_TASK_H_
#define EXAMPLES_C_SAPI_RTOS_FREERTOS_STATIC_MEM_FREERTOS_01_BLINKY_INC_TASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include "queue.h"
#include "semphr.h"
#include "string.h"
#include "stdlib.h"
#include "DriverDinamicMemoryRTOS.h"

/*offset apartir del SOF*/
#define OFFSET_OP 		 1
#define OFFSET_TAMANO	 2
#define OFFSET_DATO	  	 4
#define MIN_LOWER		65
#define MAX_LOWER		90
#define MIN_UPPER		97
#define MAX_UPPER		122
#define UP_LW_LW_UP		32

#define NUM_ELEMENTOS_REST_FRAME 6  /* SOF OP T(2) EOF Y \0 */

/*HexBcd to Decimal*/
#define HEXBCD_TO_DECIMAL(X) (X/10)*16 + (X%10)   /*96 ascii = 57 54 = 1001 0100 en Bcd 96 lo conviert a decimal para que de el 96 en decimal*/

/** ======= Datos para llenar buffer local======================================*/
typedef struct {
	char Buffer[106];
	uint8_t Ready;
	uint8_t Index;
	uint8_t StartFrame;
}DataFrame_t;
extern volatile DataFrame_t Data;

/** =======Operaciones a usar===================================================*/
typedef enum{
	OP0 = 0,  	/* Convertir los caracteres recibidos a mayúsculas. (CMD/RTA)*/
	OP1,  		/*Convertir los caracteres recibidos a minúsculas. (CMD/RTA)*/
	OP2,		/*Reportar stack disponible (RTA)*/
	OP3			/*Reportar heap disponible. (RTA)*/
}Enum_Op_t;

/** =======Parametros de la trama de  llegada ==================================*/
typedef struct {
	char _SOF;
	Enum_Op_t Operation;
	uint8_t T[2];
	char* Datos;
	char* BufferAux;
	char _EOF;
}Frame_parameters_t;

extern volatile Frame_parameters_t Frame_parameters;

extern SemaphoreHandle_t SemTxUart;
extern SemaphoreHandle_t SemRxUart;
extern SemaphoreHandle_t SemMutexUart;
extern Module_Data_t ModuleData;
extern TaskHandle_t xTaskHandle_RxNotify ;

extern QueueHandle_t xPointerQueue_OP0;
extern QueueHandle_t xPointerQueue_OP1;
extern QueueHandle_t xPointerQueue_OP2;
extern QueueHandle_t xPointerQueue_OP3;
extern QueueHandle_t xPointerQueue_3;

void TaskService( void* taskParmPtr );
void TaskTxUart( void* taskParmPtr );
void CallbackRx( void *noUsado );
void Transmit_UART ( void* noUsado );

void Task_ToMayusculas_OP0( void* taskParmPtr );
void Task_ToMinusculas_OP1( void* taskParmPtr );
void Task_ReportStack_OP2( void* taskParmPtr );
void Task_ReportHeap_OP3( void* taskParmPtr );




#endif /* EXAMPLES_C_SAPI_RTOS_FREERTOS_STATIC_MEM_FREERTOS_01_BLINKY_INC_TASK_H_ */
