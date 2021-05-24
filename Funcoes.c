/*
 * Funcoes.c
 *
 * Created: 23/05/2021 23:24:41
 *  Author: pedro
 */ 
#include "Funcoes.h"


void respirador(uint8_t volume_respiracao)
{
	static uint8_t inspirando = 1, posicao = 0;
	
	if (inspirando)
	{
		OCR1A += 250;
		if (posicao < volume_respiracao-1) posicao++;
		else inspirando = 0;
	}
	else
	{
		OCR1A -= 250;
		if (posicao > 0) posicao--;
		else inspirando = 1;
	}
	
	if (OCR1A == 2000)
	{
		PORTD |= 0b10000000;
	}
	else
	{
		PORTD &= 0b01111111;
	}
}
void Sinaisvitais(uint16_t batimentos, uint16_t temperatura_corporal, uint16_t saturacao_O2, char pressao[7])
{
	char s_batimentos[3], s_temperatura_corporal[4], s_saturacao_O2[4];
	
	nokia_lcd_clear();
	
	
	nokia_lcd_set_cursor(0,0);
	nokia_lcd_write_string("SINAIS VITAIS",1);
	nokia_lcd_set_cursor(0,2);
	

	itoa(batimentos, s_batimentos, 10);
	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string(s_batimentos,1);
	nokia_lcd_set_cursor(40,10);
	nokia_lcd_write_string("bpm",1);

	itoa(temperatura_corporal, s_temperatura_corporal, 10);
	nokia_lcd_set_cursor(0,20);
	nokia_lcd_write_string(s_temperatura_corporal,1);
	nokia_lcd_set_cursor(40,20);
	nokia_lcd_write_string("°C",1);
	
	itoa(saturacao_O2, s_saturacao_O2, 10);
	nokia_lcd_set_cursor(0,30);
	nokia_lcd_write_string(s_saturacao_O2,1);
	nokia_lcd_set_cursor(40,30);
	nokia_lcd_write_string("%SpO2",1);
	
	
	nokia_lcd_set_cursor(0,40);
	nokia_lcd_write_string(pressao,1);
	nokia_lcd_set_cursor(45,40);
	nokia_lcd_write_string("mmHg",1);
	
	nokia_lcd_render();
}
void parametros(char sel, uint16_t frequencia, uint16_t valvula, uint8_t volume_respiracao)
{
	char s_frequencia[3], s_valvula[4], s_volume_respiracao[2];
	
	nokia_lcd_clear();
	
	
	nokia_lcd_set_cursor(0,0);
	nokia_lcd_write_string("PARAMETROS",1);
	nokia_lcd_set_cursor(0,2);
	
	itoa(frequencia, s_frequencia, 10);
	nokia_lcd_set_cursor(0,15);
	nokia_lcd_write_string(s_frequencia,1);
	nokia_lcd_set_cursor(40,15);
	nokia_lcd_write_string("rsp/min",1);
	
	
	itoa(valvula, s_valvula, 10);
	nokia_lcd_set_cursor(0,25);
	nokia_lcd_write_string(s_valvula,1);
	nokia_lcd_set_cursor(40,25);
	nokia_lcd_write_string("%O2",1);
	
	
	itoa(volume_respiracao, s_volume_respiracao, 10);
	nokia_lcd_set_cursor(0,35);
	nokia_lcd_write_string(s_volume_respiracao,1);
	nokia_lcd_set_cursor(40,35);
	nokia_lcd_write_string("vol",1);
	
	if (sel == 'r')
	{
		nokia_lcd_set_cursor(30,15);
	}
	if (sel == 'v')
	{
		nokia_lcd_set_cursor(30,25);
	}
	if (sel == 'i')
	{
		nokia_lcd_set_cursor(30,35);
	}
	nokia_lcd_write_string("*",1);
	
	nokia_lcd_render();
}
