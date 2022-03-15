#include "msp.h"
#include "../GitHubAnotherTest/SharedIncFiles/clock.h"
#include "../GitHubAnotherTest/SharedIncFiles/LaunchPad.h"
#include "../GitHubAnotherTest/SharedIncFiles/Texas.h"
#include "../GitHubAnotherTest/SharedIncFiles/SysTickInts.h"
#include "../GitHubAnotherTest/SharedIncFiles/CortexM.h"

/**
 * main.c
 */

// bit-banded addresses
#define BLUEOUT  (*((volatile uint8_t *)(0x42098068)))
#define GREENOUT (*((volatile uint8_t *)(0x42098064)))
#define REDOUT   (*((volatile uint8_t *)(0x42098060)))
#define SW1       0x02                  // on the left side of the LaunchPad board
#define SW2       0x10                  // on the right side of the LaunchPad board
#define RED       0x01
#define GREEN     0x02
#define BLUE      0x04
#define SW2IN ((*((volatile uint8_t *)(0x42098010)))^1)
#define SW1IN ((*((volatile uint8_t *)(0x42098004)))^1)

// Version 3 provides for abstraction
void Switch_Init(void){
  P1->SEL0 &= ~0x12;
  P1->SEL1 &= ~0x12;    // 1) configure P1.4 and P1.1 as GPIO
  P1->DIR &= ~0x12;     // 2) make P1.4 and P1.1 in
  P1->REN |= 0x12;      // 3) enable pull resistors on P1.4 and P1.1
  P1->OUT |= 0x12;      //    P1.4 and P1.1 are pull-up
}

void RedLED_Init(void){
  P1->SEL0 &= ~0x01;
  P1->SEL1 &= ~0x01;   // 1) configure P1.0 as GPIO
  P1->DIR |= 0x01;     // 2) make P1.0 out
}

// bit-banded address
#define REDLED (*((volatile uint8_t *)(0x42098040)))

void ColorLED_Init(void){
  P2->SEL0 &= ~0x07;
  P2->SEL1 &= ~0x07;    // 1) configure P2.2-P2.0 as GPIO
  P2->DIR |= 0x07;      // 2) make P2.2-P2.0 out
  P2->DS |= 0x07;       // 3) activate increased drive strength
  P2->OUT &= ~0x07;     //    all LEDs off
}

void SysTick_Wait1us(uint32_t delay){
    SysTick-> LOAD = (delay*48 -1); // 48 corresponds to 1 us (From 48MHz)
    SysTick->VAL = 0;
    while((SysTick->CTRL&0x00010000) == 0) {};
}

void SysTick_Handler(void) {
    if(SW1IN||SW2IN){ // Single Red on if either is pressed
          REDLED=1;
          Clock_Delay1ms(100);
          REDLED=0;
        }
}

int main(void){
  Clock_Init48MHz();  // makes bus clock 48 MHz
  Switch_Init();
  ColorLED_Init();
  RedLED_Init();
  SysTick_Init_Ints(6000000,2);
  GREENOUT = 0;
  EnableInterrupts();
  while(1){
      WaitForInterrupt();
      /*
    if(SW1IN||SW2IN){ // Single Red on if either is pressed
      REDLED=1;
    }else{
      REDLED=0;
    }
    */
    if(SW1IN){ // Color=Blue if SW1 is pressed
      BLUEOUT = 1;
    }else{
      BLUEOUT = 0;
    }
    if(SW2IN){ // Color=Red if SW2 is pressed
      REDOUT = 1;
    }else{
      REDOUT = 0;
    }
  }
}
