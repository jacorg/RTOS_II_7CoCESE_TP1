/*
 * General.h
 *
 *  Created on: May 24, 2019
 *      Author: julian
 */

#ifndef EXAMPLES_C_SAPI_RTOS_FREERTOS_DYNAMIC_MEM_FREERTOS2_01_INC_GENERAL_H_
#define EXAMPLES_C_SAPI_RTOS_FREERTOS_DYNAMIC_MEM_FREERTOS2_01_INC_GENERAL_H_

#include "DriverDinamicMemoryRTOS.h"
#include "Task1.h"


/*Almacena en el buffer de la RX ISR*/
void Add_IncommingFrame(UBaseType_t uxSavedInterruptStatus ,BaseType_t xHigherPriorityTaskWoken, volatile char c);

/*selecionar puntero a cola segun operacion*/
void* SelecQueueFromOperation(Enum_Op_t OP);

void packetToLower(uint8_t *ptrToPacketLower);

void packetToUpper(uint8_t *ptrToPacketUpper);

void PrintUartBuffMutex(char * Message,char *Buf, SemaphoreHandle_t SemMutexUart);

void PrintUartMessageMutex(char * Message, SemaphoreHandle_t SemMutexUart);

void TaskCreateAll(void);

void QueueCreateAll(void);
void semaphoreCreateAll(void);

#endif /* EXAMPLES_C_SAPI_RTOS_FREERTOS_DYNAMIC_MEM_FREERTOS2_01_INC_GENERAL_H_ */
