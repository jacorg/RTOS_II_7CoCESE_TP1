/*
 * Task1.h
 *
 *  Created on: May 19, 2019
 *      Author: Grupo 1 - RTOS2 Julian, Gustavo, Jacobo, Rafael
 *      Agregado: tiempo_boton_oprimido() 19.6.19
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

#define _SOF '{'
#define _EOF '}'
/*HexBcd to Decimal*/
#define HEXBCD_TO_DECIMAL(X) (X/10)*16 + (X%10)   /*96 ascii = 57 54 = 1001 0100 en Bcd 96 lo conviert a decimal para que de el 96 en decimal*/

/*		Medir Performance		*/
typedef enum State_med  {
	Time_LL = 0,
	Time_R,
	Time_I,
	Time_F,
	Time_S,
	Time_T
} State_perf;

static uint32_t  Id_Frame = 0;

typedef struct {

	State_perf State_Token;
	uint32_t  Id_de_paquete;
	char * 	   PayLoad;
	uint32_t  t_sof;    			/* tiempo llegada  {*/
	uint32_t  t_eof;				/* tiempo fin trama }*/
	uint32_t  t_InitConvert;		/* tiempo inicio Mayus*/
	uint32_t  t_EndConvert;			/* tiempo fin Mayus*/
	uint32_t  t_InitTx;				/* tiempo inicio Tx uart*/
	uint32_t  t_EndTx;				/* tiempo fin Tx uart*/
    uint16_t  Length_Frame;			/* tiempo llegada {*/
    uint16_t  Memory_Allocated;		/* tiempo llegada {*/
    void (*Completion_HandlerFCN)( void *T, BaseType_t * xHig );
} Token_t;

/** ======= Datos para llenar buffer local ISR======================================*/
typedef struct {
	char Buffer[106];
	uint32_t  t_sof;    			/* tiempo llegada  {*/
	uint32_t  t_eof;
    uint32_t  Id_Frame;
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
	OP3,		/*Reportar heap disponible. (RTA)*/
	OP4
}Enum_Op_t;

/** =======Parametros de la trama de  llegada ==================================*/
typedef struct {
	Enum_Op_t Operation;
	uint8_t T;
	char* BufferAux;
	Token_t *Token;
}Frame_parameters_t;



extern SemaphoreHandle_t SemTxUart;
extern SemaphoreHandle_t SemRxUart;
extern SemaphoreHandle_t SemMutexUart;
extern Module_Data_t ModuleData;
extern TaskHandle_t xTaskHandle_RxNotify ;

extern QueueHandle_t xPointerQueue_OP0,xPointerQueue_OP1, xPointerQueue_OP2,xPointerQueue_OP3,xPointerQueue_OP4 , xPointerQueue_3;

void TaskService( void* taskParmPtr );
void TaskTxUart( void* taskParmPtr );
void CallbackRx( void *noUsado );
void Transmit_UART ( void* noUsado );
void tiempo_boton_oprimido ( TickType_t contadorTick, int TECid );

void Task_ToMayusculas_OP0( void* taskParmPtr );
void Task_ToMinusculas_OP1( void* taskParmPtr );
void Task_ReportStack_OP2( void* taskParmPtr );
void Task_ReportHeap_OP3( void* taskParmPtr );
void Task_OP4( void* taskParmPtr );



#endif /* EXAMPLES_C_SAPI_RTOS_FREERTOS_STATIC_MEM_FREERTOS_01_BLINKY_INC_TASK_H_ */
