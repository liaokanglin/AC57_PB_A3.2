#include "generic/typedef.h"
#include "asm/dsi.h"
#include "asm/imb.h"
#include "asm/lcd_config.h"
#include "device/lcd_driver.h"
#include "gpio.h"
#include "os/os_compat.h"

#ifdef MIPI_LCD_1280x720_RM68200GA1
//------------------------------------------------------//
// lcd command initial
//------------------------------------------------------//
//
/**************************************************
IC Name: RM68200GA1
Panel Maker/Size: IVO5.0
Panel ID: C050SWAK-0
Purpose: CODE
Remark: V1.0
Version: 160902
Date: GAMMA2.0
**************************************************/

const static u8 init_cmd_list[] = {
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xFE, 0x01,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x24, 0xC0,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x25, 0x53,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x26, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x27, 0x0A,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x29, 0x0A,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x2B, 0xE5,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x16, 0x52,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x2F, 0x54,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x34, 0x57,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x1B, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x12, 0x0A,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x1A, 0x06,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x46, 0x3A,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x52, 0x78,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x53, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x54, 0x78,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x55, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xFE, 0x03,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x00, 0x05,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x01, 0x16,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x02, 0x09,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x03, 0x0D,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x04, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x05, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x06, 0x50,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x07, 0x05,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x08, 0x16,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x09, 0x0B,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x0A, 0x0F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x0B, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x0C, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x0D, 0x50,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x0E, 0x04,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x0F, 0x05,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x10, 0x06,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x11, 0x07,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x12, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x13, 0xA9,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x14, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x15, 0xC4,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x16, 0x08,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x17, 0x08,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x18, 0x09,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x19, 0x0A,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x1A, 0x0B,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x1B, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x1C, 0xA9,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x1D, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x1E, 0x84,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x1F, 0x08,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x20, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x21, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x22, 0x04,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x23, 0x1B,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x24, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x25, 0x28,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x26, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x27, 0x1F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x28, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x29, 0x28,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x2A, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x2B, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x2D, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x2F, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x30, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x31, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x32, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x33, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x34, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x35, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x36, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x37, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x38, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x39, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x3A, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x3B, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x3D, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x3F, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x40, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x3F, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x41, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x42, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x43, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x44, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x45, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x46, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x47, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x48, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x49, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x4A, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x4B, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x4C, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x4D, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x4E, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x4F, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x50, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x51, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x52, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x53, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x54, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x55, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x56, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x58, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x59, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x5A, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x5B, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x5C, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x5D, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x5E, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x5F, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x60, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x61, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x62, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x63, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x64, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x65, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x66, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x67, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x68, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x69, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x6A, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x6B, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x6C, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x6D, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x6E, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x6F, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x70, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x71, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x72, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x73, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x74, 0x04,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x75, 0x04,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x76, 0x04,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x77, 0x04,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x78, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x79, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x7A, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x7B, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x7C, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x7D, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x7E, 0x86,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x7F, 0x02,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x80, 0x0E,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x81, 0x0C,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x82, 0x0A,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x83, 0x08,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x84, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x85, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x86, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x87, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x88, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x89, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x8A, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x8B, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x8C, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x8D, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x8E, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x8F, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x90, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x91, 0x04,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x92, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x93, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x94, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x95, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x96, 0x05,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x97, 0x01,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x98, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x99, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x9A, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x9B, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x9C, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x9D, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x9E, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0x9F, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xA0, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xA2, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xA3, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xA4, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xA5, 0x09,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xA6, 0x0B,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xA7, 0x0D,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xA9, 0x0F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xAA, 0x03,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xAB, 0x07,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xAC, 0x01,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xAD, 0x05,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xAE, 0x0D,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xAF, 0x0F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xB0, 0x09,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xB1, 0x0B,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xB2, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xB3, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xB4, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xB5, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xB6, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xB7, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xB8, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xB9, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xBA, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xBB, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xBC, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xBD, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xBE, 0x07,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xBF, 0x03,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xC0, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xC1, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xC2, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xC3, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xC4, 0x02,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xC5, 0x06,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xC6, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xC7, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xC8, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xC9, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xCA, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xCB, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xCC, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xCD, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xCE, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xCF, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xD0, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xD1, 0x3F,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xD2, 0x0A,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xD3, 0x08,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xD4, 0x0E,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xD5, 0x0C,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xD6, 0x04,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xD7, 0x00,
    _W, DELAY(0), PACKET_DCS, SIZE(2), 0xDC, 0x02,
    _W, DELAY(200), PACKET_DCS, SIZE(2), 0xDE, 0x10,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0xFE, 0x04,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x60, 0x00,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x61, 0x0B,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x62, 0x12,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x63, 0x0E,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x64, 0x07,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x65, 0x15,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x66, 0x0F,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x67, 0x0B,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x68, 0x16,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x69, 0x0C,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x6A, 0x11,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x6B, 0x09,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x6C, 0x11,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x6D, 0x19,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x6E, 0x13,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x6F, 0x0A,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x70, 0x00,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x71, 0x0B,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x72, 0x12,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x73, 0x0E,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x74, 0x07,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x75, 0x15,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x76, 0x0F,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x77, 0x0B,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x78, 0x16,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x79, 0x0C,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x7A, 0x11,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x7B, 0x09,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x7C, 0x11,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x7D, 0x19,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x7E, 0x13,
    _W, DELAY(200), PACKET_DCS, SIZE(2), 0x7F, 0x0A,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0xFE, 0x0E,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x01, 0x75,
    _W, DELAY(200), PACKET_DCS, SIZE(2), 0x49, 0x56,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0xFE, 0x00,
    _W, DELAY(0),  PACKET_DCS, SIZE(2), 0x58, 0xA9,
    _W, DELAY(200), PACKET_DCS, SIZE(1), 0x11,
    _W, DELAY(200), PACKET_DCS, SIZE(1), 0x29,
};

#define freq 400

#define lane_num 4
/*
 *  bpp_num
 *  16: PIXEL_RGB565_COMMAND/PIXEL_RGB565_VIDEO
 *  18: PIXEL_RGB666/PIXEL_RGB666_LOOSELY
 *  24: PIXEL_RGB888
 */
#define bpp_num  24

#define vsa_line 2
#define vbp_line 17
#define vda_line 1280
#define vfp_line 10

#define hsa_pixel  10
#define hbp_pixel  56
#define hda_pixel  720
#define hfp_pixel  58

REGISTER_MIPI_DEVICE_BEGIN(mipi_dev_t) = {
    .info = {
        .xres           = LCD_DEV_WIDTH,
        .yres           = LCD_DEV_HIGHT,
        .target_xres 	= LCD_DEV_WIDTH,
        .target_yres 	= LCD_DEV_HIGHT,
        .buf_addr       = LCD_DEV_BUF,
        .buf_num        = LCD_DEV_BNUM,
        .sample         = LCD_DEV_SAMPLE,
        .test_mode      = false,
        .test_mode_color = 0xff0000,
        .canvas_color   = 0x0000ff,
        .format         = FORMAT_RGB888,
        .len            = LEN_256,
        .interlaced_mode = INTERLACED_NONE,

        .rotate_en      = true,             // 旋转使能
        .hori_mirror_en = false,             // 水平镜像使能
        .vert_mirror_en = true,            // 垂直镜像使能

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
        .x0_lane = MIPI_LANE_EN  | MIPI_LANE_D0,
        .x1_lane = MIPI_LANE_EN  | MIPI_LANE_D1,
        .x2_lane = MIPI_LANE_EN  | MIPI_LANE_CLK,
        .x3_lane = MIPI_LANE_EN  | MIPI_LANE_D2,
        .x4_lane = MIPI_LANE_EN  | MIPI_LANE_D3,
    },
    .video_timing = {
        .video_mode = VIDEO_STREAM_VIDEO,
        .sync_mode  = SYNC_PULSE_MODE,
        .color_mode = COLOR_FORMAT_RGB888,
        .pixel_type = PIXEL_RGB888,
        .virtual_ch = 0,
        .hs_eotp_en = true,

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
        /* 以下参数只需修改freq */
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
        .pll_freq = freq,
        .source = PLL4_SRC_x12M,
    },

    .cmd_list = init_cmd_list,
    .cmd_list_item = sizeof(init_cmd_list),
    .debug_mode = false,
},
REGISTER_MIPI_DEVICE_END()


static int lcd_1280x720_rm68200ga1(void *_data)
{
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;
    u8 lcd_reset = data->lcd_io.lcd_reset;
    u8 lcd_standby  = data->lcd_io.lcd_standby;

    printf("lcd_1280x720_rm68200ga1...\n");

    if ((u8) - 1 != lcd_standby) { /*4 lane*/
        printf("lcd_lane = %d\n", lcd_standby);
        gpio_direction_output(lcd_standby, 1);
    }

    /*
     * lcd reset
     */

    if ((u8) - 1 != lcd_reset) {
        gpio_direction_output(lcd_reset, 0);
        delay_2ms(5);
        gpio_direction_output(lcd_reset, 1);
        delay_2ms(5);
    }

#ifdef CONFIG_PWM_BACKLIGHT_ENABLE
    pwm_ch0_backlight_init(data->lcd_io.backlight);
#endif

    return 0;
}

static void mipi_backlight_ctrl(void *_data, u8 on)
{
    struct lcd_platform_data *data = (struct lcd_platform_data *)_data;

    if ((u8) - 1 == data->lcd_io.backlight) {
        return;
    }
    if (on) {
#ifdef CONFIG_PWM_BACKLIGHT_ENABLE
        pwm_ch0_backlight_on();
#else
        gpio_direction_output(data->lcd_io.backlight, data->lcd_io.backlight_value);
#endif
    } else {
#ifdef CONFIG_PWM_BACKLIGHT_ENABLE
        pwm_ch0_backlight_off();
#else
        gpio_direction_output(data->lcd_io.backlight, !data->lcd_io.backlight_value);
#endif
    }
}

REGISTER_LCD_DEVICE_DRIVE(dev)  = {
    .logo      = "rm68200",
    .type      = LCD_MIPI,
    .dev       = &mipi_dev_t,
    .init      = lcd_1280x720_rm68200ga1,
    .bl_ctrl   = mipi_backlight_ctrl,
};

#endif
