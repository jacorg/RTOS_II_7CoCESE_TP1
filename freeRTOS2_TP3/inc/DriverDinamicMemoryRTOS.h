
#ifndef RTOS_2_7MA_EJ_JULIAN_INC_MODULO_H_
#define RTOS_2_7MA_EJ_JULIAN_INC_MODULO_H_
#include <stdint.h>
#include "qmpool.h"


/**/
typedef long (*xQueueSendFCN)( void *, const void * const , uint32_t , const long  );

typedef long (*xQueueReceiveFCN)( void *, void * const , uint32_t);


typedef void (*MemoryFreeFCN)(void *);

typedef void* (*MemoryAllocFCN)(uint64_t);

typedef void* (*xQueueCreateFCN)(const unsigned long ,const unsigned long, const uint8_t );

typedef long (*xQueueSendFromISRFCN)( void *, const void * const , long * const , const long );


typedef void (*Add_IncommingFrameFCN)(unsigned long  ,long , volatile char );


/*Estructura Datos driver memoria Dinamica*/
typedef struct {
	char *xPointerQueue;
	uint8_t xMaxStringLength;
	xQueueSendFCN xQueueSendFunction;
	xQueueSendFromISRFCN xQueueSendFromISRFunction;
	xQueueReceiveFCN xQueueReceiveFunction;
	MemoryAllocFCN MemoryAllocFunction;
	MemoryFreeFCN MemoryFreeFunction;
	xQueueCreateFCN xQueueCreateFunction;
	Add_IncommingFrameFCN Add_IncommingFrameFunction;
}Module_Data_t;


/*Prototypes*/
void ModuleDinamicMemory_initialize( Module_Data_t *obj , uint32_t MaxLength, xQueueSendFCN xQueueSendFCN,xQueueSendFromISRFCN xQueueSendFromISRFCN, xQueueReceiveFCN xQueueReceiveFCN, xQueueCreateFCN xQueueCreateFCN, MemoryAllocFCN  MemoryAllocFCN,MemoryFreeFCN MemoryFreeFCN,Add_IncommingFrameFCN Add_IncommingFrameFCN);

void ModuleDinamicMemory_send( Module_Data_t *obj ,uint8_t Isr,long * const xHigherPriorityTaskWoken, char* pbuf , char * XpointerQueue,uint32_t portMaxDelay);
void ModuleDinamicMemory_send2( Module_Data_t *obj ,void *PcStringToSend, uint8_t Isr, long * const xHigherPriorityTaskWoken, void* pbuf ,char * XpointerQueue, uint32_t portMaxDelay);
void  ModuleDinamicMemory_receive(Module_Data_t *obj ,void * XpointerQueue, void * Frame_parameters,  uint32_t portMaxDelay);
void ModuleDinamicMemory_Free(Module_Data_t *obj , char *ultimo_mensaje);


// Wrapper 1 - 04.6.2019
void* Memory_get(QMPool * const me, uint_fast16_t const margin, uint8_t interrupcion, uint64_t tamanioomalloc, uint8_t QmpoolOrMalloc);
void  Memory_free(QMPool *const me, void *b, uint8_t interrupcion, void *ptrfreemalloc, uint8_t QmpoolOrMalloc); // Wrapper 2 - 04.6.2019

void QMPools_inicializar(void); // Inicializa los QMPools..-


#endif /* RTOS_2_7MA_EJ_JULIAN_INC_MODULO_H_ */
