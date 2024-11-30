#include <p18f4620.h>

#include "Fan_PWM.h"

char Saved_Fan_Speed_Mode = 0;
char Fan_Speed_Mode;

void Update_PWM_DC(char dc) 
{ 
	float dc_f;
	int dc_I;
	PR2 = 0b00000100 ;                      // Set the PWM frequency to 25 KHz 
	T2CON = 0b00000111 ;                   
	dc_f = ( 4.0 * dc / 20.0) ;             // Calculate fractional duty cycle
	dc_I = (int) dc_f;                      // Truncate duty cycle to find whole part
	if (dc_I > dc) dc_I++;                  // Round up duty cycle
	CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100;
	CCPR1L = (dc_I) >> 2;
}

int Get_RPM(void)
{
    int RPS = TMR3L / 2;    // Two pulses per revolution 
    TMR3L = 0;              // reset TMR3L
    return RPS * 60;        // return RPM = 60 * RPS
}

void Beep(void){
    PORTBbits.RB3 = 1;      // Apply voltage to transistor base to pass current to the buzzer
    Delay();                // Produce beep
    PORTBbits.RB3 = 0;
    Update_PWM_DC(Fan_Speed_Mode * PWM_DIV);    // Refresh PWM 
}

void Delay(void)
{
    for(int k=0;k<0x7fff;k++);
}