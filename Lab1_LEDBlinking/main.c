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
#define HXT_STT 1ul<<0

#define CLK_SEL_MASK ~(0x07<<0)
#define CLK_SEL_PLL 0x02

#define GPIO_GPCLED_MASK ~(0xFFul << 24)
#define GPIO_GPCLED_PUSH_PULL (0x55ul << 24)

#define DELAY_TIME 2350000

void Delay_s(uint32_t count);

int main(void)
{
	//-------------------------------------------------------------------------------------------------------------
	//System initialization
	//-------------------------------------------------------------------------------------------------------------
   SYS_UnlockReg();
	
	// Enable clk - 10 kHz LIRC
	CLK -> PWRCON |= HXT_CLK;
	while (!(CLK -> CLKSTATUS & HXT_STT));
	
	// Clk src selection
	CLK -> CLKSEL0 &= CLK_SEL_MASK;

	SYS_LockReg();
	
	// GPIO mode
	PC -> PMD &= GPIO_GPCLED_MASK;
	PC -> PMD |= GPIO_GPCLED_PUSH_PULL;
	
	
	//-------------------------------------------------------------------------------------------------------------
	//Main program
	//-------------------------------------------------------------------------------------------------------------
	while (1) {
		PC -> DOUT |= 0x0Ful << 12;
		PC -> DOUT &= ~(1ul << 12);
		Delay_s(DELAY_TIME);
		PC -> DOUT |= 0x0Ful << 12;
		PC -> DOUT &= ~(1ul << 13);
		Delay_s(DELAY_TIME);
		PC -> DOUT |= 0x0Ful << 12;
		PC -> DOUT &= ~(1ul << 14);
		Delay_s(DELAY_TIME);
		PC -> DOUT |= 0x0Ful << 12;
		PC -> DOUT &= ~(1ul << 15);
		Delay_s(DELAY_TIME);
	}
}

void Delay_s(uint32_t count) {
	uint32_t n;
	for (n = 0; n < count; n++) {}
}