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

int main(void)
{
	//-------------------------------------------------------------------------------------------------------------
	//System initialization
	//-------------------------------------------------------------------------------------------------------------
    SYS_Init();
	
	
	//-------------------------------------------------------------------------------------------------------------
	//Peripherals initialization. Following are the most common tasks:
	//1. Enable peripheral clock
	//2. Configure I/O pins.
	//3. Configure the peripheral (e.g. operating mode, etc.)
	//4. Configure peripheral interrupt (in case peripheral interrupt is used in your application)
	//-------------------------------------------------------------------------------------------------------------	
	//Ex: The following code configure GPIO C and its pin PC12
    GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
	
	
	//-------------------------------------------------------------------------------------------------------------
	//Main program
	//-------------------------------------------------------------------------------------------------------------
    while(1){
		//your code here
			PC -> DOUT ^= 1 << 12;
			CLK_SysTickDelay(1000000);
			PC -> DOUT ^= 1 << 13;
			CLK_SysTickDelay(1000000);
			PC -> DOUT ^= 1 << 14;
			CLK_SysTickDelay(1000000);
			PC -> DOUT ^= 1 << 15;
			CLK_SysTickDelay(1000000);
        }
}
