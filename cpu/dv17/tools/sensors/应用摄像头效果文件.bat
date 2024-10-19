


::！！！！！更改摄像头效果文件请改此处, 注意路径信息！！！！

"..\..\tools\ISP Tool\J4L_to_BIN文件转换器\ispcfg2bin.exe"  2313\JL_JT_NIGHT.J4L  ..\res\isp_cfg_0.bin
"..\..\tools\ISP Tool\J4L_to_BIN文件转换器\ispcfg2bin.exe"  2313\JL_JT_INDOOR.J4L  	..\res\isp_cfg_1.bin
"..\..\tools\ISP Tool\J4L_to_BIN文件转换器\ispcfg2bin.exe"  2313\JL_JT_OUTCLUDY.J4L  ..\res\isp_cfg_2.bin
"..\..\tools\ISP Tool\J4L_to_BIN文件转换器\ispcfg2bin.exe"  2313\JL_JT_OUTDOOR.J4L  ..\res\isp_cfg_3.bin

set /p  run="是否启动下载？(y / n)
echo %run%
if %run%==y  (
    cd ..
    call 下载代码-toflash.bat
)





