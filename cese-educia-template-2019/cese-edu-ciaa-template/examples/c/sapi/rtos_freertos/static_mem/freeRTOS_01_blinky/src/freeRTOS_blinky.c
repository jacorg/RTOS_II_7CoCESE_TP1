#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include "queue.h"
#include "Task.h"



typedef char Stringtx[32];  /*Tipo de Dato para La cola */
/*Task send string*/
/*frertos*/


int main(void)
{
   boardConfig();



   printf( "init\r\n" );
   xTaskCreateStatic( myTask, "myTask", configMINIMAL_STACK_SIZE, NULL,
                     tskIDLE_PRIORITY+1, myTaskStack, &myTaskTCB);

  // SemTxUart = vSemaphoreCreateBinary();

   /*Tarea para transmitir por la Uart*/
   xTaskCreateStatic(TaskTxUart, (const char *)"TaskTxUart",configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1,  myTaskStack1, &myTaskTCB1);
   //xPointerQueue = xQueueCreate(1 , sizeof(char *)); /*cola punteros tipo char*/
   xPointerQueue = xQueueCreate(1 , sizeof(Stringtx));
   vTaskStartScheduler();

   while(1);

   return 0;
}
