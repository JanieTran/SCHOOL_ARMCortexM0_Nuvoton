//-------------------------------------------------------------------------------------------------------------
//This part provide Project Description, key functions, and other project-related information
//Project     : Lab 1 - Buzzer
//Author      : Phuong Tran
//Date        : 26th March, 2018
//Board       : Nu-LB-NUC140
//MCU         : NUC140VE3CN
//Functions   : The Nu-LB-NUC140 development board has a built-in Buzzer (Fig. 2)
// 					Develop an embedded software program to control the Buzzer to sound an alarm every 2 seconds.
//-------------------------------------------------------------------------------------------------------------


#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

#define HXT_CLK 1ul << 0
#define HXT_STT 1ul << 0

#define CLK_SEL_MASK ~(0x07ul << 0)

#define GPIO_B11_BUZZER_MASK ~(0x03ul << 22)
#define GPIO_B11_BUZZER_PP 0x01ul << 22

#define DELAY_SEC 2350000

void Delay_s(uint32_t count);

int main(void)
{
	//-------------------------------------------------------------------------------------------------------------
	//System initialization
	//-------------------------------------------------------------------------------------------------------------
  SYS_UnlockReg();
	
	// Enable clk - 12 MHz HXT
	CLK -> PWRCON |= HXT_CLK;
	while (!(CLK -> CLKSTATUS & HXT_STT));
	
	// Clk src selection
	CLK -> CLKSEL0 &= CLK_SEL_MASK;
	
	SYS_LockReg();
	
	// GPIO mode
	PB -> PMD &= GPIO_B11_BUZZER_MASK;
	PB -> PMD |= GPIO_B11_BUZZER_PP;
	
	//-------------------------------------------------------------------------------------------------------------
	//Main program
	//-------------------------------------------------------------------------------------------------------------
	while (1) {
		PB -> DOUT &= ~(1ul << 11);
		Delay_s(1);
		PB -> DOUT |= 1ul << 11;
		Delay_s(2);
	}
}

void Delay_s(uint32_t count) {
	uint32_t n, i;
	for (i = 0; i < count; i++) {
		for (n = 0; n < DELAY_SEC; n++) {}
	}
}
