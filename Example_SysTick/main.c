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

#define HXT_CLK 1ul<<0
#define HXT_STATUS 1ul<<0

#define CLK_SEL_MASK (~(0x07ul<<0))
#define STCLK_SEL 0x03ul<<3

#define CSR_CLKSRC (~(1ul << 2))
#define CSR_EN 1ul<<0

#define CVR_CLR (~(0xFFFFFFul << 0))
#define RVR_500MS 3000000
#define COUNTFLAG 1ul<<16

int main(void)
{
	// Unlock protected registers
	SYS_UnlockReg();
	
	// Enable 12MHz CPU clk source
	CLK -> PWRCON |= HXT_CLK;
	while (!(CLK -> CLKSTATUS & HXT_STATUS));
	
	// Clk selection 
	CLK -> CLKSEL0 &= CLK_SEL_MASK;
		// HCLK_S
	CLK -> CLKSEL0 |= STCLK_SEL;
	
	// Select SysTick clk
	SysTick -> CTRL &= CSR_CLKSRC;
	// Enable SysTick timer
	SysTick -> CTRL &= ~(1ul << 0);
	SysTick -> CTRL |= CSR_EN;
	
	// Clear CVR
	SysTick -> VAL &= CVR_CLR;
	// Set RVR to 3000000 => 0.5s
	SysTick -> LOAD &= CVR_CLR;
	SysTick -> LOAD = RVR_500MS;
	
	// Lock protected registers
	SYS_LockReg();
	
	// GPIO mode control
	PC -> PMD &= ~(0x03ul) << 24;
	PC -> PMD |= (0x01ul) << 24;
	
	while (1) {
		if (SysTick -> CTRL & COUNTFLAG) {
			PC -> DOUT ^= 1 << 12;
		}
	}
}
