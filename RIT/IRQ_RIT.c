#include "lpc17xx.h"
#include "RIT.h"
#include "../led/led.h"
#include "../mappa.h"
#include "../timer/timer.h"

#define LEDRun 5

extern uint8_t GPIO1;
extern uint8_t GPIO2;
/******************************************************************************
 Descriptions:    REPETITIVE INTERRUPT TIMER handler
******************************************************************************/

 void RIT_IRQHandler (void)
{    
  static int down1=0;  
  static int down2=0;
  
  if (GPIO1 ==1 && (LPC_GPIO2->FIOPIN & (1<<11)) == 0){
      reset_RIT();
   }
  else if (GPIO1==1){
    reset_RIT();
     ++down1;
      if(down1==3){
        rotazione();
        blinkLedDirezione();
        down1=0;
        GPIO1 = 0;
        disable_RIT();
        reset_RIT();
        LPC_SC->EXTPOLAR = 0x0;
        LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 1 pin selection */
        NVIC_EnableIRQ(EINT1_IRQn);              /* disable Button interrupts      */
        LPC_SC->EXTPOLAR = 0x2;
      }
      
}
  
  if (GPIO2 ==1 && (LPC_GPIO2->FIOPIN & (1<<12)) == 0){
    down2++;
    reset_RIT();
    if(down2==3){
          blinkLedMovimento(); //inizio a fare lampeggiare il led del movimento
          enable_timer(0);     //conta un uno slot al secondo
    }
  }
  else if(GPIO2==1){
      reset_RIT();
      down2=0;    
      GPIO2 = 0;
      NVIC_EnableIRQ(EINT2_IRQn);               /* disable Button interrupts      */
      LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 2 pin selection*/
       disable_RIT();
      disable_timer(0);
      reset_timer(0);
      disable_timer(1);
      reset_timer(1);
      LED_Off(5);
  }

  
  LPC_RIT->RICTRL |= 0x1;  /* clear interrupt flag */
  
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/