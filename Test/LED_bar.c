#include <msp430.h>

volatile unsigned int mode = 0; // 0=off, 1=left fill, 2=right fill, 3=all off

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

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode
    
    P2DIR |= BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7;
    P2OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7)
    
    // Configure Timer A0 for periodic interrupts
    TA0CTL = TASSEL__ACLK | MC__UP | TACLR; // ACLK, up mode
    TA0CCR0 = 32768 / 4; // ~4Hz with ACLK
    TA0CCTL0 = CCIE; // Enable CCR0 interrupt
    
    __enable_interrupt();
    
    while(1) {

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