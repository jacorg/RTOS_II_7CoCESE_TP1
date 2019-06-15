/*
 * General.h
 *
 *  Created on: May 24, 2019
 *      Author: julian,jacobo,rafael,gustavo
 */

#ifndef EXAMPLES_C_SAPI_RTOS_FREERTOS_DYNAMIC_MEM_FREERTOS2_01_INC_GENERAL_H_
#define EXAMPLES_C_SAPI_RTOS_FREERTOS_DYNAMIC_MEM_FREERTOS2_01_INC_GENERAL_H_

#include "DriverDinamicMemoryRTOS.h"
#include "Task1.h"
#include "qmpool.h"



#define STACK_	0
#define HEAP_	1

enum mem_alloc_strat { eUseMalloc   = 00,  eUseQMPool};

extern uint8_t QmPoolOrMalloc;
extern QMPool mem_pool_big;
extern QMPool mem_pool_med;

// Tamaño chico
extern QMPool mem_pool_small;
/*Almacena en el buffer de la RX ISR*/
void Add_IncommingFrame(UBaseType_t uxSavedInterruptStatus ,BaseType_t xHigherPriorityTaskWoken, volatile char c);
#define  Memory_Get_ pvPortMalloc

#define  Memory_Free_ vPortFree
/*selecionar puntero a cola segun operacion*/
void* SelecQueueFromOperation(Enum_Op_t OP);

void packetToLower(uint8_t *ptrToPacketLower);

void packetToUpper(uint8_t *ptrToPacketUpper);

void PrintUartBuffMutex(char * Message,char *Buf, SemaphoreHandle_t SemMutexUart);

void PrintUartMessageMutex(char * Message, SemaphoreHandle_t SemMutexUart);

void TaskCreateAll(void);

void QueueCreateAll(void);

void semaphoreCreateAll(void);

char* itoa(int value, char* result, int base);

void Service(Module_Data_t *obj );

void Report( Module_Data_t *obj , char * XpointerQueue, uint8_t SelectHeapOrStack);



#endif /* EXAMPLES_C_SAPI_RTOS_FREERTOS_DYNAMIC_MEM_FREERTOS2_01_INC_GENERAL_H_ */
