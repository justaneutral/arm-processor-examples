#include "../../../common/include/ti_bsp_defs.h"

--ram_model
-heap  0x20000
-stack 0x20000
--args 0x1000
--diag_suppress=10068 /* to suppress no matching section error */
--cinit_compression=off
-e _c_int00_secure

#define DDR0_ALLOCATED_START    (0x80000000)
#define C7X_ALLOCATED_START     (DDR0_ALLOCATED_START + 0x19800000)

MEMORY
{
    L2SRAM (RWX):         org = 0x7E000000,          len = 0x00100000
    L2SRAM_AUX (RWX):     org = 0x7F000000,          len = 0x00040000

    C7X_DDR_UNCACHED:     org = C7X_ALLOCATED_START, len = 0x1000000
    C7X_DDR_CACHED:       org = 0x9A800000,          len = 0x900000
    C7X_DDR_CACHED_AWE:   org = 0x9B100000,          len = 0x710000

    /* Used for shared console log */
    SHM_SHARED_LOG:       org = SHM_SHARED_LOG_BASE, len = SHM_SHARED_LOG_SIZE

    /* Tuning Packet for primary core */
    SHM_TUNING_PKT:       org = SHM_TUNING_PKT_BASE,  len = SHM_TUNING_PKT_SIZE

    /* Shared memory for Multi-Instance AWE between Primary & Secondary Cores */
    SHARED_HEAP_MEM: 	 org = SHM_C7A53_SHARED_HEAP_BASE, len = SHM_C7A53_SHARED_HEAP_SIZE

    SHM_AVB_CTRL_CMD:    org = SHM_AVB_CNTL_RX_BASE, len = SHM_AVB_CNTL_RX_SIZE
    SHM_AVB_CTRL_RSP:    org = SHM_AVB_CNTL_TX_BASE, len = SHM_AVB_CNTL_TX_SIZE
    SHM_AVB_PCM_DATA_RX: org = SHM_AVB_DATA_RX_BASE, len = SHM_AVB_DATA_RX_SIZE
    SHM_AVB_PCM_DATA_TX: org = SHM_AVB_DATA_TX_BASE, len = SHM_AVB_DATA_TX_SIZE
}

SECTIONS
{
    .vecs       >       C7X_DDR_UNCACHED ALIGN(0x200000)
    .text:_c_int00_secure > C7X_DDR_CACHED ALIGN(0x200000)
    .text       >       C7X_DDR_CACHED

    .bss > C7X_DDR_CACHED ALIGN(128)
    RUN_START(__BSS_START)
    RUN_END(__BSS_END)

    .data       >       C7X_DDR_CACHED   /* Initialized data */
    .cinit      >       C7X_DDR_CACHED   /* could be part of const */
    .init_array >       C7X_DDR_CACHED   /* C++ initializations */
    .stack (NOLOAD)  >  C7X_DDR_CACHED ALIGN(0x20000) /* Needed for nested-interrupt */
    .args       >       C7X_DDR_CACHED
    .cio        >       C7X_DDR_UNCACHED
    .const      >       C7X_DDR_CACHED
    .switch     >       C7X_DDR_CACHED  /* For exception handling. */
    .sysmem (NOLOAD) >  C7X_DDR_CACHED  /* heap */

    audio_layout0_task_stack (NOLOAD) > L2SRAM

    GROUP:      >  C7X_DDR_CACHED
    {
        .data.Mmu_tableArray          : type=NOINIT
        .data.Mmu_tableArraySlot      : type=NOINIT
        .data.Mmu_level1Table         : type=NOINIT
        .data.gMmu_tableArray_NS      : type=NOINIT
        .data.Mmu_tableArraySlot_NS   : type=NOINIT
        .data.Mmu_level1Table_NS      : type=NOINIT
    }

    .bss.log_shared_mem (NOLOAD) : {} > SHM_SHARED_LOG
    .awe_tuning_pkt (NOLOAD) : {} > SHM_TUNING_PKT

    .ti_avb_ctrl_cmd (NOLOAD) > SHM_AVB_CTRL_CMD
    .ti_avb_ctrl_rsp (NOLOAD) > SHM_AVB_CTRL_RSP

    /* AWE Core - Data placement Heap */
    .awe_heap_fast (NOLOAD) > L2SRAM ALIGN(256)
    .awe_heap_fastb (NOLOAD) > C7X_DDR_CACHED_AWE ALIGN(256)
    .awe_heap_slow (NOLOAD) > C7X_DDR_CACHED_AWE ALIGN(256)

    /* AWE core - Code placement */
    .awe_prio0_fastcode > L2SRAM_AUX
    .awe_fw_fastcode  > L2SRAM_AUX
    .awe_mod_fastcode > C7X_DDR_CACHED

    .awe_mod_slowcode > C7X_DDR_CACHED
    .awe_fw_slowcode  > C7X_DDR_CACHED
    .awe_mod_slowdmdata > C7X_DDR_CACHED

    awe_mcasp_buffer (NOLOAD) > C7X_DDR_CACHED

    /* Shared Heap for Multi-Instance AWE */
    .awe_shared_heap (NOLOAD) > SHARED_HEAP_MEM ALIGN(128)

    /* AVB shared memory buffer */
    .asdk_avb_rx_buffer (NOLOAD)  > SHM_AVB_PCM_DATA_RX
    .asdk_avb_tx_buffer (NOLOAD)  > SHM_AVB_PCM_DATA_TX

}
