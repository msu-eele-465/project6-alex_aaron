#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>




// LCD control pin definitions (connected to P3)
#define RS BIT0      // P3.0 - Register Select
#define EN BIT1      // P3.1 - Enable

// LCD data pins (4-bit mode) on P3
#define D4 BIT2      // P3.2 - Data line 4
#define D5 BIT3      // P3.3 - Data line 5
#define D6 BIT4      // P3.4 - Data line 6
#define D7 BIT5      // P3.5 - Data line 7

// Define LCD port and direction register (all on P3)
#define LCD_PORT     P3OUT
#define LCD_DIR      P3DIR

// Function prototypes
void delay_ms(unsigned int ms);
void pulseEnable(void);
void sendNibble(uint8_t nibble);
void sendByte(uint8_t byte, bool isData);
void lcdCommand(uint8_t cmd);
void lcdData(uint8_t data);
void lcdInit(void);
void lcdPrint(const char *str);


// Initialize the LCD in 4-bit mode
void lcdInit(void) {
    delay_ms(50);  // Wait for LCD power-up

    // Initialization sequence to switch from 8-bit to 4-bit mode
    sendNibble(0x03); delay_ms(5);
    sendNibble(0x03); delay_ms(5);
    sendNibble(0x03); delay_ms(1);
    sendNibble(0x02);           // Set 4-bit mode

    // Function Set - 4-bit, 2 line, 5x8 font
    lcdCommand(0x28); 

    // Display On, Cursor On, Blink On
    lcdCommand(0x0F); delay_ms(1); 

    // Clear Display
    lcdCommand(0x01); delay_ms(2); 

    // Entry Mode Set: Increment Cursor
    lcdCommand(0x06); delay_ms(1);
}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    // Set all LCD pins (P3.0 - P3.5) as outputs
    LCD_DIR |= RS | EN | D4 | D5 | D6 | D7;
    LCD_PORT &= ~(RS | EN | D4 | D5 | D6 | D7);  // Clear all outputs initially

    lcdInit();              // Initialize the LCD


    /*
    
    Poll Ambiant Temp

    Poll Plant Temp

    key = old_key
    Poll Keypad
    set new_key to keypad letter
    if not 'X' and new_key != old_key
        reset timer
    end if

    while (Not 'D')
    if 'A' 
        Drive Heat MOSFET
    else if 'B'
        Drive Cool MOSFET
    else if 'C'
        if AmbientTemp > Plant Temp
            Drive Cool MOSFET
        else
            Drive Heat MOSFET
        end if;
    end if
    end While

    Display Temp

    Display Ambient

    Poll Time
    Display Time

    */


    char peltMode[] = "match   ";
    char ambTemp[] = "20.0°C";
    char plantTemp[] = "70.0°C";
    char num[] = "3 ";
    char time[] = "020s  ";

    char line[] = strcat(peltMode, ambTemp, num, time, plantTemp);

    lcdPrint(line);


    // lcdPrint("Hello World!");  // Print message

    while (1);              // Infinite loop (nothing else to do)
}

// Delay in milliseconds using busy-wait loop (approximate at 1 MHz)
void delay_ms(unsigned int ms) {
    while (ms--) {
        __delay_cycles(1000);  // Delay 1 ms
    }
}

// Pulse the Enable pin to latch data/command into the LCD
void pulseEnable(void) {
    LCD_PORT |= EN;       // Set EN high
    __delay_cycles(100);  // Short delay
    LCD_PORT &= ~EN;      // Set EN low
    __delay_cycles(100);  // Small delay after pulse
}

// Send a 4-bit nibble to the LCD
void sendNibble(uint8_t nibble) {
    // Clear all data bits
    LCD_PORT &= ~(D4 | D5 | D6 | D7);

    // Set bits according to nibble value
    if (nibble & 0x01) LCD_PORT |= D4;     // Bit 0 to D4
    if (nibble & 0x02) LCD_PORT |= D5;     // Bit 1 to D5
    if (nibble & 0x04) LCD_PORT |= D6;     // Bit 2 to D6
    if (nibble & 0x08) LCD_PORT |= D7;     // Bit 3 to D7

    pulseEnable();  // Latch the nibble into the LCD
}

// Send a full byte to the LCD (high nibble first, then low)
void sendByte(uint8_t byte, bool isData) {
    if (isData)
        LCD_PORT |= RS;    // RS = 1 for data
    else
        LCD_PORT &= ~RS;   // RS = 0 for command

    sendNibble(byte >> 4); // Send upper 4 bits
    sendNibble(byte & 0x0F); // Send lower 4 bits

    delay_ms(2);  // Wait for the command/data to be processed
}

// Send command byte to LCD
void lcdCommand(uint8_t cmd) {
    sendByte(cmd, false);  // RS = 0
}

// Send data byte (character) to LCD
void lcdData(uint8_t data) {
    sendByte(data, true);  // RS = 1
}

// Print string to LCD (character by character)
void lcdPrint(const char *str) {
    while (*str) {
        lcdData(*str++);  // Send each character
    }
}


