/*--------------------------------------------------------------------------*/
/**@file     dv17.h
   @brief    芯片资源头文件
   @details
   @author
   @date    2016-6-14
   @note    DV17
*/
/*----------------------------------------------------------------------------*/

#ifndef _DV17_
#define _DV17_

//Note:
//there are 256 words(1024 bytes) in the sfr space
//byte(8bit)       SFR offset-address is:   0x03, 0x07, 0x0b, 0x0f, 0x13 ......
//half-word(16bit) SFR offset-address is:   0x02, 0x06, 0x0a, 0x0e, 0x12 ......
//word(24/32bit)   SFR offset-address is:   0x00, 0x04, 0x08, 0x0c, 0x10 ......

#define hs_base     0x3f90000
#define ls_base     0x3f80000

//===============================================================================//
//
//      high speed sfr address define
//
//===============================================================================//
#define ls_io_base    (ls_base + 0x000*4)
#define ls_uart_base  (ls_base + 0x100*4)
#define ls_spi_base   (ls_base + 0x200*4)
#define ls_sd_base    (ls_base + 0x300*4)
#define ls_tmr_base   (ls_base + 0x400*4)
#define ls_fusb_base  (ls_base + 0x500*4)
#define ls_husb_base  (ls_base + 0x600*4)
#define ls_adda_base  (ls_base + 0x700*4)
#define ls_clk_base   (ls_base + 0x800*4)
#define ls_oth_base   (ls_base + 0x900*4)
#define ls_alnk_base  (ls_base + 0xa00*4)
#define ls_pwm_base   (ls_base + 0xb00*4)
#define ls_plnk_base  (ls_base + 0xc00*4)

#define hs_sdr_base   (hs_base + 0x000*4)
#define hs_eva_base   (hs_base + 0x100*4)
#define hs_sfc_base   (hs_base + 0x300*4)
#define hs_jpg_base   (hs_base + 0x400*4)
#define hs_oth_base   (hs_base + 0x500*4)
#define hs_dbg_base   (hs_base + 0x600*4)
#define hs_jive_base  (hs_base + 0x800*4)
#define hs_jlmd_base  (hs_base + 0x900*4)
#define hs_hog_base   (hs_base + 0x980*4)
#define hs_paps_base  (hs_base + 0xa00*4)

#define FPGA_TRI                (*(volatile u32 *)(hs_dbg_base + 0xff*4))

#define PORTA_OUT               (*(volatile u32 *)(ls_io_base + 0x00*4))         //
#define PORTA_IN                (*(volatile u32 *)(ls_io_base + 0x01*4))         //Read Only
#define PORTA_DIR               (*(volatile u32 *)(ls_io_base + 0x02*4))         //
#define PORTA_DIE               (*(volatile u32 *)(ls_io_base + 0x03*4))         //
#define PORTA_PU                (*(volatile u32 *)(ls_io_base + 0x04*4))         //
#define PORTA_PD                (*(volatile u32 *)(ls_io_base + 0x05*4))         //
#define PORTA_HD0               (*(volatile u32 *)(ls_io_base + 0x06*4))         //
#define PORTA_HD1               (*(volatile u32 *)(ls_io_base + 0x07*4))         //

#define PORTB_OUT               (*(volatile u32 *)(ls_io_base + 0x10*4))         //
#define PORTB_IN                (*(volatile u32 *)(ls_io_base + 0x11*4))         //Read Only
#define PORTB_DIR               (*(volatile u32 *)(ls_io_base + 0x12*4))         //
#define PORTB_DIE               (*(volatile u32 *)(ls_io_base + 0x13*4))         //
#define PORTB_PU                (*(volatile u32 *)(ls_io_base + 0x14*4))         //
#define PORTB_PD                (*(volatile u32 *)(ls_io_base + 0x15*4))         //
#define PORTB_HD0               (*(volatile u32 *)(ls_io_base + 0x16*4))         //
#define PORTB_HD1               (*(volatile u32 *)(ls_io_base + 0x17*4))         //

#define PORTC_OUT               (*(volatile u32 *)(ls_io_base + 0x20*4))         //
#define PORTC_IN                (*(volatile u32 *)(ls_io_base + 0x21*4))         //Read Only
#define PORTC_DIR               (*(volatile u32 *)(ls_io_base + 0x22*4))         //
#define PORTC_DIE               (*(volatile u32 *)(ls_io_base + 0x23*4))         //
#define PORTC_PU                (*(volatile u32 *)(ls_io_base + 0x24*4))         //
#define PORTC_PD                (*(volatile u32 *)(ls_io_base + 0x25*4))         //
#define PORTC_HD                (*(volatile u32 *)(ls_io_base + 0x26*4))         //

#define PORTD_OUT               (*(volatile u32 *)(ls_io_base + 0x30*4))         //
#define PORTD_IN                (*(volatile u32 *)(ls_io_base + 0x31*4))         //Read Only
#define PORTD_DIR               (*(volatile u32 *)(ls_io_base + 0x32*4))         //
#define PORTD_DIE               (*(volatile u32 *)(ls_io_base + 0x33*4))         //
#define PORTD_PU                (*(volatile u32 *)(ls_io_base + 0x34*4))         //
#define PORTD_PD                (*(volatile u32 *)(ls_io_base + 0x35*4))         //
#define PORTD_HD                (*(volatile u32 *)(ls_io_base + 0x36*4))         //

#define PORTE_OUT               (*(volatile u32 *)(ls_io_base + 0x40*4))         //
#define PORTE_IN                (*(volatile u32 *)(ls_io_base + 0x41*4))         //Read Only
#define PORTE_DIR               (*(volatile u32 *)(ls_io_base + 0x42*4))         //
#define PORTE_DIE               (*(volatile u32 *)(ls_io_base + 0x43*4))         //
#define PORTE_PU                (*(volatile u32 *)(ls_io_base + 0x44*4))         //
#define PORTE_PD                (*(volatile u32 *)(ls_io_base + 0x45*4))         //
#define PORTE_HD0               (*(volatile u32 *)(ls_io_base + 0x46*4))         //
#define PORTE_HD1               (*(volatile u32 *)(ls_io_base + 0x47*4))         //

#define PORTG_OUT               (*(volatile u32 *)(ls_io_base + 0x50*4))         //
#define PORTG_IN                (*(volatile u32 *)(ls_io_base + 0x51*4))         //Read Only
#define PORTG_DIR               (*(volatile u32 *)(ls_io_base + 0x52*4))         //
#define PORTG_DIE               (*(volatile u32 *)(ls_io_base + 0x53*4))         //
#define PORTG_PU                (*(volatile u32 *)(ls_io_base + 0x54*4))         //
#define PORTG_PD                (*(volatile u32 *)(ls_io_base + 0x55*4))         //
#define PORTG_HD0               (*(volatile u32 *)(ls_io_base + 0x56*4))         //
#define PORTG_HD1               (*(volatile u32 *)(ls_io_base + 0x57*4))         //

#define PORTH_OUT               (*(volatile u32 *)(ls_io_base + 0x60*4))         //
#define PORTH_IN                (*(volatile u32 *)(ls_io_base + 0x61*4))         //Read Only
#define PORTH_DIR               (*(volatile u32 *)(ls_io_base + 0x62*4))         //
#define PORTH_DIE               (*(volatile u32 *)(ls_io_base + 0x63*4))         //
#define PORTH_PU                (*(volatile u32 *)(ls_io_base + 0x64*4))         //
#define PORTH_PD                (*(volatile u32 *)(ls_io_base + 0x65*4))         //
#define PORTH_HD0               (*(volatile u32 *)(ls_io_base + 0x66*4))         //
#define PORTH_HD1               (*(volatile u32 *)(ls_io_base + 0x67*4))         //

#define IOMC0                   (*(volatile u32 *)(ls_io_base + 0x70*4))         //
#define IOMC1                   (*(volatile u32 *)(ls_io_base + 0x71*4))         //
#define IOMC2                   (*(volatile u32 *)(ls_io_base + 0x72*4))         //
#define IOMC3                   (*(volatile u32 *)(ls_io_base + 0x73*4))         //
#define IOMC4                   (*(volatile u32 *)(ls_io_base + 0x74*4))         //
#define IOMC5                   (*(volatile u32 *)(ls_io_base + 0x75*4))         //
#define WKUP_CON0               (*(volatile u32 *)(ls_io_base + 0x76*4))         //
#define WKUP_CON1               (*(volatile u32 *)(ls_io_base + 0x77*4))         //
#define WKUP_CON2               (*(volatile u32 *)(ls_io_base + 0x78*4))         //write only;
#define WKUP_CON3               (*(volatile u32 *)(ls_io_base + 0x79*4))         //


#define UT0_HRXCNT              (*(volatile u32 *)(ls_uart_base + 0x00*4))
#define UT0_OTCNT               (*(volatile u32 *)(ls_uart_base + 0x01*4))
#define UT0_TXADR               (*(volatile u32 *)(ls_uart_base + 0x02*4))       //26bit write only;
#define UT0_TXCNT               (*(volatile u32 *)(ls_uart_base + 0x03*4))
#define UT0_RXEADR              (*(volatile u32 *)(ls_uart_base + 0x04*4))       //26bit write only;
#define UT0_CON                 (*(volatile u32 *)(ls_uart_base + 0x05*4))
#define UT0_BUF                 (*(volatile u8  *)(ls_uart_base + 0x06*4))
#define UT0_BAUD                (*(volatile u32 *)(ls_uart_base + 0x07*4))       //16bit write only;
#define UT0_RXSADR              (*(volatile u32 *)(ls_uart_base + 0x08*4))       //26bit write only;
#define UT0_RXCNT               (*(volatile u32 *)(ls_uart_base + 0x09*4))

#define UT1_HRXCNT              (*(volatile u32 *)(ls_uart_base + 0x10*4))
#define UT1_OTCNT               (*(volatile u32 *)(ls_uart_base + 0x11*4))
#define UT1_TXADR               (*(volatile u32 *)(ls_uart_base + 0x12*4))       //26bit write only;
#define UT1_TXCNT               (*(volatile u32 *)(ls_uart_base + 0x13*4))
#define UT1_RXEADR              (*(volatile u32 *)(ls_uart_base + 0x14*4))       //26bit write only;
#define UT1_CON                 (*(volatile u32 *)(ls_uart_base + 0x15*4))
#define UT1_BUF                 (*(volatile u8  *)(ls_uart_base + 0x16*4))
#define UT1_BAUD                (*(volatile u32 *)(ls_uart_base + 0x17*4))       //16bit write only;
#define UT1_RXSADR              (*(volatile u32 *)(ls_uart_base + 0x18*4))       //26bit write only;
#define UT1_RXCNT               (*(volatile u32 *)(ls_uart_base + 0x19*4))

#define UT2_CON                 (*(volatile u32 *)(ls_uart_base + 0x20*4))
#define UT2_BUF                 (*(volatile u8  *)(ls_uart_base + 0x21*4))
#define UT2_BAUD                (*(volatile u32 *)(ls_uart_base + 0x22*4))       //write only;
#define UT3_HRXCNT              (*(volatile u32 *)(ls_uart_base + 0x30*4))
#define UT3_OTCNT               (*(volatile u32 *)(ls_uart_base + 0x31*4))
#define UT3_TXADR               (*(volatile u32 *)(ls_uart_base + 0x32*4))       //26bit write only;
#define UT3_TXCNT               (*(volatile u32 *)(ls_uart_base + 0x33*4))
#define UT3_RXEADR              (*(volatile u32 *)(ls_uart_base + 0x34*4))       //26bit write only;
#define UT3_CON                 (*(volatile u32 *)(ls_uart_base + 0x35*4))
#define UT3_BUF                 (*(volatile u8  *)(ls_uart_base + 0x36*4))
#define UT3_BAUD                (*(volatile u32 *)(ls_uart_base + 0x37*4))       //16bit write only;
#define UT3_RXSADR              (*(volatile u32 *)(ls_uart_base + 0x38*4))       //26bit write only;
#define UT3_RXCNT               (*(volatile u32 *)(ls_uart_base + 0x39*4))


//SPI
#define SPI0_CON                (*(volatile u32 *)(ls_spi_base + 0x00*4))
#define SPI0_BAUD               (*(volatile u8  *)(ls_spi_base + 0x01*4))
#define SPI0_BUF                (*(volatile u8  *)(ls_spi_base + 0x02*4))
#define SPI0_ADR                (*(volatile u32 *)(ls_spi_base + 0x03*4))       //26bit write only;
#define SPI0_CNT                (*(volatile u32 *)(ls_spi_base + 0x04*4))       //write only;

#define SPI1_CON                (*(volatile u32 *)(ls_spi_base + 0x10*4))
#define SPI1_BAUD               (*(volatile u8  *)(ls_spi_base + 0x11*4))
#define SPI1_BUF                (*(volatile u8  *)(ls_spi_base + 0x12*4))
#define SPI1_ADR                (*(volatile u32 *)(ls_spi_base + 0x13*4))       //26bit write only;
#define SPI1_CNT                (*(volatile u32 *)(ls_spi_base + 0x14*4))       //write only;

#define SPI2_CON                (*(volatile u32 *)(ls_spi_base + 0x20*4))
#define SPI2_BAUD               (*(volatile u8  *)(ls_spi_base + 0x21*4))
#define SPI2_BUF                (*(volatile u8  *)(ls_spi_base + 0x22*4))
#define SPI2_ADR                (*(volatile u32 *)(ls_spi_base + 0x23*4))       //26bit write only;
#define SPI2_CNT                (*(volatile u32 *)(ls_spi_base + 0x24*4))       //write only;

//SD
#define SD0_CON0                (*(volatile u32 *)(ls_sd_base + 0x00*4))
#define SD0_CON1                (*(volatile u32 *)(ls_sd_base + 0x01*4))
#define SD0_CON2                (*(volatile u32 *)(ls_sd_base + 0x02*4))
#define SD0_CPTR                (*(volatile u32 *)(ls_sd_base + 0x03*4))        //26bit write only;
#define SD0_DPTR                (*(volatile u32 *)(ls_sd_base + 0x04*4))        //26bit write only;
#define SD0_CTU_CON             (*(volatile u32 *)(ls_sd_base + 0x05*4))
#define SD0_CTU_CNT             (*(volatile u32 *)(ls_sd_base + 0x06*4))

#define SD1_CON0                (*(volatile u32 *)(ls_sd_base + 0x10*4))
#define SD1_CON1                (*(volatile u32 *)(ls_sd_base + 0x11*4))
#define SD1_CON2                (*(volatile u32 *)(ls_sd_base + 0x12*4))
#define SD1_CPTR                (*(volatile u32 *)(ls_sd_base + 0x13*4))        //nbit
#define SD1_DPTR                (*(volatile u32 *)(ls_sd_base + 0x14*4))        //nbit
#define SD1_CTU_CON             (*(volatile u32 *)(ls_sd_base + 0x15*4))
#define SD1_CTU_CNT             (*(volatile u32 *)(ls_sd_base + 0x16*4))

#define SD2_CON0                (*(volatile u32 *)(ls_sd_base + 0x20*4))
#define SD2_CON1                (*(volatile u32 *)(ls_sd_base + 0x21*4))
#define SD2_CON2                (*(volatile u32 *)(ls_sd_base + 0x22*4))
#define SD2_CPTR                (*(volatile u32 *)(ls_sd_base + 0x23*4))        //nbit
#define SD2_DPTR                (*(volatile u32 *)(ls_sd_base + 0x24*4))        //nbit
#define SD2_CTU_CON             (*(volatile u32 *)(ls_sd_base + 0x25*4))
#define SD2_CTU_CNT             (*(volatile u32 *)(ls_sd_base + 0x26*4))

//TIMER

#define T0_CON                  (*(volatile u32 *)(ls_tmr_base + 0x00*4))
#define T0_CNT                  (*(volatile u32 *)(ls_tmr_base + 0x01*4))
#define T0_PRD                  (*(volatile u32 *)(ls_tmr_base + 0x02*4))
#define T0_PWM                  (*(volatile u32 *)(ls_tmr_base + 0x03*4))

#define T1_CON                  (*(volatile u32 *)(ls_tmr_base + 0x10*4))
#define T1_CNT                  (*(volatile u32 *)(ls_tmr_base + 0x11*4))
#define T1_PRD                  (*(volatile u32 *)(ls_tmr_base + 0x12*4))
#define T1_PWM                  (*(volatile u32 *)(ls_tmr_base + 0x13*4))

#define T2_CON                  (*(volatile u32 *)(ls_tmr_base + 0x20*4))
#define T2_CNT                  (*(volatile u32 *)(ls_tmr_base + 0x21*4))
#define T2_PRD                  (*(volatile u32 *)(ls_tmr_base + 0x22*4))
#define T2_PWM                  (*(volatile u32 *)(ls_tmr_base + 0x23*4))

#define T3_CON                  (*(volatile u32 *)(ls_tmr_base + 0x30*4))
#define T3_CNT                  (*(volatile u32 *)(ls_tmr_base + 0x31*4))
#define T3_PRD                  (*(volatile u32 *)(ls_tmr_base + 0x32*4))
#define T3_PWM                  (*(volatile u32 *)(ls_tmr_base + 0x33*4))

#define T4_CON                  (*(volatile u32 *)(ls_tmr_base + 0x40*4))
#define T4_CNT                  (*(volatile u32 *)(ls_tmr_base + 0x41*4))
#define T4_PRD                  (*(volatile u32 *)(ls_tmr_base + 0x42*4))
#define T4_PWM                  (*(volatile u32 *)(ls_tmr_base + 0x43*4))

//PWM
#define PWMTMR0CON              (*(volatile u32 *)(ls_pwm_base + 0x00*4))
#define PWMTMR0CNT              (*(volatile u32 *)(ls_pwm_base + 0x01*4))
#define PWMTMR0PR               (*(volatile u32 *)(ls_pwm_base + 0x02*4))
#define PWMCMP0                 (*(volatile u32 *)(ls_pwm_base + 0x03*4))
#define PWMTMR1CON              (*(volatile u32 *)(ls_pwm_base + 0x04*4))
#define PWMTMR1CNT              (*(volatile u32 *)(ls_pwm_base + 0x05*4))
#define PWMTMR1PR               (*(volatile u32 *)(ls_pwm_base + 0x06*4))
#define PWMCMP1                 (*(volatile u32 *)(ls_pwm_base + 0x07*4))
#define PWMTMR2CON              (*(volatile u32 *)(ls_pwm_base + 0x08*4))
#define PWMTMR2CNT              (*(volatile u32 *)(ls_pwm_base + 0x09*4))
#define PWMTMR2PR               (*(volatile u32 *)(ls_pwm_base + 0x0a*4))
#define PWMCMP2                 (*(volatile u32 *)(ls_pwm_base + 0x0b*4))
#define PWMTMR3CON              (*(volatile u32 *)(ls_pwm_base + 0x0c*4))
#define PWMTMR3CNT              (*(volatile u32 *)(ls_pwm_base + 0x0d*4))
#define PWMTMR3PR               (*(volatile u32 *)(ls_pwm_base + 0x0e*4))
#define PWMCMP3                 (*(volatile u32 *)(ls_pwm_base + 0x0f*4))
#define PWMTMR4CON              (*(volatile u32 *)(ls_pwm_base + 0x10*4))
#define PWMTMR4CNT              (*(volatile u32 *)(ls_pwm_base + 0x11*4))
#define PWMTMR4PR               (*(volatile u32 *)(ls_pwm_base + 0x12*4))
#define PWMCMP4                 (*(volatile u32 *)(ls_pwm_base + 0x13*4))
#define PWMTMR5CON              (*(volatile u32 *)(ls_pwm_base + 0x14*4))
#define PWMTMR5CNT              (*(volatile u32 *)(ls_pwm_base + 0x15*4))
#define PWMTMR5PR               (*(volatile u32 *)(ls_pwm_base + 0x16*4))
#define PWMCMP5                 (*(volatile u32 *)(ls_pwm_base + 0x17*4))
#define PWMTMR6CON              (*(volatile u32 *)(ls_pwm_base + 0x18*4))
#define PWMTMR6CNT              (*(volatile u32 *)(ls_pwm_base + 0x19*4))
#define PWMTMR6PR               (*(volatile u32 *)(ls_pwm_base + 0x1a*4))
#define PWMCMP6                 (*(volatile u32 *)(ls_pwm_base + 0x1b*4))
#define PWMTMR7CON              (*(volatile u32 *)(ls_pwm_base + 0x1c*4))
#define PWMTMR7CNT              (*(volatile u32 *)(ls_pwm_base + 0x1d*4))
#define PWMTMR7PR               (*(volatile u32 *)(ls_pwm_base + 0x1e*4))
#define PWMCMP7                 (*(volatile u32 *)(ls_pwm_base + 0x1f*4))

#define PWMCON0                 (*(volatile u32 *)(ls_pwm_base + 0x20*4))
#define PWMCON1                 (*(volatile u32 *)(ls_pwm_base + 0x21*4))

//FUSB
#define FUSB_CON0               (*(volatile u32 *)(ls_fusb_base + 0x00*4))
#define FUSB_CON1               (*(volatile u32 *)(ls_fusb_base + 0x01*4))
#define FUSB_EP0_CNT            (*(volatile u32 *)(ls_fusb_base + 0x02*4))      //write only;
#define FUSB_EP1_CNT            (*(volatile u32 *)(ls_fusb_base + 0x03*4))      //write only;
#define FUSB_EP2_CNT            (*(volatile u32 *)(ls_fusb_base + 0x04*4))      //write only;
#define FUSB_EP3_CNT            (*(volatile u32 *)(ls_fusb_base + 0x05*4))      //write only;
#define FUSB_EP0_ADR            (*(volatile u32 *)(ls_fusb_base + 0x06*4))      //26bit write only;
#define FUSB_EP1_TADR           (*(volatile u32 *)(ls_fusb_base + 0x07*4))      //write only;
#define FUSB_EP1_RADR           (*(volatile u32 *)(ls_fusb_base + 0x08*4))      //write only;
#define FUSB_EP2_TADR           (*(volatile u32 *)(ls_fusb_base + 0x09*4))      //write only;
#define FUSB_EP2_RADR           (*(volatile u32 *)(ls_fusb_base + 0x0a*4))      //write only;
#define FUSB_EP3_TADR           (*(volatile u32 *)(ls_fusb_base + 0x0b*4))      //write only;
#define FUSB_EP3_RADR           (*(volatile u32 *)(ls_fusb_base + 0x0c*4))      //write only;
#define FUSB_IO_CON0            (*(volatile u32 *)(ls_fusb_base + 0x0d*4))      //

#define HUSB0_SIE_CON            (*(volatile u32 *)(ls_husb_base + 0x00*4))
#define HUSB0_EP0_CNT            (*(volatile u32 *)(ls_husb_base + 0x01*4))      //write only;
#define HUSB0_EP1_CNT            (*(volatile u32 *)(ls_husb_base + 0x02*4))      //write only;
#define HUSB0_EP2_CNT            (*(volatile u32 *)(ls_husb_base + 0x03*4))      //write only;
#define HUSB0_EP3_CNT            (*(volatile u32 *)(ls_husb_base + 0x04*4))      //write only;
#define HUSB0_EP4_CNT            (*(volatile u32 *)(ls_husb_base + 0x05*4))      //write only;
#define HUSB0_EP5_CNT            (*(volatile u32 *)(ls_husb_base + 0x06*4))      //write only;
#define HUSB0_EP6_CNT            (*(volatile u32 *)(ls_husb_base + 0x07*4))      //write only;
#define HUSB0_EP0_ADR            (*(volatile u32 *)(ls_husb_base + 0x08*4))      //26bit write only;
#define HUSB0_EP1_TADR           (*(volatile u32 *)(ls_husb_base + 0x09*4))      //26bit write only;
#define HUSB0_EP1_RADR           (*(volatile u32 *)(ls_husb_base + 0x0a*4))      //26bit write only;
#define HUSB0_EP2_TADR           (*(volatile u32 *)(ls_husb_base + 0x0b*4))      //26bit write only;
#define HUSB0_EP2_RADR           (*(volatile u32 *)(ls_husb_base + 0x0c*4))      //26bit write only;
#define HUSB0_EP3_TADR           (*(volatile u32 *)(ls_husb_base + 0x0d*4))      //26bit write only;
#define HUSB0_EP3_RADR           (*(volatile u32 *)(ls_husb_base + 0x0e*4))      //26bit write only;
#define HUSB0_EP4_TADR           (*(volatile u32 *)(ls_husb_base + 0x0f*4))      //26bit write only;
#define HUSB0_EP4_RADR           (*(volatile u32 *)(ls_husb_base + 0x10*4))      //26bit write only;
#define HUSB0_EP5_TADR           (*(volatile u32 *)(ls_husb_base + 0x11*4))      //26bit write only;
#define HUSB0_EP5_RADR           (*(volatile u32 *)(ls_husb_base + 0x12*4))      //26bit write only;
#define HUSB0_EP6_TADR           (*(volatile u32 *)(ls_husb_base + 0x13*4))      //26bit write only;
#define HUSB0_EP6_RADR           (*(volatile u32 *)(ls_husb_base + 0x14*4))      //26bit write only;

#define HUSB_COM_CON0            (*(volatile u32 *)(ls_husb_base + 0x15*4))
#define HUSB0_PHY_CON0           (*(volatile u32 *)(ls_husb_base + 0x16*4))
#define HUSB0_PHY_CON1           (*(volatile u32 *)(ls_husb_base + 0x17*4))
#define HUSB0_PHY_CON2           (*(volatile u32 *)(ls_husb_base + 0x18*4))
#define HUSB0_ISO_CON0           (*(volatile u32 *)(ls_husb_base + 0x19*4))
#define HUSB0_ISO_CON1           (*(volatile u32 *)(ls_husb_base + 0x1a*4))

#define HUSB1_SIE_CON            (*(volatile u32 *)(ls_husb_base + 0x20*4))
#define HUSB1_EP0_CNT            (*(volatile u32 *)(ls_husb_base + 0x21*4))      //write only;
#define HUSB1_EP1_CNT            (*(volatile u32 *)(ls_husb_base + 0x22*4))      //write only;
#define HUSB1_EP2_CNT            (*(volatile u32 *)(ls_husb_base + 0x23*4))      //write only;
#define HUSB1_EP3_CNT            (*(volatile u32 *)(ls_husb_base + 0x24*4))      //write only;
#define HUSB1_EP4_CNT            (*(volatile u32 *)(ls_husb_base + 0x25*4))      //write only;
#define HUSB1_EP5_CNT            (*(volatile u32 *)(ls_husb_base + 0x26*4))      //write only;
#define HUSB1_EP6_CNT            (*(volatile u32 *)(ls_husb_base + 0x27*4))      //write only;
#define HUSB1_EP0_ADR            (*(volatile u32 *)(ls_husb_base + 0x28*4))      //26bit write only;
#define HUSB1_EP1_TADR           (*(volatile u32 *)(ls_husb_base + 0x29*4))      //26bit write only;
#define HUSB1_EP1_RADR           (*(volatile u32 *)(ls_husb_base + 0x2a*4))      //26bit write only;
#define HUSB1_EP2_TADR           (*(volatile u32 *)(ls_husb_base + 0x2b*4))      //26bit write only;
#define HUSB1_EP2_RADR           (*(volatile u32 *)(ls_husb_base + 0x2c*4))      //26bit write only;
#define HUSB1_EP3_TADR           (*(volatile u32 *)(ls_husb_base + 0x2d*4))      //26bit write only;
#define HUSB1_EP3_RADR           (*(volatile u32 *)(ls_husb_base + 0x2e*4))      //26bit write only;
#define HUSB1_EP4_TADR           (*(volatile u32 *)(ls_husb_base + 0x2f*4))      //26bit write only;
#define HUSB1_EP4_RADR           (*(volatile u32 *)(ls_husb_base + 0x30*4))      //26bit write only;
#define HUSB1_EP5_TADR           (*(volatile u32 *)(ls_husb_base + 0x31*4))      //26bit write only;
#define HUSB1_EP5_RADR           (*(volatile u32 *)(ls_husb_base + 0x32*4))      //26bit write only;
#define HUSB1_EP6_TADR           (*(volatile u32 *)(ls_husb_base + 0x33*4))      //26bit write only;
#define HUSB1_EP6_RADR           (*(volatile u32 *)(ls_husb_base + 0x34*4))      //26bit write only;

#define HUSB_COM_CON1            (*(volatile u32 *)(ls_husb_base + 0x35*4))
#define HUSB1_PHY_CON0           (*(volatile u32 *)(ls_husb_base + 0x36*4))
#define HUSB1_PHY_CON1           (*(volatile u32 *)(ls_husb_base + 0x37*4))
#define HUSB1_PHY_CON2           (*(volatile u32 *)(ls_husb_base + 0x38*4))
#define HUSB1_ISO_CON0           (*(volatile u32 *)(ls_husb_base + 0x39*4))
#define HUSB1_ISO_CON1           (*(volatile u32 *)(ls_husb_base + 0x3a*4))
#define HUSB_COM_CON2            (*(volatile u32 *)(ls_husb_base + 0x3b*4))

#define DAC_LEN                 (*(volatile u32 *)(ls_adda_base + 0x00*4))      //16bit write only;
#define DAC_CON                 (*(volatile u32 *)(ls_adda_base + 0x01*4))
#define DAC_CON1                (*(volatile u32 *)(ls_adda_base + 0x02*4))
#define DAC_ADR                 (*(volatile u32 *)(ls_adda_base + 0x03*4))      //26bit write only;
#define DAC_TRML                (*(volatile u32  *)(ls_adda_base + 0x04*4))      //8bit write only;
#define DAC_TRMR                (*(volatile u32  *)(ls_adda_base + 0x05*4))      //8bit write only;

#define ADC_CON                 (*(volatile u32 *)(ls_adda_base + 0x08*4))
#define ADC_ADR                 (*(volatile u32 *)(ls_adda_base + 0x0b*4))      //26bit write only;
#define ADC_LEN                 (*(volatile u32 *)(ls_adda_base + 0x0c*4))      //16bit write only;
#define ADC_RES12               (*(volatile u32 *)(ls_adda_base + 0x0d*4))      //12bit read only;

#define DAA_CON0                (*(volatile u32 *)(ls_adda_base + 0x10*4))
#define DAA_CON1                (*(volatile u32 *)(ls_adda_base + 0x11*4))
#define DAA_CON2                (*(volatile u32 *)(ls_adda_base + 0x12*4))
#define DAA_CON3                (*(volatile u32 *)(ls_adda_base + 0x13*4))      //15bit;
#define DAA_CON4                (*(volatile u32 *)(ls_adda_base + 0x14*4))      //15bit;
#define DAA_CON5                (*(volatile u32 *)(ls_adda_base + 0x15*4))      //15bit;

#define ADA_CON0                (*(volatile u32 *)(ls_adda_base + 0x20*4))      //15bit;
#define ADA_CON1                (*(volatile u32 *)(ls_adda_base + 0x21*4))      //15bit;
#define ADA_CON2                (*(volatile u32 *)(ls_adda_base + 0x22*4))      //15bit;

#define GPADC_CON               (*(volatile u32 *)(ls_adda_base + 0x30*4))
#define GPADC_RES               (*(volatile u32 *)(ls_adda_base + 0x31*4))      //10bit read only;

#define ALNK_CON0               (*(volatile u32 *)(ls_alnk_base + 0x00*4))
#define ALNK_CON1               (*(volatile u32 *)(ls_alnk_base + 0x01*4))
#define ALNK_CON2               (*(volatile u32 *)(ls_alnk_base + 0x02*4))
#define ALNK_CON3               (*(volatile u32 *)(ls_alnk_base + 0x03*4))
#define ALNK_ADR0               (*(volatile u32 *)(ls_alnk_base + 0x04*4))
#define ALNK_ADR1               (*(volatile u32 *)(ls_alnk_base + 0x05*4))
#define ALNK_ADR2               (*(volatile u32 *)(ls_alnk_base + 0x06*4))
#define ALNK_ADR3               (*(volatile u32 *)(ls_alnk_base + 0x07*4))
#define ALNK_LEN                (*(volatile u32 *)(ls_alnk_base + 0x08*4))

#define PLNK_CON                (*(volatile u32 *)(ls_plnk_base + 0x00*4))
#define PLNK_SMR                (*(volatile u32 *)(ls_plnk_base + 0x01*4))
#define PLNK_ADR                (*(volatile u32 *)(ls_plnk_base + 0x02*4))
#define PLNK_LEN                (*(volatile u32 *)(ls_plnk_base + 0x03*4))

#define PWR_CON                 (*(volatile u8  *)(ls_clk_base + 0x00*4))
#define CLK_CON0                (*(volatile u32 *)(ls_clk_base + 0x01*4))
#define CLK_CON1                (*(volatile u32 *)(ls_clk_base + 0x02*4))
#define CLK_CON2                (*(volatile u32 *)(ls_clk_base + 0x03*4))
#define LCLK_GAT                (*(volatile u32 *)(ls_clk_base + 0x04*4))
#define HCLK_GAT                (*(volatile u32 *)(ls_clk_base + 0x05*4))
#define ACLK_GAT                (*(volatile u32 *)(ls_clk_base + 0x06*4))
#define PLL0_NF                 (*(volatile u32 *)(ls_clk_base + 0x07*4))
#define PLL0_NR                 (*(volatile u32 *)(ls_clk_base + 0x08*4))
#define PLL1_NF                 (*(volatile u32 *)(ls_clk_base + 0x09*4))
#define PLL1_NR                 (*(volatile u32 *)(ls_clk_base + 0x0a*4))
#define OSA_CON                 (*(volatile u32 *)(ls_clk_base + 0x0b*4))
#define PLL_CON0                (*(volatile u32 *)(ls_clk_base + 0x0c*4))
#define PLL_CON1                (*(volatile u32 *)(ls_clk_base + 0x0d*4))
#define PLL_CON2                (*(volatile u32 *)(ls_clk_base + 0x0e*4))
#define PLL3_CON                (*(volatile u32 *)(ls_clk_base + 0x0f*4))
#define PLL3_NF                 (*(volatile u32 *)(ls_clk_base + 0x10*4))
#define PLL3_NR                 (*(volatile u32 *)(ls_clk_base + 0x11*4))

#define HTC_CON                 (*(volatile u32 *)(ls_oth_base + 0x00*4))
#define LDO_CON                 (*(volatile u32 *)(ls_oth_base + 0x01*4))
#define LVD_CON                 (*(volatile u32 *)(ls_oth_base + 0x02*4))
#define IRTC_CON                (*(volatile u32 *)(ls_oth_base + 0x03*4))
#define IRTC_BUF                (*(volatile u8  *)(ls_oth_base + 0x04*4))
#define MODE_CON                (*(volatile u8  *)(ls_oth_base + 0x05*4))
#define CRC0_FIFO               (*(volatile u8  *)(ls_oth_base + 0x06*4))       //write only;
#define CRC0_REG                (*(volatile u32 *)(ls_oth_base + 0x07*4))
#define WDT_CON                 (*(volatile u8  *)(ls_oth_base + 0x08*4))
#define CHIP_ID                 (*(volatile u8  *)(ls_oth_base + 0x09*4))       //read only;
#define IRFLT_CON               (*(volatile u8  *)(ls_oth_base + 0x0a*4))
#define IIC0_CON                (*(volatile u32 *)(ls_oth_base + 0x0b*4))
#define IIC0_BUF                (*(volatile u8  *)(ls_oth_base + 0x0c*4))
#define IIC0_BAUD               (*(volatile u8  *)(ls_oth_base + 0x0d*4))
#define IIC0_DMA_ADR            (*(volatile u32 *)(ls_oth_base + 0x0e*4))       //write only
#define IIC0_DMA_CNT            (*(volatile u32 *)(ls_oth_base + 0x0f*4))       //write only
#define IIC0_DMA_NRATE          (*(volatile u32 *)(ls_oth_base + 0x10*4))       //write only
#define IIC1_CON                (*(volatile u32 *)(ls_oth_base + 0x11*4))
#define IIC1_BUF                (*(volatile u8  *)(ls_oth_base + 0x12*4))
#define IIC1_BAUD               (*(volatile u8  *)(ls_oth_base + 0x13*4))
#define IIC1_DMA_ADR            (*(volatile u32 *)(ls_oth_base + 0x14*4))       //write only
#define IIC1_DMA_CNT            (*(volatile u32 *)(ls_oth_base + 0x15*4))       //write only
#define IIC1_DMA_NRATE          (*(volatile u32 *)(ls_oth_base + 0x16*4))       //write only
#define PWM8_CON                (*(volatile u32 *)(ls_oth_base + 0x17*4))
#define PAP_CON                 (*(volatile u32 *)(ls_oth_base + 0x18*4))
#define PAP_BUF                 (*(volatile u32 *)(ls_oth_base + 0x19*4))
#define PAP_ADR                 (*(volatile u32 *)(ls_oth_base + 0x1a*4))       //26bit write only;
#define PAP_CNT                 (*(volatile u32 *)(ls_oth_base + 0x1b*4))       //write only;
#define PAP_DAT0                (*(volatile u32 *)(ls_oth_base + 0x1c*4))       //write only;
#define PAP_DAT1                (*(volatile u32 *)(ls_oth_base + 0x1d*4))       //write only;

#define EFUSE_MPW_CON           (*(volatile u32 *)(ls_oth_base + 0x1f*4))
#define EFUSE_CON               (*(volatile u32 *)(ls_oth_base + 0x20*4))
#define MPUCON                  (*(volatile u32 *)(ls_oth_base + 0x21*4))
#define MPUSTART                (*(volatile u32 *)(ls_oth_base + 0x22*4))
#define MPUEND                  (*(volatile u32 *)(ls_oth_base + 0x23*4))
#define MPUCATCH0               (*(volatile u32 *)(ls_oth_base + 0x24*4))
#define MPUCATCH1               (*(volatile u32 *)(ls_oth_base + 0x25*4))
#define PLCNTCON                (*(volatile u8  *)(ls_oth_base + 0x26*4))
#define PLCNTVL                 (*(volatile u32 *)(ls_oth_base + 0x27*4))       //read only;
#define CS_CON                  (*(volatile u32 *)(ls_oth_base + 0x28*4))
#define CS_REG                  (*(volatile u32 *)(ls_oth_base + 0x29*4))
#define CS_FIFO                 (*(volatile u32 *)(ls_oth_base + 0x2a*4))       //write only
#define CS_RADR                 (*(volatile u32 *)(ls_oth_base + 0x2b*4))       //write only
#define CS_RCNT                 (*(volatile u32 *)(ls_oth_base + 0x2c*4))       //write only
#define RAND64L                 (*(volatile u32 *)(ls_oth_base + 0x2d*4))       //read only
#define RAND64H                 (*(volatile u32 *)(ls_oth_base + 0x2e*4))       //read only
#define ETHCON                  (*(volatile u32 *)(ls_oth_base + 0x2f*4))       //read only

#define SHA_CON                 (*(volatile u32 *)(ls_oth_base + 0x30*4))
#define SHA_ADR                 (*(volatile u32 *)(ls_oth_base + 0x31*4))
#define SHA_RES0               (*(volatile u32 *)(ls_oth_base + 0x32*4))
#define SHA_RES1               (*(volatile u32 *)(ls_oth_base + 0x33*4))
#define SHA_RES2               (*(volatile u32 *)(ls_oth_base + 0x34*4))
#define SHA_RES3               (*(volatile u32 *)(ls_oth_base + 0x35*4))
#define SHA_RES4               (*(volatile u32 *)(ls_oth_base + 0x36*4))
#define SHA_RES5               (*(volatile u32 *)(ls_oth_base + 0x37*4))
#define SHA_RES6               (*(volatile u32 *)(ls_oth_base + 0x38*4))
#define SHA_RES7               (*(volatile u32 *)(ls_oth_base + 0x39*4))

#define CRC1_FIFO               (*(volatile u8  *)(ls_oth_base + 0x40*4))       //write only;
#define CRC1_REG                (*(volatile u32 *)(ls_oth_base + 0x41*4))

#define RINGOSC_CON             (*(volatile u32 *)(ls_oth_base + 0x42*4))
#define MBIST_CON               (*(volatile u32 *)(ls_oth_base + 0x43*4))

#define SDRACON0                (*(volatile u32 *)(ls_oth_base + 0x50*4))       //32bit
#define SDRACON1                (*(volatile u32 *)(ls_oth_base + 0x51*4))       //32bit
#define SDRACON2                (*(volatile u32 *)(ls_oth_base + 0x52*4))       //32bit
#define SDRACON3                (*(volatile u32 *)(ls_oth_base + 0x53*4))       //32bit
#define UDLLCON0                (*(volatile u32 *)(ls_oth_base + 0x54*4))       //32bit
#define UDLLCON1                (*(volatile u32 *)(ls_oth_base + 0x55*4))       //32bit
#define LDLLCON0                (*(volatile u32 *)(ls_oth_base + 0x56*4))       //32bit
#define LDLLCON1                (*(volatile u32 *)(ls_oth_base + 0x57*4))       //32bit



//===============================================================================//
//
//      high speed sfr address define
//
//===============================================================================//
#define SDRCON0                 (*(volatile u32 *)(hs_sdr_base + 0x00*4))
//#define SDRSPTR                 (*(volatile u32 *)(hs_sdr_base + 0x01*4))
//#define SDRQPTR                 (*(volatile u32 *)(hs_sdr_base + 0x02*4))
#define SDRREFREG               (*(volatile u32 *)(hs_sdr_base + 0x03*4))       //write only
//#define SDRDMACNT               (*(volatile u32 *)(hs_sdr_base + 0x04*4))
#define SDRCON1                 (*(volatile u32 *)(hs_sdr_base + 0x05*4))       //write only
#define SDRREFSUM               (*(volatile u32 *)(hs_sdr_base + 0x06*4))       //13bit
#define SDRDBG                  (*(volatile u32 *)(hs_sdr_base + 0x07*4))       //32bit
#define SDRCON2                 (*(volatile u32 *)(hs_sdr_base + 0x08*4))       //write only 32bit
#define SDRCON3                 (*(volatile u32 *)(hs_sdr_base + 0x09*4))       //32bit
#define SDRCON4                 (*(volatile u32 *)(hs_sdr_base + 0x0a*4))       //32bit
#define SDRCON5                 (*(volatile u32 *)(hs_sdr_base + 0x0b*4))       //write only 32bit
#define SDRCON6                 (*(volatile u32 *)(hs_sdr_base + 0x0c*4))       //32bit
#define SDRCON7                 (*(volatile u32 *)(hs_sdr_base + 0x0d*4))       //32bit
#define SDRCON8                 (*(volatile u32 *)(hs_sdr_base + 0x0e*4))       //32bit
#define SDRCON9                 (*(volatile u32 *)(hs_sdr_base + 0x0f*4))       //32bit

//eva
#define EVA_CON                 (*(volatile u32 *)(hs_eva_base + 0x00*4))

//SFC
#define SFC_CON                 (*(volatile u32 *)(hs_sfc_base + 0x00*4))
#define SFC_BAUD                (*(volatile u32 *)(hs_sfc_base + 0x01*4))

#define SFC_BASE_ADR            (*(volatile u32 *)(hs_sfc_base + 0x03*4))

//==============================================================//
//  cpu
//==============================================================//

#define cpu_base 0x3f97000

#define CC_SOFT_INT             (*(volatile u32 *)(cpu_base + 0x00*4))
#define CC_SOFT_SET             (*(volatile u32 *)(cpu_base + 0x01*4))
#define CC_SOFT_CLR             (*(volatile u32 *)(cpu_base + 0x02*4))
#define CC_CON                  (*(volatile u32 *)(cpu_base + 0x03*4))
#define CC_MBISTCTL             (*(volatile u32 *)(cpu_base + 0x0f*4))

#define C0_CON                  (*(volatile u32 *)(cpu_base + 0x10*4))       //
#define C1_CON                  (*(volatile u32 *)(cpu_base + 0x11*4))       //
#define CACHE_CON               (*(volatile u32 *)(cpu_base + 0x12*4))       //
#define CACHE_WAY               (*(volatile u32 *)(cpu_base + 0x13*4))       //
#define C0_PC_LIMH              (*(volatile u32 *)(cpu_base + 0x14*4))       // for debug only
#define C0_PC_LIML              (*(volatile u32 *)(cpu_base + 0x15*4))       // for debug only
#define C1_PC_LIMH              (*(volatile u32 *)(cpu_base + 0x16*4))       // for debug only
#define C1_PC_LIML              (*(volatile u32 *)(cpu_base + 0x17*4))       // for debug only

#define C2_CON                  (*(volatile u32 *)(cpu_base + 0x20*4))       //
#define C3_CON                  (*(volatile u32 *)(cpu_base + 0x21*4))       //
#define C23_CACHE_CON           (*(volatile u32 *)(cpu_base + 0x22*4))       //
#define C23_CACHE_WAY           (*(volatile u32 *)(cpu_base + 0x23*4))       //
#define C2_PC_LIMH              (*(volatile u32 *)(cpu_base + 0x24*4))       // for debug only
#define C2_PC_LIML              (*(volatile u32 *)(cpu_base + 0x25*4))       // for debug only
#define C3_PC_LIMH              (*(volatile u32 *)(cpu_base + 0x26*4))       // for debug only
#define C3_PC_LIML              (*(volatile u32 *)(cpu_base + 0x27*4))       // for debug only

#define WR_LIM0H                (*(volatile u32 *)(cpu_base + 0x30*4))       // for debug only
#define WR_LIM0L                (*(volatile u32 *)(cpu_base + 0x31*4))       // for debug only
#define WR_LIM1H                (*(volatile u32 *)(cpu_base + 0x32*4))       // for debug only
#define WR_LIM1L                (*(volatile u32 *)(cpu_base + 0x33*4))       // for debug only
#define WR_LIM2H                (*(volatile u32 *)(cpu_base + 0x34*4))       // for debug only
#define WR_LIM2L                (*(volatile u32 *)(cpu_base + 0x35*4))       // for debug only
#define WR_LIM3H                (*(volatile u32 *)(cpu_base + 0x36*4))       // for debug only
#define WR_LIM3L                (*(volatile u32 *)(cpu_base + 0x37*4))       // for debug only
#define WR_LIM4H                (*(volatile u32 *)(cpu_base + 0x38*4))       // for debug only
#define WR_LIM4L                (*(volatile u32 *)(cpu_base + 0x39*4))       // for debug only
#define WR_LIM5H                (*(volatile u32 *)(cpu_base + 0x3a*4))       // for debug only
#define WR_LIM5L                (*(volatile u32 *)(cpu_base + 0x3b*4))       // for debug only
#define WR_LIM6H                (*(volatile u32 *)(cpu_base + 0x3c*4))       // for debug only
#define WR_LIM6L                (*(volatile u32 *)(cpu_base + 0x3d*4))       // for debug only
#define WR_LIM7H                (*(volatile u32 *)(cpu_base + 0x3e*4))       // for debug only
#define WR_LIM7L                (*(volatile u32 *)(cpu_base + 0x3f*4))       // for debug only

#define C23_WR_LIM0H            (*(volatile u32 *)(cpu_base + 0x40*4))       // for debug only
#define C23_WR_LIM0L            (*(volatile u32 *)(cpu_base + 0x41*4))       // for debug only
#define C23_WR_LIM1H            (*(volatile u32 *)(cpu_base + 0x42*4))       // for debug only
#define C23_WR_LIM1L            (*(volatile u32 *)(cpu_base + 0x43*4))       // for debug only
#define C23_WR_LIM2H            (*(volatile u32 *)(cpu_base + 0x44*4))       // for debug only
#define C23_WR_LIM2L            (*(volatile u32 *)(cpu_base + 0x45*4))       // for debug only
#define C23_WR_LIM3H            (*(volatile u32 *)(cpu_base + 0x46*4))       // for debug only
#define C23_WR_LIM3L            (*(volatile u32 *)(cpu_base + 0x47*4))       // for debug only
#define C23_WR_LIM4H            (*(volatile u32 *)(cpu_base + 0x48*4))       // for debug only
#define C23_WR_LIM4L            (*(volatile u32 *)(cpu_base + 0x49*4))       // for debug only
#define C23_WR_LIM5H            (*(volatile u32 *)(cpu_base + 0x4a*4))       // for debug only
#define C23_WR_LIM5L            (*(volatile u32 *)(cpu_base + 0x4b*4))       // for debug only
#define C23_WR_LIM6H            (*(volatile u32 *)(cpu_base + 0x4c*4))       // for debug only
#define C23_WR_LIM6L            (*(volatile u32 *)(cpu_base + 0x4d*4))       // for debug only
#define C23_WR_LIM7H            (*(volatile u32 *)(cpu_base + 0x4e*4))       // for debug only
#define C23_WR_LIM7L            (*(volatile u32 *)(cpu_base + 0x4f*4))       // for debug only

#define C0_IF_UACNTL            (*(volatile u32 *)(cpu_base + 0x50*4))       //
#define C0_IF_UACNTH            (*(volatile u32 *)(cpu_base + 0x51*4))       //
#define C0_RD_UACNTL            (*(volatile u32 *)(cpu_base + 0x52*4))       //
#define C0_RD_UACNTH            (*(volatile u32 *)(cpu_base + 0x53*4))       //
#define C0_WR_UACNTL            (*(volatile u32 *)(cpu_base + 0x54*4))       //
#define C0_WR_UACNTH            (*(volatile u32 *)(cpu_base + 0x55*4))       //
#define C0_TL_CKCNTL            (*(volatile u32 *)(cpu_base + 0x56*4))       //
#define C0_TL_CKCNTH            (*(volatile u32 *)(cpu_base + 0x57*4))       //
#define C1_IF_UACNTL            (*(volatile u32 *)(cpu_base + 0x58*4))       //
#define C1_IF_UACNTH            (*(volatile u32 *)(cpu_base + 0x59*4))       //
#define C1_RD_UACNTL            (*(volatile u32 *)(cpu_base + 0x5a*4))       //
#define C1_RD_UACNTH            (*(volatile u32 *)(cpu_base + 0x5b*4))       //
#define C1_WR_UACNTL            (*(volatile u32 *)(cpu_base + 0x5c*4))       //
#define C1_WR_UACNTH            (*(volatile u32 *)(cpu_base + 0x5d*4))       //
#define C1_TL_CKCNTL            (*(volatile u32 *)(cpu_base + 0x5e*4))       //
#define C1_TL_CKCNTH            (*(volatile u32 *)(cpu_base + 0x5f*4))       //

#define C2_IF_UACNTL            (*(volatile u32 *)(cpu_base + 0x60*4))       //
#define C2_IF_UACNTH            (*(volatile u32 *)(cpu_base + 0x61*4))       //
#define C2_RD_UACNTL            (*(volatile u32 *)(cpu_base + 0x62*4))       //
#define C2_RD_UACNTH            (*(volatile u32 *)(cpu_base + 0x63*4))       //
#define C2_WR_UACNTL            (*(volatile u32 *)(cpu_base + 0x64*4))       //
#define C2_WR_UACNTH            (*(volatile u32 *)(cpu_base + 0x65*4))       //
#define C2_TL_CKCNTL            (*(volatile u32 *)(cpu_base + 0x66*4))       //
#define C2_TL_CKCNTH            (*(volatile u32 *)(cpu_base + 0x67*4))       //
#define C3_IF_UACNTL            (*(volatile u32 *)(cpu_base + 0x68*4))       //
#define C3_IF_UACNTH            (*(volatile u32 *)(cpu_base + 0x69*4))       //
#define C3_RD_UACNTL            (*(volatile u32 *)(cpu_base + 0x6a*4))       //
#define C3_RD_UACNTH            (*(volatile u32 *)(cpu_base + 0x6b*4))       //
#define C3_WR_UACNTL            (*(volatile u32 *)(cpu_base + 0x6c*4))       //
#define C3_WR_UACNTH            (*(volatile u32 *)(cpu_base + 0x6d*4))       //
#define C3_TL_CKCNTL            (*(volatile u32 *)(cpu_base + 0x6e*4))       //
#define C3_TL_CKCNTH            (*(volatile u32 *)(cpu_base + 0x6f*4))       //

#define DEBUG_WR_EN             (*(volatile u32 *)(cpu_base + 0x70*4))       // for debug only
#define DEBUG_MSG               (*(volatile u32 *)(cpu_base + 0x71*4))       // for debug only
#define DEBUG_MSG_CLR           (*(volatile u32 *)(cpu_base + 0x72*4))       // for debug only
#define DBG_CON0                (*(volatile u32 *)(cpu_base + 0x73*4))       // for debug only
#define PRP_CON                 (*(volatile u32 *)(cpu_base + 0x74*4))       //
#define PRP_ALLOW_NUM0          (*(volatile u32 *)(cpu_base + 0x75*4))       // for debug only
#define PRP_ALLOW_NUM1          (*(volatile u32 *)(cpu_base + 0x76*4))       // for debug only
#define DEBUG_PRP_NUM           (*(volatile u32 *)(cpu_base + 0x77*4))       // for debug only

#define C23_DEBUG_WR_EN         (*(volatile u32 *)(cpu_base + 0x80*4))       // for debug only
#define C23_DEBUG_MSG           (*(volatile u32 *)(cpu_base + 0x81*4))       // for debug only
#define C23_DEBUG_MSG_CLR       (*(volatile u32 *)(cpu_base + 0x82*4))       // for debug only
#define C23_DBG_CON0            (*(volatile u32 *)(cpu_base + 0x83*4))       // for debug only
#define C23_PRP_CON             (*(volatile u32 *)(cpu_base + 0x84*4))       //
#define C23_PRP_ALLOW_NUM0      (*(volatile u32 *)(cpu_base + 0x85*4))       // for debug only
#define C23_PRP_ALLOW_NUM1      (*(volatile u32 *)(cpu_base + 0x86*4))       // for debug only
#define C23_DEBUG_PRP_NUM       (*(volatile u32 *)(cpu_base + 0x87*4))       // for debug only

//==============================================================//
//  jpg0
//==============================================================//

#define   jpg_base 0x3f94800

#define JPG0_CON0                (*(volatile u32 *)(jpg_base + 0x00*4))
#define JPG0_CON1                (*(volatile u32 *)(jpg_base + 0x01*4))
#define JPG0_CON2                (*(volatile u32 *)(jpg_base + 0x02*4))
#define JPG0_YDCVAL              (*(volatile u32 *)(jpg_base + 0x03*4))
#define JPG0_UDCVAL              (*(volatile u32 *)(jpg_base + 0x04*4))
#define JPG0_VDCVAL              (*(volatile u32 *)(jpg_base + 0x05*4))
#define JPG0_YPTR0               (*(volatile u32 *)(jpg_base + 0x06*4))
#define JPG0_UPTR0               (*(volatile u32 *)(jpg_base + 0x07*4))
#define JPG0_VPTR0               (*(volatile u32 *)(jpg_base + 0x08*4))
#define JPG0_YPTR1               (*(volatile u32 *)(jpg_base + 0x09*4))
#define JPG0_UPTR1               (*(volatile u32 *)(jpg_base + 0x0a*4))
#define JPG0_VPTR1               (*(volatile u32 *)(jpg_base + 0x0b*4))
#define JPG0_BADDR               (*(volatile u32 *)(jpg_base + 0x0c*4))
#define JPG0_BCNT                (*(volatile u32 *)(jpg_base + 0x0d*4))
#define JPG0_MCUCNT              (*(volatile u32 *)(jpg_base + 0x0e*4))
#define JPG0_PRECNT              (*(volatile u32 *)(jpg_base + 0x0f*4))
#define JPG0_YUVLINE             (*(volatile u32 *)(jpg_base + 0x10*4))
#define JPG0_CFGRAMADDR          (*(volatile u32 *)(jpg_base + 0x11*4))
#define JPG0_CFGRAMVAL           (*(volatile u32 *)(jpg_base + 0x12*4))
#define JPG0_PTR_NUM             (*(volatile u32 *)(jpg_base + 0x13*4))
#define JPG0_HAS_BUF             (*(volatile u32 *)(jpg_base + 0x14*4))
#define JPG0_MCU_BUF             (*(volatile u32 *)(jpg_base + 0x15*4))

//#define JPG1_CON0                (*(volatile u32 *)(hs_jpg_base + 0x40*4))
//#define JPG1_CON1                (*(volatile u32 *)(hs_jpg_base + 0x41*4))
//#define JPG1_CON2                (*(volatile u32 *)(hs_jpg_base + 0x42*4))
//#define JPG1_YDCVAL              (*(volatile u32 *)(hs_jpg_base + 0x43*4))
//#define JPG1_UDCVAL              (*(volatile u32 *)(hs_jpg_base + 0x44*4))
//#define JPG1_VDCVAL              (*(volatile u32 *)(hs_jpg_base + 0x45*4))
//#define JPG1_YPTR0               (*(volatile u32 *)(hs_jpg_base + 0x46*4))
//#define JPG1_UPTR0               (*(volatile u32 *)(hs_jpg_base + 0x47*4))
//#define JPG1_VPTR0               (*(volatile u32 *)(hs_jpg_base + 0x48*4))
//#define JPG1_YPTR1               (*(volatile u32 *)(hs_jpg_base + 0x49*4))
//#define JPG1_UPTR1               (*(volatile u32 *)(hs_jpg_base + 0x4a*4))
//#define JPG1_VPTR1               (*(volatile u32 *)(hs_jpg_base + 0x4b*4))
//#define JPG1_BADDR               (*(volatile u32 *)(hs_jpg_base + 0x4c*4))
//#define JPG1_BCNT                (*(volatile u32 *)(hs_jpg_base + 0x4d*4))
//#define JPG1_MCUCNT              (*(volatile u32 *)(hs_jpg_base + 0x4e*4))
//#define JPG1_PRECNT              (*(volatile u32 *)(hs_jpg_base + 0x4f*4))
//#define JPG1_YUVLINE             (*(volatile u32 *)(hs_jpg_base + 0x50*4))
//#define JPG1_CFGRAMADDR          (*(volatile u32 *)(hs_jpg_base + 0x51*4))
//#define JPG1_CFGRAMVAL           (*(volatile u32 *)(hs_jpg_base + 0x52*4))
////#define JPG1_CFGRAMVAL           (*(volatile u32 *)(hs_jpg_base + 0x2c00*4))
//#define JPG1_PTR_NUM             (*(volatile u32 *)(hs_jpg_base + 0x53*4))

//DMA BYTE COPY
#define   dma_copy_base 0x3f94f00
#define DMA_COPY_TASK_ADR       (*(volatile u32 *)(dma_copy_base + 0x00*4))   //write only
#define DMA_COPY_CON            (*(volatile u32 *)(dma_copy_base + 0x01*4))

//JIVE
#define JIVE_CON0                (*(volatile u32 *)(hs_jive_base + 0x00*4))
#define JIVE_POINTER             (*(volatile u32 *)(hs_jive_base + 0x01*4))
#define JIVE_SQINT_CNT           (*(volatile u32 *)(hs_jive_base + 0x02*4))
#define JIVE_CHANNEL_CNT         (*(volatile u32 *)(hs_jive_base + 0x03*4))
#define JIVE_CHANNEL_COUNT       (*(volatile u32 *)(hs_jive_base + 0x03*4))

//JLMD
#define JLMD_CON0                (*(volatile u32 *)(hs_jlmd_base + 0x00*4))
#define JLMD_CON1                (*(volatile u32 *)(hs_jlmd_base + 0x01*4))
#define JLMD_CON2                (*(volatile u32 *)(hs_jlmd_base + 0x02*4))
#define JLMD_CON3                (*(volatile u32 *)(hs_jlmd_base + 0x03*4))
#define JLMD_CON4                (*(volatile u32 *)(hs_jlmd_base + 0x04*4))
#define JLMD_SRC                 (*(volatile u32 *)(hs_jlmd_base + 0x05*4))
#define JLMD_REF                 (*(volatile u32 *)(hs_jlmd_base + 0x06*4))
#define JLMD_BG                  (*(volatile u32 *)(hs_jlmd_base + 0x07*4))
#define JLMD_OBJ                 (*(volatile u32 *)(hs_jlmd_base + 0x08*4))

//HOG
#define HOG_CON0                 (*(volatile u32 *)(hs_hog_base + 0x00*4))
#define HOG_CON1                 (*(volatile u32 *)(hs_hog_base + 0x01*4))
#define HOG_CON2                 (*(volatile u32 *)(hs_hog_base + 0x02*4))
#define HOG_CON3                 (*(volatile u32 *)(hs_hog_base + 0x03*4))
#define HOG_SRC                  (*(volatile u32 *)(hs_hog_base + 0x04*4))
#define HOG_DST                  (*(volatile u32 *)(hs_hog_base + 0x05*4))
#define HOG_SVMINIT              (*(volatile u32 *)(hs_hog_base + 0x06*4))
#define HOG_SVMVEC               (*(volatile u32 *)(hs_hog_base + 0x07*4))


#define ENC_CON                 (*(volatile u32 *)(hs_oth_base + 0x00*4))
#define ENC_KEY                 (*(volatile u32 *)(hs_oth_base + 0x01*4))
#define ENC_ADR                 (*(volatile u32 *)(hs_oth_base + 0x02*4))
#define SFC_UNENC_ADRH          (*(volatile u32 *)(hs_oth_base + 0x03*4))
#define SFC_UNENC_ADRL          (*(volatile u32 *)(hs_oth_base + 0x04*4))


#define AES_CON                 (*(volatile u32 *)(hs_oth_base + 0x9*4))
#define AES_DATIN               (*(volatile u32 *)(hs_oth_base + 0x10*4))
#define AES_ENCRES0             (*(volatile u32 *)(hs_oth_base + 0x11*4))
#define AES_ENCRES1             (*(volatile u32 *)(hs_oth_base + 0x12*4))
#define AES_ENCRES2             (*(volatile u32 *)(hs_oth_base + 0x13*4))
#define AES_ENCRES3             (*(volatile u32 *)(hs_oth_base + 0x14*4))
#define AES_DECRES0             (*(volatile u32 *)(hs_oth_base + 0x15*4))
#define AES_DECRES1             (*(volatile u32 *)(hs_oth_base + 0x16*4))
#define AES_DECRES2             (*(volatile u32 *)(hs_oth_base + 0x17*4))
#define AES_DECRES3             (*(volatile u32 *)(hs_oth_base + 0x18*4))
#define AES_KEY                 (*(volatile u32 *)(hs_oth_base + 0x19*4))

#define GPDMA_RD_CON            (*(volatile u32 *)(hs_oth_base + 0x20*4))
#define GPDMA_RD_SPTR           (*(volatile u32 *)(hs_oth_base + 0x21*4))
#define GPDMA_RD_CNT            (*(volatile u32 *)(hs_oth_base + 0x22*4))

#define GPDMA_WR_CON            (*(volatile u32 *)(hs_oth_base + 0x30*4))
#define GPDMA_WR_SPTR           (*(volatile u32 *)(hs_oth_base + 0x31*4))
#define GPDMA_WR_CNT            (*(volatile u32 *)(hs_oth_base + 0x32*4))

//PAPS
#define PAPS_CON                (*(volatile u32 *)(hs_paps_base + 0x00*4))
#define PAPS_FLG                (*(volatile u32 *)(hs_paps_base + 0x01*4))
#define PAPS_PCNT               (*(volatile u32 *)(hs_paps_base + 0x02*4))
#define PAPS_SPTR               (*(volatile u32 *)(hs_paps_base + 0x03*4))
#define PAPS_HCNT               (*(volatile u32 *)(hs_paps_base + 0x04*4))
#define PAPS_VCNT               (*(volatile u32 *)(hs_paps_base + 0x05*4))
#define PAPS_ICNT               (*(volatile u32 *)(hs_paps_base + 0x06*4))
#define PAPS_OADR               (*(volatile u32 *)(hs_paps_base + 0x07*4))

// .............AVC .......................
//#define avc_base 0x3f9d000
#define avc_base 0x3f94000

//
#define AVC_CON0                (*(volatile u32 *)(avc_base + 0x00*4))
#define AVC_V0_16ROW_CNT        (*(volatile u32 *)(avc_base + 0x01*4))
#define AVC_V1_PERMIT_CNT       (*(volatile u32 *)(avc_base + 0x02*4))
#define AVC_AFBC_UNCOMPRESS     (*(volatile u32 *)(avc_base + 0x03*4))
//

#define AVC0_BUF_NUM            (*(volatile u32 *)(avc_base + 0x04*4))
#define AVC0_CON1               (*(volatile u32 *)(avc_base + 0x05*4))
#define AVC0_DB_OFFSET          (*(volatile u32 *)(avc_base + 0x06*4))
#define AVC0_MV_RANGE           (*(volatile u32 *)(avc_base + 0x07*4))
#define AVC0_CON2               (*(volatile u32 *)(avc_base + 0x08*4))
#define AVC0_YENC_BASE_ADR0     (*(volatile u32 *)(avc_base + 0x09*4))
#define AVC0_UENC_BASE_ADR0     (*(volatile u32 *)(avc_base + 0x0a*4))
#define AVC0_VENC_BASE_ADR0     (*(volatile u32 *)(avc_base + 0x0b*4))
#define AVC0_YENC_BASE_ADR1     (*(volatile u32 *)(avc_base + 0x0c*4))
#define AVC0_UENC_BASE_ADR1     (*(volatile u32 *)(avc_base + 0x0d*4))
#define AVC0_VENC_BASE_ADR1     (*(volatile u32 *)(avc_base + 0x0e*4))
#define AVC0_LDSZA_BASE_ADR     (*(volatile u32 *)(avc_base + 0x0f*4))
#define AVC0_LDSZB_BASE_ADR     (*(volatile u32 *)(avc_base + 0x10*4))
#define AVC0_LDCPA_BASE_ADR     (*(volatile u32 *)(avc_base + 0x11*4))
#define AVC0_LDCPB_BASE_ADR     (*(volatile u32 *)(avc_base + 0x12*4))
#define AVC0_WBSZA_BASE_ADR     (*(volatile u32 *)(avc_base + 0x13*4))
#define AVC0_WBSZB_BASE_ADR     (*(volatile u32 *)(avc_base + 0x14*4))
#define AVC0_WBCPA_BASE_ADR     (*(volatile u32 *)(avc_base + 0x15*4))
#define AVC0_WBCPB_BASE_ADR     (*(volatile u32 *)(avc_base + 0x16*4))
#define AVC0_CPA_START_ADR      (*(volatile u32 *)(avc_base + 0x17*4))
#define AVC0_CPA_END_ADR        (*(volatile u32 *)(avc_base + 0x18*4))
#define AVC0_CPB_START_ADR      (*(volatile u32 *)(avc_base + 0x19*4))
#define AVC0_CPB_END_ADR        (*(volatile u32 *)(avc_base + 0x1a*4))
#define AVC0_BS_ADDR            (*(volatile u32 *)(avc_base + 0x1b*4))
#define AVC0_QP_CON0            (*(volatile u32 *)(avc_base + 0x1c*4))
#define AVC0_QP_CON1            (*(volatile u32 *)(avc_base + 0x1d*4))
#define AVC0_ROI0_XY            (*(volatile u32 *)(avc_base + 0x1e*4))
#define AVC0_ROI1_XY            (*(volatile u32 *)(avc_base + 0x1f*4))
#define AVC0_ROI2_XY            (*(volatile u32 *)(avc_base + 0x20*4))
#define AVC0_ROI3_XY            (*(volatile u32 *)(avc_base + 0x21*4))
#define AVC0_ROIX_QP            (*(volatile u32 *)(avc_base + 0x22*4))
#define AVC0_VAR_MUL16          (*(volatile u32 *)(avc_base + 0x23*4))
#define AVC0_DECIMATE           (*(volatile u32 *)(avc_base + 0x24*4))
#define AVC0_BCNT               (*(volatile u32 *)(avc_base + 0x25*4))
#define AVC0_HEADER_CON         (*(volatile u32 *)(avc_base + 0x26*4))
#define AVC0_TOTAL_MB_COST      (*(volatile u32 *)(avc_base + 0x27*4))
#define AVC0_INTER_WEIGHT       (*(volatile u32 *)(avc_base + 0x28*4))
#define AVC0_DEADZONE           (*(volatile u32 *)(avc_base + 0x29*4))
#define AVC0_HAS_BUF            (*(volatile u32 *)(avc_base + 0x2a*4))
#define AVC0_FS_RANGE           (*(volatile u32 *)(avc_base + 0x2b*4))
#define AVC0_DCT_DENOISE        (*(volatile u32 *)(avc_base + 0x2c*4))
#define AVC0_PRESKIP_DECIMATE   (*(volatile u32 *)(avc_base + 0x2d*4))
#define AVC0_BS_CHECKSUM        (*(volatile u32 *)(avc_base + 0x2e*4))
#define AVC0_CON3               (*(volatile u32 *)(avc_base + 0x2f*4))
#define AVC0_DBDAT_CHECKSUM     (*(volatile u32 *)(avc_base + 0x30*4))
#define AVC0_LOAD_ENC_CNT       (*(volatile u32 *)(avc_base + 0x31*4))
#define AVC0_LOAD_FREF_CNT      (*(volatile u32 *)(avc_base + 0x32*4))
#define AVC0_PSKIP_TH           (*(volatile u32 *)(avc_base + 0x33*4))
#define AVC0_CON4               (*(volatile u32 *)(avc_base + 0x34*4))

//
#define AVC1_BUF_NUM            (*(volatile u32 *)(avc_base + 0x40*4))
#define AVC1_CON1               (*(volatile u32 *)(avc_base + 0x41*4))
#define AVC1_DB_OFFSET          (*(volatile u32 *)(avc_base + 0x42*4))
#define AVC1_MV_RANGE           (*(volatile u32 *)(avc_base + 0x43*4))
#define AVC1_CON2               (*(volatile u32 *)(avc_base + 0x44*4))
#define AVC1_YENC_BASE_ADR0     (*(volatile u32 *)(avc_base + 0x45*4))
#define AVC1_UENC_BASE_ADR0     (*(volatile u32 *)(avc_base + 0x46*4))
#define AVC1_VENC_BASE_ADR0     (*(volatile u32 *)(avc_base + 0x47*4))
#define AVC1_YENC_BASE_ADR1     (*(volatile u32 *)(avc_base + 0x48*4))
#define AVC1_UENC_BASE_ADR1     (*(volatile u32 *)(avc_base + 0x49*4))
#define AVC1_VENC_BASE_ADR1     (*(volatile u32 *)(avc_base + 0x4a*4))
#define AVC1_LDSZA_BASE_ADR     (*(volatile u32 *)(avc_base + 0x4b*4))
#define AVC1_LDSZB_BASE_ADR     (*(volatile u32 *)(avc_base + 0x4c*4))
#define AVC1_LDCPA_BASE_ADR     (*(volatile u32 *)(avc_base + 0x4d*4))
#define AVC1_LDCPB_BASE_ADR     (*(volatile u32 *)(avc_base + 0x4e*4))
#define AVC1_WBSZA_BASE_ADR     (*(volatile u32 *)(avc_base + 0x4f*4))
#define AVC1_WBSZB_BASE_ADR     (*(volatile u32 *)(avc_base + 0x50*4))
#define AVC1_WBCPA_BASE_ADR     (*(volatile u32 *)(avc_base + 0x51*4))
#define AVC1_WBCPB_BASE_ADR     (*(volatile u32 *)(avc_base + 0x52*4))
#define AVC1_CPA_START_ADR      (*(volatile u32 *)(avc_base + 0x53*4))
#define AVC1_CPA_END_ADR        (*(volatile u32 *)(avc_base + 0x54*4))
#define AVC1_CPB_START_ADR      (*(volatile u32 *)(avc_base + 0x55*4))
#define AVC1_CPB_END_ADR        (*(volatile u32 *)(avc_base + 0x56*4))
#define AVC1_BS_ADDR            (*(volatile u32 *)(avc_base + 0x57*4))
#define AVC1_QP_CON0            (*(volatile u32 *)(avc_base + 0x58*4))
#define AVC1_QP_CON1            (*(volatile u32 *)(avc_base + 0x59*4))
#define AVC1_ROI0_XY            (*(volatile u32 *)(avc_base + 0x5a*4))
#define AVC1_ROI1_XY            (*(volatile u32 *)(avc_base + 0x5b*4))
#define AVC1_ROI2_XY            (*(volatile u32 *)(avc_base + 0x5c*4))
#define AVC1_ROI3_XY            (*(volatile u32 *)(avc_base + 0x5d*4))
#define AVC1_ROIX_QP            (*(volatile u32 *)(avc_base + 0x5e*4))
#define AVC1_VAR_MUL16          (*(volatile u32 *)(avc_base + 0x5f*4))
#define AVC1_DECIMATE           (*(volatile u32 *)(avc_base + 0x60*4))
#define AVC1_BCNT               (*(volatile u32 *)(avc_base + 0x61*4))
#define AVC1_HEADER_CON         (*(volatile u32 *)(avc_base + 0x62*4))
#define AVC1_TOTAL_MB_COST      (*(volatile u32 *)(avc_base + 0x63*4))
#define AVC1_INTER_WEIGHT       (*(volatile u32 *)(avc_base + 0x64*4))
#define AVC1_DEADZONE           (*(volatile u32 *)(avc_base + 0x65*4))
#define AVC1_HAS_BUF            (*(volatile u32 *)(avc_base + 0x66*4))
#define AVC1_FS_RANGE           (*(volatile u32 *)(avc_base + 0x67*4))
#define AVC1_DCT_DENOISE        (*(volatile u32 *)(avc_base + 0x68*4))
#define AVC1_PRESKIP_DECIMATE   (*(volatile u32 *)(avc_base + 0x69*4))
#define AVC1_BS_CHECKSUM        (*(volatile u32 *)(avc_base + 0x6a*4))
#define AVC1_CON3               (*(volatile u32 *)(avc_base + 0x6b*4))
#define AVC1_DBDAT_CHECKSUM     (*(volatile u32 *)(avc_base + 0x6c*4))
#define AVC1_LOAD_ENC_CNT       (*(volatile u32 *)(avc_base + 0x6d*4))
#define AVC1_LOAD_FREF_CNT      (*(volatile u32 *)(avc_base + 0x6e*4))
#define AVC1_PSKIP_TH           (*(volatile u32 *)(avc_base + 0x6f*4))
#define AVC1_CON4               (*(volatile u32 *)(avc_base + 0x70*4))

//
#define AVC_BUS_CON             (*(volatile u32 *)(avc_base + 0x78*4))
#define AVC_FENCFREF_COOR       (*(volatile u32 *)(avc_base + 0x79*4))
#define AVC_DECABAC_COOR        (*(volatile u32 *)(avc_base + 0x7a*4))
#define AVC_BUF01_CNT           (*(volatile u32 *)(avc_base + 0x7b*4))
//...........  Full Speed USB .....................
#define FADDR       0x00
#define POWER       0x01
#define INTRTX1     0x02
#define INTRTX2     0x03
#define INTRRX1     0x04
#define INTRRX2     0x05
#define INTRUSB     0x06
#define INTRTX1E    0x07
#define INTRTX2E    0x08
#define INTRRX1E    0x09
#define INTRRX2E    0x0a
#define INTRUSBE    0x0b
#define FRAME1      0x0c
#define FRAME2      0x0d
#define INDEX       0x0e
#define DEVCTL      0x0f
#define TXMAXP      0x10
#define CSR0        0x11
#define TXCSR1      0x11
#define TXCSR2      0x12
#define RXMAXP      0x13
#define RXCSR1      0x14
#define RXCSR2      0x15
#define COUNT0      0x16
#define RXCOUNT1    0x16
#define RXCOUNT2    0x17
#define TXTYPE      0x18
#define TXINTERVAL  0x19
#define RXTYPE      0x1a
#define RXINTERVAL  0x1b

//...........  High Speed USB .....................
#define husb0_base ls_base + 0x4000

#define H0_FADDR         (*(volatile u8  *)(husb0_base + 0x000))
#define H0_POWER         (*(volatile u8  *)(husb0_base + 0x001))
#define H0_INTRTX        (*(volatile u16 *)(husb0_base + 0x002))
#define H0_INTRRX        (*(volatile u16 *)(husb0_base + 0x004))
#define H0_INTRTXE       (*(volatile u16 *)(husb0_base + 0x006))
#define H0_INTRRXE       (*(volatile u16 *)(husb0_base + 0x008))
#define H0_INTRUSB       (*(volatile u8  *)(husb0_base + 0x00a))
#define H0_INTRUSBE      (*(volatile u8  *)(husb0_base + 0x00b))
#define H0_FRAME         (*(volatile u16 *)(husb0_base + 0x00c))
#define H0_INDEX         (*(volatile u8  *)(husb0_base + 0x00e))
#define H0_TESTMODE      (*(volatile u8  *)(husb0_base + 0x00f))

#define H0_FIFO0         (*(volatile u8  *)(husb0_base + 0x020))
#define H0_FIFO1         (*(volatile u8  *)(husb0_base + 0x024))
#define H0_FIFO2         (*(volatile u8  *)(husb0_base + 0x028))
#define H0_FIFO3         (*(volatile u8  *)(husb0_base + 0x02c))
#define H0_FIFO4         (*(volatile u8  *)(husb0_base + 0x030))
#define H0_DEVCTL        (*(volatile u8  *)(husb0_base + 0x060))

#define H0_CSR0          (*(volatile u16 *)(husb0_base + 0x102))
#define H0_COUNT0        (*(volatile u16 *)(husb0_base + 0x108))
#define H0_NAKLIMIT0     (*(volatile u8  *)(husb0_base + 0x10b))
#define H0_CFGDATA       (*(volatile u8  *)(husb0_base + 0x10f))

#define H0_EP1TXMAXP     (*(volatile u16 *)(husb0_base + 0x110))
#define H0_EP1TXCSR      (*(volatile u16 *)(husb0_base + 0x112))
#define H0_EP1RXMAXP     (*(volatile u16 *)(husb0_base + 0x114))
#define H0_EP1RXCSR      (*(volatile u16 *)(husb0_base + 0x116))
#define H0_EP1RXCOUNT    (*(volatile u16 *)(husb0_base + 0x118))
#define H0_EP1TXTYPE     (*(volatile u8  *)(husb0_base + 0x11a))
#define H0_EP1TXINTERVAL (*(volatile u8  *)(husb0_base + 0x11b))
#define H0_EP1RXTYPE     (*(volatile u8  *)(husb0_base + 0x11c))
#define H0_EP1RXINTERVAL (*(volatile u8  *)(husb0_base + 0x11d))
#define H0_EP1FIFOSIZE   (*(volatile u8  *)(husb0_base + 0x11f))

#define H0_EP2TXMAXP     (*(volatile u16 *)(husb0_base + 0x120))
#define H0_EP2TXCSR      (*(volatile u16 *)(husb0_base + 0x122))
#define H0_EP2RXMAXP     (*(volatile u16 *)(husb0_base + 0x124))
#define H0_EP2RXCSR      (*(volatile u16 *)(husb0_base + 0x126))
#define H0_EP2RXCOUNT    (*(volatile u16 *)(husb0_base + 0x128))
#define H0_EP2TXTYPE     (*(volatile u8  *)(husb0_base + 0x12a))
#define H0_EP2TXINTERVAL (*(volatile u8  *)(husb0_base + 0x12b))
#define H0_EP2RXTYPE     (*(volatile u8  *)(husb0_base + 0x12c))
#define H0_EP2RXINTERVAL (*(volatile u8  *)(husb0_base + 0x12d))
#define H0_EP2FIFOSIZE   (*(volatile u8  *)(husb0_base + 0x12f))

#define H0_EP3TXMAXP     (*(volatile u16 *)(husb0_base + 0x130))
#define H0_EP3TXCSR      (*(volatile u16 *)(husb0_base + 0x132))
#define H0_EP3RXMAXP     (*(volatile u16 *)(husb0_base + 0x134))
#define H0_EP3RXCSR      (*(volatile u16 *)(husb0_base + 0x136))
#define H0_EP3RXCOUNT    (*(volatile u16 *)(husb0_base + 0x138))
#define H0_EP3TXTYPE     (*(volatile u8  *)(husb0_base + 0x13a))
#define H0_EP3TXINTERVAL (*(volatile u8  *)(husb0_base + 0x13b))
#define H0_EP3RXTYPE     (*(volatile u8  *)(husb0_base + 0x13c))
#define H0_EP3RXINTERVAL (*(volatile u8  *)(husb0_base + 0x13d))
#define H0_EP3FIFOSIZE   (*(volatile u8  *)(husb0_base + 0x13f))

#define H0_EP4TXMAXP     (*(volatile u16 *)(husb0_base + 0x140))
#define H0_EP4TXCSR      (*(volatile u16 *)(husb0_base + 0x142))
#define H0_EP4RXMAXP     (*(volatile u16 *)(husb0_base + 0x144))
#define H0_EP4RXCSR      (*(volatile u16 *)(husb0_base + 0x146))
#define H0_EP4RXCOUNT    (*(volatile u16 *)(husb0_base + 0x148))
#define H0_EP4TXTYPE     (*(volatile u8  *)(husb0_base + 0x14a))
#define H0_EP4TXINTERVAL (*(volatile u8  *)(husb0_base + 0x14b))
#define H0_EP4RXTYPE     (*(volatile u8  *)(husb0_base + 0x14c))
#define H0_EP4RXINTERVAL (*(volatile u8  *)(husb0_base + 0x14d))
#define H0_EP4FIFOSIZE   (*(volatile u8  *)(husb0_base + 0x14f))

#define H0_RX_DPKTDIS    (*(volatile u16 *)(husb0_base + 0x340))
#define H0_TX_DPKTDIS    (*(volatile u16 *)(husb0_base + 0x342))
#define H0_C_T_UCH       (*(volatile u16 *)(husb0_base + 0x344))

#define husb1_base ls_base + 0x6000

#define H1_FADDR         (*(volatile u8  *)(husb1_base + 0x000))
#define H1_POWER         (*(volatile u8  *)(husb1_base + 0x001))
#define H1_INTRTX        (*(volatile u16 *)(husb1_base + 0x002))
#define H1_INTRRX        (*(volatile u16 *)(husb1_base + 0x004))
#define H1_INTRTXE       (*(volatile u16 *)(husb1_base + 0x006))
#define H1_INTRRXE       (*(volatile u16 *)(husb1_base + 0x008))
#define H1_INTRUSB       (*(volatile u8  *)(husb1_base + 0x00a))
#define H1_INTRUSBE      (*(volatile u8  *)(husb1_base + 0x00b))
#define H1_FRAME         (*(volatile u16 *)(husb1_base + 0x00c))
#define H1_INDEX         (*(volatile u8  *)(husb1_base + 0x00e))
#define H1_TESTMODE      (*(volatile u8  *)(husb1_base + 0x00f))

#define H1_FIFO0         (*(volatile u8  *)(husb1_base + 0x020))
#define H1_FIFO1         (*(volatile u8  *)(husb1_base + 0x024))
#define H1_FIFO2         (*(volatile u8  *)(husb1_base + 0x028))
#define H1_FIFO3         (*(volatile u8  *)(husb1_base + 0x02c))
#define H1_FIFO4         (*(volatile u8  *)(husb1_base + 0x030))
#define H1_DEVCTL        (*(volatile u8  *)(husb1_base + 0x060))

#define H1_CSR0          (*(volatile u16 *)(husb1_base + 0x102))
#define H1_COUNT0        (*(volatile u16 *)(husb1_base + 0x108))
#define H1_NAKLIMIT0     (*(volatile u8  *)(husb1_base + 0x10b))
#define H1_CFGDATA       (*(volatile u8  *)(husb1_base + 0x10f))

#define H1_EP1TXMAXP     (*(volatile u16 *)(husb1_base + 0x110))
#define H1_EP1TXCSR      (*(volatile u16 *)(husb1_base + 0x112))
#define H1_EP1RXMAXP     (*(volatile u16 *)(husb1_base + 0x114))
#define H1_EP1RXCSR      (*(volatile u16 *)(husb1_base + 0x116))
#define H1_EP1RXCOUNT    (*(volatile u16 *)(husb1_base + 0x118))
#define H1_EP1TXTYPE     (*(volatile u8  *)(husb1_base + 0x11a))
#define H1_EP1TXINTERVAL (*(volatile u8  *)(husb1_base + 0x11b))
#define H1_EP1RXTYPE     (*(volatile u8  *)(husb1_base + 0x11c))
#define H1_EP1RXINTERVAL (*(volatile u8  *)(husb1_base + 0x11d))
#define H1_EP1FIFOSIZE   (*(volatile u8  *)(husb1_base + 0x11f))

#define H1_EP2TXMAXP     (*(volatile u16 *)(husb1_base + 0x120))
#define H1_EP2TXCSR      (*(volatile u16 *)(husb1_base + 0x122))
#define H1_EP2RXMAXP     (*(volatile u16 *)(husb1_base + 0x124))
#define H1_EP2RXCSR      (*(volatile u16 *)(husb1_base + 0x126))
#define H1_EP2RXCOUNT    (*(volatile u16 *)(husb1_base + 0x128))
#define H1_EP2TXTYPE     (*(volatile u8  *)(husb1_base + 0x12a))
#define H1_EP2TXINTERVAL (*(volatile u8  *)(husb1_base + 0x12b))
#define H1_EP2RXTYPE     (*(volatile u8  *)(husb1_base + 0x12c))
#define H1_EP2RXINTERVAL (*(volatile u8  *)(husb1_base + 0x12d))
#define H1_EP2FIFOSIZE   (*(volatile u8  *)(husb1_base + 0x12f))

#define H1_EP3TXMAXP     (*(volatile u16 *)(husb1_base + 0x130))
#define H1_EP3TXCSR      (*(volatile u16 *)(husb1_base + 0x132))
#define H1_EP3RXMAXP     (*(volatile u16 *)(husb1_base + 0x134))
#define H1_EP3RXCSR      (*(volatile u16 *)(husb1_base + 0x136))
#define H1_EP3RXCOUNT    (*(volatile u16 *)(husb1_base + 0x138))
#define H1_EP3TXTYPE     (*(volatile u8  *)(husb1_base + 0x13a))
#define H1_EP3TXINTERVAL (*(volatile u8  *)(husb1_base + 0x13b))
#define H1_EP3RXTYPE     (*(volatile u8  *)(husb1_base + 0x13c))
#define H1_EP3RXINTERVAL (*(volatile u8  *)(husb1_base + 0x13d))
#define H1_EP3FIFOSIZE   (*(volatile u8  *)(husb1_base + 0x13f))

#define H1_EP4TXMAXP     (*(volatile u16 *)(husb1_base + 0x140))
#define H1_EP4TXCSR      (*(volatile u16 *)(husb1_base + 0x142))
#define H1_EP4RXMAXP     (*(volatile u16 *)(husb1_base + 0x144))
#define H1_EP4RXCSR      (*(volatile u16 *)(husb1_base + 0x146))
#define H1_EP4RXCOUNT    (*(volatile u16 *)(husb1_base + 0x148))
#define H1_EP4TXTYPE     (*(volatile u8  *)(husb1_base + 0x14a))
#define H1_EP4TXINTERVAL (*(volatile u8  *)(husb1_base + 0x14b))
#define H1_EP4RXTYPE     (*(volatile u8  *)(husb1_base + 0x14c))
#define H1_EP4RXINTERVAL (*(volatile u8  *)(husb1_base + 0x14d))
#define H1_EP4FIFOSIZE   (*(volatile u8  *)(husb1_base + 0x14f))

#define H1_TX_DPKTDIS    (*(volatile u16 *)(husb1_base + 0x342))
#define H1_C_T_UCH       (*(volatile u16 *)(husb1_base + 0x344))

//==============================================================//
//  xbus
//==============================================================//
#define eva_base(i) (0x3f98000 + 0x800*i)
#define xbus_sfr_ptr(num)   (*(volatile u32 *)(eva_base(0) + num*4))

#define xbus_ch00_lvl           xbus_sfr_ptr(0x00)
#define xbus_ch01_lvl           xbus_sfr_ptr(0x01)
#define xbus_ch02_lvl           xbus_sfr_ptr(0x02)
#define xbus_ch03_lvl           xbus_sfr_ptr(0x03)
#define xbus_ch04_lvl           xbus_sfr_ptr(0x04)
#define xbus_ch05_lvl           xbus_sfr_ptr(0x05)
#define xbus_ch06_lvl           xbus_sfr_ptr(0x06)
#define xbus_ch07_lvl           xbus_sfr_ptr(0x07)
#define xbus_ch08_lvl           xbus_sfr_ptr(0x08)
#define xbus_ch09_lvl           xbus_sfr_ptr(0x09)
#define xbus_ch10_lvl           xbus_sfr_ptr(0x0a)
#define xbus_ch11_lvl           xbus_sfr_ptr(0x0b)
#define xbus_ch12_lvl           xbus_sfr_ptr(0x0c)
#define xbus_ch13_lvl           xbus_sfr_ptr(0x0d)
#define xbus_ch14_lvl           xbus_sfr_ptr(0x0e)
#define xbus_ch15_lvl           xbus_sfr_ptr(0x0f)

#define xbus_ch16_lvl           xbus_sfr_ptr(0x10)
#define xbus_ch17_lvl           xbus_sfr_ptr(0x11)
#define xbus_ch18_lvl           xbus_sfr_ptr(0x12)
#define xbus_ch19_lvl           xbus_sfr_ptr(0x13)
#define xbus_ch20_lvl           xbus_sfr_ptr(0x14)
#define xbus_ch21_lvl           xbus_sfr_ptr(0x15)
#define xbus_ch22_lvl           xbus_sfr_ptr(0x16)
#define xbus_ch23_lvl           xbus_sfr_ptr(0x17)
#define xbus_ch24_lvl           xbus_sfr_ptr(0x18)
#define xbus_ch25_lvl           xbus_sfr_ptr(0x19)
#define xbus_ch26_lvl           xbus_sfr_ptr(0x1a)
#define xbus_ch27_lvl           xbus_sfr_ptr(0x1b)
#define xbus_ch28_lvl           xbus_sfr_ptr(0x1c)
#define xbus_ch29_lvl           xbus_sfr_ptr(0x1d)
#define xbus_ch30_lvl           xbus_sfr_ptr(0x1e)
#define xbus_ch31_lvl           xbus_sfr_ptr(0x1f)

#define xbus_lv1_prd            xbus_sfr_ptr(0x20)
#define xbus_lv2_prd            xbus_sfr_ptr(0x21)
#define xbus_dist0_con          xbus_sfr_ptr(0x22)
#define xbus_dist1_con          xbus_sfr_ptr(0x23)
#define xbus_dist0_ext			xbus_sfr_ptr(0x24)
#define xbus_dist1_ext			xbus_sfr_ptr(0x25)

//==============================================================//
//  isc
//==============================================================//
#define isc_sfr_ptr(num)   (*(volatile u32 *)(eva_base(1) + num*4))

#define isc_pnd_con             isc_sfr_ptr(0x00)

#define isc_sen0_con            isc_sfr_ptr(0x08)
#define isc_sen0_vblk           isc_sfr_ptr(0x09)
#define isc_sen0_vact           isc_sfr_ptr(0x0a)
#define isc_sen0_hblk           isc_sfr_ptr(0x0b)
#define isc_sen0_hact           isc_sfr_ptr(0x0c)

#define isc_sen1_con            isc_sfr_ptr(0x10)
#define isc_sen1_vblk           isc_sfr_ptr(0x11)
#define isc_sen1_vact           isc_sfr_ptr(0x12)
#define isc_sen1_hblk           isc_sfr_ptr(0x13)
#define isc_sen1_hact           isc_sfr_ptr(0x14)

#define isc_lcds_con            isc_sfr_ptr(0x18)
#define isc_lcds_vblk           isc_sfr_ptr(0x19)
#define isc_lcds_vact           isc_sfr_ptr(0x1a)
#define isc_lcds_hblk           isc_sfr_ptr(0x1b)
#define isc_lcds_hact           isc_sfr_ptr(0x1c)

//==============================================================//
//  isp0
//==============================================================//
#define isp0_sfr_ptr(num)   (*(volatile u32 *)(eva_base(2) + num*4))


//isp0_debug
#define isp0_debug_v_st         isp0_sfr_ptr(0x190)
#define isp0_debug_h_st         isp0_sfr_ptr(0x191)
#define isp0_debug_con          isp0_sfr_ptr(0x192)
#define isp0_debug_buf          isp0_sfr_ptr(0x193)

#define isp0_pnd_con            isp0_sfr_ptr(0x000)
#define isp0_scn_con            isp0_sfr_ptr(0x001)

#define isp0_src_con            isp0_sfr_ptr(0x010)
#define isp0_src_con1            isp0_sfr_ptr(0x011)
#define isp0_src_tcfg0          isp0_sfr_ptr(0x012)
#define isp0_src_tcfg1          isp0_sfr_ptr(0x013)
#define isp0_src_tcfg2          isp0_sfr_ptr(0x014)
#define isp0_src_tcfg3          isp0_sfr_ptr(0x015)
#define isp0_src_img_size       isp0_sfr_ptr(0x016)
#define isp0_src_dma_size       isp0_sfr_ptr(0x017)
#define isp0_src_dma_base0      isp0_sfr_ptr(0x018)
#define isp0_src_dma_base1      isp0_sfr_ptr(0x019)

#define isp0_blc_l_off_r        isp0_sfr_ptr(0x020)
#define isp0_blc_l_off_gr       isp0_sfr_ptr(0x021)
#define isp0_blc_l_off_gb       isp0_sfr_ptr(0x022)
#define isp0_blc_l_off_b        isp0_sfr_ptr(0x023)
#define isp0_blc_s_off_r        isp0_sfr_ptr(0x024)
#define isp0_blc_s_off_gr       isp0_sfr_ptr(0x025)
#define isp0_blc_s_off_gb       isp0_sfr_ptr(0x026)
#define isp0_blc_s_off_b        isp0_sfr_ptr(0x027)

#define isp0_dg_l_gain          isp0_sfr_ptr(0x030)
#define isp0_dg_s_gain          isp0_sfr_ptr(0x031)

#define isp0_hdr_con            isp0_sfr_ptr(0x040)
#define isp0_hdr_l_gain         isp0_sfr_ptr(0x041)
#define isp0_hdr_s_gain         isp0_sfr_ptr(0x042)
#define isp0_hdr_m_coeff        isp0_sfr_ptr(0x043)
#define isp0_hdr_low_th         isp0_sfr_ptr(0x044)
#define isp0_hdr_high_th        isp0_sfr_ptr(0x045)
#define isp0_hdr_w_lut          isp0_sfr_ptr(0x046)

#define isp0_hdr_ob             isp0_sfr_ptr(0x047)
#define isp0_hdr_merge_gain     isp0_sfr_ptr(0x048)


#define isp0_dpc_th0            isp0_sfr_ptr(0x050)
#define isp0_dpc_th1            isp0_sfr_ptr(0x051)
#define isp0_dpc_th2            isp0_sfr_ptr(0x052)

#define isp0_lsc_cen_x          isp0_sfr_ptr(0x060)
#define isp0_lsc_cen_y          isp0_sfr_ptr(0x061)
#define isp0_lsc_dth_th         isp0_sfr_ptr(0x062)
#define isp0_lsc_dth_prm0       isp0_sfr_ptr(0x063)
#define isp0_lsc_dth_prm1       isp0_sfr_ptr(0x064)
#define isp0_lsc_lut_r          isp0_sfr_ptr(0x065)
#define isp0_lsc_lut_g          isp0_sfr_ptr(0x066)
#define isp0_lsc_lut_b          isp0_sfr_ptr(0x067)

#define isp0_pre_wb_r_gain      isp0_sfr_ptr(0x070)
#define isp0_pre_wb_g_gain      isp0_sfr_ptr(0x071)
#define isp0_pre_wb_b_gain      isp0_sfr_ptr(0x072)
#define isp0_pos_wb_r_gain      isp0_sfr_ptr(0x073)
#define isp0_pos_wb_g_gain      isp0_sfr_ptr(0x074)
#define isp0_pos_wb_b_gain      isp0_sfr_ptr(0x075)
#define isp0_stc_wb_base0       isp0_sfr_ptr(0x076)
#define isp0_stc_wb_base1       isp0_sfr_ptr(0x077)
#define isp0_stc_wb_base2       isp0_sfr_ptr(0x078)
#define isp0_stc_wb_base3       isp0_sfr_ptr(0x079)
#define isp0_stc_wb_basex       isp0_sfr_ptr(0x07a)
#define isp0_stc_wb_en          isp0_sfr_ptr(0x07b)
#define isp0_stc_wb_r_max       isp0_sfr_ptr(0x07c)
#define isp0_stc_wb_g_max       isp0_sfr_ptr(0x07d)
#define isp0_stc_wb_b_max       isp0_sfr_ptr(0x07e)
#define isp0_stc_wb_r_min       isp0_sfr_ptr(0x07f)
#define isp0_stc_wb_g_min       isp0_sfr_ptr(0x080)
#define isp0_stc_wb_b_min       isp0_sfr_ptr(0x081)
#define isp0_stc_wb_w_th        isp0_sfr_ptr(0x082)
#define isp0_stc_wb_y_min       isp0_sfr_ptr(0x083)
#define isp0_stc_wb_y_max       isp0_sfr_ptr(0x084)

#define isp0_drc_lut            isp0_sfr_ptr(0x090)
#define isp0_drc_ltmo_en        isp0_sfr_ptr(0x091)
#define isp0_drc_level          isp0_sfr_ptr(0x092)
#define isp0_drc_ysel           isp0_sfr_ptr(0x093)

#define isp0_abd_en             isp0_sfr_ptr(0x0a0)
#define isp0_abd_edg_boost      isp0_sfr_ptr(0x0a1)
#define isp0_abd_ns_profile     isp0_sfr_ptr(0x0a2)
#define isp0_abd_lnr_factor     isp0_sfr_ptr(0x0a3)
#define isp0_abd_thd_smooth     isp0_sfr_ptr(0x0a4)
#define isp0_abd_thd_texture    isp0_sfr_ptr(0x0a5)
#define isp0_abd_sgm_smooth     isp0_sfr_ptr(0x0a6)
#define isp0_abd_sgm_texture    isp0_sfr_ptr(0x0a7)

#define isp0_ccm_r_coe0         isp0_sfr_ptr(0x0b0)
#define isp0_ccm_r_coe1         isp0_sfr_ptr(0x0b1)
#define isp0_ccm_r_coe2         isp0_sfr_ptr(0x0b2)
#define isp0_ccm_r_off          isp0_sfr_ptr(0x0b3)
#define isp0_ccm_g_coe0         isp0_sfr_ptr(0x0b4)
#define isp0_ccm_g_coe1         isp0_sfr_ptr(0x0b5)
#define isp0_ccm_g_coe2         isp0_sfr_ptr(0x0b6)
#define isp0_ccm_g_off          isp0_sfr_ptr(0x0b7)
#define isp0_ccm_b_coe0         isp0_sfr_ptr(0x0b8)
#define isp0_ccm_b_coe1         isp0_sfr_ptr(0x0b9)
#define isp0_ccm_b_coe2         isp0_sfr_ptr(0x0ba)
#define isp0_ccm_b_off          isp0_sfr_ptr(0x0bb)

#define isp0_gma_r_lut          isp0_sfr_ptr(0x0c0)
#define isp0_gma_g_lut          isp0_sfr_ptr(0x0c1)
#define isp0_gma_b_lut          isp0_sfr_ptr(0x0c2)

#define isp0_hsv_lut            isp0_sfr_ptr(0x0d0)
#define isp0_hsv_hdw_en         isp0_sfr_ptr(0x0d1)
#define isp0_hsv_sat_w0         isp0_sfr_ptr(0x0d2)
#define isp0_hsv_sat_w1         isp0_sfr_ptr(0x0d3)
#define isp0_hsv_sat_th0        isp0_sfr_ptr(0x0d4)
#define isp0_hsv_sat_th1        isp0_sfr_ptr(0x0d5)
#define isp0_hsv_sat_slope      isp0_sfr_ptr(0x0d6)

#define isp0_csc_y_lut          isp0_sfr_ptr(0x0e0)

#define isp0_dnr_con            isp0_sfr_ptr(0x0f0)
#define isp0_dnr_lvl            isp0_sfr_ptr(0x0f1)
#define isp0_dnr_sim_th         isp0_sfr_ptr(0x0f2)
#define isp0_dnr_smt_th         isp0_sfr_ptr(0x0f3)
#define isp0_dnr_txt_wt         isp0_sfr_ptr(0x0f4)
#define isp0_dnr_cen_x          isp0_sfr_ptr(0x0f5)
#define isp0_dnr_cen_y          isp0_sfr_ptr(0x0f6)
#define isp0_dnr_r_offs         isp0_sfr_ptr(0x0f7)
#define isp0_dnr_rc_lvl         isp0_sfr_ptr(0x0f8)
#define isp0_dnr_rc_max         isp0_sfr_ptr(0x0f9)

#define isp0_tnr_con            isp0_sfr_ptr(0x100)
#define isp0_tnr_base           isp0_sfr_ptr(0x101)
#define isp0_tnr_size           isp0_sfr_ptr(0x102)
#define isp0_tnr_lvl            isp0_sfr_ptr(0x103)
#define isp0_tnr_edge_th        isp0_sfr_ptr(0x104)
#define isp0_tnr_edge_sen0      isp0_sfr_ptr(0x105)
#define isp0_tnr_edge_sen1      isp0_sfr_ptr(0x106)
#define isp0_tnr_sim_th         isp0_sfr_ptr(0x107)
#define isp0_tnr_smt_th         isp0_sfr_ptr(0x108)
#define isp0_tnr_txt_wt         isp0_sfr_ptr(0x109)

#define isp0_shp_con            isp0_sfr_ptr(0x110)
#define isp0_shp_lone_th        isp0_sfr_ptr(0x111)
#define isp0_shp_mf_str         isp0_sfr_ptr(0x112)
#define isp0_shp_lmt_max        isp0_sfr_ptr(0x113)
#define isp0_shp_lmt_min        isp0_sfr_ptr(0x114)
#define isp0_shp_agg_fact       isp0_sfr_ptr(0x115)
#define isp0_shp_smt_th0        isp0_sfr_ptr(0x116)
#define isp0_shp_smt_th1        isp0_sfr_ptr(0x117)
#define isp0_shp_smt_lvl        isp0_sfr_ptr(0x118)
#define isp0_shp_h_sft          isp0_sfr_ptr(0x119)
#define isp0_shp_h_th0          isp0_sfr_ptr(0x11a)
#define isp0_shp_h_th1          isp0_sfr_ptr(0x11b)
#define isp0_shp_h_amt0         isp0_sfr_ptr(0x11c)
#define isp0_shp_h_amt1         isp0_sfr_ptr(0x11d)
#define isp0_shp_h_coe0         isp0_sfr_ptr(0x11e)
#define isp0_shp_h_coe1         isp0_sfr_ptr(0x11f)
#define isp0_shp_h_coe2         isp0_sfr_ptr(0x120)
#define isp0_shp_h_coe3         isp0_sfr_ptr(0x121)
#define isp0_shp_h_coe4         isp0_sfr_ptr(0x122)
#define isp0_shp_h_coe5         isp0_sfr_ptr(0x123)
#define isp0_shp_h_coe6         isp0_sfr_ptr(0x124)
#define isp0_shp_h_coe7         isp0_sfr_ptr(0x125)
#define isp0_shp_h_coe8         isp0_sfr_ptr(0x126)
#define isp0_shp_m_sft          isp0_sfr_ptr(0x127)
#define isp0_shp_m_th0          isp0_sfr_ptr(0x128)
#define isp0_shp_m_th1          isp0_sfr_ptr(0x129)
#define isp0_shp_m_amt0         isp0_sfr_ptr(0x12a)
#define isp0_shp_m_amt1         isp0_sfr_ptr(0x12b)
#define isp0_shp_m_coe0         isp0_sfr_ptr(0x12c)
#define isp0_shp_m_coe1         isp0_sfr_ptr(0x12d)
#define isp0_shp_m_coe2         isp0_sfr_ptr(0x12e)
#define isp0_shp_m_coe3         isp0_sfr_ptr(0x12f)
#define isp0_shp_m_coe4         isp0_sfr_ptr(0x130)
#define isp0_shp_m_coe5         isp0_sfr_ptr(0x131)
#define isp0_shp_m_coe6         isp0_sfr_ptr(0x132)
#define isp0_shp_m_coe7         isp0_sfr_ptr(0x133)
#define isp0_shp_m_coe8         isp0_sfr_ptr(0x134)
#define isp0_shp_l_sca0         isp0_sfr_ptr(0x135)
#define isp0_shp_l_sca1         isp0_sfr_ptr(0x136)
#define isp0_shp_l_sca2         isp0_sfr_ptr(0x137)
#define isp0_shp_l_sca3         isp0_sfr_ptr(0x138)
#define isp0_shp_l_sca4         isp0_sfr_ptr(0x139)
#define isp0_shp_l_sca5         isp0_sfr_ptr(0x13a)
#define isp0_shp_l_sca6         isp0_sfr_ptr(0x13b)
#define isp0_shp_l_sca7         isp0_sfr_ptr(0x13c)
#define isp0_shp_c_sft          isp0_sfr_ptr(0x13d)
#define isp0_shp_c_th           isp0_sfr_ptr(0x13e)
#define isp0_shp_c_coe0         isp0_sfr_ptr(0x13f)
#define isp0_shp_c_coe1         isp0_sfr_ptr(0x140)
#define isp0_shp_c_coe2         isp0_sfr_ptr(0x141)
#define isp0_shp_c_coe3         isp0_sfr_ptr(0x142)
#define isp0_shp_c_coe4         isp0_sfr_ptr(0x143)
#define isp0_shp_c_coe5         isp0_sfr_ptr(0x144)
#define isp0_shp_c_coe6         isp0_sfr_ptr(0x145)
#define isp0_shp_c_coe7         isp0_sfr_ptr(0x146)
#define isp0_shp_c_coe8         isp0_sfr_ptr(0x147)

#define isp0_cbs_y_gain         isp0_sfr_ptr(0x150)
#define isp0_cbs_u_gain         isp0_sfr_ptr(0x151)
#define isp0_cbs_v_gain         isp0_sfr_ptr(0x152)
#define isp0_cbs_y_offs         isp0_sfr_ptr(0x153)
#define isp0_cbs_u_offs         isp0_sfr_ptr(0x154)
#define isp0_cbs_v_offs         isp0_sfr_ptr(0x155)
#define isp0_cbs_hue_sin        isp0_sfr_ptr(0x156)
#define isp0_cbs_hue_cos        isp0_sfr_ptr(0x157)

#define isp0_cbs_y_th0          isp0_sfr_ptr(0x158)
#define isp0_cbs_y_th1          isp0_sfr_ptr(0x159)
#define isp0_cbs_y_th2          isp0_sfr_ptr(0x15a)
#define isp0_cbs_y_th3          isp0_sfr_ptr(0x15b)
#define isp0_cbs_uv_th          isp0_sfr_ptr(0x15c)
#define isp0_cbs_y_th           isp0_sfr_ptr(0x15d)
#define isp0_cbs_uv_coe         isp0_sfr_ptr(0x15e)
#define isp0_cbs_con            isp0_sfr_ptr(0x15f)



#define isp0_out_hst            isp0_sfr_ptr(0x160)
#define isp0_out_hed            isp0_sfr_ptr(0x161)
#define isp0_out_vst            isp0_sfr_ptr(0x162)
#define isp0_out_ved            isp0_sfr_ptr(0x163)

#define isp0_stc_ae_base0       isp0_sfr_ptr(0x170)
#define isp0_stc_ae_base1       isp0_sfr_ptr(0x171)
#define isp0_stc_ae_base2       isp0_sfr_ptr(0x172)
#define isp0_stc_ae_base3       isp0_sfr_ptr(0x173)
#define isp0_stc_ae_basex       isp0_sfr_ptr(0x174)
#define isp0_stc_ae_en          isp0_sfr_ptr(0x175)
#define isp0_stc_ae_lv1         isp0_sfr_ptr(0x176)
#define isp0_stc_ae_lv2         isp0_sfr_ptr(0x177)
#define isp0_stc_ae_lv3         isp0_sfr_ptr(0x178)
#define isp0_stc_ae_lv4         isp0_sfr_ptr(0x179)
#define isp0_stc_ae_lv5         isp0_sfr_ptr(0x17a)
#define isp0_stc_ae_lv6         isp0_sfr_ptr(0x17b)
#define isp0_stc_ae_lv7         isp0_sfr_ptr(0x17c)


#define isp0_ir_con             isp0_sfr_ptr(0x180)
#define isp0_ir_lut_r           isp0_sfr_ptr(0x181)
#define isp0_ir_lut_g           isp0_sfr_ptr(0x182)
#define isp0_ir_lut_b           isp0_sfr_ptr(0x183)
#define isp0_ir_base0           isp0_sfr_ptr(0x184)
#define isp0_ir_base1           isp0_sfr_ptr(0x185)
#define isp0_ir_base2           isp0_sfr_ptr(0x186)
#define isp0_ir_base3           isp0_sfr_ptr(0x187)
#define isp0_ir_basex           isp0_sfr_ptr(0x188)
#define isp0_ir_cen_x           isp0_sfr_ptr(0x189)
#define isp0_ir_cen_y           isp0_sfr_ptr(0x18a)
#define isp0_ir_oe_th           isp0_sfr_ptr(0x18b)
#define isp0_ir_ct_gain         isp0_sfr_ptr(0x18c)
#define isp0_ir_ob_level        isp0_sfr_ptr(0x18d)

//==============================================================//
//  isp1
//==============================================================//
#define isp1_sfr_ptr(num)   (*(volatile u32 *)(eva_base(3) + num*4))

#define isp1_pnd_con            isp1_sfr_ptr(0x000)
#define isp1_src_con            isp1_sfr_ptr(0x001)
#define isp1_src_haw            isp1_sfr_ptr(0x002)
#define isp1_src_vaw            isp1_sfr_ptr(0x003)

#define isp2_src_con            isp1_sfr_ptr(0x004)
#define isp2_src_haw            isp1_sfr_ptr(0x005)
#define isp2_src_vaw            isp1_sfr_ptr(0x006)

//==============================================================//
//  imc
//==============================================================//
#define imc_sfr_ptr(num)   (*(volatile u32 *)(eva_base(4) + num*4))

#define imc_pnd_con             imc_sfr_ptr(0x000)

#define imc_rep0_con            imc_sfr_ptr(0x001)
#define imc_rep0_h_cfg          imc_sfr_ptr(0x002)
#define imc_rep0_v_cfg          imc_sfr_ptr(0x003)
#define imc_rep0_y_base         imc_sfr_ptr(0x004)
#define imc_rep0_u_base         imc_sfr_ptr(0x005)
#define imc_rep0_v_base         imc_sfr_ptr(0x006)

#define imc_rep1_con            imc_sfr_ptr(0x009)
#define imc_rep1_h_cfg          imc_sfr_ptr(0x00a)
#define imc_rep1_v_cfg          imc_sfr_ptr(0x00b)
#define imc_rep1_y_base         imc_sfr_ptr(0x00c)
#define imc_rep1_u_base         imc_sfr_ptr(0x00d)
#define imc_rep1_v_base         imc_sfr_ptr(0x00e)

#define imc_ch0_com_con         imc_sfr_ptr(0x020)
#define imc_ch0_src_con         imc_sfr_ptr(0x021)
#define imc_ch0_crop_h          imc_sfr_ptr(0x022)
#define imc_ch0_crop_v          imc_sfr_ptr(0x023)
#define imc_ch0_h_stp           imc_sfr_ptr(0x024)
#define imc_ch0_h_wth           imc_sfr_ptr(0x025)
#define imc_ch0_v_stp           imc_sfr_ptr(0x026)
#define imc_ch0_v_wth           imc_sfr_ptr(0x027)
#define imc_ch0_dma_con         imc_sfr_ptr(0x028)
#define imc_ch0_dma_cnt         imc_sfr_ptr(0x029)
#define imc_ch0_dma_y_bs        imc_sfr_ptr(0x02a)
#define imc_ch0_dma_u_bs        imc_sfr_ptr(0x02b)
#define imc_ch0_dma_v_bs        imc_sfr_ptr(0x02c)
#define imc_ch0_osd_con         imc_sfr_ptr(0x02d)
#define imc_ch0_osd_color0      imc_sfr_ptr(0x02e)
#define imc_ch0_osd_color1      imc_sfr_ptr(0x02f)
#define imc_ch0_osd_color2      imc_sfr_ptr(0x030)
#define imc_ch0_osd_color3      imc_sfr_ptr(0x031)
#define imc_ch0_osd0_h_cfg      imc_sfr_ptr(0x032)
#define imc_ch0_osd0_v_cfg      imc_sfr_ptr(0x033)
#define imc_ch0_osd0_base       imc_sfr_ptr(0x034)
#define imc_ch0_osd1_h_cfg      imc_sfr_ptr(0x035)
#define imc_ch0_osd1_v_cfg      imc_sfr_ptr(0x036)
#define imc_ch0_osd1_base       imc_sfr_ptr(0x037)

#define imc_ch1_com_con         imc_sfr_ptr(0x040)
#define imc_ch1_src_con         imc_sfr_ptr(0x041)
#define imc_ch1_crop_h          imc_sfr_ptr(0x042)
#define imc_ch1_crop_v          imc_sfr_ptr(0x043)
#define imc_ch1_h_stp           imc_sfr_ptr(0x044)
#define imc_ch1_h_wth           imc_sfr_ptr(0x045)
#define imc_ch1_v_stp           imc_sfr_ptr(0x046)
#define imc_ch1_v_wth           imc_sfr_ptr(0x047)
#define imc_ch1_dma_con         imc_sfr_ptr(0x048)
#define imc_ch1_dma_cnt         imc_sfr_ptr(0x049)
#define imc_ch1_dma_y_bs        imc_sfr_ptr(0x04a)
#define imc_ch1_dma_u_bs        imc_sfr_ptr(0x04b)
#define imc_ch1_dma_v_bs        imc_sfr_ptr(0x04c)
#define imc_ch1_osd_con         imc_sfr_ptr(0x04d)
#define imc_ch1_osd_color0      imc_sfr_ptr(0x04e)
#define imc_ch1_osd_color1      imc_sfr_ptr(0x04f)
#define imc_ch1_osd_color2      imc_sfr_ptr(0x050)
#define imc_ch1_osd_color3      imc_sfr_ptr(0x051)
#define imc_ch1_osd0_h_cfg      imc_sfr_ptr(0x052)
#define imc_ch1_osd0_v_cfg      imc_sfr_ptr(0x053)
#define imc_ch1_osd0_base       imc_sfr_ptr(0x054)
#define imc_ch1_osd1_h_cfg      imc_sfr_ptr(0x055)
#define imc_ch1_osd1_v_cfg      imc_sfr_ptr(0x056)
#define imc_ch1_osd1_base       imc_sfr_ptr(0x057)

#define imc_ch2_com_con         imc_sfr_ptr(0x060)
#define imc_ch2_src_con         imc_sfr_ptr(0x061)
#define imc_ch2_crop_h          imc_sfr_ptr(0x062)
#define imc_ch2_crop_v          imc_sfr_ptr(0x063)
#define imc_ch2_h_stp           imc_sfr_ptr(0x064)
#define imc_ch2_h_wth           imc_sfr_ptr(0x065)
#define imc_ch2_v_stp           imc_sfr_ptr(0x066)
#define imc_ch2_v_wth           imc_sfr_ptr(0x067)
#define imc_ch2_dma_con         imc_sfr_ptr(0x068)
#define imc_ch2_dma_cnt         imc_sfr_ptr(0x069)
#define imc_ch2_dma_y_bs        imc_sfr_ptr(0x06a)
#define imc_ch2_dma_u_bs        imc_sfr_ptr(0x06b)
#define imc_ch2_dma_v_bs        imc_sfr_ptr(0x06c)

#define imc_ch3_com_con         imc_sfr_ptr(0x070)
#define imc_ch3_src_con         imc_sfr_ptr(0x071)
#define imc_ch3_crop_h          imc_sfr_ptr(0x072)
#define imc_ch3_crop_v          imc_sfr_ptr(0x073)
#define imc_ch3_h_stp           imc_sfr_ptr(0x074)
#define imc_ch3_h_wth           imc_sfr_ptr(0x075)
#define imc_ch3_v_stp           imc_sfr_ptr(0x076)
#define imc_ch3_v_wth           imc_sfr_ptr(0x077)
#define imc_ch3_dma_con         imc_sfr_ptr(0x078)
#define imc_ch3_dma_cnt         imc_sfr_ptr(0x079)
#define imc_ch3_dma_y_bs        imc_sfr_ptr(0x07a)
#define imc_ch3_dma_u_bs        imc_sfr_ptr(0x07b)
#define imc_ch3_dma_v_bs        imc_sfr_ptr(0x07c)

#define imc_ch4_com_con         imc_sfr_ptr(0x080)
#define imc_ch4_src_con         imc_sfr_ptr(0x081)
#define imc_ch4_crop_h          imc_sfr_ptr(0x082)
#define imc_ch4_crop_v          imc_sfr_ptr(0x083)
#define imc_ch4_h_stp           imc_sfr_ptr(0x084)
#define imc_ch4_h_wth           imc_sfr_ptr(0x085)
#define imc_ch4_v_stp           imc_sfr_ptr(0x086)
#define imc_ch4_v_wth           imc_sfr_ptr(0x087)
#define imc_ch4_dma_con         imc_sfr_ptr(0x088)
#define imc_ch4_dma_cnt         imc_sfr_ptr(0x089)
#define imc_ch4_dma_y_bs        imc_sfr_ptr(0x08a)
#define imc_ch4_dma_u_bs        imc_sfr_ptr(0x08b)
#define imc_ch4_dma_v_bs        imc_sfr_ptr(0x08c)

#define imc_ch5_com_con         imc_sfr_ptr(0x090)
#define imc_ch5_src_con         imc_sfr_ptr(0x091)
#define imc_ch5_crop_h          imc_sfr_ptr(0x092)
#define imc_ch5_crop_v          imc_sfr_ptr(0x093)
#define imc_ch5_h_stp           imc_sfr_ptr(0x094)
#define imc_ch5_h_wth           imc_sfr_ptr(0x095)
#define imc_ch5_v_stp           imc_sfr_ptr(0x096)
#define imc_ch5_v_wth           imc_sfr_ptr(0x097)
#define imc_ch5_dma_con         imc_sfr_ptr(0x098)
#define imc_ch5_dma_cnt         imc_sfr_ptr(0x099)
#define imc_ch5_dma_y_bs        imc_sfr_ptr(0x09a)
#define imc_ch5_dma_u_bs        imc_sfr_ptr(0x09b)
#define imc_ch5_dma_v_bs        imc_sfr_ptr(0x09c)

#define imc_lex_com_con         imc_sfr_ptr(0x0a0)
#define imc_lex_tpz_cfg0        imc_sfr_ptr(0x0a1)
#define imc_lex_tpz_cfg1        imc_sfr_ptr(0x0a2)
#define imc_lex_tpz_cfg2        imc_sfr_ptr(0x0a3)
#define imc_lex_sca_wth         imc_sfr_ptr(0x0a4)
#define imc_lex_dma_con         imc_sfr_ptr(0x0a5)
#define imc_lex_dma_base0       imc_sfr_ptr(0x0a6)
#define imc_lex_dma_base1       imc_sfr_ptr(0x0a7)

//==============================================================//
//  imb
//==============================================================//
#define imb_sfr_ptr(num)   (*(volatile u32 *)(eva_base(5) + num*4))

#define imb_con                 imb_sfr_ptr(0x00)
#define imb_htt_with            imb_sfr_ptr(0x01)
#define imb_vtt_high            imb_sfr_ptr(0x02)
#define imb_buf_con             imb_sfr_ptr(0x03)
#define imb_mix_con             imb_sfr_ptr(0x04)
#define imb_o1b_dat             imb_sfr_ptr(0x05)
#define imb_grp0_d0             imb_sfr_ptr(0x06)
#define imb_grp0_d1             imb_sfr_ptr(0x07)
#define imb_grp0_d2             imb_sfr_ptr(0x08)
#define imb_grp1_d0             imb_sfr_ptr(0x09)
#define imb_grp1_d1             imb_sfr_ptr(0x0a)
#define imb_grp1_d2             imb_sfr_ptr(0x0b)
#define imb_osd_tab0            imb_sfr_ptr(0x0c)
#define imb_osd_tab1            imb_sfr_ptr(0x0d)

#define imb_l0_con              imb_sfr_ptr(0x20)
#define imb_l0_h_cfg0           imb_sfr_ptr(0x21)
#define imb_l0_h_cfg1           imb_sfr_ptr(0x22)
#define imb_l0_v_cfg0           imb_sfr_ptr(0x23)
#define imb_l0_v_cfg1           imb_sfr_ptr(0x24)
#define imb_l0_ha_with          imb_sfr_ptr(0x25)
#define imb_l0_aph              imb_sfr_ptr(0x26)
#define imb_l0_badr0            imb_sfr_ptr(0x27)
#define imb_l0_badr1            imb_sfr_ptr(0x28)
#define imb_l0_badr2            imb_sfr_ptr(0x29)
#define imb_l0_sc_dat           imb_sfr_ptr(0x2a)
#define imb_l0_ht_with          imb_sfr_ptr(0x2b)

#define imb_l1_con              imb_sfr_ptr(0x30)
#define imb_l1_h_cfg0           imb_sfr_ptr(0x31)
#define imb_l1_h_cfg1           imb_sfr_ptr(0x32)
#define imb_l1_v_cfg0           imb_sfr_ptr(0x33)
#define imb_l1_v_cfg1           imb_sfr_ptr(0x34)
#define imb_l1_ha_with          imb_sfr_ptr(0x35)
#define imb_l1_aph              imb_sfr_ptr(0x36)
#define imb_l1_badr0            imb_sfr_ptr(0x37)
#define imb_l1_badr1            imb_sfr_ptr(0x38)
#define imb_l1_badr2            imb_sfr_ptr(0x39)
#define imb_l1_sc_dat           imb_sfr_ptr(0x3a)
#define imb_l1_ht_with          imb_sfr_ptr(0x3b)

#define imb_l2_con              imb_sfr_ptr(0x40)
#define imb_l2_h_cfg0           imb_sfr_ptr(0x41)
#define imb_l2_h_cfg1           imb_sfr_ptr(0x42)
#define imb_l2_v_cfg0           imb_sfr_ptr(0x43)
#define imb_l2_v_cfg1           imb_sfr_ptr(0x44)
#define imb_l2_ha_with          imb_sfr_ptr(0x45)
#define imb_l2_aph              imb_sfr_ptr(0x46)
#define imb_l2_badr0            imb_sfr_ptr(0x47)
#define imb_l2_badr1            imb_sfr_ptr(0x48)
#define imb_l2_badr2            imb_sfr_ptr(0x49)
#define imb_l2_sc_dat           imb_sfr_ptr(0x4a)
#define imb_l2_ht_with          imb_sfr_ptr(0x4b)

#define imb_l3_con              imb_sfr_ptr(0x50)
#define imb_l3_h_cfg0           imb_sfr_ptr(0x51)
#define imb_l3_h_cfg1           imb_sfr_ptr(0x52)
#define imb_l3_v_cfg0           imb_sfr_ptr(0x53)
#define imb_l3_v_cfg1           imb_sfr_ptr(0x54)
#define imb_l3_ha_with          imb_sfr_ptr(0x55)
#define imb_l3_aph              imb_sfr_ptr(0x56)
#define imb_l3_badr0            imb_sfr_ptr(0x57)
#define imb_l3_badr1            imb_sfr_ptr(0x58)
#define imb_l3_badr2            imb_sfr_ptr(0x59)
#define imb_l3_sc_dat           imb_sfr_ptr(0x5a)
#define imb_l3_ht_with          imb_sfr_ptr(0x5b)

#define imb_l4_con              imb_sfr_ptr(0x60)
#define imb_l4_h_cfg0           imb_sfr_ptr(0x61)
#define imb_l4_h_cfg1           imb_sfr_ptr(0x62)
#define imb_l4_v_cfg0           imb_sfr_ptr(0x63)
#define imb_l4_v_cfg1           imb_sfr_ptr(0x64)
#define imb_l4_ha_with          imb_sfr_ptr(0x65)
#define imb_l4_aph              imb_sfr_ptr(0x66)
#define imb_l4_badr0            imb_sfr_ptr(0x67)
#define imb_l4_badr1            imb_sfr_ptr(0x68)
#define imb_l4_badr2            imb_sfr_ptr(0x69)
#define imb_l4_sc_dat           imb_sfr_ptr(0x6a)
#define imb_l4_ht_with          imb_sfr_ptr(0x6b)

#define imb_l5_con              imb_sfr_ptr(0x70)
#define imb_l5_h_cfg0           imb_sfr_ptr(0x71)
#define imb_l5_h_cfg1           imb_sfr_ptr(0x72)
#define imb_l5_v_cfg0           imb_sfr_ptr(0x73)
#define imb_l5_v_cfg1           imb_sfr_ptr(0x74)
#define imb_l5_ha_with          imb_sfr_ptr(0x75)
#define imb_l5_aph              imb_sfr_ptr(0x76)
#define imb_l5_badr0            imb_sfr_ptr(0x77)
#define imb_l5_badr1            imb_sfr_ptr(0x78)
#define imb_l5_badr2            imb_sfr_ptr(0x79)
#define imb_l5_sc_dat           imb_sfr_ptr(0x7a)
#define imb_l5_ht_with          imb_sfr_ptr(0x7b)

#define imb_l6_con              imb_sfr_ptr(0x80)
#define imb_l6_h_cfg0           imb_sfr_ptr(0x81)
#define imb_l6_h_cfg1           imb_sfr_ptr(0x82)
#define imb_l6_v_cfg0           imb_sfr_ptr(0x83)
#define imb_l6_v_cfg1           imb_sfr_ptr(0x84)
#define imb_l6_ha_with          imb_sfr_ptr(0x85)
#define imb_l6_aph              imb_sfr_ptr(0x86)
#define imb_l6_badr0            imb_sfr_ptr(0x87)
#define imb_l6_badr1            imb_sfr_ptr(0x88)
#define imb_l6_badr2            imb_sfr_ptr(0x89)
#define imb_l6_sc_dat           imb_sfr_ptr(0x8a)
#define imb_l6_ht_with          imb_sfr_ptr(0x8b)

#define imb_l7_con              imb_sfr_ptr(0x90)
#define imb_l7_h_cfg0           imb_sfr_ptr(0x91)
#define imb_l7_h_cfg1           imb_sfr_ptr(0x92)
#define imb_l7_v_cfg0           imb_sfr_ptr(0x93)
#define imb_l7_v_cfg1           imb_sfr_ptr(0x94)
#define imb_l7_ha_with          imb_sfr_ptr(0x95)
#define imb_l7_aph              imb_sfr_ptr(0x96)
#define imb_l7_badr0            imb_sfr_ptr(0x97)
#define imb_l7_badr1            imb_sfr_ptr(0x98)
#define imb_l7_badr2            imb_sfr_ptr(0x99)
#define imb_l7_sc_dat           imb_sfr_ptr(0x9a)
#define imb_l7_ht_with          imb_sfr_ptr(0x9b)

#define imb_l8_con              imb_sfr_ptr(0xa0)
#define imb_l8_h_cfg0           imb_sfr_ptr(0xa1)
#define imb_l8_h_cfg1           imb_sfr_ptr(0xa2)
#define imb_l8_v_cfg0           imb_sfr_ptr(0xa3)
#define imb_l8_v_cfg1           imb_sfr_ptr(0xa4)
#define imb_l8_ha_with          imb_sfr_ptr(0xa5)
#define imb_l8_aph              imb_sfr_ptr(0xa6)
#define imb_l8_badr0            imb_sfr_ptr(0xa7)
#define imb_l8_badr1            imb_sfr_ptr(0xa8)
#define imb_l8_badr2            imb_sfr_ptr(0xa9)
#define imb_l8_sc_dat           imb_sfr_ptr(0xaa)
#define imb_l8_ht_with          imb_sfr_ptr(0xab)

#define imb_l9_con              imb_sfr_ptr(0xb0)
#define imb_l9_h_cfg0           imb_sfr_ptr(0xb1)
#define imb_l9_h_cfg1           imb_sfr_ptr(0xb2)
#define imb_l9_v_cfg0           imb_sfr_ptr(0xb3)
#define imb_l9_v_cfg1           imb_sfr_ptr(0xb4)
#define imb_l9_ha_with          imb_sfr_ptr(0xb5)
#define imb_l9_aph              imb_sfr_ptr(0xb6)
#define imb_l9_badr0            imb_sfr_ptr(0xb7)
#define imb_l9_badr1            imb_sfr_ptr(0xb8)
#define imb_l9_badr2            imb_sfr_ptr(0xb9)
#define imb_l9_sc_dat           imb_sfr_ptr(0xba)
#define imb_l9_ht_with          imb_sfr_ptr(0xbb)

#define imb_out_con             imb_sfr_ptr(0xc0)
#define imb_out_badr            imb_sfr_ptr(0xc1)
#define imb_o_y_badr0           imb_sfr_ptr(0xc2)
#define imb_o_u_badr0           imb_sfr_ptr(0xc3)
#define imb_o_v_badr0           imb_sfr_ptr(0xc4)
#define imb_o_y_badr1           imb_sfr_ptr(0xc5)
#define imb_o_u_badr1           imb_sfr_ptr(0xc6)
#define imb_o_v_badr1           imb_sfr_ptr(0xc7)
#define imb_scn_con             imb_sfr_ptr(0xc8)

//==============================================================//
//  imr
//==============================================================//
#define imr_sfr_ptr(num)   (*(volatile u32 *)(eva_base(6) + num*4))

#define imr_scn_con             imr_sfr_ptr(0x00)
#define imr_con                 imr_sfr_ptr(0x01)
#define imr_haw                 imr_sfr_ptr(0x02)
#define imr_vaw                 imr_sfr_ptr(0x03)
#define imr_b0_adr0             imr_sfr_ptr(0x04)
#define imr_b0_adr1             imr_sfr_ptr(0x05)
#define imr_b0_adr2             imr_sfr_ptr(0x06)
#define imr_b1_adr0             imr_sfr_ptr(0x07)
#define imr_b1_adr1             imr_sfr_ptr(0x08)
#define imr_b1_adr2             imr_sfr_ptr(0x09)
#define imr_wr_adr0             imr_sfr_ptr(0x0a)
#define imr_wr_adr1             imr_sfr_ptr(0x0b)
#define imr_wr_adr2             imr_sfr_ptr(0x0c)

#define fec_con                 imr_sfr_ptr(0x10)
#define fec_src_info            imr_sfr_ptr(0x11)
#define fec_dst_info            imr_sfr_ptr(0x12)
#define fec_stride              imr_sfr_ptr(0x13)
#define fec_inimg_adr           imr_sfr_ptr(0x14)
#define fec_ouimg_adr           imr_sfr_ptr(0x15)
#define fec_grid_adr            imr_sfr_ptr(0x16)

//==============================================================//
//  imd
//==============================================================//
#define dmm_sfr_ptr(num)    (*(volatile u32 *)(eva_base(7) + 0x000 + num*4))
#define dpi_sfr_ptr(num)    (*(volatile u32 *)(eva_base(7) + 0x200 + num*4))

#define imd_dmm_r_gain          dmm_sfr_ptr(0x00)
#define imd_dmm_g_gain          dmm_sfr_ptr(0x01)
#define imd_dmm_b_gain          dmm_sfr_ptr(0x02)
#define imd_dmm_r_offs          dmm_sfr_ptr(0x03)
#define imd_dmm_g_offs          dmm_sfr_ptr(0x04)
#define imd_dmm_b_offs          dmm_sfr_ptr(0x05)
#define imd_dmm_r_coe0          dmm_sfr_ptr(0x06)
#define imd_dmm_r_coe1          dmm_sfr_ptr(0x07)
#define imd_dmm_r_coe2          dmm_sfr_ptr(0x08)
#define imd_dmm_g_coe0          dmm_sfr_ptr(0x09)
#define imd_dmm_g_coe1          dmm_sfr_ptr(0x0a)
#define imd_dmm_g_coe2          dmm_sfr_ptr(0x0b)
#define imd_dmm_b_coe0          dmm_sfr_ptr(0x0c)
#define imd_dmm_b_coe1          dmm_sfr_ptr(0x0d)
#define imd_dmm_b_coe2          dmm_sfr_ptr(0x0e)

#define imd_dmm_gmm_r0          dmm_sfr_ptr(0x10)
#define imd_dmm_gmm_g0          dmm_sfr_ptr(0x11)
#define imd_dmm_gmm_b0          dmm_sfr_ptr(0x12)
#define imd_dmm_gmm_r1          dmm_sfr_ptr(0x13)
#define imd_dmm_gmm_g1          dmm_sfr_ptr(0x14)
#define imd_dmm_gmm_b1          dmm_sfr_ptr(0x15)
#define imd_dmm_y_gain          dmm_sfr_ptr(0x16)
#define imd_dmm_u_gain          dmm_sfr_ptr(0x17)
#define imd_dmm_v_gain          dmm_sfr_ptr(0x18)
#define imd_dmm_y_offs          dmm_sfr_ptr(0x19)
#define imd_dmm_u_offs          dmm_sfr_ptr(0x1a)
#define imd_dmm_v_offs          dmm_sfr_ptr(0x1b)

#define imd_dmm_haw             dmm_sfr_ptr(0x20)
#define imd_dmm_vaw             dmm_sfr_ptr(0x21)
#define imd_dmm_htw             dmm_sfr_ptr(0x22)
#define imd_dmm_vtw             dmm_sfr_ptr(0x23)
#define imd_dmm_db_con          dmm_sfr_ptr(0x24)
#define imd_dmm_sc_dat          dmm_sfr_ptr(0x25)
#define imd_scn_con             dmm_sfr_ptr(0x26)

#define imd_dmm_tpz_cfg0        dmm_sfr_ptr(0x27)
#define imd_dmm_tpz_cfg1        dmm_sfr_ptr(0x28)
#define imd_dmm_tpz_cfg2        dmm_sfr_ptr(0x29)
#define imd_dmm_tpz_sca_wth     dmm_sfr_ptr(0x2a)


#define imd_dmm_y_oadr          dmm_sfr_ptr(0x30)
#define imd_dmm_y_eadr          dmm_sfr_ptr(0x31)
#define imd_dmm_u_oadr          dmm_sfr_ptr(0x32)
#define imd_dmm_u_eadr          dmm_sfr_ptr(0x33)
#define imd_dmm_v_oadr          dmm_sfr_ptr(0x34)
#define imd_dmm_v_eadr          dmm_sfr_ptr(0x35)

#define imd_sca_con             dmm_sfr_ptr(0x40)
#define imd_sca_stp_h           dmm_sfr_ptr(0x41)
#define imd_sca_wth_h           dmm_sfr_ptr(0x42)
#define imd_sca_stp_v           dmm_sfr_ptr(0x43)
#define imd_sca_wth_v           dmm_sfr_ptr(0x44)

#define imd_bgm_con             dmm_sfr_ptr(0x45)
#define imd_bgm_dat             dmm_sfr_ptr(0x46)
#define imd_bgm_hst             dmm_sfr_ptr(0x47)
#define imd_bgm_vst             dmm_sfr_ptr(0x48)
#define imd_bgm_haw             dmm_sfr_ptr(0x49)
#define imd_bgm_vaw             dmm_sfr_ptr(0x4a)

#define imd_dpi_clk_con         dpi_sfr_ptr(0x00)
#define imd_dpi_io_con          dpi_sfr_ptr(0x01)
#define imd_dpi_con             dpi_sfr_ptr(0x02)
#define imd_dpi_fmt             dpi_sfr_ptr(0x03)
#define imd_dpi_emi             dpi_sfr_ptr(0x04)
#define imd_dpi_htt_cfg         dpi_sfr_ptr(0x05)
#define imd_dpi_hsw_cfg         dpi_sfr_ptr(0x06)
#define imd_dpi_hst_cfg         dpi_sfr_ptr(0x07)
#define imd_dpi_haw_cfg         dpi_sfr_ptr(0x08)
#define imd_dpi_vtt_cfg         dpi_sfr_ptr(0x09)
#define imd_dpi_vsw_cfg         dpi_sfr_ptr(0x0a)
#define imd_dpi_vst_o_cfg       dpi_sfr_ptr(0x0b)
#define imd_dpi_vst_e_cfg       dpi_sfr_ptr(0x0c)
#define imd_dpi_vaw_cfg         dpi_sfr_ptr(0x0d)

//==============================================================//
//  mipi csi
//==============================================================//
#define csi_sfr_ptr(num)   (*(volatile u32 *)(eva_base(8) + num*4))

#define csi_sys_con             csi_sfr_ptr(0x00)

#define csi_rmap_con            csi_sfr_ptr(0x01)
#define csi_lane_con            csi_sfr_ptr(0x02)
#define csi_tval_con            csi_sfr_ptr(0x03)
#define csi_task_con            csi_sfr_ptr(0x04)
#define csi_task_haw            csi_sfr_ptr(0x05)
#define csi_task_vaw            csi_sfr_ptr(0x06)
#define csi_phy_con0            csi_sfr_ptr(0x07)
#define csi_phy_con1            csi_sfr_ptr(0x08)
#define csi_phy_con2            csi_sfr_ptr(0x09)

#define lvds_rx_pll_con         csi_sfr_ptr(0x10)
#define lvds_rx_pll_nf          csi_sfr_ptr(0x11)
#define lvds_rx_pll_nr          csi_sfr_ptr(0x12)
#define lvds_rx_phy_con         csi_sfr_ptr(0x13)
#define lvds_rx_dec0_con        csi_sfr_ptr(0x14)
#define lvds_rx_lane0_con       csi_sfr_ptr(0x15)
#define lvds_rx_dec1_con        csi_sfr_ptr(0x16)
#define lvds_rx_lane1_con       csi_sfr_ptr(0x17)
#define lvds_rx_out_con         csi_sfr_ptr(0x18)
#define lvds_rx_out_haw         csi_sfr_ptr(0x19)
#define lvds_rx_out_vaw         csi_sfr_ptr(0x1a)


//==============================================================//
//  mipi
//==============================================================//
#define dsi_s_sfr_ptr(num)    (*(volatile u32 *)(eva_base(9) + 0x000 + num*4))
#define dsi_d_sfr_ptr(num)    (*(volatile u32 *)(eva_base(9) + 0x100 + num*4))

#define dsi_sys_con             dsi_s_sfr_ptr(0x00)
#define dsi_vdo_fmt             dsi_s_sfr_ptr(0x10)
#define dsi_vdo_vsa             dsi_s_sfr_ptr(0x11)
#define dsi_vdo_vbp             dsi_s_sfr_ptr(0x12)
#define dsi_vdo_vact            dsi_s_sfr_ptr(0x13)
#define dsi_vdo_vfp             dsi_s_sfr_ptr(0x14)
#define dsi_vdo_hsa             dsi_s_sfr_ptr(0x15)
#define dsi_vdo_hbp             dsi_s_sfr_ptr(0x16)
#define dsi_vdo_hact            dsi_s_sfr_ptr(0x17)
#define dsi_vdo_hfp             dsi_s_sfr_ptr(0x18)
#define dsi_vdo_bllp0           dsi_s_sfr_ptr(0x19)
#define dsi_vdo_bllp1           dsi_s_sfr_ptr(0x1a)
#define dsi_frm_tval            dsi_s_sfr_ptr(0x1b)
#define dsi_tval_con0           dsi_s_sfr_ptr(0x20)
#define dsi_tval_con1           dsi_s_sfr_ptr(0x21)
#define dsi_tval_con2           dsi_s_sfr_ptr(0x22)
#define dsi_tval_con3           dsi_s_sfr_ptr(0x23)
#define dsi_tval_con4           dsi_s_sfr_ptr(0x24)
#define dsi_tval_con5           dsi_s_sfr_ptr(0x25)
#define dsi_rmap_con            dsi_s_sfr_ptr(0x26)
#define dsi_pll_con0            dsi_s_sfr_ptr(0x30)
#define dsi_pll_con1            dsi_s_sfr_ptr(0x31)
#define dsi_phy_con0            dsi_s_sfr_ptr(0x32)
#define dsi_phy_con1            dsi_s_sfr_ptr(0x33)
#define dsi_phy_con2            dsi_s_sfr_ptr(0x34)
#define dsi_phy_con3            dsi_s_sfr_ptr(0x35)

#define dsi_task_con            dsi_d_sfr_ptr(0x00)
#define dsi_bus_con             dsi_d_sfr_ptr(0x01)
#define dsi_cmd_con0            dsi_d_sfr_ptr(0x02)
#define dsi_cmd_con1            dsi_d_sfr_ptr(0x03)
#define dsi_cmd_con2            dsi_d_sfr_ptr(0x04)
#define dsi_cmd_fifo            dsi_d_sfr_ptr(0x05)
#define dsi_lane_con            dsi_d_sfr_ptr(0x06)

//==============================================================//
//  avo
//==============================================================//
#define avo_base0_sfr_ptr(num)  (*(volatile u32 *)(eva_base(7) + num*4))
#define avo_con0                avo_base0_sfr_ptr(0x00)
#define avo_con1                avo_base0_sfr_ptr(0x01)
#define avo_con2                avo_base0_sfr_ptr(0x02)

#define avo_base1_sfr_ptr(num)  (*(volatile u32 *)(eva_base(7) + 0x200 + num*4))
#define avo_soft_reset          avo_base1_sfr_ptr(62)
#define avo_system_con          avo_base1_sfr_ptr(52)
#define avo_slave_mode          avo_base1_sfr_ptr(13)
#define avo_clrbar_mode         avo_base1_sfr_ptr(5 )

#define avo_chroma_freq0        avo_base1_sfr_ptr(0 )
#define avo_chroma_freq1        avo_base1_sfr_ptr(1 )
#define avo_chroma_freq2        avo_base1_sfr_ptr(2 )
#define avo_chroma_freq3        avo_base1_sfr_ptr(3 )
#define avo_chroma_phase        avo_base1_sfr_ptr(4 )

#define avo_black_level_h       avo_base1_sfr_ptr(14)
#define avo_black_level_l       avo_base1_sfr_ptr(15)
#define avo_blank_level_h       avo_base1_sfr_ptr(16)
#define avo_blank_level_l       avo_base1_sfr_ptr(17)
#define avo_white_level_h       avo_base1_sfr_ptr(30)
#define avo_white_level_l       avo_base1_sfr_ptr(31)

#define avo_hsync_width         avo_base1_sfr_ptr(8 )
#define avo_burst_width         avo_base1_sfr_ptr(9 )
#define avo_back_porch          avo_base1_sfr_ptr(10)
#define avo_breeze_way          avo_base1_sfr_ptr(41)
#define avo_front_porch         avo_base1_sfr_ptr(44)

#define avo_start_line          avo_base1_sfr_ptr(51)
#define avo_num_lines_h         avo_base1_sfr_ptr(23)
#define avo_num_lines_l         avo_base1_sfr_ptr(24)
#define avo_line_period_h       avo_base1_sfr_ptr(49)
#define avo_line_period_l       avo_base1_sfr_ptr(50)

#define avo_cb_burst_amp        avo_base1_sfr_ptr(11)
#define avo_cr_burst_amp        avo_base1_sfr_ptr(12)
#define avo_cb_gain             avo_base1_sfr_ptr(32)
#define avo_cr_gain             avo_base1_sfr_ptr(34)
//==============================================================//
//  cvbs
//==============================================================//
#define cvbs_sfr_ptr(num)       (*(volatile u32 *)(eva_base(0xa) + num*4))
#define cvbs_clk_con            cvbs_sfr_ptr(0x0)
#define cvbs_dac_con0           cvbs_sfr_ptr(0x1)
#define cvbs_dac_con1           cvbs_sfr_ptr(0x2)
#define cvbs_dac_con2           cvbs_sfr_ptr(0x3)
#define cvbs_dac_con3           cvbs_sfr_ptr(0x4)
#define cvbs_dac_con4           cvbs_sfr_ptr(0x5)

#define tve_sfr_ptr(num)        (*(volatile u32 *)(eva_base(0xa) + 0x400 + num*4))
#define tve_con0                tve_sfr_ptr(0)
#define tve_con1                tve_sfr_ptr(1)
#define tve_con2                tve_sfr_ptr(2)
#define tve_con3                tve_sfr_ptr(3)
#define tve_con4                tve_sfr_ptr(4)
#define tve_con5                tve_sfr_ptr(5)
#define tve_con6                tve_sfr_ptr(6)
#define tve_con7                tve_sfr_ptr(7)
#define tve_con8                tve_sfr_ptr(8)
#define tve_con9                tve_sfr_ptr(9)
#define tve_con10               tve_sfr_ptr(10)
#define tve_con11               tve_sfr_ptr(11)
#define tve_con12               tve_sfr_ptr(12)
#define tve_con13               tve_sfr_ptr(13)
#define tve_con14               tve_sfr_ptr(14)
#define tve_con15               tve_sfr_ptr(15)
#define tve_con16               tve_sfr_ptr(16)
#define tve_con17               tve_sfr_ptr(17)
#define tve_con18               tve_sfr_ptr(18)

#define cve_sfr_ptr(num)        (*(volatile u32 *)(eva_base(0xa) + 0x600 + num*4))
#define cve_soft_reset          cve_sfr_ptr(62)
#define cve_system_con          cve_sfr_ptr(52)
#define cve_slave_mode          cve_sfr_ptr(13)
#define cve_clrbar_mode         cve_sfr_ptr(5)
#define cve_chroma_freq0        cve_sfr_ptr(0)
#define cve_chroma_freq1        cve_sfr_ptr(1)
#define cve_chroma_freq2        cve_sfr_ptr(2)
#define cve_chroma_freq3        cve_sfr_ptr(3)
#define cve_chroma_phase        cve_sfr_ptr(4)
#define cve_black_level_h       cve_sfr_ptr(14)
#define cve_black_level_l       cve_sfr_ptr(15)
#define cve_blank_level_h       cve_sfr_ptr(16)
#define cve_blank_level_l       cve_sfr_ptr(17)
#define cve_white_level_h       cve_sfr_ptr(30)
#define cve_white_level_l       cve_sfr_ptr(31)
#define cve_hsync_width         cve_sfr_ptr(8)
#define cve_burst_width         cve_sfr_ptr(9)
#define cve_back_porch          cve_sfr_ptr(10)
#define cve_breeze_way          cve_sfr_ptr(41)
#define cve_front_porch         cve_sfr_ptr(44)
#define cve_start_line          cve_sfr_ptr(51)
#define cve_num_lines_h         cve_sfr_ptr(23)
#define cve_num_lines_l         cve_sfr_ptr(24)
#define cve_line_period_h       cve_sfr_ptr(49)
#define cve_line_period_l       cve_sfr_ptr(50)
#define cve_cb_burst_amp        cve_sfr_ptr(11)
#define cve_cr_burst_amp        cve_sfr_ptr(12)
#define cve_cb_gain             cve_sfr_ptr(32)
#define cve_cr_gain             cve_sfr_ptr(34)

//==============================================================//
//  mcv
//==============================================================//
#define mcv_sfr_ptr(num)   (*(volatile u32 *)(eva_base(0xb) + num*4))

#define mcv_scon                mcv_sfr_ptr(0x00)
#define mcv_mcon                mcv_sfr_ptr(0x01)
#define mcv0_dcon               mcv_sfr_ptr(0x02)
#define mcv0_vact               mcv_sfr_ptr(0x03)
#define mcv0_hact               mcv_sfr_ptr(0x04)
#define mcv1_dcon               mcv_sfr_ptr(0x05)
#define mcv1_vact               mcv_sfr_ptr(0x06)
#define mcv1_hact               mcv_sfr_ptr(0x07)
#define mcv2_dcon               mcv_sfr_ptr(0x08)
#define mcv2_vact               mcv_sfr_ptr(0x09)
#define mcv2_hact               mcv_sfr_ptr(0x0a)
#define mcv3_dcon               mcv_sfr_ptr(0x0b)
#define mcv3_vact               mcv_sfr_ptr(0x0c)
#define mcv3_hact               mcv_sfr_ptr(0x0d)

//==============================================================//
//  ltx
//==============================================================//
#define ltx_sfr_ptr(num)   (*(volatile u32 *)(eva_base(0xc) + num*4))

#define lvds_tx_con             ltx_sfr_ptr(0x00)
#define lvds_tx_vtt             ltx_sfr_ptr(0x01)
#define lvds_tx_vsyc            ltx_sfr_ptr(0x02)
#define lvds_tx_vstr            ltx_sfr_ptr(0x03)
#define lvds_tx_vend            ltx_sfr_ptr(0x04)
#define lvds_tx_htt             ltx_sfr_ptr(0x05)
#define lvds_tx_hsyc            ltx_sfr_ptr(0x06)
#define lvds_tx_hstr            ltx_sfr_ptr(0x07)
#define lvds_tx_hend            ltx_sfr_ptr(0x08)
#define lvds_tx_ln_map          ltx_sfr_ptr(0x09)

#define lvds_tx_phy_con0        ltx_sfr_ptr(0x0a)
#define lvds_tx_phy_con1        ltx_sfr_ptr(0x0b)

//==============================================================//
//  ???
//==============================================================//
#define OETH_REG_BASE           0x3f88000
#define OETH_REG_BASE_ADD       0x400
#define OETH_BD_BASE            (OETH_REG_BASE + OETH_REG_BASE_ADD)
#define OETH_TOTAL_BD           128


#endif




