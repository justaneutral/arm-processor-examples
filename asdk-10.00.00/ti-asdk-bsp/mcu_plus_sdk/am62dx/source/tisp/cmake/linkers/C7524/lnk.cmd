-heap  0xF0000   // 756 kB
-stack 0x4000    //  16 kB
--cinit_compression=off
--args 0x1000
--diag_suppress=10068 // "no matching section"

MEMORY
{

  L2SRAM_CINIT (RWX)  : org = 0x7E000000, len = 0x000100 //for 256byte init     c7x_0 = 7E000000, c7x_1 = 7E200000
  L2SRAM (RWX)        : org = 0x7E000100, len = 0x1fff00 //for 2MBytes  EL2
  L2SRAMAUX   (RWX): org = 0x7F000000, len = 0x040000   // for 256 KBytes J7AEN c7x_0 = 7F000000, c7x_1 = 7F800000

  EXTMEM_STATIC   (RWX): org = 0x80000000, len = 0x4000000
  EXTMEM          (RWX): org = 0x84000000, len = 0x200000

}

SECTIONS
{
  .sram_start START(_sram_start) > L2SRAM NOINIT

//  .kernel: {
//    *.obj (.text:optimized) { SIZE(_kernel_size) }
//  } > EXTMEM

//  .kernel_data SIZE(_data_size)
  .l1dmemory        > L2SRAM
  .l2dmemory        > L2SRAM
  .text:            > L2SRAM
  .text:touch:      > L2SRAM
  .text:_c_int00:   > L2SRAM_CINIT
  .neardata:        > L2SRAM
  .rodata:          > L2SRAM
  .bss:             > L2SRAM
  .init_array:      > L2SRAM
  .far:             > L2SRAM
  .fardata:         > L2SRAM
  .neardata         > L2SRAM
  .rodata           > L2SRAM
  .data:            > L2SRAM
  .switch:          > L2SRAM
  .args:            > L2SRAM align = 0x4, fill = 0 {_argsize = 0x200; }
  .sysmem:          > L2SRAM
  .cinit:           > EXTMEM
  .const:           > L2SRAM START(const_start) SIZE(const_size)
  .pinit:           > L2SRAM
  .cio:             > L2SRAM
  .stack:           > L2SRAM
  .ddrData          > EXTMEM_STATIC
  .staticData       > EXTMEM_STATIC
  .l2sramaux        > L2SRAMAUX
  xdc.meta:        > L2SRAM, type = COPY
}

