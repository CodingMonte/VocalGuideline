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
/*			  인터럽트 서비스 루틴 함수				*/
/* ---------------------------------------------------------------------------- */
ISR(USART0_RX_vect)				// 10글자 입력 받으면 LED가 Toggle 되고
{						// count를 10으로 맞춤
  if((UCSR0A & 0x80) == 0x80)
  {
    RXD = UDR0;
    count++;
    if(count == 10)
	LED_toggle();
  }
}
/* ---------------------------------------------------------------------------- */
/*				Main 함수					*/
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
	  {					// glitch 보호 do~while문
	    glitch = TCNT1;
	  } while (glitch >= 3124);
	  ICR1 = 3214;				// CTC모드에서 Top으로 사용됨 즉, TOP이 높으면 더 주파수가 출력되므로 높음 음이 출력됨
	  Delay_ms(25);				// 16MHz/8/2/(1+3214)) = 320Hz
	  
	  do
	  {
	    glitch = TCNT1;
	  } while (glitch >= 2082);
	  ICR1 = 2082;				// 16MHz/8/2/(1+3214)) = 480Hz  영광아 그래서 높은 음 낮은 음이 반복되면서 전화기 소리가 나는거야
	  Delay_ms(25);
	}
	TCCR1B = 0x18;				// Speaker Off(클록 입력 차단 : 타이머/ 카운터 기능 정지)
	count = 0;				// count 초기화
      }
    }
}
/* ---------------------------------------------------------------------------- */
/*			  Uart, Timer1 초기화 함수				*/
/* ---------------------------------------------------------------------------- */
void USART_Init()
{
  UBRR1H = 0;	  // baudrate = 9600 bps
  UBRR1L = 103;	  // UBRR1 = 16MHz / 16 / 9600 - 1 = 약 103
  UCSR0A = 0x00;  // 0000 0000	    asynchronous normal mode
  UCSR0B = 0x90;  // 1001 1000	    interrupt enable, 8 data, 수신부만 작동
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
    - COM1A0 -> 1 OC1A를 사용한다는 뜻 compare match에서 OC1A 출력을 토글  
    - WGM11 -> 0  동작모드 CTC
    - WGM10 -> 0 
    TCCR1B
    - WGM13 -> 1  동작모드 CTC	  (결과적으로 WGM13~WGM1~0 : 1100)
    - WGM12 -> 1	    
    - CS11  -> 1   clk_io / 8
    TCCR1C

   
*/