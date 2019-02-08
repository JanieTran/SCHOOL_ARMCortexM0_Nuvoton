//-------------------------------------------------------------------------------------------------------------
//This part provide Project Description, key functions, and other project-related information
//Project     :
//Author      :
//Date        : 
//Board       : Nu-LB-NUC140
//MCU         : NUC140VE3CN
//Functions   :
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
	
	// CLK CONFIG
	//========================================================
	// Enable 12 MHz HXT
	CLK -> PWRCON |= 1ul << 0;
	while (!(CLK -> CLKSTATUS & 1ul << 0));
	
	// Reset CLKSEL0
	CLK -> CLKSEL0 &= ~(0x07ul << 0);
	
	// Reset CLKDIV
	CLK -> CLKDIV &= ~(0x0Ful << 0);
	//========================================================
	
	// PLL CONFIG
	//========================================================
	// Select clk src 12 MHz HXT
	CLK -> PLLCON &= ~(1ul << 19);
	// Enable PLL out
	CLK -> PLLCON &= ~(1ul << 18);
	// PLL normal mode
	CLK -> PLLCON &= ~(1ul << 16);
	
	// Reset FB_DV and set to 38 => NF = 40
	CLK -> PLLCON &= ~(0x01FFul << 0);
	CLK -> PLLCON |= 38 << 0;

	// Select PLL for CPU clk src
	while (!(CLK -> CLKSTATUS & 1ul << 2));
	CLK -> CLKSEL0 |= 0x02ul << 0;
	//========================================================
	
	// TIMER2 CONFIG
	//========================================================	
	// Set clk src for TM2 to HCLK
	CLK -> CLKSEL1 &= ~(0x07ul << 16);
	CLK -> CLKSEL1 |= 0x02ul << 16;
	
	// Enable TM2
	CLK -> APBCLK |= 1ul << 4;
	
	// Set PRESCALE to 39 -> TM2 clk freq = 40/(39+1) = 1 MHz
	TIMER2 -> TCSR &= ~(0xFFul << 0);
	TIMER2 -> TCSR |= 39 << 0;
	
	// Reset CRST
	TIMER2 -> TCSR |= 1ul << 26;
	
	// Set periodic mode
	TIMER2 -> TCSR &= ~(0x03ul << 27);
	TIMER2 -> TCSR |= 1ul << 27;
	
	// Timer counter mode
	TIMER2 -> TCSR &= ~(1ul << 24);
	
	// TDR
	TIMER2 -> TCSR |= 1ul << 16;
	
	// TimeOut 0.25s => TCMPR = 0.25 / (1/1 MHz) = 250000
	TIMER2 -> TCMPR = 250000 - 1;
	
	// Start counting
	TIMER2 -> TCSR |= 1ul << 30;
	//========================================================
	
	// GPIO CONFIG
	//========================================================
	// GPC15 - LED8
	PC -> PMD &= ~(0x03ul << 30);
	PC -> PMD |= 1ul << 30;
	//========================================================
	
	SYS_LockReg();
	//-------------------------------------------------------------------------------------------------------------
	//Main program
	//-------------------------------------------------------------------------------------------------------------
	while (1) {
		while (!(TIMER2 -> TDR == (250000 - 1)));
		PC -> DOUT ^= 1ul << 15;		
	}
}
