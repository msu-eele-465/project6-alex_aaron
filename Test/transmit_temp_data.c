#include <msp430.h>
#include <string.h>

float Temp_Plant;
float Temp_Ambient;
int plant_mode;

uint8_t Packet[9];  // 1 byte for plant_mode, 4 bytes for each float

int Data_Cnt = 0;

void floatToBytes(float f, uint8_t *bytes) {
    memcpy(bytes, &f, sizeof(float));
}

int main(void) {

    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog

    // Initialize values
    Temp_Ambient = 72.4;
    Temp_Plant = 73.4;
    plant_mode = 1;

    // Convert float to byte arrays
    Packet[0] = (uint8_t)plant_mode;
    floatToBytes(Temp_Plant, &Packet[1]);
    floatToBytes(Temp_Ambient, &Packet[5]);

    // I2C Setup
    UCB0CTLW0 |= UCSWRST; // Hold eUSCI in reset
    UCB0CTLW0 |= UCSSEL__SMCLK | UCMODE_3 | UCMST | UCTR; // SMCLK, I2C master, transmit
    UCB0BRW = 10;
    UCB0I2CSA = 0x48; // <-- your actual 7-bit slave address here
    UCB0CTLW1 |= UCASTP_2; // Auto stop after UCB0TBCNT
    UCB0TBCNT = sizeof(Packet);

    // Set I2C pins
    P1SEL1 &= ~(BIT2 | BIT3);
    P1SEL0 |= BIT2 | BIT3;

    // Unlock GPIO
    PM5CTL0 &= ~LOCKLPM5;

    UCB0CTLW0 &= ~UCSWRST; // Release eUSCI from reset

    UCB0IE |= UCTXIE0;
    __enable_interrupt();

    while (1) {
        // Optional: update values here and refresh Packet contents

        // Refresh data in Packet before each transmission if dynamic
        Packet[0] = (uint8_t)plant_mode;
        floatToBytes(Temp_Plant, &Packet[1]);
        floatToBytes(Temp_Ambient, &Packet[5]);

        UCB0CTLW0 |= UCTXSTT; // Start transmission
        __delay_cycles(10000); // Simple delay (adjust as needed)
    }

    return 0;
}

// I2C Transmit ISR
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