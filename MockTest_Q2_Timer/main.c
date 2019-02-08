//-------------------------------------------------------------------------------------------------------------
//This part provide Project Description, key functions, and other project-related information
//Project     : Mock Test - Question 2 - Timer2
//Author      : Phuong Tran
//Date        : 2nd April, 2018
//Board       : Nu-LB-NUC140
//MCU         : NUC140VE3CN
//Functions   : Develop an embedded software program to use Timer Controller Channel 2 (TM2)
// of the NUC140 MCU to generate a 50 Hz clock signal at external pin GPIOB.10.
// TM2 is configured to operate at clock frequency of 10 MHz.
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
	
	// CLK INIT
	//===============================================================
	// CPU clk 12 MHz HXT
	CLK -> PWRCON &= ~(1ul << 0);
	CLK -> PWRCON |= 1ul << 0;
	while (!(CLK -> CLKSTATUS & (1ul << 0)));
	
	// Reset CLKSEL0 and CLKDIV
	CLK -> CLKSEL0 &= ~(0x07ul << 0);
	CLK -> CLKDIV &= ~(0x0Ful << 0);
	//===============================================================
	
	// PLL CONFIG
	//===============================================================
	// PLL_SRC: 0 => 12 MHz HXT
	CLK -> PLLCON &= ~(1ul << 19);
	// Enable PLL clk out
	CLK -> PLLCON &= ~(1ul << 18);
	// PLL normal mode
	CLK -> PLLCON &= ~(1ul << 16);
	
	// Reset FB_DV
	CLK -> PLLCON &= ~(0x01FFul << 0);
	// Set FB_DV to 8 => NF = 10 MHz
	CLK -> PLLCON |= 0x08ul << 0;
	
	while (!(CLK -> CLKSTATUS & (1ul << 2)));
	//===============================================================
	
	// CLKSEL0 CONFIG
	//===============================================================
	// Select PLLFOUT for HCLK_S
	CLK -> CLKSEL0 |= 0x02ul << 0;
	//===============================================================
	
	// TIMER2 CONFIG
	//===============================================================
	// GPB10 for TM2
	SYS -> GPB_MFP |= 1ul << 10;
	SYS -> ALT_MFP &= ~(1ul << 0);
	
	// TM2 clk selection and config
	// Reset TMR2_S in CLKSEL1
	CLK -> CLKSEL1 &= ~(0x07ul << 16);
	// Set clk src for TMR2 to HCLK
	CLK -> CLKSEL1 |= 0x02 << 16;
	
	// Enable TMR2_EN in APB clk control reg
	CLK -> APBCLK |= 1ul << 4;
	
	// Reset PRESCALE ot TCSR to 0
	TIMER2 -> TCSR &= ~(0xFFul << 0);
	
	// Reset CRST of TM2
	TIMER2 -> TCSR &= ~(1ul << 26);
	TIMER2 -> TCSR |= 1ul << 26;
	
	// Definer operation mode
	// Reset timer counting mode
	TIMER2 -> TCSR &= ~(0x03ul << 27);
	TIMER2 -> TCSR |= 0x02ul << 27;
	// Event counter mode
	TIMER2 -> TCSR &= ~(1ul << 24);
	
	// TDR updated continuouslly while timer counting is counting
	TIMER2 -> TCSR |= 1ul << 16;
	
	// TimeOut = 50 Hz = 0.02s => TCMPR = 0.02 / (1/10 MHz) = 200000 / 2 = 100000
	TIMER2 -> TCMPR = 100000 - 1;
	
	// Start counting
	TIMER2 -> TCSR |= 1ul << 30;
	//===============================================================
	
	//-------------------------------------------------------------------------------------------------------------
	//Main program
	//-------------------------------------------------------------------------------------------------------------
	while (1) {
		
	}
}
