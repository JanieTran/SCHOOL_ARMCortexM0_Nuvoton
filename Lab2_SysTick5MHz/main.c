//-------------------------------------------------------------------------------------------------------------
//This part provide Project Description, key functions, and other project-related information
//Project     : Lab 2 - SysTick 5 MHz
//Author      : Phuong Tran
//Date        : 29th March, 2018
//Board       : Nu-LB-NUC140
//MCU         : NUC140VE3CN
//Functions   : - SysTick clock is 5 MHz
//							- LED7 on/off period is 0.2 s. This delay time is controlled by SysTick.
//-------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

#define SYSTICK_RELOAD 1000000

int main(void)
{
	//-------------------------------------------------------------------------------------------------------------
	//System initialization
	//-------------------------------------------------------------------------------------------------------------
	SYS_UnlockReg();
	
	// CLK INIT
	//==================================================
	// HXT 12 MHz
	CLK -> PWRCON |= 1ul << 0;
	while (!(CLK -> CLKSTATUS & 1ul << 0));
	//==================================================
	
	// CLK SELECTION - CLK DIV
	//==================================================
	CLK -> CLKSEL0 &= ~(0x07ul << 0);
	CLK -> CLKDIV &= ~(0x0Ful << 0);
	//==================================================
	
	// PLL CONFIG
	//==================================================
	// PLL_SRC: 0 - 12 MHz HXT
	CLK -> PLLCON &= ~(1ul << 19);
	// Enable PLL clk out
	CLK -> PLLCON &= ~(1ul << 18);
	// PLL normal mode
	CLK -> PLLCON &= ~(1ul << 16);
	
	// Reset FB_DV
	CLK -> PLLCON &= ~(0x01FFul << 0);
	// Set FB_DV to 3 => NF = 5 MHz
	CLK -> PLLCON |= 0x03ul << 0;
	
	while (!(CLK -> CLKSTATUS & (1ul << 2)));	
	//==================================================
	
	// GPIO MODE
	//==================================================
	PC -> PMD &= ~(0x03ul <<  28);
	PC -> PMD |= 0x01 << 28;
	//==================================================
	
	// SYSTICK CONFIG
	//==================================================
	// Select HCLK SysTick clk
	SysTick -> CTRL &= ~(1ul << 2);
	SysTick -> CTRL |= 1ul << 2;
	
	// Select PLLFOUT in HCLK_S in CLKSEL0
	CLK -> CLKSEL0 |= 0x02 << 0;
	
	// SysTick Reload Value: 0.2s / (1/5MHz) = 1000000
	SysTick -> LOAD = SYSTICK_RELOAD - 1;
	SysTick -> VAL = 0;
	
	// Enable SysTick
	SysTick -> CTRL |= 1ul << 0;	
	//==================================================
	
	SYS_LockReg();
	
	//-------------------------------------------------------------------------------------------------------------
	//Main program
	//-------------------------------------------------------------------------------------------------------------
	while (1) {
		// Check COUNTFLAG
		while (!(SysTick -> CTRL & (1ul << 16)));
		PC -> DOUT ^= 1 << 14;
	}
}
