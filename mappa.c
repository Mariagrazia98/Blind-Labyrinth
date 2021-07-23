#include "timer/timer.h"	
#include "led/led.h"
#include "button_EXINT/button.h"
#include "lpc17xx.h"

#ifndef VARIABLES
#define VARIABLES

/**********************************************************************************
dati
**********************************************************************************/
	
unsigned const char mappa [13][15]=   {{2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
											 {0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
											 {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
											 {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
											 {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
											 {1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0},
											 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
											 {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
											 {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
											 {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
											 {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
											 {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
											 {2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2}};

uint8_t posizione[2]; 		/* posizione del robot */
uint8_t direzione;    		/* direzione del robot =>  3(LED 8) NORD - 2(LED 9) EST - 1(LED 10) SUD - 0(LED 11) OVEST */
uint8_t vittoria  = 0;    /* flag che indica la fine del gioco */
uint8_t frequenza = 0;		/* flag che indica la frequenza a cui lampeggia il led del movimento */

/**********************************************************************************
funzioni
**********************************************************************************/
int calcolaDistanza(void){
  int distanza = 0;
  int i = 0;
  uint8_t x = posizione[0];
  uint8_t y = posizione[1];
    
  if (direzione == 3 || direzione == 0){  //N-O 
    i = -1;
  } else{ //S-E avanti
    i = 1;
  }
  
  if (direzione == 3 || direzione == 1){ //N-S
    while( (x+i >= 0 && x+i < 13) && mappa[x+i][y]!= 1) {
            distanza = distanza + 1;
      x = x + i;
            if (mappa[x][y] == 2){
                return 100; //'flag' per non fare lampeggiare il led
            }
    }
  }
  
  if (direzione == 2 || direzione == 0){ //E-O
    while( (y+i >=0 && y+i < 15) && mappa[x][y+i]!= 1) {
      distanza = distanza + 1;
            y = y + i;
            if (mappa[x][y] == 2){
                return 100;
            }
    }
  }
    
    return distanza;
}
/***********************************************************************************
blinkLedDirezione() imposta il timer2 per il blink dei DIRECTION LEDs
***********************************************************************************/			
void blinkLedDirezione(){
			int distanza = calcolaDistanza();
			disable_timer(2);
			reset_timer(2);
    
		
			if(distanza>=3 && distanza<=5){ //2Hz
				init_timer(2,0x005F5E10);				/* 0.25 secondi on, 0.25 secondi off, 0.25*25*10^6 */
			}
			else if (distanza==1 || distanza==2){ //4Hz
				init_timer(2,0x002FAF08);				/* 0.125 secondi on, 0.125 secondi off, 0.125*25*10^6 */
			}
			else if (distanza==0){ //8Hz, l'ostacolo si trova nella casella successiva a quella corrente
				init_timer(2,0x0017D784);				/* 0.0625 secondi on, 0.0625 secondi off, 0.0625*25*10^6 */
			}
			if (distanza>=0 && distanza<=5){
				enable_timer(2);	
			}
			else { 														/* si trova vicino a un uscita o è lontano da un ostacolo */
				LED_On(direzione);							/* non lampeggia */
			}
}	
/***********************************************************************************
blinkLedMovimento() imposta il timer1 per il blink del led movimento 
***********************************************************************************/
void blinkLedMovimento(){ 
			int i;
			int frequenza=5;
			uint8_t x=posizione[0];
			uint8_t y=posizione[1];
	
      disable_timer(1);
      reset_timer(1);		
			
      if ( direzione==3 || direzione==0) {
					i=-1;
      }
			else {
					i=1;
			}	
			
			if ( direzione==3 || direzione==1){ /* asse verticale */
					if ( x+i>=0 && x+i<13 && mappa[x+i][y]!=1){ 
								frequenza=1;
				}
			}
			else {															/* asse orrizontale */
				if ( y+i<15 && y+i>=0 && mappa[x][y+i]!=1 ){ 
							frequenza=1;
				}
			}
			
			if(frequenza==1){
				init_timer(1, 0x00BEBC20);				/* 0.5 secondi on, 0.5 secondi off, 0.5*25*10^6	*/
			}
			else {
				init_timer(1, 0x002625A0); 				/* 0.1 secondi on, 0.1 secondi off, 0.2*25*10^6 */
			}
				enable_timer(1);
  }
/**********************************************************************************
rotazione modifica la direzione del robot
**********************************************************************************/
void rotazione(){
	LED_Off(direzione);
	if (direzione==0){
		direzione=3;
	}
	else{
		direzione=direzione-1;
	}
	LED_On(direzione);
}
/**********************************************************************************
muovi permette al robot di avanzare di una casella, se è possibile
**********************************************************************************/
void muovi(){
			int i;
			uint8_t x=posizione[0];
			uint8_t y=posizione[1];
	
			disable_timer(1);
			reset_timer(1);
			if ( direzione==3 || direzione==0) {
					i=-1;
			}
			else {
					i=1;
			}
			
			if ( direzione==3 || direzione==1){
				if ( x+i>=0 && x+i<13 && mappa[x+i][y]!=1){ 
								posizione[0]=posizione[0]+i;
				}
			}
			else {
					if ( y+i<15 && y+i>=0 && mappa[x][y+i]!=1 ){ 
							posizione[1]=posizione[1]+i;
					}
				}
			
			if(mappa[posizione[0]][posizione[1]]==2){ /* ha raggiunto una delle caselle verdi*/
					vittoria=1; 													/* flag che indica il termine del gioco */
					disable_timer(0);
					disable_timer(1);
					disable_timer(2);
					NVIC_DisableIRQ(EINT1_IRQn);					/* disable Button interrupts			 */
					NVIC_DisableIRQ(EINT2_IRQn);					/* disable Button interrupts			 */
					NVIC_DisableIRQ(TIMER0_IRQn); 				/* disable TIMER0 								 */
					NVIC_DisableIRQ(TIMER1_IRQn); 				/* disable TIMER1									 */
					NVIC_DisableIRQ(RIT_IRQn); 						/* disable RIT									   */
					
					reset_timer(2);
					init_timer(2,0x00BEBC20);							/* i led lampeggiano a 1Hz, 0.5 on, 0.5 off 0.5*25*10^6 */
					enable_timer(2);	
					return;
			}
			blinkLedMovimento();
			blinkLedDirezione();
}


	
#endif
