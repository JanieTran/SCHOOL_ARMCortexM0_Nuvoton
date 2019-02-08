//-------------------------------------------------------------------------------------------------------------
//This part provide Project Description, key functions, and other project-related information
//Project     : Example - SPI3
//Author      : Phuong Tran
//Date        : 7th May, 2018
//Board       : Nu-LB-NUC140
//MCU         : NUC140VE3CN
//Functions   : Develop an embedded software program to use NUC140 MCU’s SPI3 as a data transfer device. 
//-------------------------------------------------------------------------------------------------------------


#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

int main(void) {
	//-------------------------------------------------------------------------------------------------------------
	//System initialization
	//-------------------------------------------------------------------------------------------------------------
  SYS_UnlockReg();
	
	// CLK INIT
	//==============================================================
	// Enable 12MHz HXT
	CLK -> PWRCON |= 1ul << 0;
	while (!(CLK -> CLKSTATUS & 1ul << 0));
	
	// Reset CLKSEL and CLKDIV
	CLK -> CLKSEL0 &= ~(0x07ul << 0);
	CLK -> CLKDIV &= ~(0x0Ful << 0);	
	//==============================================================
	
	// PLL CONFIG
	//==============================================================
	// Select clk src 12 MHz HXT
	CLK -> PLLCON &= ~(1ul << 19);
	// Enable PLL out
	CLK -> PLLCON &= ~(1ul << 18);
	// PLL normal mode
	CLK -> PLLCON &= ~(1ul << 16);
	
	// Reset FB_DV and set to 48 => NF = 50
	CLK -> PLLCON &= ~(0x01FFul << 0);
	CLK -> PLLCON |= 48;

	// Select PLL for CPU clk src
	while (!(CLK -> CLKSTATUS & 1ul << 2));
	CLK -> CLKSEL0 |= 0x02ul << 0;
	//==============================================================

	// ALTERNATE FUNCTION - SPI3
	//==============================================================
	// SS pin
	SYS -> GPD_MFP |= 1ul << 8;
	// SPICLK
	SYS -> GPD_MFP |= 1ul << 9;
	// MOSI0
	SYS -> GPD_MFP |= 1ul << 11;
	//==============================================================
	
	// SPI CONFIG
	//==============================================================
	// Enable SPI3 in APBCLK
	CLK -> APBCLK |= 1ul << 15;
	
	// CNTRL REG
	// 1-bit transfer
	SPI3 -> CNTRL &= ~(1ul << 22);
	// Master mode
	SPI3 -> CNTRL &= ~(1ul << 18);
	// Disable interrupt
	SPI3 -> CNTRL &= ~(1ul << 17);
	// SPICLK idle high
	SPI3 -> CNTRL |= 1ul << 11;
	// MSB first
	SPI3 -> CNTRL &= ~(1ul << 10);
	// 1 word/transaction
	SPI3 -> CNTRL &= ~(0x03ul << 8);
	// 32 bits/word
	SPI3 -> CNTRL &= ~(0x1Ful << 3);
	// Transmit and receive at falling edge of SPICLK
	SPI3 -> CNTRL |= 1ul << 2;
	SPI3 -> CNTRL |= 1ul << 1;
	
	// DIVIDER REG
	// SPICLK = 1 MHz => DIVIDER = 24
	SPI3 -> DIVIDER = 24;
	
	// SSR REG
	// Slave Select edge-trigger
	SPI3 -> SSR &= ~(1ul << 4);
	// Automative SS
	SPI3 -> SSR |= 1ul << 3;
	// SS Active level falling edge
	SPI3 -> SSR &= ~(1ul << 2);
	// SS control automative
	SPI3 -> SSR &= ~(0x03ul << 0);
	SPI3 -> SSR |= 1ul << 0;
	
	// DATA TRANSMIT REG
	// Data to transmit = 0xCDEB
	SPI3 -> TX[0] = 0xCDEB;
	//==============================================================
	
	SYS_LockReg();
	
	//-------------------------------------------------------------------------------------------------------------
	//Main program
	//-------------------------------------------------------------------------------------------------------------
	while (1) {
		// GO_BUSY start transmitting
		SPI3 -> CNTRL |= 1ul << 0;
		while (SPI3 -> CNTRL & 1ul << 0);
	}
}
