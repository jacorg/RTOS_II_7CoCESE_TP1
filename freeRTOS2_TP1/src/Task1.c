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

	while(TRUE) {
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
			/*Envía el puntero al buffer con la trama a la cola*/
			ModuleDinamicMemory_send(&ModuleData,0,NULL,(char*)Frame_parameters.BufferAux,XPointerQueUe ,portMAX_DELAY);
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
		ModuleDinamicMemory_send(&ModuleData,0,NULL,rx, xPointerQueue_3,portMAX_DELAY);
		/*Libera memoria dinamica*/
		ModuleDinamicMemory_Free(&ModuleData, rx);
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
		ModuleDinamicMemory_send(&ModuleData,0,NULL,rx, xPointerQueue_3,portMAX_DELAY);

		/*Libera memoria dinamica*/
		ModuleDinamicMemory_Free(&ModuleData, rx);
	}
}

/*=================================================================================
 	 	 	 	 	 	 	 	 | Tarea Reportar stack disponible |
 =================================================================================*/
void Task_ReportStack_OP2( void* taskParmPtr ){
	UBaseType_t uxHighWaterMark;
	char *BSend;
	char tempStack[30],BuffA[20];
	while(1){
		BSend = ModuleDinamicMemory_receive(&ModuleData,xPointerQueue_OP2,  portMAX_DELAY);
		uxHighWaterMark = uxTaskGetStackHighWaterMark( &xTaskHandle_MayOP0);
		memset(tempStack, 0, sizeof(tempStack ) );
		sprintf(BuffA,"%u",uxHighWaterMark);
		//"{%c%u%u}"
		sprintf(tempStack, "{200}",Frame_parameters.Operation,strlen(BuffA), uxHighWaterMark );
		strncpy(BSend, tempStack, strlen(tempStack));

		// Enviar a cola de TaskTxUARt
		ModuleDinamicMemory_send(&ModuleData,0,NULL,BSend, xPointerQueue_3,portMAX_DELAY);
		/*Libera memoria dinamica*/
		ModuleDinamicMemory_Free(&ModuleData, BSend);
	}
}
/*=================================================================================
 	 	 	 	 	 	 	 	 | Tarea Reportar heap disponible |
 =================================================================================*/
void Task_ReportHeap_OP3( void* taskParmPtr ){
	char *BSend;
	char tempHeap[30],BuffA[22];
	while(1){
		BSend = ModuleDinamicMemory_receive(&ModuleData,xPointerQueue_OP3,  portMAX_DELAY);
		/** Decodificar OP */
		Frame_parameters.Operation = *(BSend +  OFFSET_OP)-'0';

		memset(tempHeap, 0, sizeof(tempHeap) );
		sprintf(BuffA,"%u",xPortGetFreeHeapSize());
		//{%c%u%s}"
		sprintf(tempHeap, "{300}", Frame_parameters.Operation,strlen(BuffA), xPortGetFreeHeapSize() );
		strncpy(BSend, tempHeap, strlen(tempHeap));
		// Enviar a cola de TaskTxUARt
		ModuleDinamicMemory_send(&ModuleData,0,NULL,BSend, xPointerQueue_3,portMAX_DELAY);
		/*Libera memoria dinamica*/
		ModuleDinamicMemory_Free(&ModuleData, BSend);
	}
}

/*=================================================================================
 	 	 	 	 	 	 	 	 | Tarea tx |
 =================================================================================*/
void TaskTxUart( void* taskParmPtr ){
	char * BSend;
	char Txbuffer[100];
	static uint16_t cont=0;
	while(true){

		/*Recibe por la cola*/
		BSend = ModuleDinamicMemory_receive(&ModuleData, xPointerQueue_3, portMAX_DELAY);
		cont++;
		gpioToggle( LED3 );
		if( uartTxReady( UART_USB ) ){
			sprintf( Txbuffer, "%s",BSend);
			//Transmit_UART( 0 );   // La primera vez – con esto arranca
			uartWriteString(UART_USB,Txbuffer);
		}
		if(cont >= 100){

			cont = 0;
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
