# Introduction

This s a standalone program / bootloader used for booting directly from Micron SPI-NAND on MediaTek MT7621 platform.

# Prerequisites
- The bootstrap of MT7621 board is set to "Boot from SPI 3-byte address".
- A Micron SPI-NAND flash chip with "SPI NOR Read Configuration" enabled.

# Toolchain
Prebuilt toolchain for Windows 64-bit platform is provided (use it with MSYS2).  
You can also build your own toolchain, but remember to enable LTO (--enable-lto)

# Binary structure

## 1st stage loader
The 1st stage loader only occupies the first page of the SPI-NAND flash.  
It's used to setup a basic environment to load 2nd stage loader into locked L2 cache and transfer control to 2nd stage loader.  
The size of 1st stage loader must not be greater than the page size of the SPI-NAND flash (i.e. 2KB or 4KB).

## 2nd stage loader
The 2nd stage loader runs in locked L2 cache and will do all initializations that 1st stage loader can not do (CPU/DDR initialization & cache reset).  
The 2nd stage loader then loads 3rd stage loader into memory and transfer control.  
The size of 2nd stage loader must not be greater than 192KB (3/4 of 256KB L2 cache).

## 3rd stage loader
The 3rd stage loader will not be provided by this code.  
You can use you customized bootloader (Breed, U-Boot or whatever) to act as the 3rd stage loader.  
The 3rd stage loader must have a image header to describe its size/load address/entry point/compression.  
Accepted headers are: Breed private header and U-Boot legacy image header.
Accepted compression is LZMA.

## Output image
The 1st and 2nd stage loaders will be concatenate in to one binary file.

# Compilation
`make CROSS_COMPILE=<toolchain-prefix>`

Optional make parameters (passed to make commandline):

| Name | Arg type | Default value | Description |
| -------- | ----- | - |  ---- |
| BAUDRATE | int | 115200 | Baudrate of the UART console output |
| _4K_PAGE_SUPPORT | bool (1 for true) |  | Build for SPI-NAND with 4KB page size & 256KB block size |
| STAGE3_OFFSET | hex |  | The offset of 3rd-stage image in NAND |
| STAGE3_PAYLOAD | string |  | Automatically concatenate 3rd-stage image after final output image |

If STAGE3_PAYLOAD is specified, STAGE3_OFFSET will be ignored, and the offset of 3rd stage will be set to the size of final output image

## Final output image
build/target/output-loader.bin

# CPU/DDR configuration
Please edit src/mt7621/stage2/entry.S

```
EXPORT(_dramc_cpu_freq)
	.word	880
EXPORT(_dramc_ddr_freq)
	.word	1

EXPORT(_dramc_ddr2_act_sel)
	.word	2
EXPORT(_dramc_ddr3_act_sel)
	.word	2
```

## CPU frequency
As-is, 880 stands for 880MHz, step is 20MHz

## DDR frequency
1 - 1200MHz  
2 - 1066MHz (actual 1040MHz)  
3 - 800MHz  
4 - 400MHz

## DDR2 AC timing selection
0 - 512Mb (Generic 64MB)  
1 - 512Mb (Optimized for W9751G6KB_A02, DDR frequency is 1066MHz)  
2 - 1024Mb (Generic 128MB)  
3 - 1024Mb (Optimized for W971GG6KB25, DDR frequency is 800MHz)  
4 - 1024Mb (Optimized for W971GG6KB18, DDR frequency is 1066MHz)

## DDR3 AC timing selection
0 - 1024Mb (Generic 128MB)  
1 - 1024Mb (Optimized for MT7621DA, integrated DDR KGD)  
2 - 2048Mb (Generic 256MB)  
3 - 4096Mb (Generic 512MB)

# License
BSD-3-Clause
