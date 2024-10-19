@echo off

@echo ********************************************************************************
@echo 			DOWNLOAD SDK DV17 UVC			
@echo ********************************************************************************
@echo %date%

cd %~dp0

::uvc_isd_download.exe -tonorflash -dev dv20 -boot 0x4000 -div8 -wait 300 -uboot uboot.boot -app app.bin cfg_tool.bin -tone dir_isp_res -uvc DV20,USB 2 -reboot 100 
uvc_isd_download.exe -tonorflash -dev dv20 -boot 0x4000 -div8 -wait 300 -uvc UVC,USB 2

cd ..\

::pause

