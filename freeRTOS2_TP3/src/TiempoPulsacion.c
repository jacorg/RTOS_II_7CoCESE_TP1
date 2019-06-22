/*
 * TiempoPulsacion.c
 *
 * Created on: june 19, 2019
 * TP3 Grupo 1 RTOS2
 *      Author: julian,jacobo,rafael,gustavo
 */

#include "sapi.h"
#include "FrameworkEventos.h"
#include "TiempoPulsacion.h"

#include "Task1.h"

typedef struct PulsadorStruct
{
	int idPulsador;
	int tiempoInicial;
	int tiempoFinal;
} Pulsador;

#define MAX_PULSADORES 4

Pulsador vectorTpoPulsadores[MAX_PULSADORES];

void TpoPulsadorInit ( Pulsador * p, int _idPulsador )
{
	p->idPulsador	 	= _idPulsador;
	p->tiempoInicial 	= 0;
	p->tiempoFinal		= 0;
}

void TpoPulsadoresInit ( Modulo_t * pModulo )
{
	TpoPulsadorInit( &vectorTpoPulsadores[0], 1 );
	TpoPulsadorInit( &vectorTpoPulsadores[1], 2 );
	TpoPulsadorInit( &vectorTpoPulsadores[2], 3 );
	TpoPulsadorInit( &vectorTpoPulsadores[3], 4 );
}

static Modulo_t * mod;

void DriverTiempoPulsacion ( Evento_t *evn )
{
	//int i;
	static TickType_t contadorTick = 0;

	switch( evn->signal )
	{
		case SIG_MODULO_INICIAR:
			mod = (Modulo_t *) evn->receptor;
			TpoPulsadoresInit(mod);
			timerArmarUnico(mod, mod->periodo);
			break;

		case SIG_BOTON_APRETADO:
			contadorTick = xTaskGetTickCount();
			break;

		case SIG_BOTON_LIBERADO:
			contadorTick = xTaskGetTickCount() - contadorTick;
			tiempo_boton_oprimido( contadorTick, evn->valor );
			break;

		default:	// Ignoro todas las otras seniales
			break;
	}
}
