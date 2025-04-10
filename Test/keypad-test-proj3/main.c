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
void readKeypad();
void handleButtonPress(int button);

int main(void) {
  WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
  keypad_init();
  int l = 0;

  while (1) {
    readKeypad();
    if (!(P1IN & BIT7)) {
        l++;
        if (l <= 3) {
        P6OUT &= ~BIT6;
    } else {
        P6OUT |= BIT6;
        l = 0;
    }
    delay();
    }    
  }
}

void keypad_init() {
  // LED P6.6 as an output for lock
  P6DIR |= BIT6;
  P6OUT &= ~BIT6;

  //  as Outputs (col)
  P1DIR |= BIT0 | BIT1 | BIT2 | BIT3;

  // P1.4 - P1.7 as inputs (row) with pull-up resistor
  P1DIR &= ~(BIT4 | BIT5 | BIT6 | BIT7);
  P1REN |= BIT4 | BIT5 | BIT6 | BIT7;
  P1OUT |= BIT4 | BIT5 | BIT6 | BIT7;

  // Port 3 as an output
  P3DIR = 0xFF;
  P3OUT = 0x00;

  // Disable digital I/O low-power default
  PM5CTL0 &= ~LOCKLPM5;
}

void delay() {
  volatile unsigned int i;
  for (i = 0xFFFF; i > 0; i--)
    ;
}

void readKeypad() {
  // Pulling first column high while the rest low.
  P1OUT &= ~BIT0;
  P1OUT |= BIT1;
  P1OUT |= BIT2;
  P1OUT |= BIT3;

  if (!(P1IN & BIT7)) { // Checking if first button is pressed
    handleButtonPress(1);
  }
  if (!(P1IN & BIT6)) { // Checking if second button is pressed
    handleButtonPress(2);
  }
  if (!(P1IN & BIT5)) { // Checking if third button is pressed
    handleButtonPress(3);
  }
  if (!(P1IN & BIT4)) { // Checking if fourth button is pressed
    handleButtonPress(4);
  }

  // Pulling second column high while the rest low.
  P1OUT |= BIT0;
  P1OUT &= ~BIT1;
  P1OUT |= BIT2;
  P1OUT |= BIT3;

  if (!(P1IN & BIT7)) { // Checking if first button is pressed
    handleButtonPress(5);
  }
  if (!(P1IN & BIT6)) { // Checking if second button is pressed
    handleButtonPress(6);
  }
  if (!(P1IN & BIT5)) { // Checking if third button is pressed
    handleButtonPress(7);

  }
  if (!(P1IN & BIT4)) { // Checking if fourth button is pressed
    handleButtonPress(8);
  }

  // Pulling third column high while the rest low.
  P1OUT |= BIT0;
  P1OUT |= BIT1;
  P1OUT &= ~BIT2;
  P1OUT |= BIT3;

  if (!(P1IN & BIT7)) { // Checking if first button is pressed
    handleButtonPress(9);
  }
  if (!(P1IN & BIT6)) { // Checking if second button is pressed
    handleButtonPress(10);
  }
  if (!(P1IN & BIT5)) { // Checking if third button is pressed
    handleButtonPress(11);
  }
  if (!(P1IN & BIT4)) { // Checking if fourth button is pressed
    handleButtonPress(12);
  }

  // Pulling fourth column high while the rest low.
  P1OUT |= BIT0;
  P1OUT |= BIT1;
  P1OUT |= BIT2;
  P1OUT &= ~BIT3;

  if (!(P1IN & BIT7)) { // Checking if first button is pressed
    handleButtonPress(13);
  }
  if (!(P1IN & BIT6)) { // Checking if second button is pressed
    handleButtonPress(14);
  }
  if (!(P1IN & BIT5)) { // Checking if third button is pressed
    handleButtonPress(15);
  }
  if (!(P1IN & BIT4)) { // Checking if fourth button is pressed
    handleButtonPress(16);
  }
}

void handleButtonPress(int button) {
  int j;
  switch (button) {
  case 1: // if A is pressed
    P3OUT = 0xAA;
    break;
  case 2: // if B is pressed
    for (j = 0; j <= 255; j++) {
      P3OUT = j;
      delay();
      delay();
    }
    break;
  case 3: // if C is pressed
    P3OUT = 0xFE;
    for (j = 0; j < 8; j++) {
      delay(); delay(); delay(); delay();
      P3OUT = (P3OUT << 1) | 0x01;
    }
    P3OUT = 0xFE;
    break;
  case 4: // if D is pressed
    P3OUT = 0x18;
    delay(); delay();
    P3OUT = 0x00;
    delay();
    P3OUT = 0x24;
    delay(); delay();
    P3OUT = 0x00;
    delay();
    P3OUT = 0x42;
    delay(); delay();
    P3OUT = 0x00;
    delay();
    P3OUT = 0x81;
    delay(); delay();
    P3OUT = 0x00;
    delay();
    P3OUT = 0x42;
    delay(); delay();
    P3OUT = 0x00;
    delay();
    P3OUT = 0x24;
    delay(); delay();
    P3OUT = 0x00;
    delay();
    P3OUT = 0x18;
    break;
  case 5: // if 3 is pressed
    P3OUT = 0x03;
    break;
  case 6: // if 6 is pressed
    P3OUT = 0x06;
    break;
  case 7: // if 9 is pressed
    P3OUT = 0x09;
    break;
  case 8: // if # is pressed
    P3OUT = 0x00;
    delay(); delay();
    P3OUT = 0x73;
    delay(); delay();
    P3OUT = 0x00;
    delay(); delay();
    P3OUT = 0x6F;
    delay(); delay();
    P3OUT = 0x00;
    delay(); delay();
    P3OUT = 0x6F;
    delay(); delay();
    P3OUT = 0x00;
    delay(); delay();
    P3OUT = 0x73;
    delay(); delay();
    P3OUT = 0x00;
    break;
  case 9: // if 2 is pressed
    P3OUT = 0x02;
    break;
  case 10: // if 5 is pressed
    P3OUT = 0x05;
    break;
  case 11: // if 8 is pressed
    P3OUT = 0x08;
    break;
  case 12: // if 0 is pressed
    P3OUT = 0x00;
    break;
  case 13: // if 1 is pressed
    P3OUT = 0x01;
    break;
  case 14: // if 4 is pressed
    P3OUT = 0x04;
    break;
  case 15: // if 7 is pressed
    P3OUT = 0x07;
    break;
  case 16: // if * is pressed
    P3OUT = 0xFF;
    break;
  }
}