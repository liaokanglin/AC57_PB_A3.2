// *INDENT-OFF*

EXTERN(
#include "sdk_used_list.c"
);

#include "app_config.h"
MEMORY
{
	rom(rx)    	      : ORIGIN =  0x0000000, LENGTH = 0x400000
	sdram_mem(rwx)    : ORIGIN =  0x4000000, LENGTH = SDRAM_SIZE
	ram0(rwx)         : ORIGIN =  0x3f00000, LENGTH = 0x3f17000-0x3f00000
}

SECTIONS
{
/********************************************/

#ifdef CONFIG_SFC_ENABLE
	. = ORIGIN(rom);
#else
    . = ORIGIN(sdram_mem);
#endif

    .rom_code ALIGN(4):
    {
        _text_rodata_begin = .;
        PROVIDE(text_rodata_begin = .);

        *startup.S.o(.text)

        *sfc_boot.o(.text*)
        *sfc_boot.o(.rodata*)
        *boot_main.o(.text*)
        *boot_main.o(.rodata*)

        *(.text*)

        . = ALIGN(32);
        _ctors_begin = .;
        PROVIDE(ctors_begin = .);
        *(.ctors*)
        _ctors_end = .;
        PROVIDE(ctors_end = .);
        . = ALIGN(32);

        *(.rodata*)

		*(.jlsp_code)
        . = ALIGN(4);

        _text_rodata_end = .;
        PROVIDE(text_rodata_end = .);

    }
#ifdef CONFIG_SFC_ENABLE
	>rom
    . = ORIGIN(sdram_mem);
#else
	>sdram_mem
#endif
    PROVIDE(ctors_count = _ctors_end - _ctors_begin);

	 .data ALIGN(4):
	  {
		*(.stack)
        . = ALIGN(32);
        *(.data)

		. = ALIGN(4);
		vfs_ops_begin = .;
			*(.vfs_operations)
		vfs_ops_end = .;

		_lcd_device_begin = .;
		PROVIDE(lcd_device_begin = .);
			*(.lcd_device)
		_lcd_device_end = .;
		PROVIDE(lcd_device_end = .);

		_lcd_device_drive_begin = .;
		PROVIDE(lcd_device_drive_begin = .);
			*(.lcd_device_drive)
		_lcd_device_drive_end = .;
		PROVIDE(lcd_device_drive_end = .);

        _gsensor_dev_begin = .;
		PROVIDE(gsensor_dev_begin = .);
			*(.gsensor_dev)
		_gsensor_dev_end = .;
		PROVIDE(gsensor_dev_end = .);

        _touch_panel_dev_begin = .;
        PROVIDE(touch_panel_dev_begin = .);
        *(.touch_panel_dev)
            _touch_panel_dev_end = .;
        PROVIDE(touch_panel_dev_end = .);

		_isp_scr_begin = .;
		PROVIDE(isp_scr_begin = .);
			*(.isp_scr_work)
		_isp_scr_end = .;
		PROVIDE(isp_scr_end = .);

		_power_dev_begin = .;
		PROVIDE(power_dev_begin = .);
			*(.power_dev)
		_power_dev_end = .;
		PROVIDE(power_dev_end = .);

		_eth_phy_device_begin = .;
		PROVIDE(eth_phy_device_begin = .);
			*(.eth_phy_device)
		_eth_phy_device_end = .;
		PROVIDE(eth_phy_device_end = .);

		#include "ui/ui/ui.ld"
        #include "system/system.ld"


        . = ALIGN(32);
		#include "system/cpu/AC54xx/device.ld"
		. = ALIGN(32);
		#include "system/device/device.ld"
		#include "system/device/device_data.ld"
		. = ALIGN(32);
		#include "system/system_data.ld"

		#include "server/server.ld"

		#include "net/net_data.ld"


		. = (( . + 31) / 32 * 32);
		_layer_buf_begin = .;
		PROVIDE(layer_buf_begin = .);
		*(.layer_buf_t)
		_layer_buf_end = .;
		PROVIDE(layer_buf_end = .);

		. = (( . + 31) / 32 * 32);
		_font_info_begin = .;
		PROVIDE(font_info_begin = .);
		*(.font_info_t)
		_font_info_end = .;
		PROVIDE(font_info_end = .);

		. = (( . + 31) / 32 * 32);
		_screen_tool_info_begin = .;
		PROVIDE(screen_tool_info_begin = .);
		*(.screen_tool_info)
		_screen_tool_info_end = .;
		PROVIDE(screen_tool_info_end = .);

		_pwm_device_begin = .;
		PROVIDE(pwm_device_begin = .);
		 *(.pwm_dev)
		_pwm_device_end = .;
		PROVIDE(pwm_device_end = .);

		_wireless_driver_begin = .;
		PROVIDE(wireless_driver_begin = .);
		*(.wireless_driver)
			_wireless_driver_end = .;
		PROVIDE(wireless_driver_end = .);
		*(.m4a_dec_lib.text)

        . = ALIGN(4);
		*(.jlsp_const)
		. = ALIGN(4);
		*(.jlsp_data)
		. = ALIGN(4);
		*(.dns_common_data)
		. = ALIGN(4);
		*(.dns_16k_data)
		. = ALIGN(4);
		*(.dns_8k_data)
		. = ALIGN(4);
		*(.dns_param_data_single)

	  } > sdram_mem

    .bss ALIGN(32) (NOLOAD):
    {
		_system_data_begin = .;
        *(.bss)
        *(COMMON)
		*(.usb_slave_var)
        *(.mem_heap)
        *(.memp_memory_x)
		. = (( . + 31) / 32 * 32);
		_lcd_dev_buf_addr = .;
		PROVIDE(lcd_dev_buf_addr = .);
		*(.lcd_dev_buf)

		. = ALIGN(4);

		_system_data_end = .;

    } > sdram_mem
    _HEAP_BEGIN = ( . +31 )/ 32 *32 ;
    PROVIDE(HEAP_BEGIN = (. + 31) / 32 *32);
   _HEAP_END = 0x4000000 + SDRAM_SIZE - 32;
   PROVIDE(HEAP_END = 0x4000000 + SDRAM_SIZE - 32);
   _MALLOC_SIZE = _HEAP_END - _HEAP_BEGIN;
   PROVIDE(MALLOC_SIZE = _HEAP_END - _HEAP_BEGIN);

	/* . = 0x3f08000 ; */
	/* .sram ALIGN(4): */
	/* { */
        /* *(.boot) */
	/* } */


/********************************************/
    . =ORIGIN(ram0);
    .ram0_data ALIGN(4):
    {
        . = ALIGN(32);
        _VM_CODE_START = . ;
        *(.vm)
        _VM_CODE_END = . ;
        *(.flushinv_icache)
        . = ALIGN(32);
        *(.ram_code)
        . = ALIGN(32);
    } > ram0

    .ram0_bss ALIGN(4):
    {
		/* *(.stack) */
        . = ALIGN(32);
        /*
         * sd卡全局变量要放sram，否则会读写出错
         */
        *(.sd_var)
		*(.sram)
        *(.usb0_fifo)
		*(.plnk_buf)

		. = ALIGN(4);
    } > ram0

/********************************************/
    /* . =ORIGIN(ram2); */
    /* .ram2_data ALIGN(4): */
    /* { */
    /* } > ram2 */
    /* .ram2_bss ALIGN(4): */
    /* { */
    /* } > ram2 */
/********************************************/
    /* . =ORIGIN(xrom); */
    /* .xrom  0x2000000 : */
    /* { */
    /* } >xrom */
/********************************************/



    text_begin = ADDR(.rom_code ) ;
    text_size = SIZEOF(.rom_code ) ;
    bss_begin = ADDR(.bss ) ;
    bss_size  = SIZEOF(.bss);
    bss_size1 = _system_data_end - _system_data_begin;

    bss_begin1 = _system_data_begin;

    data_begin = LOADADDR(.data);
    data_addr  = ADDR(.data);
    data_size =  SIZEOF(.data) ;

	data_vma = ADDR(.data);
	data_lma = text_begin + SIZEOF(.rom_code);

/********************************************/
    _ram0_bss_vma = ADDR(.ram0_bss);
    _ram0_bss_size  =  SIZEOF(.ram0_bss);
    _ram0_data_vma  = ADDR(.ram0_data);
    _ram0_data_lma =  text_begin + SIZEOF(.rom_code) + SIZEOF(.data);
    _ram0_data_size =  SIZEOF(.ram0_data);



    /* _ram2_bss_vma = ADDR(.ram2_bss); */
    /* _ram2_bss_size  =  SIZEOF(.ram2_bss); */
    /* _ram2_data_vma  = ADDR(.ram2_data); */
    /* _ram2_data_lma =  text_begin + SIZEOF(.rom_code)+SIZEOF(.ram0_data); */
    /* _ram2_data_size =  SIZEOF(.ram2_data); */

/********************************************/
}
EXTERN(
lib_eth_version
lib_sdx_version
lib_usb_version
lib_dev_version
lib_vm_version
lib_dlmalloc_version
lib_mm_version
lib_rtos_version
lib_cpu_version
lib_isp_version
lib_log_version
lib_fs_version
lib_sys_version
lib_common_version
lib_rtl8189f_version
lib_hostap_wap_version
lib_hi3881_version
lib_rtl8192f_version
lib_wifi_dev_version
lib_s9082_version
lib_rtl8821cs_version
lib_marvell8801_version
lib_rtl8188e_version
lib_hostap_wap_version
lib_marvell8782_version
lib_rtl8822bs_version
lib_rtl8189e_version
lib_rtl8192e_version
lib_sdio_card_version
lib_mbedtls_version
lib_http_server_version
lib_hsm_version
lib_json_version
lib_net_server_version
lib_lwip_version
lib_ftpd_version
lib_iperf_version
lib_rtsp_version
lib_ftpcli_version
lib_httpcli_version
lib_curl_version
lib_font_version
lib_res_version
lib_ui_version
lib_ui_server_version
lib_video_rec_server_version
lib_sys_upgrade_version
lib_video_engine_server_version
lib_audio_server_version
lib_server_core_version
lib_usb_server_version
lib_video_dec_server_version
lib_video_framework_version
lib_jpeg_version
lib_h264_version


)

