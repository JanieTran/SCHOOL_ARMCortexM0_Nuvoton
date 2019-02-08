//-------------------------------------------------------------------------------------------------------------
//This part provide Project Description, key functions, and other project-related information
//Project     : Lab 2 - Push Button
//Author      : Phuong Tran
//Date        : 11th April, 2018
//Board       : Nu-LB-NUC140
//MCU         : NUC140VE3CN
//Functions   : Count the button pressing event. In particular, if a user presses that push-
// button exactly 5 times, LED5 will toggle.
//-------------------------------------------------------------------------------------------------------------


#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

int main(void)
{
	//-------------------------------------------------------------------------------------------------------------
	//System initialization
	//-------------------------------------------------------------------------------------------------------------
	SYS_UnlockReg();
	
	//=============================================================
	// CLK INIT
	//=============================================================
	
	// Enable 12 MHz HXT
	CLK -> PWRCON |= 1ul << 0;
	while (!(CLK -> CLKSTATUS & 1ul << 0));
	
	// Enable 10 kHz LIRC
	CLK -> PWRCON |= 1ul << 3;
	while (!(CLK -> CLKSTATUS & 1ul << 3));
	
	// Reset CLKSEL0
	CLK -> CLKSEL0 &= ~(0x07ul << 0);
	// Set CLKDIV to 11 => HCLK : 12 / (11 + 1) = 1 MHz
	CLK -> CLKDIV &= ~(0x0Ful << 0);
	CLK -> CLKDIV = 11;
	
	//=============================================================
	// TIMER0 INIT
	//=============================================================
	
	// Select TM0 function on GPB8
	SYS -> GPB_MFP |= 1ul << 8;
	
	// Enable TMR0_S using HCLK in CLKSEL1
	CLK -> CLKSEL1 &= ~(0x07ul << 8);
	CLK -> CLKSEL1 |= 0x02ul << 8;
	
	// Enable TMR0_EN in APBCLK
	CLK -> APBCLK |= 1ul << 2;
	
	// Reset PRESCALE
	TIMER0 -> TCSR &= ~(0xFFul << 0);
	
	// Update TDR
	TIMER0 -> TCSR |= 1ul << 16;
	
	// Enable timer counter mode
	TIMER0 -> TCSR |= 1ul << 24;
	
	// Reset CRST
	TIMER0 -> TCSR |= 1ul << 26;
	
	// Periodic counting mode
	TIMER0 -> TCSR &= ~(0x03ul << 27);
	TIMER0 -> TCSR |= 0x02ul << 27;
	
	// Start counting
	TIMER0 -> TCSR |= 1ul << 30;
	
	// TEXCON CONFIG
	
	// Event trigger condition: falling edge
	TIMER0 -> TEXCON &= ~(1ul << 0);
	TIMER0 -> TEXCON &= ~(0x03ul << 1);
	
	// Enable input capture
	TIMER0 -> TEXCON |= 1ul << 3;
	
	//=============================================================
	// GPIO INIT
	//=============================================================
	
	// Enable input from GPB15 - Push Button
	PB -> PMD &= ~(0x03ul << 30);
	
	// Enable GPC12 - LED5
	PC -> PMD &= ~(0x03ul << 24);
	PC -> PMD |= 1ul << 24;
	
	SYS_LockReg();	
	//-------------------------------------------------------------------------------------------------------------
	//Main program
	//-------------------------------------------------------------------------------------------------------------
	while (1) {
		if (TIMER0 -> TDR == 5) {
			PC -> DOUT ^= 1 << 12;
			TIMER0 -> TEXCON |= 1ul << 4;
		}
	}
}
