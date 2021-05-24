/*
 * Funcoes.h
 *
 * Created: 23/05/2021 23:24:59
 *  Author: pedro
 */ 


#ifndef FUNCOES_H_
#define FUNCOES_H_
#define FREQ_MIN 5
#define FREQ_MAX 30

#define VOLUME_MIN 1
#define VOLUME_MAX 8
#define CONSTANTE_RESPIRACAO 30000 
#define TEMPO_SENSORES_MS 150
#define TEMPO_ATUALIZACAO_DISPLAY_MS 200
#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include "nokia5110.h"



void respirador(uint8_t volume_respiracao);
void Sinaisvitais(uint16_t batimentos, uint16_t temperatura_corporal, uint16_t saturacao_O2, char pressao[7]);
void parametros(char sel, uint16_t frequencia, uint16_t valvula, uint8_t volume_respiracao);



#endif /* FUNCOES_H_ */