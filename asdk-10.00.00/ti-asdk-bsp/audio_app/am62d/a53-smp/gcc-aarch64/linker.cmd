/*
 *  Copyright (C) 2023 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

INCLUDE ../../../common/include/ti_bsp_defs_gcc.h

ENTRY(_c_int00)

	__TI_STACK_SIZE = 65536;
	__TI_HEAP_SIZE = 131072;

MEMORY {

	DDR : ORIGIN =  0x80000000, LENGTH = 0x2000000

	/* shared memory segments */
	/* On A53,
	 * - make sure there is a MMU entry which maps below regions as non-cache
	 */
    USER_SHM_MEM            : ORIGIN = 0x82000000, LENGTH = 0x80
    
    /* Used for shared console log */
    SHM_SHARED_LOG			: ORIGIN = SHM_SHARED_LOG_BASE, LENGTH = SHM_SHARED_LOG_SIZE
    
    /* Tuning Packet for primary core */
    SHM_TUNING_PKT			: ORIGIN = SHM_TUNING_PKT_BASE,  LENGTH = SHM_TUNING_PKT_SIZE
    
    /* Shared memory for Multi-Instance AWE between Primary & Secondary Cores */
    SHARED_HEAP_MEM         : ORIGIN = SHM_C7A53_SHARED_HEAP_BASE, LENGTH = SHM_C7A53_SHARED_HEAP_SIZE   /* 254 kB */

    SHM_AVB_CTRL_CMD        : ORIGIN =  SHM_AVB_CNTL_RX_BASE, LENGTH = SHM_AVB_CNTL_RX_SIZE
    SHM_AVB_CTRL_RSP        : ORIGIN =  SHM_AVB_CNTL_TX_BASE, LENGTH = SHM_AVB_CNTL_TX_SIZE
    SHM_AVB_PCM_DATA_RX     : ORIGIN =  SHM_AVB_DATA_RX_BASE, LENGTH = SHM_AVB_DATA_RX_SIZE
    SHM_AVB_PCM_DATA_TX     : ORIGIN =  SHM_AVB_DATA_TX_BASE, LENGTH = SHM_AVB_DATA_TX_SIZE
}

SECTIONS {

	.vecs : {} > DDR
		.text : {} > DDR
		.rodata : {} > DDR

		.data : ALIGN (8) {
			__data_load__ = LOADADDR (.data);
			__data_start__ = .;
			*(.data)
				*(.data*)
				. = ALIGN (8);
			__data_end__ = .;
		} > DDR

    /* General purpose user shared memory, used in some examples */
    .bss.user_shared_mem (NOLOAD) : { KEEP(*(.bss.user_shared_mem)) } > USER_SHM_MEM
    
    .bss.log_shared_mem (NOLOAD) : {KEEP(*(.bss.log_shared_mem))} > SHM_SHARED_LOG
    .awe_tuning_pkt (NOLOAD) : {KEEP(*(.awe_tuning_pkt))} > SHM_TUNING_PKT
    
    .bss : {
        __bss_start__ = .;
        *(.bss)
        *(.bss.*)
        . = ALIGN (8);
        *(COMMON)
        __bss_end__ = .;
        . = ALIGN (8);
    } > DDR

    .heap (NOLOAD) : {
        __heap_start__ = .;
        KEEP(*(.heap))
        . = . + __TI_HEAP_SIZE;
        __heap_end__ = .;
    } > DDR

    .stack (NOLOAD) : ALIGN(16) {
        __TI_STACK_BASE = .;
        KEEP(*(.stack))
        . = . + __TI_STACK_SIZE;
        __TI_STACK_BASE1 = .;
        KEEP(*(.stack))
        . = . + __TI_STACK_SIZE;
        __TI_STACK_BASE2 = .;
        KEEP(*(.stack))
        . = . + __TI_STACK_SIZE;
        __TI_STACK_BASE3 = .;
        KEEP(*(.stack))
        . = . + __TI_STACK_SIZE;
    } > DDR

    /* AWE Core - Data placement Heap */
    .awe_heap_fast (NOLOAD) : ALIGN (256) {} > DDR
    .awe_heap_fastb (NOLOAD) : ALIGN (256) {} > DDR
    .awe_heap_slow (NOLOAD) : ALIGN (256) {} > DDR
    
    /* AWE core - Code placement */
    .awe_prio0_fastcode : ALIGN (128) {} > DDR
    .awe_fw_fastcode : ALIGN (128) {} > DDR
    .awe_mod_fastcode : ALIGN (128) {} > DDR

    .awe_mod_slowcode : ALIGN (128) {} > DDR
    .awe_fw_slowcode : ALIGN (128) {} > DDR
    .awe_mod_slowdmdata : ALIGN (128) {} > DDR

    awe_mcasp_buffer (NOLOAD) : ALIGN (0x1000) {} > DDR

    /* Shared Heap for Multi-Instance AWE */
    .awe_shared_heap (NOLOAD) : ALIGN(128) {} > SHARED_HEAP_MEM

    /* AVB shared memory buffer */
    .asdk_avb_rx_buffer (NOLOAD) : ALIGN(128) {} > SHM_AVB_PCM_DATA_RX
    .asdk_avb_tx_buffer (NOLOAD) : ALIGN(128) {} > SHM_AVB_PCM_DATA_TX
}
