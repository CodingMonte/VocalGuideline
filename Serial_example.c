/* ============================================================================ */
/*		  Exp01_3.c : Basic C Program (1) with header file		*/
/* ============================================================================ */
/*			    Designed and programmed by Duck-Yong Yoon in 2020.  */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "OK-128TFT-V2.h"

void USART_Init();
void Timer_Init();
volatile unsigned char RXD, count = 0;

/* ---------------------------------------------------------------------------- */
/*			  ���ͷ�Ʈ ���� ��ƾ �Լ�				*/
/* ---------------------------------------------------------------------------- */
ISR(USART0_RX_vect)				// 10���� �Է� ������ LED�� Toggle �ǰ�
{						// count�� 10���� ����
  if((UCSR0A & 0x80) == 0x80)
  {
    RXD = UDR0;
    count++;
    if(count == 10)
	LED_toggle();
  }
}
/* ---------------------------------------------------------------------------- */
/*				Main �Լ�					*/
/* ---------------------------------------------------------------------------- */
int main(void)
{
  unsigned char i;
  unsigned int glitch;
  
  MCU_initialize();                             // initialize ATmega128A MCU

  USART_Init();					// initializing Uart0
  Timer_Init();					// initializing Timer1
  sei();					// global interrupt Enable
  
  while(1)
    { 
      if(count >= 10)
      {
	for(i = 0; i < 20; i++)
	{
	  TCCR1B = 0x1A;			// Speaker On
	  do 
	  {					// glitch ��ȣ do~while��
	    glitch = TCNT1;
	  } while (glitch >= 3124);
	  ICR1 = 3214;				// CTC��忡�� Top���� ���� ��, TOP�� ������ �� ���ļ��� ��µǹǷ� ���� ���� ��µ�
	  Delay_ms(25);				// 16MHz/8/2/(1+3214)) = 320Hz
	  
	  do
	  {
	    glitch = TCNT1;
	  } while (glitch >= 2082);
	  ICR1 = 2082;				// 16MHz/8/2/(1+3214)) = 480Hz  ������ �׷��� ���� �� ���� ���� �ݺ��Ǹ鼭 ��ȭ�� �Ҹ��� ���°ž�
	  Delay_ms(25);
	}
	TCCR1B = 0x18;				// Speaker Off(Ŭ�� �Է� ���� : Ÿ�̸�/ ī���� ��� ����)
	count = 0;				// count �ʱ�ȭ
      }
    }
}
/* ---------------------------------------------------------------------------- */
/*			  Uart, Timer1 �ʱ�ȭ �Լ�				*/
/* ---------------------------------------------------------------------------- */
void USART_Init()
{
  UBRR1H = 0;	  // baudrate = 9600 bps
  UBRR1L = 103;	  // UBRR1 = 16MHz / 16 / 9600 - 1 = �� 103
  UCSR0A = 0x00;  // 0000 0000	    asynchronous normal mode
  UCSR0B = 0x90;  // 1001 1000	    interrupt enable, 8 data, ���źθ� �۵�
  UCSR0C = 0x06;  // 0000 0110	    no parity, 1stop, 8 data
  RXD = UDR0;	  // dummy read
}
void Timer_Init()
{
  sbi(DDRB,5);	  // PortB set		  OC1A output
  TCCR1A = 0x40;  // 0100 0000		  CTC mode(12), use OC1A
  TCCR1B = 0x1A;  // 0001 1010		  f(OC1A) = 16Mhz/8/2/(1+TOP)
  TCCR1C = 0x00;  // 0000 0000
  TCNT1 = 0x0000; // 0000 0000 0000 0000  // clear Timer1
}
/*  TCCR1A 
    - COM1A0 -> 1 OC1A�� ����Ѵٴ� �� compare match���� OC1A ����� ���  
    - WGM11 -> 0  ���۸�� CTC
    - WGM10 -> 0 
    TCCR1B
    - WGM13 -> 1  ���۸�� CTC	  (��������� WGM13~WGM1~0 : 1100)
    - WGM12 -> 1	    
    - CS11  -> 1   clk_io / 8
    TCCR1C

   
*/