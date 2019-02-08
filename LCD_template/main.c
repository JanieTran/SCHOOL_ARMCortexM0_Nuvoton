#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Draw2D.h"

#include "picture.h"
//------------------------------------------------------------------------------------------------------------------------------------
// Definitions
//------------------------------------------------------------------------------------------------------------------------------------
#define HXT_CLK 1ul<<0
#define LXT_CLK 1ul<<1
#define HIRC_CLK 1ul<<2
#define LIRC_CLK 1ul<<3

#define CLK_SEL_MASK (~(0x07<<0))
#define HXT_CLK_SEL 0x00
#define LXT_CLK_SEL 0x01
#define PLL_CLK_SEL 0x02
#define HIRC_CLK_SEL 0x07
#define LIRC_CLK_SEL 0x03

#define HXT_STATUS 1ul<<0
#define LXT_STATUS 1ul<<1
#define PLL_STATUS 1ul<<2
#define HIRC_STATUS 1ul<<4
#define LIRC_STATUS 1ul<<3
#define CLK_SW_STATUS 1ul<<7


#define PLLCON_FB_DV_MASK (~(0x01FFul << 0))
#define PLLCON_FB_DV_VAL 48

#define CPUCLOCKDIVIDE 1

#define SYSTICK_DLAY_us 1000000
#define BOUNCING_DLY 200000

//------------------------------------------------------------------------------------------------------------------------------------
// Functions declaration
//------------------------------------------------------------------------------------------------------------------------------------
void System_Config(void);
void SPI3_Config(void);

void LCD_start(void);
void LCD_command(unsigned char temp);
void LCD_data(unsigned char temp);
void LCD_clear(void);
void LCD_SetAddress(uint8_t PageAddr, uint8_t ColumnAddr);

void KeyPadEnable(void);
uint8_t KeyPadScanning(void);

//------------------------------------------------------------------------------------------------------------------------------------
// Main program
//------------------------------------------------------------------------------------------------------------------------------------

int main(void)
{
    uint32_t i = 0;
    //--------------------------------
    //System initialization
    //--------------------------------
    System_Config();

    //--------------------------------
    //SPI3 initialization
    //--------------------------------
    SPI3_Config();

    //--------------------------------
    //LCD initialization
    //--------------------------------    
    LCD_start();
    LCD_clear();

    //--------------------------------
    //LCD static content
    //--------------------------------    
    
    //--------------------------------
    //LCD dynamic content
    //--------------------------------              
    while (1) {

    }
}

//------------------------------------------------------------------------------------------------------------------------------------
// Functions definition
//------------------------------------------------------------------------------------------------------------------------------------

void LCD_start(void)
{
    LCD_command(0xE2); // Set system reset
	LCD_command(0xA1); // Set Frame rate 100 fps  
    LCD_command(0xEB); // Set LCD bias ratio E8~EB for 6~9 (min~max)  
    LCD_command(0x81); // Set V BIAS potentiometer
	LCD_command(0xA0); // Set V BIAS potentiometer: A0 ()        	
    LCD_command(0xC0);  
	LCD_command(0xAF); // Set Display Enable
}

void LCD_command(unsigned char temp)
{
  SPI3->SSR |= 1ul << 0;  
  SPI3->TX[0] = temp;
  SPI3->CNTRL |= 1ul << 0;
  while(SPI3->CNTRL & (1ul << 0));
  SPI3->SSR &= ~(1ul << 0);
}

void LCD_data(unsigned char temp)
{
  SPI3->SSR |= 1ul << 0;  
  SPI3->TX[0] = 0x0100+temp;
  SPI3->CNTRL |= 1ul << 0;
  while(SPI3->CNTRL & (1ul << 0));
  SPI3->SSR &= ~(1ul << 0);
}

void LCD_clear(void)
{	
    int16_t i;
	LCD_SetAddress(0x0, 0x0);			  								  
	for (i = 0; i < 132 *8; i++)
	{
		LCD_data(0x00);
	}
}

void LCD_SetAddress(uint8_t PageAddr, uint8_t ColumnAddr)
{
  LCD_command(0xB0 | PageAddr);
  LCD_command(0x10 | (ColumnAddr>>4)&0xF); 
  LCD_command(0x00 | (ColumnAddr & 0xF));
}

void KeyPadEnable(void){
    GPIO_SetMode(PA, BIT0, GPIO_MODE_QUASI);
    GPIO_SetMode(PA, BIT1, GPIO_MODE_QUASI);
    GPIO_SetMode(PA, BIT2, GPIO_MODE_QUASI); 
    GPIO_SetMode(PA, BIT3, GPIO_MODE_QUASI);
    GPIO_SetMode(PA, BIT4, GPIO_MODE_QUASI);
    GPIO_SetMode(PA, BIT5, GPIO_MODE_QUASI);
    }

uint8_t KeyPadScanning(void){
    PA0=1; PA1=1; PA2=0; PA3=1; PA4=1; PA5=1;
    if (PA3==0) return 1;
    if (PA4==0) return 4;
    if (PA5==0) return 7;
    PA0=1; PA1=0; PA2=1; PA3=1; PA4=1; PA5=1;
    if (PA3==0) return 2;
    if (PA4==0) return 5;
    if (PA5==0) return 8;
    PA0=0; PA1=1; PA2=1; PA3=1; PA4=1; PA5=1;
    if (PA3==0) return 3;
    if (PA4==0) return 6;
    if (PA5==0) return 9;	
    return 0;
    }

void System_Config (void){
    SYS_UnlockReg(); // Unlock protected registers
    // Enable clock sources
    //HXT - External High frequency Crystal 12 MHz
    CLK->PWRCON |= HXT_CLK;
    while(!(CLK->CLKSTATUS & HXT_STATUS));    
    //LXT - External Low frequency Crystal 32 kHz
    CLK->PWRCON |= LXT_CLK;
    while(!(CLK->CLKSTATUS & LXT_STATUS));
    
    //PLL
    //CLK->PLLCON |= 1ul<<19;   //0: PLL input is HXT 12MHz (default). 1: PLL input is HIRC 22MHz
    CLK->PLLCON &= ~(1ul<<18);  //0: enable PLL clock out. 1: disable PLL clock (default)
    CLK->PLLCON &= ~(1ul<<16);  //0: PLL in normal mode. 1: PLL in power-down mode (default)
    CLK->PLLCON &= PLLCON_FB_DV_MASK;
    CLK->PLLCON |= PLLCON_FB_DV_VAL; //frequency: 1 MHz * (PLLCON_FB_DV_VAL+2)
    while(!(CLK->CLKSTATUS & PLL_STATUS));
    
    //clock source selection
    CLK->CLKSEL0 &= CLK_SEL_MASK;
    CLK->CLKSEL0 |= PLL_CLK_SEL;
    
    //clock division
    CLK->CLKDIV &= ~0x0F;
    CLK->CLKDIV |= (CPUCLOCKDIVIDE-1);

    //enable clock of SPI3
    CLK->APBCLK |= 1ul << 15;
    
	SYS_LockReg();  // Lock protected registers    
    }
    
void SPI3_Config (void){
    SYS->GPD_MFP |= 1ul << 11; //1: PD11 is configured for alternative function
    SYS->GPD_MFP |= 1ul << 9; //1: PD9 is configured for alternative function
    SYS->GPD_MFP |= 1ul << 8; //1: PD8 is configured for alternative function
  
    SPI3->CNTRL &= ~(1ul << 23); //0: disable variable clock feature
    SPI3->CNTRL &= ~(1ul << 22); //0: disable two bits transfer mode
    SPI3->CNTRL &= ~(1ul << 18); //0: select Master mode
    SPI3->CNTRL &= ~(1ul << 17); //0: disable SPI interrupt    
    SPI3->CNTRL |= 1ul << 11; //1: SPI clock idle high 
    SPI3->CNTRL &= ~(1ul << 10); //0: MSB is sent first   
    SPI3->CNTRL &= ~(3ul << 8); //00: one transmit/receive word will be executed in one data transfer
   
    SPI3->CNTRL &= ~(31ul << 3); //Transmit/Receive bit length
    SPI3->CNTRL |= 9ul << 3;     //9: 9 bits transmitted/received per data transfer
    
    SPI3->CNTRL |= (1ul << 2);  //1: Transmit at negative edge of SPI CLK       
    SPI3->DIVIDER = 0; // SPI clock divider. SPI clock = HCLK / ((DIVIDER+1)*2). HCLK = 50 MHz
    }
