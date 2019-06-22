/*
 * General.c
 *
 *  Created on: May 24, 2019
 *      Author: julian,jacobo,rafael,gustavo
 */

#include "General.h"
uint8_t QmPoolOrMalloc;

volatile uint32_t * _DWT_CTRL = (uint32_t *)0xE0001000   ;
volatile uint32_t * _DWT_CYCCNT = (uint32_t *)0xE0001004;


volatile uint32_t *_DEMCR = (uint32_t *) 0xE000EDFC;
volatile uint32_t *_LAR  = (uint32_t *)0xE0001FB0;   // <-- added lock access register


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
	case OP4:	/*Operacion 4*/
		XpointerSelected = xPointerQueue_OP4;
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
	xTaskCreate(Task_OP4, (const char *)"Task_OP4",configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY + 1, NULL);
}

/*=================================================================================
 	 	 	 	 	 	 	 	queue create
 =================================================================================*/
void QueueCreateAll(void){

	xPointerQueue_OP0	= xQueueCreate(16 , sizeof(Frame_parameters_t)); /*Create queue OP0*/
	xPointerQueue_OP1	= xQueueCreate(16 , sizeof(Frame_parameters_t)); /*Create queue OP0*/
	xPointerQueue_OP2	= xQueueCreate(16 , sizeof(Frame_parameters_t)); /*Create queue OP0*/
	xPointerQueue_OP3	= xQueueCreate(16 , sizeof(Frame_parameters_t)); /*Create queue OP0*/
	xPointerQueue_OP4	= xQueueCreate(16 , sizeof(Frame_parameters_t)); /*Create queue OP0*/
	xPointerQueue_3		= xQueueCreate(16 , sizeof(Frame_parameters_t)); /*Create queue OP0*/

}
/*=================================================================================
 	 	 	 	 	 	 	 	semaphore create
 =================================================================================*/

void semaphoreCreateAll(void){
	SemTxUart 	 =  xSemaphoreCreateBinary();
	SemMutexUart =	xSemaphoreCreateMutex() ;
	SemReady = xSemaphoreCreateBinary();
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
/*=================================================================================
 	 	 	 	 	 	 	     	Servicio
 =================================================================================*/

void Service(Module_Data_t *obj ){

	//Token_t Token;
	static Frame_parameters_t Frame_parameters  ;
	char *PtrSOF = NULL;
	char *PtrEOF = NULL;
	void* XPointerQueUe = NULL; /*Puntero auxiliar  a cola*/
	void *PcStringToSend;
	PcStringToSend = NULL;
	static uint8_t firstEntryOP = 0;
	static uint32_t IdBackup = 0;

	/*Proteger datos para hacer copia local*/
	taskENTER_CRITICAL();
	Frame_parameters.BufferAux = obj->MemoryAllocFunction(sizeof(Data.Buffer));
	strcpy((char*)Frame_parameters.BufferAux ,(const char*)Data.Buffer);
	Data.Ready = 0;
	xSemaphoreGive(SemReady);
	taskEXIT_CRITICAL();

	/*Buscar posición del inicio de la trama*/
	PtrSOF = strchr((const char*)Frame_parameters.BufferAux,_SOF);

	if( PtrSOF != NULL ){
		/** Decodificar T */
		Frame_parameters.T =  ( *(PtrSOF +  OFFSET_TAMANO)-'0' )*10 + (*(PtrSOF +  OFFSET_TAMANO + 1)-'0' ) ;
		/** Decodificar OP */
		Frame_parameters.Operation = *(PtrSOF +  OFFSET_OP)-'0';
		/* Cantidad de memoria a reservar*/
		obj->xMaxStringLength = Frame_parameters.T + NUM_ELEMENTOS_REST_FRAME;
	}
	//-----------------------------------------------------
	if(Frame_parameters.Operation == OP4){
		Frame_parameters.Token = obj->MemoryAllocFunction(sizeof(Token_t));
		taskENTER_CRITICAL();
		Frame_parameters.Token->PayLoad = Frame_parameters.BufferAux; //{402ab}
		Frame_parameters.Token->t_sof = Data.t_sof;
		Frame_parameters.Token->t_eof = Data.t_eof;
		if(!firstEntryOP){
			firstEntryOP = 1;
			Frame_parameters.Token->Id_de_paquete = (IdBackup == 0 ? IdBackup : IdBackup+1) ;
		}
		taskEXIT_CRITICAL();
		IdBackup = Frame_parameters.Token->Id_de_paquete;
	}else firstEntryOP = 0;
	/*Selecionar operaacion*/
	XPointerQueUe = SelecQueueFromOperation(Frame_parameters.Operation);
	if(XPointerQueUe != NULL){

		/*Envía el puntero al buffer con la trama a la cola-valida cantidad de datos*/
		if(*(Frame_parameters.BufferAux + OFFSET_DATO + Frame_parameters.T) == _EOF ){
			/*si cumple el protocolo*/
			ModuleDinamicMemory_send2(obj,&Frame_parameters,0,NULL,NULL,XPointerQueUe ,portMAX_DELAY);
		}else if(Frame_parameters.Operation == OP4) {
			/*Libera si no cumple elprotocolo*/
			ModuleData.MemoryFreeFunction((Frame_parameters.BufferAux));
			ModuleData.MemoryFreeFunction((Frame_parameters.Token));
		}else ModuleData.MemoryFreeFunction((Frame_parameters.BufferAux));
	}

}
/*=================================================================================
 * 	 	 	 	 	 	 	     	Report  Heap = 1 or stack = 0
 =================================================================================*/

void Report( Module_Data_t *obj , char * XpointerQueue, uint8_t SelectHeapOrStack){

	Frame_parameters_t Frame_parameters;//TOKEN 10.6.19

	uint64_t Heap_Stack;
	char BuffA[20];
	char * PcStringToSend = NULL;


	PcStringToSend = NULL;
	ModuleDinamicMemory_receive(obj,XpointerQueue, &Frame_parameters, portMAX_DELAY);


	Heap_Stack = SelectHeapOrStack ? xPortGetFreeHeapSize() : uxTaskGetStackHighWaterMark(NULL);

	itoa(Heap_Stack ,BuffA,10);

	/*Puntero donde se copia el stack*/

	if (PcStringToSend == NULL) PcStringToSend = obj->MemoryAllocFunction(strlen(BuffA)+ NUM_ELEMENTOS_REST_FRAME);

	if(PcStringToSend != NULL){

		//TOKEN 10.6.19
		sprintf(PcStringToSend+2,"%02d%s%}",strlen(BuffA),BuffA);

		*PcStringToSend = *(Frame_parameters.BufferAux);
		*(PcStringToSend + 1) = *(Frame_parameters.BufferAux + 1);
	}
	ModuleData.MemoryFreeFunction( Frame_parameters.BufferAux);
	Frame_parameters.BufferAux = PcStringToSend;
	// Enviar a cola de TaskTxUARt
	ModuleDinamicMemory_send2(obj,&Frame_parameters,0,NULL,NULL, xPointerQueue_3,portMAX_DELAY);
}


/*=================================================================================
						Almacena en el buffer de la RX ISR
=================================================================================*/
void Add_IncommingFrame(UBaseType_t uxSavedInterruptStatus ,BaseType_t xHigherPriorityTaskWoken, volatile char c){
	char *PtrSOF = NULL;
	char *PtrEOF = NULL;
	void* XPointerQueUe = NULL; /*Puntero auxiliar  a cola*/
	static uint8_t InitTimeFlag = 1;
	uint8_t T = 0;

	/*Verifica Inicio de trama*/
	if(_SOF == c) Data.StartFrame = 1;


	if(Data.StartFrame && !Data.Ready){

		if(InitTimeFlag) {
			InitTimeFlag = 0;
			taskENTER_CRITICAL_FROM_ISR();
			*_DWT_CYCCNT = 0;
			Data.t_sof = cyclesCounterToUs(*_DWT_CYCCNT); //cyclesCounterToUs
			taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
		}
		/*Proteger acceso al buffer*/
		uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
		Data.Buffer[Data.Index++]= c;
		taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
	}
	else return;

	if(Data.Index > sizeof(Data)-1) Data.Index =0;  /*Garantiza no desbordamiento del buffer*/

	Data.Buffer[Data.Index] = 0; 					/*char NULL pos siguiente*/

	if(_EOF == c  ){ // {004abcd}
		PtrSOF = strchr((const char*)Data.Buffer,_SOF);
		T =  ( *(PtrSOF +  OFFSET_TAMANO)-'0' )*10 + (*(PtrSOF +  OFFSET_TAMANO + 1)-'0' ) ;
		if( T == (Data.Index - 5) )
		//if( *(PtrSOF + OFFSET_DATO + T) == _EOF  )
		{
			InitTimeFlag = 1;
			taskENTER_CRITICAL_FROM_ISR();
			Data.t_eof = cyclesCounterToUs(*_DWT_CYCCNT);
			taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
			Data.StartFrame = 0;
			Data.Ready = 1;
			xSemaphoreTakeFromISR(SemReady,&xHigherPriorityTaskWoken);
			/*Frame buena en el buffer*/

			xTaskNotifyFromISR(xTaskHandle_RxNotify,0,eNoAction,&xHigherPriorityTaskWoken);
			Data.Index =0;
		}else {

			memset(Data.Buffer,0,sizeof(Data.Buffer));
			Data.Index =0;
		}
	}
}
