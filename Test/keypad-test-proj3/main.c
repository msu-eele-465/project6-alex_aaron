#include <msp430.h>
#include <stdbool.h>

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
 */

// Function prototypes
void keypad_init();
void delay();
char readKeypad();

void keypad_init() {
    // LED P6.6 & P1.0
    P6DIR |=  BIT6;
    P6OUT &= ~BIT6;

    P1DIR |=  BIT0;
    P1OUT &= ~BIT0;

    //  Set row as Inputs
    P1DIR &= ~(BIT4 | BIT5 | BIT6 | BIT7);
    P1REN |= BIT4 | BIT5 | BIT6 | BIT7;
    P1OUT |= BIT4 | BIT5 | BIT6 | BIT7;

    // P5.3 as colmn 4 as output & set low
    P5DIR |= BIT3;
    P5OUT &= ~BIT3;

    // Disable digital I/O low-power default
    PM5CTL0 &= ~LOCKLPM5;
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
    keypad_init();
    int l = 0;
    char key;

    while (1) {
        key = readKeypad();

        if        (key=='A'){
            P1OUT &= ~BIT0;
            P6OUT ^= BIT6;
        } else if (key=='B') {
            P1OUT &= ~BIT0;
            P6OUT ^= BIT6;
        } else if (key=='C') {
            P1OUT &= ~BIT0;
            P6OUT ^= BIT6;
        } else if (key=='D') {
            P1OUT &= ~BIT0;
            P6OUT ^= BIT6;
        } else {
            P6OUT &= ~BIT6;
            P1OUT |= BIT0;
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