@echo off

set a=isd_config.cfg
:set a=isd_tools_ddr2_400mhz.cfg
:set a=isd_tools_ddr2_500mhz.cfg
:set a=isd_tools_ddr2-450MHz_20190107.cfg

echo [ 选择升级模式 ]
echo 3: 资源文件+代码
echo 5: 资源文件+代码+uboot
echo q: 取消
echo .

choice /c 35q /n /m "请输入选项："

if %errorlevel% EQU 1 (
    set /A bfumode = 3
) else if %errorlevel% EQU 2 (
    set /A bfumode = 5
) else if %errorlevel% EQU 3 (
    exit
)

isd_download.exe -cfg %a% -input uboot.boot ver.bin sdram.apu -resource res audlogo 32 -disk file -aline 4096 -bfumode %bfumode% -dev upgrade_bfu

del upgrade\JL_AC*.bfu upgrade\*.bin

file_package.exe -file sdram.bin -dir upgrade -o package
copy /B jl_isd.bfu+package upgrade\JL_AC5X.bfu
del jl_isd.bfu package
echo.
echo 升级文件在upgrade目录下，将upgrade目录下的所有文件copy到SD卡的根目录，插卡上电即可升级
echo.
pause