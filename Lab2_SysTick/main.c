//-------------------------------------------------------------------------------------------------------------
//This part provide Project Description, key functions, and other project-related information
//Project     : Lab 2 - SysTick
//Author      : Phuong Tran
//Date        : 28th, March, 2018
//Board       : Nu-LB-NUC140
//MCU         : NUC140VE3CN
//Functions   :		- NUC140 CPU clock is HCLK=12 MHz
//								- SysTick clock is 32.768 kHz
//								- LED6 on/off period is 0.125 s. This delay time is controlled by SysTick.
//
//
//-------------------------------------------------------------------------------------------------------------


#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

#define SYSTICK_LVR 4906

int main(void)
{
	//-------------------------------------------------------------------------------------------------------------
	//System initialization
	//-------------------------------------------------------------------------------------------------------------
	
	SYS_UnlockReg();
	
	// SYSTEM INIT
	//===========================================
	// Enable clk src - HXT 12 MHz
	CLK -> PWRCON |= 1ul << 0;
	while (!(CLK -> CLKSTATUS & (1ul << 0)));
	
	// Enable clk src - LXT 32 kHz
	CLK -> PWRCON |= 1ul << 1;
	while (!(CLK -> CLKSTATUS & (1ul << 1)));
	
	// Reset clk src select reg
	CLK -> CLKSEL0 &= ~(0x07ul << 0);
	
	// Reset CPU clk freq divider
	CLK -> CLKDIV &= ~(0x0Ful);
	//===========================================
	
	// GPIO INIT
	//===========================================
	// GPC13 - LED 6
	PC -> PMD &= ~(0x02ul << 26);
	PC -> PMD |= 0x01ul << 26;
	//===========================================
	
	// SYSTICK INIT
	//===========================================
	// Select STCLK as SysTick clk src
	SysTick -> CTRL &= ~(0x01ul << 2);
	
	// Reset STCLK_S in CLKSEL0
	CLK -> CLKSEL0 &= ~(0x07ul << 3);
	// Select STCLK 32.758 kHz
	CLK -> CLKSEL0 |= 0x01ul << 3;
	
	// SysTick Reload Value
	SysTick -> LOAD = SYSTICK_LVR - 1;
	// Clear current value reg
	SysTick -> VAL = 0;
	
	// Start SysTick - Enable SysTick timer counter
	SysTick -> CTRL |= 0x01ul << 0;
	//===========================================
	
	SYS_LockReg();
	
	//-------------------------------------------------------------------------------------------------------------
	//Main program
	//-------------------------------------------------------------------------------------------------------------
	while (1) {
		// Check COUNTFLAG
		while (!(SysTick -> CTRL & (0x01ul << 16)));
		PC -> DOUT ^= 1 << 13;
	}
}
