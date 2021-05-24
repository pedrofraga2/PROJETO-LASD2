/*
 * PROJETO_LASD.c
 *
 * Created: 23/05/2021 23:19:06
 * Author : pedro
 */ 


#include <avr/interrupt.h>
#include <avr/io.h>
#include "Funcoes.h"

//VARIÁVEIS

uint16_t tempo_ms = 0;
char sel = 's';

uint8_t freq_respiracao = FREQ_MIN, volume_respiracao = VOLUME_MAX, tempo_monitoramento =5000;
uint16_t tempo_respiracao, valvula = 0;


uint16_t temperatura_corporal = 0, saturacao_O2 = 0, ultimo_batimento = 0, freq_batimentos = 0, flag_monitoramento = 0, flag_5s = 0;
char pressao[7];

//INTERRUPÇÕES

ISR(TIMER0_COMPA_vect)
{
	tempo_ms++;
	if (tempo_ms%tempo_monitoramento == 0)
	flag_monitoramento = 1;
	if (tempo_ms%10000 == 0)
	Monitoramento();
	if (tempo_ms%tempo_respiracao == 0)
	respirador(volume_respiracao);
	if (tempo_ms%1000 == 0)
	flag_5s;
	if (tempo_ms%TEMPO_SENSORES_MS == 0)
	ADMUX ^= 0b00000001;
	if (tempo_ms%TEMPO_ATUALIZACAO_DISPLAY_MS == 0)
	{
		if (sel == 's')
		Sinaisvitais(freq_batimentos, temperatura_corporal, saturacao_O2, pressao);
		else
		if(sel == 'r' || sel == 'v' || sel == 'i')
		parametros(sel, freq_respiracao, valvula, volume_respiracao);
		else
		if(sel == 'k')
		Monitoramento();
	}
}

ISR(PCINT2_vect)
{
	static uint8_t subida = 1;
	
	if(PORTD = (1<<4))
	{
		if (subida)
		{
			freq_batimentos = 60000/(tempo_ms - ultimo_batimento);
			
			ultimo_batimento = tempo_ms;
			subida = 0;
		}
		else subida = 1;
	}
	
	
	if (PIND & (1<<0))
	{
		PORTB |=(1<<3);

	}
	
	

}

ISR(PCINT0_vect)
{
	static uint8_t descida = 1;
	
	if (descida)
	{
		if (sel == 's')
		{
			sel = 'r';
			parametros(sel, freq_respiracao, valvula, volume_respiracao);
		}
		else
		{
			if (sel == 'r')
			{
				sel = 'v';
				parametros(sel, freq_respiracao, valvula, volume_respiracao);
			}
			else
			if (sel == 'v')
			{
				sel = 'i';
				parametros(sel, freq_respiracao, valvula, volume_respiracao);
			}
			else
			if (sel == 'i')
			{
				sel = 's';
				Sinaisvitais(freq_batimentos, temperatura_corporal, saturacao_O2, pressao);
			}
		}
		
		descida = 0;
	}
	else descida = 1;
}

ISR(INT0_vect)
{
	if (sel == 'r')
	{
		if (freq_respiracao < FREQ_MAX)
		{
			freq_respiracao++;
			tempo_respiracao = CONSTANTE_RESPIRACAO/(volume_respiracao*freq_respiracao);
			parametros(sel, freq_respiracao, valvula, volume_respiracao);
		}
	}
	else if (sel == 'v')
	{
		if (OCR1B < 4000)
		{
			OCR1B += 200;
			valvula += 10;
			parametros(sel, freq_respiracao, valvula, volume_respiracao);
		}
	}
	else if (sel == 'i')
	{
		if (volume_respiracao < VOLUME_MAX)
		{
			volume_respiracao++;
			tempo_respiracao = CONSTANTE_RESPIRACAO/(volume_respiracao*freq_respiracao);
			parametros(sel, freq_respiracao, valvula, volume_respiracao);
		}
		
	}
	else if (sel = 'k')
	{
		sel = 's'; // CASO SEJA PRESSIONADO "SIM", O DISPLAY RETORNA À TELA INICIAL
		
	}
	

}

ISR(INT1_vect)
{
	if (sel == 'r')
	{
		if (freq_respiracao > FREQ_MIN)
		{
			freq_respiracao--;
			tempo_respiracao = CONSTANTE_RESPIRACAO/(volume_respiracao*freq_respiracao);
			Sinaisvitais(sel, freq_respiracao, valvula, volume_respiracao);
		}
	}
	else if (sel == 'v')
	{
		if (OCR1B > 2000)
		{
			OCR1B -= 200;
			valvula -= 10;
			Sinaisvitais(sel, freq_respiracao, valvula, volume_respiracao);
		}
	}
	else if (sel == 'i')
	{
		if (volume_respiracao > VOLUME_MIN)
		{
			volume_respiracao--;
			tempo_respiracao = CONSTANTE_RESPIRACAO/(volume_respiracao*freq_respiracao);
			Sinaisvitais(sel, freq_respiracao, valvula, volume_respiracao);
		}
	}
	else if (sel = 'k')
	{
		Monitoramento22();// CASO SEJA PRESSIONADO "SIM", O DISPLAY RETORNA À FUNÇÃO MONITORAMENTO22
		
	}
	
}

ISR(ADC_vect)
{
	uint16_t leitura_adc = ADC;
	
	if (ADMUX%2 == 0) //
	{
		temperatura_corporal = 0.0490196*leitura_adc + 10;
	}
	else
	{
		saturacao_O2 = 0.122249*leitura_adc;
	}
	
	if (temperatura_corporal < 35 || temperatura_corporal > 41 || saturacao_O2 < 60 || OCR1A == 2000)
	{
		PORTD |= 0b10000000;
	}
	else
	{
		PORTD &= 0b01111111;
	}
}


void USART_Init(unsigned int ubrr)
{
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (3<<UCSZ00);
}


ISR(USART_RX_vect)
{
	unsigned char recebido = UDR0;
	static char pressao_recebida[8];
	static int8_t posicao = -1, posicao_x = 0;
	
	if (posicao == -1)
	{
		if (recebido != ';')
		strcpy(pressao, "ERRO!");
		else
		{
			strcpy(pressao_recebida, "       ");
			posicao = 0;
		}
	}
	else if (recebido != ':')
	{
		if (recebido == ';')
		{
			strcpy(pressao, "ERRO!");
			strcpy(pressao_recebida, "       ");
			posicao = 0;
		}
		else
		{
			if (recebido == 'x') posicao_x = posicao;
			pressao_recebida[posicao] = recebido;
			posicao++;
		}
	}
	else
	{
		if (posicao < 3 || posicao > 7 || posicao_x == 0 || posicao_x > 3 || posicao-posicao_x < 2 || posicao-posicao_x > 4)
		strcpy(pressao, "ERRO!");
		else if (pressao_recebida[0] == '-' || pressao_recebida[posicao_x+1] == '-')
		strcpy(pressao, "ERRO!");
		else
		{
			strcpy(pressao, "       ");
			strcpy(pressao, pressao_recebida);
		}
		
		strcpy(pressao_recebida, "       ");
		posicao = -1;
	}
}


void Monitoramento()
{
	
	sel = 'k';
	
	nokia_lcd_clear();
	
	
	nokia_lcd_set_cursor(0,0);
	nokia_lcd_write_string("VOCE ESTA SE",1);
	nokia_lcd_set_cursor(0,8);
	nokia_lcd_write_string("SENTINDO BEM?",1);
	nokia_lcd_set_cursor(0,40);
	nokia_lcd_write_string("SIM",1);
	nokia_lcd_set_cursor(64,40);
	nokia_lcd_write_string("NAO",1);
	
	
	

	nokia_lcd_render();
	
	
	
	
}
void Monitoramento22()
{
	PORTD|= (1<<7); //Ativa PIND7 -> ALARME
	
	nokia_lcd_clear();
	
	
	nokia_lcd_set_cursor(0,0);
	nokia_lcd_write_string("ENFERMEIROS",1);
	nokia_lcd_set_cursor(0,8);
	nokia_lcd_write_string("A CAMINHO",1);
	nokia_lcd_set_cursor(0,40);

	nokia_lcd_render();
	
	PORTD &= (0<<7); //Desativa PIND7 -> DESLIGA ALARME

	sel = 's';

}

int main(void)
{
	
	DDRB &= 0b10111111;
	PORTB |= 0b00000000;
	DDRD &= 0b11100000;
	DDRD |= 0b00000000;
	PORTD |= 0b00001111;
	DDRC &= 0b11111100;
	PORTC &= 0b11111100;
	
	
	DDRB |= 0b00001110;
	DDRC |= 0b11111100;
	DDRD |= 0b00000000;
	
	
	TCCR0A = 0b00000010;
	TCCR0B = 0b00000011;
	OCR0A = 249;
	TIMSK0 = 0b00000010;
	
	
	ICR1 = 39999;
	TCCR1A = 0b10100010;
	TCCR1B = 0b00011010;
	OCR1A = 2000;
	OCR1B = 2000;
	
	
	EICRA = 0b00001010;
	EIMSK = 0b00000011;
	PCICR = 0b00000101;
	PCMSK2 = 0b00010011;
	PCMSK0 = 0b01000000;
	
	
	ADMUX = 0b01000000;
	ADCSRA = 0b11101111;
	ADCSRB = 0x00;
	DIDR0 = 0b00111100;
	
	USART_Init(MYUBRR);

	sei();
	
	tempo_respiracao = CONSTANTE_RESPIRACAO/(volume_respiracao*freq_respiracao);
	
	nokia_lcd_init();
	Sinaisvitais(freq_batimentos, temperatura_corporal, saturacao_O2, pressao);
	
	while (1)
	{
		
		
	}
}