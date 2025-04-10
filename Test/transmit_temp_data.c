#include <msp430.h>

int Data_Cnt;
int Temp_Plant;
int Temp_Ambient;
char Packet[] = {0x03 /*replace with correct register address*/, 0x33 /*Byte one of ambient temp*/, 0x44 /*byte two of ambient temp*/,};

int main(void) {

    WDTCTL |= WDTHOLD;

    UCB0CTLW0 |= UCSWRST;

    UCB0CTLW0 |= UCSSEL__SMCLK;
    UCB0BRW |= 10;

    UCB0CTLW0 |= UCMODE_3;      // put in i2c mode
    UCB0CTLW0 |= UCMST;         // put in master mode
    UCB0CTLW0 |= UCTR;          // transmit mode (to start it will change throughout the project)
    UCB0I2CSA = 0xABCD;         // enter slave address
    UCB0CTLW1 |= UCASTP_2;

    UCB0TBCNT = sizeof(Packet);

    P1SEL1 &= ~BIT3;            // SCL
    P1SEL0 |= BIT3;
    P1SEL1 &= ~BIT2;
    P1SEL0 |= BIT2;             // SDA

    PM5CTL0 &= ~LOCKLPM5;

    UCB0CTLW0 &= ~UCSWRST;

    UCB0IE |= UCTXIE0;
    __enable_interrupt();

    int i;
    while(1) {
        UCB0CTLW0 |= UCTXSTT;   //start condition
        for(i=0; i<100; i++){}  // delay
    }
    return 0;
}

#pragma vector=EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void){
    if(Data_Cnt == (sizeof(Packet) - 1)){
        UCB0TXBUF = Packet[Data_Cnt];
        Data_Cnt = 0;
    } else {
        UCB0TXBUF = Packet[Data_Cnt];
        Data_Cnt++;
    }
}