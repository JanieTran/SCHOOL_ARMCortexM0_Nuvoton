//-------------------------------------------------------------------------------------------------------------
//This part provide Project Description, key functions, and other project-related information
//Project     :
//Author      :
//Date        : 
//Board       : Nu-LB-NUC140
//MCU         : NUC140VE3CN
//Functions   :
//
//
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
	
	// Enable LXT 32.768 kHz
	CLK -> PWRCON |= 1ul << 1;
	while (!(CLK -> CLKSTATUS & 1ul << 1));
	
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
	
	// Reset FB_DV and set FB_DV to 48 => NF = 50
	CLK -> PLLCON &= ~(0x01FFul << 0);
	CLK -> PLLCON |= 48;
	
	while (!(CLK -> CLKSTATUS &= (1ul << 2)));
	//===========================================================
	
	// GPIO INIT
	//===========================================================
	// GPC15 - LED 8
	PC -> PMD &= ~(0x03ul << 30);
	PC -> PMD |= 1ul << 30;
	//===========================================================
	
	// SYSTICK CONFIG
	//===========================================================
	// Select STCLK as SysTick clk
	SysTick -> CTRL &= ~(1ul << 2);
	
	// Select 32.768 kHz STCLK src
	CLK -> CLKSEL0 &= ~(0x07ul << 3);
	CLK -> CLKSEL0 |= 1ul << 3;
	
	// SysTick Reload Value: 0.5s / (1/32.768kHz) = 16384
	SysTick -> LOAD = 16384 - 1;
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
		PC -> DOUT ^= 1ul << 15;
	}
}
