//-------------------------------------------------------------------------------------------------------------
//This part provide Project Description, key functions, and other project-related information
//Project     : Lab 4 - SPI3 and LCD (Example)
//Author      : 
//Date        : 7th May, 2018
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
#include "LCD.h"

void LCD_start(void);
void LCD_command(unsigned char temp);
void LCD_data(unsigned char temp);
void LCD_clear(void);
void LCD_SetAddress(uint8_t PageAddr, uint8_t ColumnAddr);

int main(void)
{
	//-------------------------------------------------------------------------------------------------------------
	//System initialization
	//-------------------------------------------------------------------------------------------------------------
	SYS_UnlockReg();
	
	// CLK INIT
	//=========================================================
	CLK -> PWRCON |= 1ul << 0;
	while (!(CLK -> CLKSTATUS & 1ul << 0));
	
	// PLL CONFIG
	CLK->PLLCON &= ~(1ul<<19); 
	CLK->PLLCON &= ~(1ul<<16);
	CLK->PLLCON &= (~(0x01FFul << 0));
	CLK->PLLCON |= 8;
	CLK->PLLCON &= ~(1ul<<18);
	while(!(CLK->CLKSTATUS & (0x01ul << 2)));
	
	// Clk src selection
	CLK -> CLKSEL0 &= ~(0x07ul << 0);
	CLK -> CLKSEL0 |= 0x02ul << 0;
	
	// Clear CLKDIV
	CLK -> CLKDIV &= ~(0x0Ful << 0);
	
	// Enable clk for SPI3
	CLK -> APBCLK |= 1ul << 15;
	//=========================================================
	
	SYS_LockReg();
	
	// SPI3 CONFIG
	//=========================================================
	// Alternative function
	SYS -> GPD_MFP |= 1ul << 11;
	SYS -> GPD_MFP |= 1ul << 9;
	SYS -> GPD_MFP |= 1ul << 8;
	
	// CNTRL REG
	// Disable variable clk feature
	SPI3 -> CNTRL &= ~(1ul << 23);
	// 1-bit transfer
	SPI3 -> CNTRL &= ~(1ul << 22);
	// Master mode
	SPI3 -> CNTRL &= ~(1ul << 18);
	// Disable SPI interrupt
	SPI3 -> CNTRL &= ~(1ul << 17);
	// SPI clk idle high
	SPI3 -> CNTRL |= 1ul << 11;
	// MSB first
	SPI3 -> CNTRL &= ~(1ul << 10);
	// 1 word/transaction
	SPI3 -> CNTRL &= ~(0x03ul << 8);
	// 9 bits/word
	SPI3 -> CNTRL &= ~(31ul << 3);
	SPI3 -> CNTRL |= 0x09ul << 3;
	// Transmit at falling edge
	SPI3 -> CNTRL |= 1ul << 2;
	
	// DIVIDER REG
	SPI3 -> DIVIDER = 24;
	//=========================================================
	
	
	//-------------------------------------------------------------------------------------------------------------
	//Main program
	//-------------------------------------------------------------------------------------------------------------
	LCD_start();
	LCD_clear();
	
	LCD_SetAddress(0,2);
	LCD_data(0xF0);
	
	LCD_SetAddress(0,129);
	LCD_data(0x0F);
	
	LCD_SetAddress(7,129);
	LCD_data(0x33);
	
	LCD_SetAddress(7,2);
	LCD_data(0x55);
}

void LCD_start(void) {
	LCD_command(0xE2);
	LCD_command(0xA1);
	LCD_command(0xEB);
	LCD_command(0x81);
	LCD_command(0xA0);
	LCD_command(0xC0);
	LCD_command(0xAF);
}

void LCD_clear(void) {
	int16_t i;
	LCD_SetAddress(0x0,0x0);
	
	for (i = 0; i < 132*8; i++) {
		LCD_data(0x00);
	}
}

void LCD_command(unsigned char temp) {
	SPI3 -> SSR |= 1ul << 0;
	SPI3 -> TX[0] = temp;
	SPI3 -> CNTRL |= 1ul << 0;
	while (SPI3 -> CNTRL & 1ul << 0);
	SPI3 -> SSR &= ~(1ul << 0);
}

void LCD_data(unsigned char temp) {
	SPI3 -> SSR |= 1ul << 0;
	SPI3 -> TX[0] = 0x0100 + temp;
	SPI3 -> CNTRL |= 1ul << 0;
	while (SPI3 -> CNTRL & 1ul << 0);
	SPI3 -> SSR &= ~(1ul << 0);
}

void LCD_SetAddress(uint8_t PageAddr, uint8_t ColumnAddr) {
	LCD_command(0xB0 | PageAddr);
	LCD_command(0x10 | (ColumnAddr>>4)&0xF);
	LCD_command(0x00 | (ColumnAddr & 0xF));
}
	