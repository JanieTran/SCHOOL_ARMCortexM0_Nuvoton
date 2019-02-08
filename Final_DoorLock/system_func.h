#ifndef SYSTEM_FUNC
#define SYSTEM_FUNC

#include "system_const.h"

//------------------------------------------------------------------------------------------------------------------------------------
// Functions declaration
//------------------------------------------------------------------------------------------------------------------------------------
void System_Config(void);
void Timer0_Config(void);
void Start_Timer(uint32_t ms);
void SPI3_Config(void);
void Interrupt_Config(void);

void LCD_start(void);
void LCD_command(unsigned char temp);
void LCD_data(unsigned char temp);
void LCD_clear(void);
void LCD_SetAddress(uint8_t PageAddr, uint8_t ColumnAddr);

void KeyPadEnable(void);
uint8_t KeyPadScanning(void);

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

void Timer0_Config(void) {
    // Reset TMR0_S in CLKSEL1
	CLK -> CLKSEL1 &= ~(0x07ul << 8);
	// Select clk src HCLK
	CLK -> CLKSEL1 |= 0x02ul << 8;
	
	// Enable TMR0_EN in APB clk control reg
	CLK -> APBCLK |= 0x01ul << 2;
	
    // Reset CRST of TM0
	TIMER0 -> TCSR |= 0x01ul << 26;

	// Set PRESCALE of TCSR to 50 => Clk crs 1 MHz
	TIMER0 -> TCSR &= ~(0xFFul << 0);
    TIMER0 -> TCSR |= 49;
	
	// Define TM0 operation mode
    // TM1 counting mode: oneshot
	TIMER0 -> TCSR &= ~(0x03ul << 27);
	// Timer counter
	TIMER0 -> TCSR &= ~(0x01ul << 24);
	
	// TDR updated continuously while timer counter is counting
	TIMER0 -> TCSR |= 0x01ul << 16;
}

void Start_Timer(uint32_t ms) {
    uint32_t count = ms * 1000;

    // Reset CRST of TM0
	TIMER0 -> TCSR |= 0x01ul << 26;

	// Set PRESCALE of TCSR to 50 => Clk crs 1 MHz
	TIMER0 -> TCSR &= ~(0xFFul << 0);
    TIMER0 -> TCSR |= 49;

    // Set max count value
	TIMER0 -> TCMPR = count;

    // Start couting
	TIMER0 -> TCSR |= 0x01ul << 30;

    // Wait until timer is done counting
    while (!(TIMER0 -> TDR == count));
}

void Interrupt_Config(void) {
    // System Interupt
	NVIC -> ISER[0] |= 0x01ul << 3;
	NVIC -> IP[0] &= ~(0x03ul << 30);
	
	// GPIO interupt
	PB -> IMD &= ~(0x01ul << 15);
	PB -> IEN |= 0x01ul << 15;
}

#endif
