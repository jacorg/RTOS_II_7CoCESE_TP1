/*
 * General.c
 *
 *  Created on: May 24, 2019
 *      Author: julian
 */

#include "General.h"


/*=================================================================================
						Almacena en el buffer de la RX ISR
=================================================================================*/
void Add_IncommingFrame(UBaseType_t uxSavedInterruptStatus ,BaseType_t xHigherPriorityTaskWoken, volatile char c){
	char *PtrSOF = NULL;
	char *PtrEOF = NULL;
	void* XPointerQueUe = NULL; /*Puntero auxiliar  a cola*/

	/*Verifica Inicio de trama*/
	if( Frame_parameters._SOF == c) Data.StartFrame = 1;

	if(Data.StartFrame){
		/*Proteger acceso al buffer*/
		uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
		Data.Buffer[Data.Index++]= c;
		taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
	}
	else return;

	if(Data.Index > sizeof(Data)-1) Data.Index =0;  /*Garantiza no desbordamiento del buffer*/

	Data.Buffer[Data.Index] = 0; 					/*char NULL pos siguiente*/

	if(Frame_parameters._EOF == c){
		Data.StartFrame = 0;
		Data.Ready = 1;
		/*Frame buena en el buffer*/

		xTaskNotifyFromISR(xTaskHandle_RxNotify,0,eNoAction,&xHigherPriorityTaskWoken);
		Data.Index =0;
	}
}


/*=================================================================================
 	 	 	 	 	 	selecionar puntero a cola segun operacion
 =================================================================================*/
void* SelecQueueFromOperation(Enum_Op_t OP){
	void * XpointerSelected = NULL;
	switch(OP){

	case OP0:   /*Operacion 0*/
		XpointerSelected = xPointerQueue_OP0;
		break;
	case OP1:	/*Operacion 1*/
		XpointerSelected = xPointerQueue_OP1;
		break;
	case OP2:	/*Operacion 2*/
		XpointerSelected = xPointerQueue_OP2;
		break;
	case OP3:	/*Operacion 3*/
		XpointerSelected = xPointerQueue_OP3;
		break;
	}
	return XpointerSelected;
}

/*=================================================================================
 	 	 	 	 	 	 	 	 packetToLower
 =================================================================================*/
void packetToLower(uint8_t *ptrToPacketLower){

	uint16_t tSizePacket;
	uint8_t i;
	tSizePacket = ((*(ptrToPacketLower + OFFSET_TAMANO)) -'0')*10;
	tSizePacket = tSizePacket + ( (*(ptrToPacketLower+OFFSET_OP+OFFSET_TAMANO)) -'0');
	for(i = 0; i < tSizePacket ; i++){
		if( *(ptrToPacketLower + i + OFFSET_DATO) >= MIN_LOWER &&  *(ptrToPacketLower + i + OFFSET_DATO) <= MAX_LOWER)
		    *(ptrToPacketLower + i + OFFSET_DATO) = *(ptrToPacketLower + i + OFFSET_DATO) + UP_LW_LW_UP;
	}
}
/*=================================================================================
 	 	 	 	 	 	 	 	 packetToUpper
 =================================================================================*/
void packetToUpper(uint8_t *ptrToPacketUpper){
	uint16_t tSizePacket;
	uint8_t i;
	tSizePacket = ( *( ptrToPacketUpper + OFFSET_TAMANO) -'0')*10;
	tSizePacket = tSizePacket + ( *( ptrToPacketUpper + OFFSET_OP+OFFSET_TAMANO) -'0');
	for(i = 0;i < tSizePacket; i++){
		if( *(ptrToPacketUpper + i + OFFSET_DATO) >= MIN_UPPER &&  *(ptrToPacketUpper + i + OFFSET_DATO) <= MAX_UPPER)
			*(ptrToPacketUpper + i + OFFSET_DATO) = *(ptrToPacketUpper + i + OFFSET_DATO)-UP_LW_LW_UP;
	}
}
/*=================================================================================
 	 	 	 	 	 	 	 	 Print string buffer + message con mutex
 =================================================================================*/
void PrintUartBuffMutex(char * Message,char *Buf, SemaphoreHandle_t SemMutexUart){
	xSemaphoreTake(SemMutexUart,portMAX_DELAY);
	printf(Message,Buf );
	xSemaphoreGive(SemMutexUart);
}
/*=================================================================================
 	 	 	 	 	 	 	 	 Print only message con mutex
 =================================================================================*/
void PrintUartMessageMutex(char * Message, SemaphoreHandle_t SemMutexUart){
	xSemaphoreTake(SemMutexUart,portMAX_DELAY);
	printf(Message);
	xSemaphoreGive(SemMutexUart);
}
/*=================================================================================
 	 	 	 	 	 	 	 	task create
 =================================================================================*/
void TaskCreateAll(void){

	xTaskCreate(TaskTxUart, (const char *)"TaskTxUart",configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(TaskService, (const char *)"TaskService",configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY + 2, &xTaskHandle_RxNotify);
	xTaskCreate(Task_ToMayusculas_OP0, (const char *)"Task_ToMayusculas_OP0",configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(Task_ToMinusculas_OP1, (const char *)"Task_ToMinusculas_OP1",configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(Task_ReportStack_OP2, (const char *)"Task_ToMayusculas_OP2",configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(Task_ReportHeap_OP3, (const char *)"Task_ToMinusculas_OP3",configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY + 1, NULL);
}

/*=================================================================================
 	 	 	 	 	 	 	 	queue create
 =================================================================================*/
void QueueCreateAll(void){

	xPointerQueue_OP0	= xQueueCreate(16 , sizeof(char *)); /*Create queue OP0*/
	xPointerQueue_OP1	= xQueueCreate(16 , sizeof(char *)); /*Create queue OP0*/
	xPointerQueue_OP2	= xQueueCreate(16 , sizeof(char *)); /*Create queue OP0*/
	xPointerQueue_OP3	= xQueueCreate(16 , sizeof(char *)); /*Create queue OP0*/
	xPointerQueue_3		= xQueueCreate(16 , sizeof(char *)); /*Create queue OP0*/

}
/*=================================================================================
 	 	 	 	 	 	 	 	semaphore create
 =================================================================================*/

void semaphoreCreateAll(void){
	SemTxUart 	 =  xSemaphoreCreateBinary();
	SemMutexUart =	xSemaphoreCreateMutex() ;
}
/*=================================================================================
 	 	 	 	 	 	 	     	conversions
 =================================================================================*/

char* itoa(int value, char* result, int base) {
  // check that the base if valid
  if (base < 2 || base > 36) { *result = '\0'; return result; }

  char* ptr = result, *ptr1 = result, tmp_char;
  int tmp_value;

  do {
     tmp_value = value;
     value /= base;
     *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
  } while ( value );

  // Apply negative sign
  if (tmp_value < 0) *ptr++ = '-';
  *ptr-- = '\0';
  while(ptr1 < ptr) {
     tmp_char = *ptr;
     *ptr--= *ptr1;
     *ptr1++ = tmp_char;
  }
  return result;
}
