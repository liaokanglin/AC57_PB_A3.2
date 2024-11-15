#include "generic/typedef.h"
#include "asm/dsi.h"
#include "asm/lcd_config.h"
#include "device/lcd_driver.h"
#include "gpio.h"
#include "asm/pwm.h"
#include "system/includes.h"

#ifdef LCD_DSI_VDO_4LANE_1280x800_RP

#define freq 394

#define lane_num 4
#define bpp_num  24

//垂直时序要求比较严
#define vsa_line 2
#define vbp_line 10
#define vda_line 1280
#define vfp_line 10

#define hsa_pixel  10
#define hbp_pixel  10
#define hda_pixel  800
#define hfp_pixel  20

//	dcs_send_short_p0_bta(0x28);


const static u8 init_cmd_list[] = {




//////////////////Initial  CODE///////////////////////

 _W, DELAY(0),  PACKET_DCS, SIZE(4),
0xFF,
0x98,
0x81,
0x03,

//GIP_1

 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x01,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x02,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x03,
0x73,       //STVA=STV1~4
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x04,
0x13,       //STVB=STV0 
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x05,
0x00,        
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x06,
0x0A,       //STVA_Rise
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x07,
0x05,       //STVB_Rise 
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x08,
0x00,        
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x09,
0x28,       //FTI1R(A) STV1=3.6H      
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x0a,
0x00,            
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x0b,
0x00,        
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x0c,
0x00,        
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x0d,
0x28,       //FTI2F(B) STV0=3.6H    
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x0e,
0x00,        
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x0f,
0x28,       //CLW1(ALR) Duty=45%
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x10,
0x28,       //CLW2(ARR) Duty=45%
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x11,
0x00,           
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x12,
0x00,        
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x13,
0x00,        
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x14,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x15,
0x00,        
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x16,
0x00,        
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x17,
0x00,        
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x18,
0x00,        
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x19,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x1a,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x1b,
0x00,   
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x1c,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x1d,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x1e,
0x40,       //CLKA 40自動反 C0手動反(X8參考CLKB)
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x1f,
0x80,       //C0
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x20,
0x06,       //CLKA_Rise
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x21,
0x01,       //CLKA_Fall
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x22,
0x00,        
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x23,
0x00,        
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x24,
0x00,        
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x25,
0x00,        
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x26,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x27,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x28,
0x33,       //CLK Phase
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x29,
0x33,       //CLK overlap
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x2a,
0x00,  
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x2b,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x2c,
0x04,       //GCH R
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x2d,
0x04,       //GCL R 
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x2e,
0x05,       //GCH F        
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x2f,
0x05,       //GCL F       
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x30,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x31,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x32,
0x31,       //GCH/L ext2/1行為  5E 01:31   5E 00:42      
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x33,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x34,
0x00,      
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x35,
0x0A,       //GCH/L 區間 00:VS前 01:VS後 10:跨VS 11:frame中             
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x36,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x37,
0x08,       //GCH/L      
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x38,
0x00,	
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x39,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x3a,
0x00, 
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x3b,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x3c,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x3d,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x3e,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x3f,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x40,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x41,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x42,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x43,
0x08,       //GCH/L      
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x44,
0x00,


//GIP_2
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x50,
0x01,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x51,
0x23,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x52,
0x44,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x53,
0x67,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x54,
0x89,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x55,
0xab,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x56,
0x01,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x57,
0x23,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x58,
0x45,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x59,
0x67,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x5a,
0x89,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x5b,
0xab,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x5c,
0xcd,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x5d,
0xef,

//GIP_3
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x5e,
0x11,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x5f,
0x02,    //FW_CGOUT_L[1]    
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x60,
0x08,     //FW_CGOUT_L[2]  STV0
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x61,
0x0E,     //FW_CGOUT_L[3]  CLK1
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x62,
0x0F,     //FW_CGOUT_L[4]  CLK3
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x63,
0x0C,     //FW_CGOUT_L[5]  CLK5
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x64,
0x0D,     //FW_CGOUT_L[6]  CLK7
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x65,
0x17,     //FW_CGOUT_L[7]  GCL
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x66,
0x01,     //FW_CGOUT_L[8]  VDS
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x67,
0x01,     //FW_CGOUT_L[9]  VDS
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x68,
0x02,     //FW_CGOUT_L[10] VGL  
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x69,
0x02,     //FW_CGOUT_L[11] VGL 
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x6a,
0x00,     //FW_CGOUT_L[12] VSD
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x6b,
0x00,     //FW_CGOUT_L[13] VSD 
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x6c,
0x02,     //FW_CGOUT_L[14]    
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x6d,
0x02,     //FW_CGOUT_L[15]   
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x6e,
0x16,     //FW_CGOUT_L[16] GCH      
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x6f,
0x16,     //FW_CGOUT_L[17] GCH  
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x70,
0x06,     //FW_CGOUT_L[18] STV1
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x71,
0x06,     //FW_CGOUT_L[19] STV1
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x72,
0x07,     //FW_CGOUT_L[20] STV3
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x73,
0x07,     //FW_CGOUT_L[21] STV3  
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x74,
0x02,     //FW_CGOUT_L[22]  
  
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x75,
0x02,     //BW_CGOUT_L[1]   
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x76,
0x08,     //BW_CGOUT_L[2]    
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x77,
0x0E,     //BW_CGOUT_L[3]    
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x78,
0x0F,     //BW_CGOUT_L[4]    
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x79,
0x0C,     //BW_CGOUT_L[5]     
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x7a,
0x0D,     //BW_CGOUT_L[6]     
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x7b,
0x17,     //BW_CGOUT_L[7]   
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x7c,
0x01,     //BW_CGOUT_L[8]    
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x7d,
0x01,     //BW_CGOUT_L[9]      
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x7e,
0x02,    //BW_CGOUT_L[10]
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x7f,
0x02,     //BW_CGOUT_L[11]    
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x80,
0x00,     //BW_CGOUT_L[12]   
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x81,
0x00,     //BW_CGOUT_L[13] 
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x82,
0x02,     //BW_CGOUT_L[14]      
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x83,
0x02,     //BW_CGOUT_L[15]   
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x84,
0x16,     //BW_CGOUT_L[16]      
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x85,
0x16,     //BW_CGOUT_L[17]
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x86,
0x06,     //BW_CGOUT_L[18]
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x87,
0x06,     //BW_CGOUT_L[19]
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x88,
0x07,     //BW_CGOUT_L[20]   
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x89,
0x07,     //BW_CGOUT_L[21]   
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x8A,
0x02,     //BW_CGOUT_L[22]   

//CMD_Page 4
 _W, DELAY(0),  PACKET_DCS, SIZE(4),
0xFF,
0x98,
0x81,
0x04,

 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x6E,
0x1A,           //VGH 12V
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x6F,
0x37,           // reg vcl + pumping ratio VGH=3x VGL=-3x
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x3A,
0xA4,           //POWER SAVING
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x8D,
0x1F,           //VGL -12V
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x87,
0xBA,           //ESD
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xB2,
0xD1,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x88,
0x0B,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x38,
0x01,      
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x39,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xB5,
0x02,           //gamma bias
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x31,
0x25,           //source bias
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x3B,
0x98,  			
			
//CMD_Page 1
 _W, DELAY(0),  PACKET_DCS, SIZE(4),
0xFF,
0x98,
0x81,
0x01,

 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x22,
0x0A,          //BGR,0x SS
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x31,
0x00,          //Column inversion
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x53,
0x53,          //VCOM1
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x55,
0x3D,          //VCOM2 
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x50,
0x9E,          //VREG1OUT 4.6V
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x51,
0x99,          //VREG2OUT -4.6V
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x60,
0x06,          //SDT
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x62,
0x20,


//============Gamma START=============

//Pos Register
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xA0,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xA1,
0x17,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xA2,
0x26,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xA3,
0x13,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xA4,
0x16,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xA5,
0x29,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xA6,
0x1E,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xA7,
0x1F,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xA8,
0x8B,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xA9,
0x1D,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xAA,
0x2A,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xAB,
0x7B,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xAC,
0x1A,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xAD,
0x19,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xAE,
0x4E,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xAF,
0x24,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xB0,
0x29,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xB1,
0x4F,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xB2,
0x5C,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xB3,
0x3E,



//Neg Register
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xC0,
0x00,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xC1,
0x17,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xC2,
0x26,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xC3,
0x13,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xC4,
0x16,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xC5,
0x29,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xC6,
0x1E,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xC7,
0x1F,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xC8,
0x8B,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xC9,
0x1D,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xCA,
0x2A,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xCB,
0x7B,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xCC,
0x1A,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xCD,
0x19,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xCE,
0x4E,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xCF,
0x24,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xD0,
0x29,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xD1,
0x4F,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xD2,
0x5C,
 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0xD3,
0x3E,

//============ Gamma END===========			
	
//CMD_Page 0			
 _W, DELAY(0),  PACKET_DCS, SIZE(4),
0xFF,
0x98,
0x81,
0x00,

 _W, DELAY(0),  PACKET_DCS, SIZE(2),
0x35,
0x00,

 _W, DELAY(120),  PACKET_DCS, SIZE(1),
0x11,

 _W, DELAY(10),  PACKET_DCS, SIZE(1),
0x29,

///////////////////Initial  CODE/////////////////////

};

REGISTER_MIPI_DEVICE_BEGIN(mipi_dev_t) = {
    .info = {
        .xres 			= 1280,//LCD_DEV_WIDTH,
        .yres 			= 800,//LCD_DEV_HIGHT,
        .target_xres 	 = 1280,//LCD_DEV_WIDTH,
        .target_yres 	 = 800,//LCD_DEV_HIGHT,
        .buf_addr 		= LCD_DEV_BUF,
        .buf_num 		= LCD_DEV_BNUM,
        .sample         = LCD_DEV_SAMPLE,
        .test_mode 		= 0,
        .test_mode_color = 0x0000ff,
        .canvas_color = 0x000000,
        .format 		= FORMAT_RGB888,
        .len 			= LEN_256,

        .rotate_en 		= true,			// 旋转使能
        .hori_mirror_en = true,			// 水平镜像使能
        .vert_mirror_en = false,		// 垂直镜像使能

        .adjust = {
            .y_gain = 0x100,
            .u_gain = 0x100,
            .v_gain = 0x100,
            .r_gain = 0x80,
            .g_gain = 0x80,
            .b_gain = 0x80,
            .r_coe0 = 0x80,
            .g_coe1 = 0x80,
            .b_coe2 = 0x80,
            .r_gma  = 100,
            .g_gma  = 100,
            .b_gma  = 100,
        },
    },
    .lane_mapping = {
        .x0_lane = MIPI_LANE_EN| MIPI_LANE_EX |MIPI_LANE_D0,
        .x1_lane = MIPI_LANE_EN| MIPI_LANE_EX |MIPI_LANE_D1,
        .x2_lane = MIPI_LANE_EN| MIPI_LANE_EX |MIPI_LANE_CLK,
        .x3_lane = MIPI_LANE_EN| MIPI_LANE_EX |MIPI_LANE_D2,
        .x4_lane = MIPI_LANE_EN| MIPI_LANE_EX |MIPI_LANE_D3,

    },
    .video_timing = {
        .video_mode = VIDEO_STREAM_VIDEO,//视频模式 类似于dump panel
        .sync_mode  = SYNC_PULSE_MODE,
        .color_mode = COLOR_FORMAT_RGB888,
        .pixel_type = PIXEL_RGB888,
        .virtual_ch   = 0,
        .hs_eotp_en = 1,

        .dsi_vdo_vsa_v  = vsa_line,
        .dsi_vdo_vbp_v  = vbp_line,
        .dsi_vdo_vact_v = vda_line,
        .dsi_vdo_vfp_v  = vfp_line,

        .dsi_vdo_hsa_v   = ((bpp_num * hsa_pixel) / 8) - 10,
        .dsi_vdo_hbp_v   = ((bpp_num * hbp_pixel) / 8) - 10,
        .dsi_vdo_hact_v  = ((bpp_num * hda_pixel) / 8),
        .dsi_vdo_hfp_v   = ((bpp_num * hfp_pixel) / 8) - 6,

        .dsi_vdo_bllp0_v = ((bpp_num * (hbp_pixel + hda_pixel + hfp_pixel) / 8) - 10),
        .dsi_vdo_bllp1_v = ((bpp_num * hda_pixel) / 8),
    },
    .timing = {
        .tval_lpx   = ((80     * freq / 1000) / 2 - 1),
        .tval_wkup  = ((100000 * freq / 1000) / 8 - 1),
        .tval_c_pre = ((40     * freq / 1000) / 2 - 1),
        .tval_c_sot = ((300    * freq / 1000) / 2 - 1),
        .tval_c_eot = ((100    * freq / 1000) / 2 - 1),
        .tval_c_brk = ((150    * freq / 1000) / 2 - 1),
        .tval_d_pre = ((60     * freq / 1000) / 2 - 1),
        .tval_d_sot = ((160    * freq / 1000) / 2 - 1),
        .tval_d_eot = ((100    * freq / 1000) / 2 - 1),
        .tval_d_brk = ((150    * freq / 1000) / 2 - 1),
        .tval_c_rdy = 400/* 64 */,
    },
    .pll4 = {
        .pll_freq =394,// freq,
        .source = PLL4_SRC_x12M,
    },

    .cmd_list = init_cmd_list,
    .cmd_list_item = sizeof(init_cmd_list),
    .debug_mode = 0,
},
REGISTER_MIPI_DEVICE_END()

static int dsi_vdo_mipi_init(void *_data)
{
    printf("mipi lcd 1280x320 wt10988 init...\n");
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;
    u8 lcd_reset = data->lcd_io.lcd_reset;
    printf("lcd_reset : %d\n", lcd_reset);
    //reset pin
    gpio_direction_output(lcd_reset, 1);
    delay_2ms(100);
    gpio_direction_output(lcd_reset, 0);
    delay_2ms(100);
    gpio_direction_output(lcd_reset, 1);
    delay_2ms(85);

    return 0;
}

extern sys_pwm_ctrl();
static void mipi_backlight_ctrl(void *_data, u8 on)
{
    static u8 frist_lcd_backlight=1;
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;
    #ifdef CONFIG_PWM_BACKLIGHT_ENABLE
    if (on) {
        if(frist_lcd_backlight){
            frist_lcd_backlight=0;
            delay_2ms(50);
            pwm_ch0_backlight_init(data->lcd_io.backlight);
        }
        pwm_ch0_backlight_set_duty(db_select("bkl"));
    } else {
        pwm_ch0_backlight_set_duty(0);
    }
    #else
    if (on) {
        if(frist_lcd_backlight){
            frist_lcd_backlight=0;
            delay_2ms(50);
        }
        gpio_direction_output(data->lcd_io.backlight, data->lcd_io.backlight_value);
    } else {
        gpio_direction_output(data->lcd_io.backlight, !data->lcd_io.backlight_value);
    }
    #endif
}

REGISTER_LCD_DEVICE_DRIVE(dev)  = {
    .logo = "ek79030_v2",
    .type = LCD_MIPI,
    .dev  = &mipi_dev_t,
    .init = dsi_vdo_mipi_init,
    .bl_ctrl = mipi_backlight_ctrl,
//    .bl_ctrl_flags = BL_CTRL_BACKLIGHT,
};

#endif
