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
