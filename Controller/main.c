#include <msp430.h>
#include <stdbool.h>
#include <math.h>

/**
 *  PROJECT 6: Keypad Test
 *
 *  Aaron McLean & Alex Deadmond    EELE 465
 *
 *  Last Updated: 02/20/2025
 *
 *  A program that takes inputs from a membrain
 *  keypad and detemines a locked or unlocked state,
 *  and outputs state and patterns on an LED strip.
 *  The program also drives the heating and cooling
 *  of a peltier device using two different temperature
 *  inputs.
 */

volatile unsigned int mode = 0;

// Function prototypes
void keypad_init();
void delay();
char readKeypad();

void fillLeft(void) {
    static unsigned int position = 0;
    
    P2OUT = (1 << position) - 1; // Turn on LEDs up to current position
    position++;
    
    if (position > 8) {
        position = 0;
        P2OUT = 0; // Reset all LEDs
    }
}

void fillRight(void) {
    static unsigned int position = 0;
    
    P2OUT = 0xFF << (8 - position); // Turn on LEDs from the right
    position++;
    
    if (position > 8) {
        position = 0;
        P2OUT = 0; // Reset all LEDs
    }
}

void keypad_init() {

    //  Set row as Inputs
    P1DIR &= ~(BIT4 | BIT5 | BIT6 | BIT7);
    P1REN |= BIT4 | BIT5 | BIT6 | BIT7;
    P1OUT |= BIT4 | BIT5 | BIT6 | BIT7;

    // P5.3 as colmn 4 as output & set low
    P5DIR |= BIT3;
    P5OUT &= ~BIT3;


}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
    keypad_init();

    // LED Bar Init
    P2DIR |= BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7;
    P2OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7);

    // Configure Timer A0 for periodic interrupts
    TA0CTL = TASSEL__ACLK | MC__UP | TACLR; // ACLK, up mode
    TA0CCR0 = 32768 / 4; // ~4Hz with ACLK
    TA0CCTL0 = CCIE; // Enable CCR0 interrupt
    
    __enable_interrupt();

    int l = 0;
    char key;

    // Disable digital I/O low-power default
    PM5CTL0 &= ~LOCKLPM5;

    while (1) {
        key = readKeypad();

        if        (key=='A'){
            mode = 2;
        } else if (key=='B') {
            mode = 1;
        } else if (key=='C') {
            mode = 3;
        } else if (key=='D') {
            mode = 3;
        } else {
            mode = 3;
        }
        delay();
    }
}

void delay() {
  volatile unsigned int i;
  for (i = 0xFFFF; i > 0; i--)
    ;
}

char readKeypad() {
  if        (!(P1IN & BIT4)) { // 'A'
    return 'A';
  } else if (!(P1IN & BIT5)) { // 'B'
    return 'B';
  } else if (!(P1IN & BIT6)) { // 'C'
    return 'C';
  } else if (!(P1IN & BIT7)) { // 'D'
    return 'D';
  } else {
    return 'x';
  }
}

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0_ISR(void) {
    switch(mode) {
        case 1: // Fill from left
            fillLeft();
            break;
        case 2: // Fill from right
            fillRight();
            break;
        case 3: // All off
            P2OUT = 0;
            break;
        default: // Off or invalid mode
            P2OUT = 0;
            break;
    }
}