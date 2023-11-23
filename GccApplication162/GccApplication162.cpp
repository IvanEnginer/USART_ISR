#define F_CPU 8000000UL
#define BAUD 9600
#define UBRR_VAL F_CPU/16/BAUD-1

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>

void usart_init(unsigned int speed)
{
	UBRRH=(unsigned char)(speed>>8);
	UBRRL=(unsigned char) speed;
	UCSRA=0x00;
	UCSRB|=(1<<RXEN)|(1<<TXEN);
	UCSRB|=(1<<RXCIE);
	UCSRC=(1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
}

void InitPort(void)
{
	UDR = 'b';
	_delay_ms(1);
	UDR = 'd';
}

char Data;
bool IN_0_SW_ON = false;
bool OUT_0_SW_ON = false;
bool InterruptCounter = false;

bool To_Interface = false; 
bool InterfaceCounter = false;

ISR(USART_RXC_vect)
{
	Data = UDR;
	switch(Data)
	{
		case 'a':
		IN_0_SW_ON = true;
		InterruptCounter = true;
		break;
		case 'b':
		OUT_0_SW_ON = true;
		InterruptCounter = true;		
		break;
		case 'S':
		To_Interface = true;
		InterfaceCounter = true;
		break;
	}
}

int main(void)
{
	DDRA = 0xFF;
	DDRB = 0xFF;
	DDRC = 0x00;
	PORTA |= (1<<PA0);
	PORTB |= (1<<PB0);

	usart_init(UBRR_VAL);
	InitPort();
	sei();	
    while(1)
    {
		_delay_ms(1);
		if (InterfaceCounter)
		{
			DDRA = 0xFF;
			DDRB = 0xFF;
			DDRC = 0x00;
			PORTA |= (1<<PA0);
			PORTB |= (1<<PB0);
			PORTB &= ~(1<<PB1);
			bool IN_0_SW_ON = false;
			bool OUT_0_SW_ON = false;
			bool InterruptCounter = false;					
			InitPort();				
			InterfaceCounter = false;	
		}				
		_delay_ms(1);
		if (InterruptCounter)
		{
			if (IN_0_SW_ON)
			{
				PORTA &= ~(1<<PA0);
				PORTA |= (1<<PA1);				
				PORTB &= ~(1<<PB0);
				_delay_ms(1);				
				if (bit_is_clear(PINC, PINC0))
				{
					UDR = 'a';					
				}
				_delay_ms(1);
				if (bit_is_set(PINC, PINC1))
				{
					PORTB |= (1<<PB1);
					UDR = 'c';					
				}
				IN_0_SW_ON = false;
			}
			if (OUT_0_SW_ON)
			{
				PORTB |= (1<<PB0);
				PORTB &= ~(1<<PB1);
				PORTA |= (1<<PA0);
				PORTA &= ~(1<<PA1);
				_delay_ms(1);				
				if (bit_is_set(PINC, PINC0))
				{
					UDR = 'b';
				}
				_delay_ms(1);
				if (bit_is_clear(PINC, PINC1))
				{
					PORTB &= ~(1<<PB1);
					UDR = 'd';
				}
										
				OUT_0_SW_ON = false;
			}
			InterruptCounter = false;
		}
    }
}