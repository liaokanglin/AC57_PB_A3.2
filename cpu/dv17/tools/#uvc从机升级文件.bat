@echo off
set update_Folder=upgrade\
set AC57_bfu_name=%update_Folder%AC57s.bfu

echo %AC57_bfu_name%

if exist %update_Folder% (         
       del upgrade\*.bfu
    ) else (  
             
        md %update_Folder%  
    )  
packet_bfu.exe -packet dv17loader.enc jl_isd.bin -o %AC57_bfu_name%

echo AC57x >>%AC57_bfu_name%

echo.
echo 57从机升级文件(AC57s.bfu)在upgrade目录下，upgrade目录下的AC57s.bfu文件可用作从机升级
echo.

pause