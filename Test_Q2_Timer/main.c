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
	
	// CLK INIT
	//===============================================================
	// Enable 12 MHz HXT
	CLK -> PWRCON |= 1ul << 0;
	while (!(CLK -> CLKSTATUS & (1ul << 0)));
	
	// CPU clk 22.1184 MHz HIRC
	CLK -> PWRCON |= 1ul << 2;
	while (!(CLK -> CLKSTATUS & (1ul << 4)));
	
	// Reset CLKSEL0 and CLKDIV
	CLK -> CLKSEL0 &= ~(0x07ul << 0);
	CLK -> CLKSEL0 |= 0x07ul << 0;
	CLK -> CLKDIV &= ~(0x0Ful << 0);
	//===============================================================
	
	// TIMER3 CONFIG
	//===============================================================
	// GPB11 for TM3
	SYS -> GPB_MFP |= 1ul << 11;
	SYS -> ALT_MFP &= ~(1ul << 4);
	
	// TM3 clk selection and config
	// Reset TMR3_S in CLKSEL1 => TMR3 clk src: 12 MHz HXT
	CLK -> CLKSEL1 &= ~(0x07ul << 20);
	
	// Enable TMR3_EN in APB clk control reg
	CLK -> APBCLK |= 1ul << 5;
	
	// Reset PRESCALE of TCSR and set to 12
	TIMER3 -> TCSR &= ~(0xFFul << 0);
	TIMER3 -> TCSR |= 0x0Bul << 0;
	
	// Reset CRST of TM2
	TIMER3 -> TCSR &= ~(1ul << 26);
	TIMER3 -> TCSR |= 1ul << 26;
	
	// Definer operation mode
	// Reset timer counting mode and set to toggle mode
	TIMER3 -> TCSR &= ~(0x03ul << 27);
	TIMER3 -> TCSR |= 0x02ul << 27;
	// Event counter mode
	TIMER3 -> TCSR &= ~(1ul << 24);
	
	// TDR updated continuouslly while timer counting is counting
	TIMER3 -> TCSR |= 1ul << 16;
	
	// TimeOut = 1 Hz = 1s => TCMPR = 1 / (1/1 MHz) = 1000000 / 2 = 500000
	TIMER3 -> TCMPR = 500000 - 1;
	
	// Start counting
	TIMER3 -> TCSR |= 1ul << 30;
	//===============================================================
	
	SYS_LockReg();
	
	//-------------------------------------------------------------------------------------------------------------
	//Main program
	//-------------------------------------------------------------------------------------------------------------
	while (1) {
		
	}
}
