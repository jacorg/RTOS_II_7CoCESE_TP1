/*
 * Task.c
 *
 *  Created on: May 19, 2019
 *   Author: julian,jacobo,rafael,gustavo
 *   Grupo 1
 */

#include "Task1.h"

#include "General.h"

/*Datos de Trama Recibida*/
volatile DataFrame_t Data;


/*Datos de trama para decodificar */


SemaphoreHandle_t SemTxUart,SemRxUart, SemMutexUart;  /*UartTx*/ /*UartRx*/ /*Mutex proteger Uart*/

/*instanciar Driver memoria dinamica*/
Module_Data_t ModuleData;

/*Notificación para llegada de trama*/
TaskHandle_t xTaskHandle_RxNotify = NULL;

/*Puntero para crear la cola*/
QueueHandle_t xPointerQueue_OP0, xPointerQueue_OP1, xPointerQueue_OP2, xPointerQueue_OP3,xPointerQueue_OP4,xPointerQueue_3;

/**/
TaskHandle_t xTaskHandle_MayOP0, xTaskHandle_MinOP1;

/*=================================================================================
 	 	 	 	 	 	 	 	 | Tarea  |
 =================================================================================*/

void TaskService( void* taskParmPtr ){
	while(TRUE) {
		/*Notifica que llego trama Buena*/
		xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
		Service(&ModuleData);
		gpioToggle( LEDB );
	}
}

/*=================================================================================
 	 	 	 	 	 	 	 	 | Tarea Mayusculizar |
 =================================================================================*/
void Task_ToMayusculas_OP0( void* taskParmPtr ){
	Frame_parameters_t Frame_parameters;
	while(1){
		ModuleDinamicMemory_receive(&ModuleData, xPointerQueue_OP0,&Frame_parameters, portMAX_DELAY);
		packetToUpper((Frame_parameters.BufferAux));
		// Enviar a cola de TaskTxUARt
		ModuleDinamicMemory_send2(&ModuleData,&Frame_parameters,0,NULL,NULL, xPointerQueue_3,portMAX_DELAY);
	}
}

/*=================================================================================
 	 	 	 	 	 	 	 	 | Tarea Minusculizar |
 =================================================================================*/
void Task_ToMinusculas_OP1( void* taskParmPtr ){
	Frame_parameters_t Frame_parameters;
	while(1){
		ModuleDinamicMemory_receive(&ModuleData, xPointerQueue_OP1,&Frame_parameters, portMAX_DELAY);
		packetToLower((Frame_parameters.BufferAux));
		// Enviar a cola de TaskTxUARt
		ModuleDinamicMemory_send2(&ModuleData,&Frame_parameters,0,NULL,NULL, xPointerQueue_3,portMAX_DELAY);
	}
}

/*=================================================================================
 	 	 	 	 	 	 	 	 | Tarea Reportar stack disponible |
 =================================================================================*/
void Task_ReportStack_OP2( void* taskParmPtr ){
	while(1){
		Report(&ModuleData,xPointerQueue_OP2,STACK_);
	}
}
/*=================================================================================
 	 	 	 	 	 	 	 	 | Tarea Reportar heap disponible |
 =================================================================================*/
void Task_ReportHeap_OP3( void* taskParmPtr ){
	while(1){
		Report(&ModuleData,xPointerQueue_OP3,HEAP_);
	}
}

/*=================================================================================
 	 	 	 	 	 	 	 	 | Tarea Reportar heap disponible |
 =================================================================================*/
void Task_OP4( void* taskParmPtr ){
	static Frame_parameters_t Frame_parameters;
	while(1){
		ModuleDinamicMemory_receive(&ModuleData, xPointerQueue_OP4,&Frame_parameters, portMAX_DELAY);


		Frame_parameters.Token->t_InitConvert =cyclesCounterToUs(*_DWT_CYCCNT);
		packetToUpper((Frame_parameters.Token->PayLoad));
		Frame_parameters.Token->t_EndConvert = cyclesCounterToUs(*_DWT_CYCCNT);
		// Enviar a cola de TaskTxUARt
		ModuleDinamicMemory_send2(&ModuleData,&Frame_parameters,0,NULL,NULL, xPointerQueue_3,portMAX_DELAY);
		//Frame_parameters.Token->Id_de_paquete ++;
	}
}
/*=================================================================================
 	 	 	 	 	 	 	 	 | Tarea tx |
 =================================================================================*/
void TaskTxUart( void* taskParmPtr ){
	char Txbuffer[200];
	char *PtrSOF;
	Frame_parameters_t Frame_parameters = {0 , 0, NULL,NULL};

	while(true){

		/*Recibe por la cola*/
		ModuleDinamicMemory_receive(&ModuleData, xPointerQueue_3,&Frame_parameters, portMAX_DELAY);

		gpioToggle( LED3 );
		if( uartTxReady( UART_USB ) ){
			PtrSOF = strchr((const char*)Frame_parameters.BufferAux,_SOF);
			Frame_parameters.Operation = *(PtrSOF +  OFFSET_OP)-'0';
			if(Frame_parameters.Operation != OP4){
				sprintf( Txbuffer, "%s\r\n",(Frame_parameters.BufferAux));
				uartWriteString(UART_USB,Txbuffer);
			}
			else{
				snprintf( Txbuffer,100, "\r\nPerformances:\r\n05%s ID:%d\r\nTiempos en us:\r\nTsof:%lu Teof:%lu Ticonv:%lu Teconv:%lu Tstx:%lu",(Frame_parameters.Token->PayLoad),Frame_parameters.Token->Id_de_paquete, Frame_parameters.Token->t_sof, Frame_parameters.Token->t_eof,Frame_parameters.Token->t_InitConvert,Frame_parameters.Token->t_EndConvert, (Frame_parameters.Token->t_InitTx = cyclesCounterToUs(*_DWT_CYCCNT)));
				uartWriteString(UART_USB,Txbuffer);
				memset(Txbuffer,0,sizeof(Txbuffer));
				snprintf( Txbuffer,100, " Tetx:%lu\r\n",(Frame_parameters.Token->t_EndTx = cyclesCounterToUs(*_DWT_CYCCNT)));
				uartWriteString(UART_USB,Txbuffer);
				memset(Txbuffer,0,sizeof(Txbuffer));
				snprintf( Txbuffer,72, "[TimeProcPack] : %lu\r\n[TimeProcMayus] : %lu\r\n[TxTimeProc] : %lu\r\n",(Frame_parameters.Token->t_eof),(Frame_parameters.Token->t_EndConvert -Frame_parameters.Token->t_InitConvert), (Frame_parameters.Token->t_EndTx -Frame_parameters.Token->t_InitTx ) );
				uartWriteString(UART_USB,Txbuffer);
				memset(Txbuffer,0,sizeof(Txbuffer));
				snprintf( Txbuffer,72, "[sizePack] : %d\r\n[sizemem] : %u \r\n",(strlen(Frame_parameters.BufferAux)),sizeof(Token_t) );
				uartWriteString(UART_USB,Txbuffer);
				Frame_parameters.Token->Id_de_paquete ++;

			}

		}
		ModuleData.MemoryFreeFunction((Frame_parameters.BufferAux));
		if(Frame_parameters.Operation == OP4) ModuleData.MemoryFreeFunction((Frame_parameters.Token)); //TOKEN 10.6.19
	}
}

/*=================================================================================
 	 	 	 	 	 	 	 	 | Callback IT RX |
 =================================================================================*/
void CallbackRx( void *noUsado ){

	UBaseType_t uxSavedInterruptStatus;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	volatile char c = uartRxRead( UART_USB );  /*Char received*/

	/*Funcion pertenece al driver   R6*/
	ModuleData.Add_IncommingFrameFunction(uxSavedInterruptStatus ,xHigherPriorityTaskWoken,c);

	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
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
