//-------------------------------------------------------------------------------------------------------------
//This part provide Project Description, key functions, and other project-related information
//Project     : Lab 1 - Clock Controller GPIO
//Author      : Phuong Tran
//Date        : 19th March, 2018
//Board       : Nu-LB-NUC140
//MCU         : NUC140VE3CN
//Functions   :
// - Configure Clock Controller to use different clock sources and clock frequencies
// - Configure and control GPIO to output signals to NUC140 MCU external pins
// - Configure and control GPIO to receive input signals from NUC140 MCU external pins
//-------------------------------------------------------------------------------------------------------------


#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

#define DELAYTIME 1000

#define HXT_CLK 1ul<<0

#define CLK_SEL_MASK (~(0x07ul<<0)) // ~(0b00000111)
#define PLL_CLK_SEL 0x02ul // 0b00000010

#define HXT_STATUS 1ul<<0
#define PLL_STATUS 1ul<<2

#define PLLCON_FB_DV_MASK (~(0x01FFul << 0)) // ~0b0000000111111111
#define PLLCON_FB_DV_VAL 46 // 0b101110

#define CPUCLOCKDIVIDE 1

void Delay_s(uint32_t count);

int main(void)
{
	// Unclick protected registers
	SYS_UnlockReg();
	
	// Enable clock sources HXT - External High Freq Crystal 12MHz
	CLK -> PWRCON |= HXT_CLK;
	// Check status and wait until the clk source is stable
	while (!(CLK -> CLKSTATUS & HXT_STATUS));
	
	//----------------------------------------------------------
	// PLL CONFIGURATION
	
	// 0: PLL source clk 12 MHz HXT
	CLK -> PLLCON &= ~(1ul << 19);
	// 0: PLL normal mode
	CLK -> PLLCON &= ~(1ul << 16);
	
	// Reset FB_DV to 0
	CLK -> PLLCON &= PLLCON_FB_DV_MASK;
	// Set FB_DV to 46
	CLK -> PLLCON |= PLLCON_FB_DV_VAL;
	
	// 0: Enable PLL clk out
	CLK -> PLLCON &= ~(1ul << 18);
	
	// Final PLL config
	// [19:0] = 0b0000-11-00001-000101110
	// FIN = 12   NR = 3   NF = 48   NO = 4
	// FOUT = 48
	
	// Check status and wait until the clk source is stable
	while (!(CLK -> CLKSTATUS & PLL_STATUS));
	
	//----------------------------------------------------------
	
	// CLOCK SOURCE SELECTION
	// Clk source selection for CPU/AHB/APB: PLL_FOUT
	CLK -> CLKSEL0 &= CLK_SEL_MASK;
	CLK -> CLKSEL0 |= PLL_CLK_SEL;
	
	// CLOCK FREQ DIVISION
	// Reset HCLK_N to 0
	CLK -> CLKDIV &= ~0x0Ful; // ~0b00001111
	CLK -> CLKDIV |= (CPUCLOCKDIVIDE - 1);
	
	// Lock protected registers
	SYS_LockReg();
	
	// GPIO MODE CONTROL
	// Clear Port C Pin 12
	PC -> PMD &= ~(0X03ul) << 24;
	// Output push-pull mode Port C Pin 12
	PC -> PMD |= (0x01ul) << 24;
	
	while (1) {
		// GPIO Port Data Output Value
		PC -> DOUT ^= 1 << 12; // ^: XOR - same 0, different 1
		Delay_s(DELAYTIME);
	}
}

void Delay_s(uint32_t count) {
	uint32_t n;
	for (n = 0; n < count; n++) {}
}
