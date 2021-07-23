#include "button.h"
#include "lpc17xx.h"
#include "../led/led.h" 					/* do not needed anymore, make your project clean */
#include "../RIT/RIT.h"		  			/* you now need RIT library 			 */
#include "../timer/timer.h"	
#include "../mappa.h"

extern uint8_t posizione[2];	
extern char mappa [13][15];
extern uint8_t  direzione;
uint8_t GPIO1=0;									/* flag che consentono di eseguire correttamente l'azione nel RIT_IRQHandler */
uint8_t GPIO2=0;


/**********************************************************************************
Handler del tasto INT0, avvio del gioco
***********************************************************************************/
void EINT0_IRQHandler (void){   	
   
  NVIC_ClearPendingIRQ(EINT1_IRQn);        /* cancello le evenutuali interruzioni pendenti */
  NVIC_ClearPendingIRQ(EINT2_IRQn);  
  
  NVIC_EnableIRQ(EINT1_IRQn);              /* abilito irq in nvic e imposto le priorità   */
  NVIC_SetPriority(EINT1_IRQn,1);         
  NVIC_EnableIRQ(EINT2_IRQn);              
  NVIC_SetPriority(EINT2_IRQn,1);         
  
  posizione[0] = 7;                         /* posizione iniziale 											  */
  posizione[1] = 7;
  direzione = 2;                            /* direzione EST															*/

  blinkLedDirezione();
  
  NVIC_DisableIRQ(EINT0_IRQn);              /* disable Button interrupts       */
  LPC_SC->EXTINT &= (1 << 1);               /* clear pending interrupt         */
  LPC_SC->EXTINT &= (1 << 2);               /* clear pending interrupt         */    
  LPC_SC->EXTINT &= (1 << 0);               /* clear pending interrupt         */
      
}

/***********************************************************************************
Handler del tasto EINT1, rotazione del robot
***********************************************************************************/
void EINT1_IRQHandler (void)	  	
{	
	GPIO1 = 1;																
	enable_RIT();															/* enable RIT to count 50ms				  */ 
	LPC_PINCON->PINSEL4    &= ~(1 << 22);     /* GPIO pin selection 							*/
	NVIC_DisableIRQ(EINT1_IRQn);							/* disable Button interrupts				*/
  LPC_SC->EXTINT &= (1 << 1);     
}
/***********************************************************************************
Handler del tasto EINT1, movimento del robot
***********************************************************************************/
void EINT2_IRQHandler (void)	  	
{
	GPIO2 = 1;
	enable_RIT();                   				  /* enable RIT to count 50ms					*/
	NVIC_DisableIRQ(EINT2_IRQn); 							// disable Button interrupts				*/
	LPC_PINCON->PINSEL4    &= ~(1 << 24);			// GPIO pin selection    						*/								 
	LPC_SC->EXTINT &= (1 << 2);    
}


