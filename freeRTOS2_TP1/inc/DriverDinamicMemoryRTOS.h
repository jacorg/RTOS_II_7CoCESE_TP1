
#ifndef RTOS_2_7MA_EJ_JULIAN_INC_MODULO_H_
#define RTOS_2_7MA_EJ_JULIAN_INC_MODULO_H_
#include <stdint.h>

/**/
typedef long (*xQueueSendFCN)( void *, const void * const , uint32_t , const long  );

typedef long (*xQueueReceiveFCN)( void *, void * const , uint32_t);

typedef void (*vPortFreeFCN)(void *);

typedef void* (*pvPortMallocFCN)(uint64_t);

typedef void* (*xQueueCreateFCN)(const unsigned long ,const unsigned long, const uint8_t );

typedef long (*xQueueSendFromISRFCN)( void *, const void * const , long * const , const long );

/*Estructura Datos driver memoria Dinamica*/
typedef struct {
    char *xPointerQueue;
    uint8_t xMaxStringLength;
    xQueueSendFCN xQueueSendFunction;
    xQueueSendFromISRFCN xQueueSendFromISRFunction;
    xQueueReceiveFCN xQueueReceiveFunction;
    pvPortMallocFCN pvPortMallocFunction;
    vPortFreeFCN vPortFreeFunction;
    xQueueCreateFCN xQueueCreateFunction;
}Module_Data_t;


/*Prototypes*/
void ModuleDinamicMemory_initialize( Module_Data_t *obj , uint32_t MaxLength, xQueueSendFCN xQueueSendFCN,xQueueSendFromISRFCN xQueueSendFromISRFCN, xQueueReceiveFCN xQueueReceiveFCN, xQueueCreateFCN xQueueCreateFCN, pvPortMallocFCN pvPortMallocFCN,vPortFreeFCN vPortFreeFCN);

void ModuleDinamicMemory_send( Module_Data_t *obj ,uint8_t Isr,long * const xHigherPriorityTaskWoken, char* pbuf , char * XpointerQueue,uint32_t portMaxDelay);

char* ModuleDinamicMemory_receive(Module_Data_t *obj, char * XpointerQueue ,uint32_t portMaxDelay);

void ModuleDinamicMemory_Free(Module_Data_t *obj , char *ultimo_mensaje);

#endif /* RTOS_2_7MA_EJ_JULIAN_INC_MODULO_H_ */
