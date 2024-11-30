#include <p18f4620.h>

#include "Interrupt.h"
#include "Fan_PWM.h"

extern char Saved_Fan_Speed_Mode;
extern char Fan_Speed_Mode;
char Preset_Flag = 0;
char PortA_Sample;

void Init_Interrupt(void){
    INTCON2bits.INTEDG1 = 0;    // falling edge interrupts
    INTCON2bits.INTEDG2 = 0;
    
    INTCON3bits.INT1IF = 0;     // clear the interrupt flags
    INTCON3bits.INT2IF = 0;
    
    INTCON3bits.INT1IE = 1;     // enable INT1 and INT2
    INTCON3bits.INT2IE = 1;  
    INTCONbits.GIE = 1;         // enable global high priority interrupts
}

void interrupt high_priority INTX_isr(void){
    if(INTCON3bits.INT1IF == 1) INT1_isr();
    if(INTCON3bits.INT2IF == 1) INT2_isr();
}

// SAVE MODE:
// Captures current fan mode

void INT1_isr(void){
    INTCON3bits.INT1IF = 0;
    Saved_Fan_Speed_Mode = Fan_Speed_Mode;
    Beep();
}

// PRESET MODE:
// Preset flag indicates that the saved mode can be loaded only once without
// Changing the current mode of the mode switches.

void INT2_isr(void){
    INTCON3bits.INT2IF = 0;
    if(!Preset_Flag){                 // Only apply preset once until fan mode is changed
        Preset_Flag = 1;
        PortA_Sample = PORTA & 0x07;  // Used to determine whether input mode has been changed
        Fan_Speed_Mode = Saved_Fan_Speed_Mode;
        Beep();
    }
}