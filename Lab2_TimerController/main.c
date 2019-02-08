//-------------------------------------------------------------------------------------------------------------
//This part provide Project Description, key functions, and other project-related information
//Project     : Lab 2 - Timer Controller
//Author      : Phuong Tran
//Date        : 28th March, 2018
//Board       : Nu-LB-NUC140
//MCU         : NUC140VE3CN
//Functions   : configure and use Channel 0 (TM0) and
//			Channel 1 (TM1) of Timer Controller module as a normal timing control unit and a clock generator,
//			respectively, at the same time.
//-------------------------------------------------------------------------------------------------------------


#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

#define TIMER0_COUNTS 16384
#define TIMER1_COUNTS 6000

int main(void)
{
	//-------------------------------------------------------------------------------------------------------------
	//System initialization
	//-------------------------------------------------------------------------------------------------------------
	SYS_UnlockReg();
	
	// CLK INIT
	//=================================================
	// Enable clk - HXT 12 MHz
	CLK -> PWRCON |= 1ul << 0;
	while (!(CLK -> CLKSTATUS & (1ul << 0)));
	
	// Reset clk selection reg and clk div reg
	CLK -> CLKSEL0 &= ~(0x07ul << 0);
	CLK -> CLKDIV &= ~(0x0Ful);
	//=================================================
	
	// GPIO INIT
	//=================================================
	// GPC13 - LED7
	PC -> PMD &= ~(0x03ul << 28);
	PC -> PMD |= 0x01 << 28;
	//=================================================
	
	// TIMER0 INIT
	//=================================================
	// TM0 clk selection and config
	// Use clk selection reg 1 for TM0
	
	// Reset TMR0_S in CLKSEL1
	CLK -> CLKSEL1 &= ~(0x07ul << 8);
	// Select clk src 32.768 kHz LXT
	CLK -> CLKSEL1 |= 0x01ul << 8;
	
	// Enable TMR0_EN in APB clk control reg
	CLK -> APBCLK |= 0x01ul << 2;
	
	// Reset PRESCALE of TCSR to 0
	TIMER0 -> TCSR &= ~(0xFFul << 0);
	
	// Reset CRST of TM0
	TIMER0 -> TCSR |= 0x01ul << 26;
	
	// Define TM0 operation mode
	// Reset timer counting mode
	TIMER0 -> TCSR &= ~(0x03ul << 27);
	// TM1 counting mode: periodic
	TIMER0 -> TCSR |= 0x01ul << 27;
	// Disable timer counter mode
	TIMER0 -> TCSR &= ~(0x01ul << 24);
	
	// TDR updated continuously while timer counter is counting
	TIMER0 -> TCSR |= 0x01ul << 16;
	
	// TimeOut = 0.5s -> Counter's TCMPR = 0.5/(1/(32768)) = 16384
	TIMER0 -> TCMPR = TIMER0_COUNTS;	
	
	// Start couting
	TIMER0 -> TCSR |= 0x01ul << 30;
	//=================================================
	
	// TIMER1 INIT
	//=================================================
	// GPB9 for TM1
	SYS -> GPB_MFP |= 0x01ul << 9;
	SYS -> ALT_MFP &= ~(0x01ul << 1);
	
	// TM1 clk selection and config
	// Reset TMR1_S in CLKSEL1
	CLK -> CLKSEL1 &= ~(0x07ul << 12);
	// Select clk scr from HCLK
	CLK -> CLKSEL1 |= 0x02ul << 12;
	
	// Enable TMR1_EN in APB clk control reg
	CLK -> APBCLK |= 0x01ul << 3;
	
	// Reset PRESCALE of TCSR to 0
	TIMER1 -> TCSR &= ~(0xFFul << 0);
	
	// Reset CRST TM1
	TIMER1 -> TCSR |= 0x01ul << 26;
	
	// Define TM1 operation mode
	// Timer counting mode: toggle
	TIMER1 -> TCSR &= ~(0x03ul << 27);
	TIMER1 -> TCSR |= 0x02ul << 27;
	// Disable timer counter mode
	TIMER1 -> TCSR &= ~(0x01ul << 24);
	
	// TDR updated continuously while timer counting is couting
	TIMER1 -> TCSR |= 0x01ul << 16;
	
	// TimeOut = 0.5s -> Counter's TCMPR = 0.5/(1/(12 MHz)) = 6000
	TIMER1 -> TCMPR = TIMER1_COUNTS;
	
	// Start counting
	TIMER1 -> TCSR |= 1ul << 30;
	//=================================================
	
	SYS_LockReg();
	
	//-------------------------------------------------------------------------------------------------------------
	//Main program
	//-------------------------------------------------------------------------------------------------------------
	while (1) {
		// Check timer data reg to get the timer counter;s current count value
		while (!(TIMER0 -> TDR == TIMER0_COUNTS));
		PC -> DOUT ^= 1 << 14;
	}
}
