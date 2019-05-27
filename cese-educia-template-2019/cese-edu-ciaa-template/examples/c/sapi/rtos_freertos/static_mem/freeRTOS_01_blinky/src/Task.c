/*
 * Task.c
 *
 *  Created on: May 19, 2019
 *      Author: julian
 */


#include "Task.h"

StackType_t myTaskStack[configMINIMAL_STACK_SIZE];
StackType_t myTaskStack1[configMINIMAL_STACK_SIZE];
StaticTask_t myTaskTCB;
StaticTask_t myTaskTCB1;

QueueHandle_t xPointerQueue;
//ueueHandle_t SemTxUart;
/*=================================================================================*/

void myTask_1( void* taskParmPtr )
{
	char * PcStringToSend;
	char x[50];
	const size_t xMaxStringLength = 50 ; /*max number characters*/
	BaseType_t xStringNumber=0;
	BaseType_t xcopy;
   printf( "myTask\r\n" );

   gpioWrite( LED1, ON );
   // Envia la tarea al estado bloqueado durante 1 s (delay)
   vTaskDelay( 1000 / portTICK_RATE_MS );
   gpioWrite( LED1, OFF );

   // Tarea periodica cada 500 ms
   portTickType xPeriodicity =  500 / portTICK_RATE_MS;
   portTickType xLastWakeTime = xTaskGetTickCount();

   while(TRUE) {

	  //PcStringToSend = pvPortMalloc(xMaxStringLength);
	  snprintf(x,xMaxStringLength,"lalala %d\r\n",xStringNumber); /*lleno buffer a enviar con una variable*/
      gpioToggle( LEDB );
      printf( "Blink!\r\n" );

      xQueueSend(xPointerQueue,&x,portMAX_DELAY);
     // xSemaphoreGive(SemTxUart);
      // Envia la tarea al estado bloqueado durante xPeriodicity (delay periodico)
      vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
     // vPortFree(PcStringToSend);
   }
}

/*Task receive==================================================================================*/
void TaskTxUart( void* taskParmPtr ){
char rx[50];
printf( "TaskTxUart\r\n" );
	while(true){
		printf( "si\r\n" );
		//if( pdTRUE == xSemaphoreTake(SemTxUart,portMAX_DELAY) )
		{
			if( pdTRUE == xQueueReceive(xPointerQueue,rx,1000))
			{
				printf( "rx %s\r\n",rx );
			}
			printf( "no\r\n" );
		}
	}
}
