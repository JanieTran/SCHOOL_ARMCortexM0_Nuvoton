//-------------------------------------------------------------------------------------------------------------
//This part provide Project Description, key functions, and other project-related information
//Project     : Mock Test - Question 1 - LED Blinking
//Author      : Phuong Tran
//Date        : 2nd April, 2018
//Board       : Nu-LB-NUC140
//MCU         : NUC140VE3CN
//Functions   : Develop an embedded software program to turn on/off the LED7 after every 0.3 second.
// The following design specification must be followed:
// -	System Timer (SysTick) is used as the timing control module in the program
// -	SysTick clock frequency: 1 MHz
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
	
	// CLK SRC CONFIG
	//===========================================================
	// CPU clk scr - 12 MHz HXT
	CLK -> PWRCON |= 1ul << 0;
	while (!(CLK -> CLKSTATUS & 1ul << 0));
	
	// Reset CLKSEL0 and set PLLFOUT for HCLK
	CLK -> CLKSEL0 &= ~(0x07ul << 0);
	CLK -> CLKSEL0 |= 0x02 << 0;
	// Reset CLKDIV
	CLK -> CLKDIV &= ~(0x0Ful << 0);
	//===========================================================
	
	// PLL CONFIG
	//===========================================================
	// PLL_SRC: 0 => 12 MHz HXT
	CLK -> PLLCON &= ~(1ul << 19);
	// Enable PLL clk out
	CLK -> PLLCON &= ~(1ul << 18);
	// PLL normal mode
	CLK -> PLLCON &= ~(1ul << 16);
	
	// Reset FB_DV == set FB_DV to 0 => NF = 2
	CLK -> PLLCON &= ~(0x01FFul << 0);
	
	while (!(CLK -> CLKSTATUS &= (1ul << 2)));
	//===========================================================
	
	// GPIO INIT
	//===========================================================
	// GPC14 - LED 7
	PC -> PMD &= ~(0x03ul << 28);
	PC -> PMD |= 0x01 << 28;
	//===========================================================
	
	// SYSTICK CONFIG
	//===========================================================
	// Select STCLK as SysTick clk
	SysTick -> CTRL &= ~(1ul << 2);
	
	// SysTick Reload Value: 0.3s / (1/1MHz) = 300000
	SysTick -> LOAD = 300000 - 1;
	SysTick -> VAL = 0;
	
	// Enable SysTick
	SysTick -> CTRL |= 1ul << 0;
	//===========================================================
	
	SYS_LockReg();
	
	//-------------------------------------------------------------------------------------------------------------
	//Main program
	//-------------------------------------------------------------------------------------------------------------
	while (1) {
		// Check COUNTFLAG
		while (!(SysTick -> CTRL & (1ul << 16)));
		PC -> DOUT ^= 1ul << 14;
	}
}
