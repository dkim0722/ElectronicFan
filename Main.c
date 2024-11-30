#include <xc.h>
#include <p18f4620.h>
#include <string.h>

#include "ST7735_TFT.h"
#include "Fan_PWM.h"

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF

#define FAN_EN PORTDbits.RD2
#define FANON_LED PORTCbits.RC6

extern char Saved_Fan_Speed_Mode;
extern char Fan_Speed_Mode;
extern char Preset_Flag;
extern char PortA_Sample;

char System_Power;
char TFT_Fan_Powered_Text[] = "       ";
char TFT_Fan_Mode_Text[] = "Mode:  ";
char TFT_Saved_Fan_Mode_Text[] = "Saved:  ";
char TFT_Fan_RPM[] = "RPM:     ";
int rpm;

void Update_TFT_Screen(void)
{
    System_Power = (Fan_Speed_Mode == 0 ? 0 : 1);      // System Power indicates whether Fan is on or off
    if(!System_Power){                                 // Fan Off
        strcpy(TFT_Fan_Powered_Text, "Fan Off");
        drawtext(_width * 0.15, _height * 0.25, TFT_Fan_Powered_Text, ST7735_WHITE, ST7735_BLACK, 2);
        FAN_EN = 0;        // If fan is off, make sure that transistor is off by applying 0 V to base
        FANON_LED = 0;
    }
    else{                                              // Fan On
        strcpy(TFT_Fan_Powered_Text, "Fan On ");
        drawtext(_width * 0.15, _height * 0.25, TFT_Fan_Powered_Text, ST7735_YELLOW, ST7735_BLACK, 2);
        Update_PWM_DC(PWM_DIV * Fan_Speed_Mode);
        FAN_EN = 1;                                    // Turn on transistor
        FANON_LED = 1;
    }
    PORTE = 0x07 & Fan_Speed_Mode;                              // Output Fan Mode to RGB LED
    TFT_Fan_Mode_Text[5] = Fan_Speed_Mode + '0';                // Mode, RPM, and Saved Mode Text
    drawtext(_width * 0.15, _height * 0.375, TFT_Fan_Mode_Text, ST7735_WHITE, ST7735_BLACK, 2);
    TFT_Saved_Fan_Mode_Text[6] = Saved_Fan_Speed_Mode + '0';   
    drawtext(_width * 0.15, _height * 0.5, TFT_Saved_Fan_Mode_Text, ST7735_WHITE, ST7735_BLACK, 2);
    rpm = Get_RPM();                  // implicit type conversion magic
    TFT_Fan_RPM[4] = rpm/1000 + '0';  // If digit = 0, then set character to '0' otherwise set character to 'digit'
    TFT_Fan_RPM[5] = (rpm/100)%10 + '0';
    TFT_Fan_RPM[6] = (rpm/10)%10  + '0';
    TFT_Fan_RPM[7] = rpm%10 + '0';
    drawtext(_width * 0.15, _height * 0.625, TFT_Fan_RPM, ST7735_WHITE, ST7735_BLACK, 2);
}

int main(void){
    OSCCON = 0x70;  // 8 MHz internal clock
    TRISA = 0x07;   // 3 Dip SW inputs for fan mode
    TRISB = 0x06;   // INT2 and INT1 for preset mode and save mode respectively
    TRISC = 0x01;   // TACH input for measuring RPM
    TRISD = 0x00;   
    TRISE = 0x00;
    ADCON1 = 0x0F;   // All digital I/O
    
    LCD_Reset();     // Initialize the TFT module
    TFT_GreenTab_Initialize();
    fillScreen(ST7735_BLACK);
    
    nRBPU = 0;
    Init_Interrupt();
    TMR3L = 0x00;                   
    T3CON = 0x03;           // Uses Timer 3 for measuring RPM
    
    while(1){
        if(Preset_Flag){                        // If the preset flag is active
            if(PortA_Sample != (PORTA & 0x07))  // Don't clear the preset flag until the mode is changed
                Preset_Flag = 0;                // And don't update fan speed mode
        }
        else{
            Fan_Speed_Mode = PORTA & 0x07;      // Else update fan speed mode as usual
        }
        Update_TFT_Screen();
    }
}
