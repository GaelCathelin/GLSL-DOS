@echo off
del %1.o
cls
rem set params=-std=c++17 -Wall -Wextra -Wno-unused-function -Wa,--32 -masm=intel -march=i586 -m16 -Oz -fno-align-functions -fno-align-labels -fno-align-loops -fno-align-jumps -ffast-math -fsingle-precision-constant -frounding-math -nostdlib -fno-ident -fPIC -flto
set params=-std=c++17 -Wall -Wextra -Wno-unused-function -Wa,--32 -masm=intel -march=i586 -m16 -Ofast -fno-align-functions -fno-align-labels -fno-align-loops -fno-align-jumps -fsingle-precision-constant -frounding-math -nostdlib -fno-ident -fPIC -flto
gcc glsldos.cpp -o %1.S -S %params% -include glm.h -include %1.glsl
gcc %1.S        -o %1.o -c %params%
gcc %1.o        -o %1.exe  %params% -Wl,--script=com.ld -Wl,--file-alignment 1 -Wl,--section-alignment 1 -Wl,-m i386pe -Wl,-e _startup
objcopy -O binary %1.exe %1.com -j .text -j .rdata
objdump -d %1.exe -Mintel,i8086 -P sections --visualize-jumps
rem objdump -d %1.exe -Mintel,i8086 -P sections --visualize-jumps=extended-color --disassembler-color=extended