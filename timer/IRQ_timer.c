#include "lpc17xx.h"
#include "timer.h"
#include "../led/led.h"
#include "../mappa.h"

#define LEDsVittoria 47	
#define LEDRun			 5		

uint8_t movimento_status=1;
uint8_t direzione_status=1;
	
extern uint8_t direzione;
extern uint8_t vittoria;
extern unsigned char led_value;					/* defined in funct_led								*/

/******************************************************************************
Timer 0 che si occupa del movimento, lancia l'interruzione dopo un secondo
******************************************************************************/

void TIMER0_IRQHandler (void)					
{
			muovi();
			LPC_TIM0->IR = 1;			/* clear interrupt flag */
			return;
}

/******************************************************************************
Timer 1 che si occupa del RUNNING LED
******************************************************************************/

void TIMER1_IRQHandler (void) 
{
			if (movimento_status == 1){
				LED_Off(LEDRun);
				movimento_status = 0;
			} else{
				LED_On(LEDRun);
				movimento_status = 1;
			}
			LPC_TIM1->IR = 1;      /* clear interrupt flag */
			return;
}
/******************************************************************************
Timer 2 che si occupa dei DIRECTION LED
******************************************************************************/
void TIMER2_IRQHandler (void){			
	
	if (vittoria == 0){
			if(direzione_status==0){
				LED_On(direzione);
				direzione_status = 1;
			}
			else {
				LED_Off(direzione);
				direzione_status = 0;
			}	
	}
	else {
		if(direzione_status==0){
				LED_Out(LEDsVittoria);
				direzione_status = 1;
			}
			else {
				LED_Out(0);
				direzione_status = 0;
			}
	}
	
  LPC_TIM2->IR = 1;      /* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
