#include <msp430.h>
#include <stdbool.h>

/**
 *  PROJECT 6: Keypad Test
 *
 *  Aaron McLean & Alex Deadmond    EELE 465
 *
 *  Last Updated: 04/14/2025
 *
 *  A program that reads the real time clock via I2C
 */

int Data_In = 0;  // Reserve a byte of memory for storing the received data.

unsigned char TimeData[2];  // Array to store minutes and hours
unsigned char RXByteCtr = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;       // stop watchdog timer

    // 1. Put eUSCI_B0 into software reset
    UCB0CTLW0 |= UCSWRST;           // UCSWRST=1 for eUSCI_B0 in SW reset

    // 2. Configure eUSCI_B0
    UCB0CTLW0 |= UCSSEL_3;          // Choose BRCLK=SMCLK=1MHz
    UCB0BRW = 10;                   // Divide BRCLK by 10 for SCL=100kHz
    UCB0CTLW0 |= UCMODE_3;          // Put into I2C mode
    UCB0CTLW0 |= UCMST;             // Put into master mode
    UCB0I2CSA = 0x0068;             // Slave address = 0x68
    UCB0TBCNT = 0x01;               // Send 1 byte of data
    UCB0CTLW1 |= UCASTP_2;          // Auto STOP when UCB0TBCNT reached

    // 3. Configure Ports
    P1SEL1 &= ~BIT3;                // We want P1.3 = SCL
    P1SEL0 |= BIT3;
    P1SEL1 &= ~BIT2;                // We want P1.2 = SDA
    P1SEL0 |= BIT2;
    PM5CTL0 &= ~LOCKLPM5;           // Disable LPM

    // 4. Take eUSCI_B0 out of SW reset
    UCB0CTLW0 &= ~UCSWRST;          // UCSWRST=0 for eUSCI_B0 in SW reset

    // 5. Enable Interrupts
    UCB0IE |= UCTXIE0;              // Enable I2C Tx0 IRQ
    UCB0IE |= UCRXIE0;              // Enable I2C Rx0 IRQ
    __enable_interrupt();           // Enable Maskable IRQs

    while(1){
        // Transmit Register Address with Write Message
        UCB0CTLW0 |= UCTR;          // Put into Tx mode
        UCB0CTLW0 |= UCTXSTT;       // Generate START cond.

        while ((UCB0IFG & UCSTPIFG) == 0);  // Wait for STOP
        UCB0IFG &= ~UCSTPIFG;       // Clear STOP flag

        // Receive Data from Rx
        UCB0CTLW0 &= ~UCTR;         // Put into Rx mode
        UCB0CTLW0 |= UCTXSTT;       // Generate START cond.

        while ((UCB0IFG & UCSTPIFG) == 0);  // Wait for STOP
        UCB0IFG &= ~UCSTPIFG;       // Clear STOP flag

        __delay_cycles(50000);      // Simple delay between reads
    }

    return 0;
}

// Interrupt Service Routines

#pragma vector=EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void){
    switch(UCB0IV){
        case 0x16:                  // ID 16: RXIFG0
            Data_In = UCB0RXBUF;   // Retrieve data

            // if (RXByteCtr < 2) {
            //     TimeData[RXByteCtr++] = UCB0RXBUF;
            // }

            break;

        case 0x18:                  // ID 18: TXIFG0
            //UCB0TXBUF = 0x03;      // Send Reg Addr
            UCB0TXBUF = 0x01;  // Start reading from register 0x00 (Minutes)
            break;

        default:
            break;
    }
}
