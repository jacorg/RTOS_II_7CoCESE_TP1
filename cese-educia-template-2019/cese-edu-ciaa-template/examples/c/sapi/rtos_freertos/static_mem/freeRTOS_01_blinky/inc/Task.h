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

extern StackType_t myTaskStack[configMINIMAL_STACK_SIZE];
extern StackType_t myTaskStack1[configMINIMAL_STACK_SIZE];
extern StaticTask_t myTaskTCB;
extern StaticTask_t myTaskTCB1;

extern QueueHandle_t xPointerQueue;
extern QueueHandle_t SemTxUart;

void myTask_1( void* taskParmPtr );
void TaskTxUart( void* taskParmPtr );
#endif /* EXAMPLES_C_SAPI_RTOS_FREERTOS_STATIC_MEM_FREERTOS_01_BLINKY_INC_TASK_H_ */
