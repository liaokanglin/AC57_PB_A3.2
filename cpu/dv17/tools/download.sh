

##!/bin/sh

${OBJDUMP} -D -address-mask=0x1ffffff -print-dbg -mcpu=r3 $1.elf > $1.lst
${OBJDUMP} -section-headers -address-mask=0x1ffffff $1.elf
${OBJSIZEDUMP} -lite -skip-zero -enable-dbg-info $1.elf | sort -k 1 > symbol_tbl.txt


${OBJCOPY} -O binary -j .rom_code $1.elf sdram_text.bin
${OBJCOPY} -O binary -j .data $1.elf data.bin
${OBJCOPY} -O binary -j .ram0_data $1.elf ram0_data.bin

cat sdram_text.bin data.bin ram0_data.bin > sdram.bin



export PROJ_BUILD=download.bat


echo "set OBJDUMP=C:\JL\pi32\bin\llvm-objdump.exe" >> ${PROJ_BUILD}
echo "set OBJCOPY=C:\JL\pi32\bin\llvm-objcopy.exe" >> ${PROJ_BUILD}
echo "set INELF=sdk.elf" >> ${PROJ_BUILD}
echo "cd %~dp0" >> ${PROJ_BUILD}
echo "%OBJCOPY% -O binary -j .rom_code  %INELF% sdram_text.bin" >> ${PROJ_BUILD}
echo "%OBJCOPY% -O binary -j .data      %INELF% data.bin" >> ${PROJ_BUILD}
echo "%OBJCOPY% -O binary -j .ram0_data %INELF% ram0_data.bin" >> ${PROJ_BUILD}
echo "copy sdram_text.bin/b + data.bin/b + ram0_data.bin/b sdram.bin" >> ${PROJ_BUILD}
echo "apu_make.exe -addr 0x4000000 -infile sdram.bin  -ofile sdram.apu -compress" >> ${PROJ_BUILD}
echo "BankLink.exe 0x3f02400 fast_boot.bin 0x3f02800 uboot.bin uboot.boot" >> ${PROJ_BUILD}




echo "isd_download.exe -cfg isd_config.cfg -input uboot.boot ver.bin sdram.apu -resource res audlogo 32 -disk norflash -div 2 -dev dv17 -boot 0x3f02000 -aline 4096 -reboot -format 0 1" >> ${PROJ_BUILD}





files="isd_config.cfg download.bat"



host-client -project ${NICKNAME}$2 -f ${files} $1.elf


cp ./$1.lst ../../../$1.lst



rm sdram_text.bin data.bin ram0_data.bin download.bat
