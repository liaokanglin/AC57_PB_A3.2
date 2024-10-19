

EXTERN(



fat_vfs_ops
ramfs_vfs_ops
sdfile_vfs_ops
devfs_vfs_ops
fatfs_ops


ui_server_info



aac_enc_sub_ops
subdev_aac_enc
video_engine_server_info
usb_server_info
pcm_enc_ops
pcm_decoder_ops
adpcm_decoder_ops
m4a_decoder
);


MEMORY
{
 rom(rx) : ORIGIN = 0x0000000, LENGTH = 0x400000
 sdram_mem(rwx) : ORIGIN = 0x4000000, LENGTH = (64 * 1024 * 1024)
 ram0(rwx) : ORIGIN = 0x3f00000, LENGTH = 0x3f17000-0x3f00000
}

SECTIONS
{





    . = ORIGIN(sdram_mem);


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




 >sdram_mem

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



  _ui_style_begin = .;
  PROVIDE(ui_style_begin = .);
     *(.ui_style)
  _ui_style_end = .;
  PROVIDE(ui_style_end = .);

        _elm_event_handler_begin_UPGRADE = .;
  PROVIDE(elm_event_handler_begin_UPGRADE = .);
           *(.elm_event_handler_UPGRADE)
  _elm_event_handler_end_UPGRADE = .;
  PROVIDE(elm_event_handler_end_UPGRADE = .);

                _elm_event_handler_begin_JL02 = .;
  PROVIDE(elm_event_handler_begin_JL02 = .);
           *(.elm_event_handler_JL02)
  _elm_event_handler_end_JL02 = .;
  PROVIDE(elm_event_handler_end_JL02 = .);

  _elm_event_handler_begin_ANI = .;
  PROVIDE(elm_event_handler_begin_ANI = .);
           *(.elm_event_handler_ANI)
  _elm_event_handler_end_ANI = .;
  PROVIDE(elm_event_handler_end_ANI = .);

  _elm_event_handler_begin_LY = .;
  PROVIDE(elm_event_handler_begin_LY = .);
           *(.elm_event_handler_LY)
  _elm_event_handler_end_LY = .;
  PROVIDE(elm_event_handler_end_LY = .);

  _control_event_handler_begin = .;
  PROVIDE(control_event_handler_begin = .);
           *(.control_event_handler)
  _control_event_handler_end = .;
  PROVIDE(control_event_handler_end = .);

  _control_ops_begin = .;
  PROVIDE(control_ops_begin = .);
           *(.control_ops)
  _control_ops_end = .;
  PROVIDE(control_ops_end = .);

  _vg_event_handler_begin = .;
  PROVIDE(vg_event_handler_begin = .);
           *(.vg_event_handler)
        _vg_event_handler_end = .;
  PROVIDE(vg_event_handler_end = .);

        _on_show_map_begin = .;
  PROVIDE(on_show_map_begin = .);
           *(.on_show_map)
        _on_show_map_end = .;
  PROVIDE(on_show_map_end = .);

        _on_click_map_begin = .;
  PROVIDE(on_click_map_begin = .);
           *(.on_click_map)
        _on_click_map_end = .;
  PROVIDE(on_click_map_end = .);

        _on_touch_map_begin = .;
  PROVIDE(on_touch_map_begin = .);
           *(.on_touch_map)
        _on_touch_map_end = .;
  PROVIDE(on_touch_map_end = .);

        _on_change_map_begin = .;
  PROVIDE(on_change_map_begin = .);
           *(.on_change_map)
        _on_change_map_end = .;
  PROVIDE(on_change_map_end = .);

        _on_key_map_begin = .;
  PROVIDE(on_key_map_begin = .);
           *(.on_key_map)
        _on_key_map_end = .;
  PROVIDE(on_key_map_end = .);

  _img_loader_begin = .;
  PROVIDE(img_loader_begin = .);
     *(.img_loader)
  _img_loader_end = .;
  PROVIDE(img_loader_end = .);

  _battery_notify_begin = .;
  PROVIDE(battery_notify_begin = .);
           *(.battery_notify)
        _battery_notify_end = .;
  PROVIDE(battery_notify_end = .);

    _lib_version_begin = .;
    PROVIDE(lib_version_begin = .);
        *(.lib_version)
    _lib_version_end = .;
    PROVIDE(lib_version_end = .);

    _sys_version_begin = .;
    PROVIDE(sys_version_begin = .);
        *(.sys.version)
    _sys_version_end = .;
    PROVIDE(sys_version_end = .);

        . = ALIGN(4);

 _initcall_begin = .;
 PROVIDE(initcall_begin = .);
  *(.initcall)
 _initcall_end = .;
 PROVIDE(initcall_end = .);

 _early_initcall_begin = .;
 PROVIDE(early_initcall_begin = .);
  *(.early.initcall)
 _early_initcall_end = .;
 PROVIDE(early_initcall_end = .);

 _late_initcall_begin = .;
 PROVIDE(late_initcall_begin = .);
    *(.late.initcall)
 _late_initcall_end = .;
 PROVIDE(late_initcall_end = .);

 _platform_initcall_begin = .;
 PROVIDE(platform_initcall_begin = .);
    *(.platform.initcall)
 _platform_initcall_end = .;
 PROVIDE(platform_initcall_end = .);

 _module_initcall_begin = .;
 PROVIDE(module_initcall_begin = .);
    *(.module.initcall)
 _module_initcall_end = .;
 PROVIDE(module_initcall_end = .);

 _sys_event_handler_begin = .;
 PROVIDE(sys_event_handler_begin = .);
    *(.sys_event.4.handler)
    *(.sys_event.3.handler)
    *(.sys_event.2.handler)
    *(.sys_event.1.handler)
    *(.sys_event.0.handler)
 _sys_event_handler_end = .;
 PROVIDE(sys_event_handler_end = .);

 _syscfg_arg_begin = .;
 PROVIDE(syscfg_arg_begin = .);
    *(.syscfg.arg)
 _syscfg_arg_end = .;
 PROVIDE(syscfg_arg_end = .);

 _syscfg_handler_begin = .;
 PROVIDE(syscfg_handler_begin = .);
    *(.syscfg.handler)
 _syscfg_handler_end = .;
 PROVIDE(syscfg_handler_end = .);

 _server_info_begin = .;
 PROVIDE(server_info_begin = .);
    *(.server_info)
 _server_info_end = .;
 PROVIDE(server_info_end = .);

 _app_begin = .;
 PROVIDE(app_begin = .);
    *(.app)
 _app_end = .;
 PROVIDE(app_end = .);

 _bus_device_begin = .;
 PROVIDE(bus_device_begin = .);
    *(.bus_device)
 _bus_device_end = .;
 PROVIDE(bus_device_end = .);

    _sys_power_hal_ops_begin = .;
    PROVIDE(sys_power_hal_ops_begin = .);
        *(.sys_power_hal_ops)
    _sys_power_hal_ops_end = .;
    PROVIDE(sys_power_hal_ops_end = .);


        . = ALIGN(32);


  _camera_dev_begin = .;
  PROVIDE(camera_dev_begin = .);
  *(.camera_dev)
  _camera_dev_end = .;
  PROVIDE(camera_dev_end = .);

  _adc_scan_begin = .;
  PROVIDE(adc_scan_begin = .);
  *(.adc_scan)
  _adc_scan_end = .;
  PROVIDE(adc_scan_end = .);

  _spi_device_begin = .;
  PROVIDE(spi_device_begin = .);
   *(.spi_device)
  _spi_device_end = .;
  PROVIDE(spi_device_end = .);
 . = ALIGN(32);

   _device_node_begin = .;
   PROVIDE(device_node_begin = .);
        *(.device)
   _device_node_end = .;
   PROVIDE(device_node_end = .);

 _video_subdev_begin = .;
 PROVIDE(video_subdev_begin = .);
   *(.video_subdev.0)
   *(.video_subdev.1)
   *(.video_subdev.2)
   *(.video_subdev.3)
   *(.video_subdev.4)
   *(.video_subdev.5)
 _video_subdev_end = .;
 PROVIDE(video_subdev_end = .);

 _audio_subdev_begin = .;
 PROVIDE(audio_subdev_begin = .);
   *(.audio_subdev.0)
   *(.audio_subdev.1)
   *(.audio_subdev.2)
   *(.audio_subdev.3)
 _audio_subdev_end = .;
 PROVIDE(audio_subdev_end = .);

 _iic_device_begin = .;
 PROVIDE(iic_device_begin = .);
   *(.iic)
 _iic_device_end = .;
 PROVIDE(iic_device_end = .);


        _avin_spi_device_begin = .;
  PROVIDE(avin_spi_device_begin = .);
   *(.sw_spi)
  _avin_spi_device_end = .;
  PROVIDE(avin_spi_device_end = .);

 _video_dev_begin = .;
 PROVIDE(video_dev_begin = .);
   *(.video_device)
 _video_dev_end = .;
 PROVIDE(video_dev_end = .);


 _uart_device_begin = .;
 PROVIDE(uart_device_begin = .);
   *(.uart)
 _uart_device_end = .;
 PROVIDE(uart_device_end = .);
 . = ALIGN(32);


 _key_driver_begin = .;
 PROVIDE(key_driver_begin = .);
    *(.key_driver)
 _key_driver_end = .;
 PROVIDE(key_driver_end = .);

 _touch_driver_begin = .;
 PROVIDE(touch_driver_begin = .);
    *(.touch_driver)
 _touch_driver_end = .;
 PROVIDE(touch_driver_end = .);

 _static_hi_timer_begin = .;
 PROVIDE(static_hi_timer_begin = .);
    *(.hi_timer)
 _static_hi_timer_end = .;
 PROVIDE(static_hi_timer_end = .);

 _sys_cpu_timer_begin = .;
 PROVIDE(sys_cpu_timer_begin = .);
    *(.sys_cpu_timer)
 _sys_cpu_timer_end = .;
 PROVIDE(sys_cpu_timer_end = .);

    _sys_config_begin = .;
    PROVIDE(sys_config_begin = .);
        *(.sys_cfg)
    _sys_config_end = .;
    PROVIDE(sys_config_end = .);


    _audio_decoder_begin = .;
    PROVIDE(audio_decoder_begin = .);
        *(.audio_decoder)
    _audio_decoder_end = .;
    PROVIDE(audio_decoder_end = .);


_ethernet_device_begin = .;
PROVIDE(ethernet_device_begin = .);
    *(.ethernet_device)
*(.eth_phy_device)
    _ethernet_device_end = .;
    PROVIDE(ethernet_device_end = .);
    _ctp_mapping_tab_begin = .;
    PROVIDE(ctp_mapping_tab_begin = .);

    _ctp_mapping_tab_system_cmd_begin = .;
    PROVIDE(ctp_mapping_tab_system_cmd_begin = .);
*(.ctp_system_cmd)
    _ctp_mapping_tab_system_cmd_end = .;
    PROVIDE(ctp_mapping_tab_system_cmd_end = .);

    _ctp_mapping_tab_video_cmd_begin = .;
    PROVIDE(ctp_mapping_tab_video_cmd_begin = .);
*(.ctp_video_cmd)
    _ctp_mapping_tab_video_cmd_end = .;
    PROVIDE(ctp_mapping_tab_video_cmd_end = .);


    _ctp_mapping_tab_photo_cmd_begin = .;
    PROVIDE(ctp_mapping_tab_photo_cmd_begin = .);
*(.ctp_photo_cmd)
    _ctp_mapping_tab_photo_cmd_end = .;
    PROVIDE(ctp_mapping_tab_photo_cmd_end = .);


    _ctp_mapping_tab_end = .;
    PROVIDE(ctp_mapping_tab_end = .);
    _net_video_stream_sub_begin = .;
    PROVIDE(net_video_stream_sub_begin = .);
*(.net_video_stream)
    _net_video_stream_sub_end = .;
    PROVIDE(net_video_stream_sub_end = .);


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
   _HEAP_END = 0x4000000 + (64 * 1024 * 1024) - 32;
   PROVIDE(HEAP_END = 0x4000000 + (64 * 1024 * 1024) - 32);
   _MALLOC_SIZE = _HEAP_END - _HEAP_BEGIN;
   PROVIDE(MALLOC_SIZE = _HEAP_END - _HEAP_BEGIN);
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

        . = ALIGN(32);



        *(.sd_var)
  *(.sram)
        *(.usb0_fifo)
  *(.plnk_buf)

  . = ALIGN(4);
    } > ram0
    text_begin = ADDR(.rom_code ) ;
    text_size = SIZEOF(.rom_code ) ;
    bss_begin = ADDR(.bss ) ;
    bss_size = SIZEOF(.bss);
    bss_size1 = _system_data_end - _system_data_begin;

    bss_begin1 = _system_data_begin;

    data_begin = LOADADDR(.data);
    data_addr = ADDR(.data);
    data_size = SIZEOF(.data) ;

 data_vma = ADDR(.data);
 data_lma = text_begin + SIZEOF(.rom_code);


    _ram0_bss_vma = ADDR(.ram0_bss);
    _ram0_bss_size = SIZEOF(.ram0_bss);
    _ram0_data_vma = ADDR(.ram0_data);
    _ram0_data_lma = text_begin + SIZEOF(.rom_code) + SIZEOF(.data);
    _ram0_data_size = SIZEOF(.ram0_data);
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
