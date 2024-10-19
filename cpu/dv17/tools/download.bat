set OBJDUMP=C:\JL\pi32\bin\llvm-objdump.exe
set OBJCOPY=C:\JL\pi32\bin\llvm-objcopy.exe
set INELF=sdk.elf
set UI_RES=res
set AUDLOGO=audlogo

cd %~dp0
%OBJCOPY% -O binary -j .rom_code %INELF% sdram_text.bin
%OBJCOPY% -O binary -j .data %INELF% data.bin
%OBJCOPY% -O binary -j .ram0_data %INELF% ram0_data.bin

copy sdram_text.bin/b + data.bin/b + ram0_data.bin/b sdram.bin

apu_make.exe -addr 0x4000000 -infile sdram.bin -ofile sdram.apu -compress

BankLink.exe 0x3f02400 fast_boot.bin 0x3f02800 uboot.bin uboot.boot
isd_download.exe -cfg isd_config.cfg -input uboot.boot ver.bin sdram.apu -resource %UI_RES% %AUDLOGO% 32 -disk norflash -div 2 -dev dv17 -boot 0x3f02000 -aline 4096 -reboot -format 0 1
