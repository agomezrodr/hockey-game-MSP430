// Adrian Gomez Rodriguez
// Computer Architecture 1
// T-Th 7:30-8:50
// Prof. Dr. Freudenthal
// TA. Daniel Cervantes 

#include "libTimer.h"
#include "buzzer.h"
#include <msp430.h>

static int count = 0; //count to check the notes

void buzzer_init(){
  timerAUpmode(); 
  P2SEL2 &= ~(BIT6 | BIT7);
  P2SEL &= ~BIT7;
  P2SEL |= BIT6;
  P2DIR = BIT6; //Speaker
}

// switch statement to create the music.
void Song(){
  switch(count){
  case 0: buzzer_set_period(300); 
          count++; 
          break; 
  case 1:
  case 2:
  case 3: buzzer_set_period(1020);
          count++; 
          break;
  case 4:
  case 5:
  case 6: buzzer_set_period(700); 
          if(count==14){
              count =0;
        } else{
            count++;
        } 
          break;
  case 7:
  case 8: 
  case 9: buzzer_set_period(3800);
           count++; 
           break;
  case 10:
  case 11:
  case 12: buzzer_set_period(2000); 
           count++; 
           break; 
  }
}

void buzzer_set_period(short cycles){
  CCR0 = cycles;
  CCR1 = cycles >> 1; //one half cycle
}
