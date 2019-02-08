//-------------------------------------------------------------------------------------------------------------
//This part provide Project Description, key functions, and other project-related information
//Project     : Lab 1 - Push Button
//Author      : Phuong Tran
//Date        : 26th, March, 2018
//Board       : Nu-LB-NUC140
//MCU         : NUC140VE3CN
//Functions   : The Nu-LB-NUC140 development board has a built-in push button SW_INT1 (Fig. 3)
// 						Develop an embedded software program to control the Buzzer to sound an alarm 3 times after every 2 seconds
// 						whenever somebody presses the button SW_INT1.
//-------------------------------------------------------------------------------------------------------------


#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

#define DELAY_SEC 2350000

void Delay_s(uint32_t count);

int main(void)
{
	uint32_t flag = 1;
	uint32_t a;
	
	//-------------------------------------------------------------------------------------------------------------
	//System initialization
	//-------------------------------------------------------------------------------------------------------------
  SYS_UnlockReg();
	
	// CLK CONFIG
	//===================================================================
	CLK -> PWRCON |= 1ul << 0;
	while (!(CLK -> CLKSTATUS & 1ul << 0));
	
	CLK -> CLKSEL0 &= ~(0x07ul << 0);
	CLK -> CLKDIV &= ~(0x0Ful << 0);
	//===================================================================
	
	// GPIO CONFIG
	//===================================================================
	// Reset GPB11 Buzzer and GPB15 Push Button
	PB -> PMD &= ~(0x03ul << 22);
	PB -> PMD &= ~(0x03ul << 30);
	
	// Set to push-pull mode
	PB -> PMD |= 1ul << 22;
	PB -> PMD |= 1ul << 30;
	//===================================================================
	
	SYS_LockReg();
	
	//-------------------------------------------------------------------------------------------------------------
	//Main program
	//-------------------------------------------------------------------------------------------------------------
	PB -> DOUT |= 1ul << 11;
	while (1) {
		if (!(PB -> PIN & (1ul << 15)) && flag) {
			flag = 0;
			for (a = 0; a < 3; a++) {
				PB -> DOUT &= ~(1ul << 11);
				Delay_s(1);
				PB -> DOUT |= 1ul << 11;
				Delay_s(2);
			}
		}
		
		if (PB -> PIN & 1ul << 15) {
			flag = 1;
		}
	}
}

void Delay_s(uint32_t count) {
	uint32_t n, i;
	for (i = 0; i < count; i++) {
		for (n = 0; n < DELAY_SEC; n++) {}
	}
}