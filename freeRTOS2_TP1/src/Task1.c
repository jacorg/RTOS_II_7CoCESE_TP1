/*
 * Task.c
 *
 *  Created on: May 19, 2019
 *      Author: julian
 */




#include "Task1.h"

#include "General.h"

/*Datos de Trama Recibida*/
volatile DataFrame_t Data;

/*Datos de trama para decodificar */
volatile Frame_parameters_t Frame_parameters = { '{',0 , {0,0}, NULL,NULL, '}' };

/*Semaforo Sincronizar UartTx*/
SemaphoreHandle_t SemTxUart;

/*Semaforo Sincronizar UartRx*/
SemaphoreHandle_t SemRxUart;

/*Semaforo Mutex proteger Uart*/
SemaphoreHandle_t SemMutexUart;

/*instanciar Driver memoria dinamica*/
Module_Data_t ModuleData;

/*Notificación para llegada de trama*/
TaskHandle_t xTaskHandle_RxNotify = NULL;

/*Puntero para crear la cola*/
QueueHandle_t xPointerQueue_OP0;

/*Puntero para crear la cola*/
QueueHandle_t xPointerQueue_OP1;

/*Puntero para crear la cola*/
QueueHandle_t xPointerQueue_OP2;

/*Puntero para crear la cola*/
QueueHandle_t xPointerQueue_OP3;

/*Puntero para crear la cola*/
QueueHandle_t xPointerQueue_3;

/**/
TaskHandle_t xTaskHandle_MayOP0;

/**/
TaskHandle_t xTaskHandle_MinOP1;





/*=================================================================================
 	 	 	 	 	 	 	 	 | Tarea  |
 =================================================================================*/

void TaskService( void* taskParmPtr )
{
	char *PtrSOF = NULL;
	char *PtrEOF = NULL;
	void* XPointerQueUe = NULL; /*Puntero auxiliar  a cola*/
	char *PcStringToSend;
	while(TRUE) {
		PcStringToSend = NULL;
		/*Notifica que llego trama Buena*/
		xTaskNotifyWait(0,0,NULL,portMAX_DELAY);

		/*Proteger datos para hacer copia local*/
		taskENTER_CRITICAL();
		Frame_parameters.BufferAux = ModuleData.pvPortMallocFunction(sizeof(Data.Buffer));
		strcpy((char*)Frame_parameters.BufferAux,(const char*)Data.Buffer);
		taskEXIT_CRITICAL();

		/*Buscar posición del inicio de la trama*/
		PtrSOF = strchr((const char*)Frame_parameters.BufferAux, Frame_parameters._SOF);

		if( PtrSOF != NULL ){
			/** Decodificar T :  T[0] -'0' *10 + T[1] - '0'*/
			Frame_parameters.T[0] =  ( *(PtrSOF +  OFFSET_TAMANO)-'0' )*10 + (*(PtrSOF +  OFFSET_TAMANO + 1)-'0' ) ;

			/** Decodificar OP */
			Frame_parameters.Operation = *(PtrSOF +  OFFSET_OP)-'0';

			/* Cantidad de memoria a reservar*/
			ModuleData.xMaxStringLength = Frame_parameters.T[0] + NUM_ELEMENTOS_REST_FRAME;
		}

		/*Selecionar operaacion*/
		XPointerQueUe = SelecQueueFromOperation(Frame_parameters.Operation);

		if(XPointerQueUe != NULL){
			if (PcStringToSend == NULL) PcStringToSend = ModuleData.pvPortMallocFunction( ModuleData.xMaxStringLength );
			/*Envía el puntero al buffer con la trama a la cola*/
			ModuleDinamicMemory_send2(&ModuleData,PcStringToSend,0,NULL,(char*)Frame_parameters.BufferAux,XPointerQueUe ,portMAX_DELAY);
		}
		/*Libero memoria del buffer aux*/
		ModuleData.vPortFreeFunction(Frame_parameters.BufferAux );
		gpioToggle( LEDB );
	}
}

/*=================================================================================
 	 	 	 	 	 	 	 	 | Tarea Mayusculizar |
 =================================================================================*/
void Task_ToMayusculas_OP0( void* taskParmPtr ){
	char * rx;
	while(1){

		rx = ModuleDinamicMemory_receive(&ModuleData,xPointerQueue_OP0,  portMAX_DELAY);
		packetToUpper(rx);

		// Enviar a cola de TaskTxUARt
		ModuleDinamicMemory_send2(&ModuleData,rx,0,NULL,rx, xPointerQueue_3,portMAX_DELAY);
		/*Libera memoria dinamica*/
		//ModuleDinamicMemory_Free(&ModuleData, rx);
	}
}

/*=================================================================================
 	 	 	 	 	 	 	 	 | Tarea Minusculizar |
 =================================================================================*/
void Task_ToMinusculas_OP1( void* taskParmPtr ){
	char * rx;
	while(1){

		rx = ModuleDinamicMemory_receive(&ModuleData,xPointerQueue_OP1,  portMAX_DELAY);
		packetToLower(rx);
		// Enviar a cola de TaskTxUARt
		ModuleDinamicMemory_send2(&ModuleData,rx,0,NULL,rx, xPointerQueue_3,portMAX_DELAY);
		/*Libera memoria dinamica*/
		//ModuleDinamicMemory_Free(&ModuleData, rx);
	}
}

/*=================================================================================
 	 	 	 	 	 	 	 	 | Tarea Reportar stack disponible |
 =================================================================================*/
void Task_ReportStack_OP2( void* taskParmPtr ){
	volatile UBaseType_t uxHighWaterMark;
	char *BSend , *tempStack;
	char BuffA[20];
	char * PcStringToSend = NULL;

	while(1){
		PcStringToSend = NULL;
		BSend = ModuleDinamicMemory_receive(&ModuleData,xPointerQueue_OP2,  portMAX_DELAY);
		uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);

		itoa(uxHighWaterMark,BuffA,10);
		/*Puntero donde se copia el stack*/
		if (PcStringToSend == NULL) PcStringToSend = ModuleData.pvPortMallocFunction(strlen(BuffA)+ NUM_ELEMENTOS_REST_FRAME);
		if(PcStringToSend != NULL){
			sprintf(PcStringToSend+2,"%02d%s}",strlen(BuffA),BuffA);
			*PcStringToSend = *BSend;
			*(PcStringToSend + 1) = *(BSend+1);
		}

		// Enviar a cola de TaskTxUARt
		ModuleDinamicMemory_send2(&ModuleData,PcStringToSend,0,NULL,PcStringToSend, xPointerQueue_3,portMAX_DELAY);

		/*Libera memoria dinamica {200} recibido del buffer*/
		ModuleDinamicMemory_Free(&ModuleData, BSend);
	}
}
/*=================================================================================
 	 	 	 	 	 	 	 	 | Tarea Reportar heap disponible |
 =================================================================================*/
void Task_ReportHeap_OP3( void* taskParmPtr ){

	char *BSend , *tempStack;
	char BuffA[20];
	char * PcStringToSend = NULL;

	while(1){
		PcStringToSend = NULL;
		BSend = ModuleDinamicMemory_receive(&ModuleData,xPointerQueue_OP3,  portMAX_DELAY);

		itoa(xPortGetFreeHeapSize(),BuffA,10);

		/*Puntero donde se copia el stack*/
		if (PcStringToSend == NULL) PcStringToSend = ModuleData.pvPortMallocFunction(strlen(BuffA)+ NUM_ELEMENTOS_REST_FRAME);
		if(PcStringToSend != NULL){
			sprintf(PcStringToSend+2,"%02d%s}",strlen(BuffA),BuffA);
			*PcStringToSend = *BSend;
			*(PcStringToSend + 1) = *(BSend+1);
		}

		// Enviar a cola de TaskTxUARt
		ModuleDinamicMemory_send2(&ModuleData,PcStringToSend,0,NULL,PcStringToSend, xPointerQueue_3,portMAX_DELAY);

		/*Libera memoria dinamica {300} recibido del buffer*/
		ModuleDinamicMemory_Free(&ModuleData, BSend);
	}
}

/*=================================================================================
 	 	 	 	 	 	 	 	 | Tarea tx |
 =================================================================================*/
void TaskTxUart( void* taskParmPtr ){
	char * BSend;
	char Txbuffer[100];
	while(true){

		/*Recibe por la cola*/
		BSend = ModuleDinamicMemory_receive(&ModuleData, xPointerQueue_3, portMAX_DELAY);
		gpioToggle( LED3 );
		if( uartTxReady( UART_USB ) ){
			sprintf( Txbuffer, "%s",BSend);
			//Transmit_UART( 0 );   // La primera vez – con esto arranca
			uartWriteString(UART_USB,Txbuffer);
		}
		ModuleDinamicMemory_Free(&ModuleData, BSend);
	}
}

/*=================================================================================
 	 	 	 	 	 	 	 	 | Callback IT RX |
 =================================================================================*/
void CallbackRx( void *noUsado ){

	UBaseType_t uxSavedInterruptStatus;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	volatile char c = uartRxRead( UART_USB );  /*Char received*/

	Add_IncommingFrame(uxSavedInterruptStatus ,xHigherPriorityTaskWoken,c);

	if(xHigherPriorityTaskWoken) portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
/*=================================================================================
 	 	 	 	 	 	 | Callback IT TX | - 24.5.2019
   	   	   	   	   	   readBuffer(char *buffer, char *ByteToTx);
 =================================================================================*/
void Transmit_UART ( void* noUsado ){
	static int start_detected = 0;
	char Txbyte;
	//if( readBuffer( &Txbuffer, &Txbyte ) )
	uartTxWrite( UART_USB, Txbyte );
}
