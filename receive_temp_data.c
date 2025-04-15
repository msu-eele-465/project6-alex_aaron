#include <msp430.h>
#include <string.h>

float Temp_Plant;
float Temp_Ambient;
int plant_mode;

uint8_t Packet[9];  // Same size as master's packet
volatile int Data_Cnt = 0;
volatile uint8_t RxFlag = 0;

void bytesToFloat(uint8_t *bytes, float *f) {
    memcpy(f, bytes, sizeof(float));
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog
    
    // Initialize variables
    Temp_Ambient = 0.0;
    Temp_Plant = 0.0;
    plant_mode = 0;
    
    // I2C Setup as slave
    UCB0CTLW0 |= UCSWRST; // Hold eUSCI in reset
    UCB0CTLW0 |= UCMODE_3 | UCSYNC; // I2C slave mode, synchronous
    UCB0I2COA0 = 0x48; // Same address as master is using (7-bit)
    UCB0CTLW0 &= ~UCTR; // Receive mode
    
    // Set I2C pins
    P1SEL1 &= ~(BIT6 | BIT7); // P1.6 = SCL, P1.7 = SDA
    P1SEL0 |= BIT6 | BIT7;
    
    // Unlock GPIO
    PM5CTL0 &= ~LOCKLPM5;
    
    // Enable interrupts
    UCB0IE |= UCRXIE; // Enable receive interrupt
    UCB0CTLW0 &= ~UCSWRST; // Release eUSCI from reset
    
    __enable_interrupt();
    
    while (1) {
        if (RxFlag) {
            // New data received, process it
            RxFlag = 0;
            
            plant_mode = (int)Packet[0];
            bytesToFloat(&Packet[1], &Temp_Plant);
            bytesToFloat(&Packet[5], &Temp_Ambient);
            
            // Do something with the received data
            // For example, control outputs based on plant_mode and temperatures
        }   
        // Low power mode while waiting
        __bis_SR_register(LPM0_bits | GIE);
        __no_operation();
    }
}

// I2C Receive ISR
#pragma vector=EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void) {
    switch (UCB0IV) {
        case EUSCI_B_I2C_UCSTTIFG:
            // Start condition received
            Data_Cnt = 0;
            break;     
        case EUSCI_B_I2C_UCRXIFG0:
            // Data received
            Packet[Data_Cnt++] = UCB0RXBUF;
            if (Data_Cnt >= sizeof(Packet)) {
                RxFlag = 1;
                __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
            }
            break;    
        case EUSCI_B_I2C_UCSTPIFG:
            // Stop condition received
            break;        
        default:
            break;
    }
}