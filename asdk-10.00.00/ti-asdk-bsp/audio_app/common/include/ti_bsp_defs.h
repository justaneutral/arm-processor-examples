// TI BSP Defs

#ifndef __TI_BSP_DEFS_H__
#define __TI_BSP_DEFS_H__

/*------------------ Shared Memory Definition : start ---------------------*/
#define SHARED_MEM_BASE					(0xA0400000)

#define SHM_AVB_DATA_RX_SIZE		    (0x80000)  /* 512kB  */
#define SHM_AVB_DATA_TX_SIZE		    (0x80000)  /* 512kB  */
#define SHM_AVB_CNTL_RX_SIZE		    (0x02000)  /* 8kB    */
#define SHM_AVB_CNTL_TX_SIZE		    (0x02000)  /* 8kB    */
#define SHM_C7A53_SHARED_HEAP_SIZE      (0x08000)  /* 32kB   */
#define SHM_TUNING_PKT_SIZE     		(0x00800)  /* 2 kB   */
#define SHM_SHARED_LOG_SIZE     		(0x3E800)  /* 200 kB */
/* Total memory use --------------------------------: 1.5 MB */

#define SHM_AVB_DATA_RX_BASE            (SHARED_MEM_BASE)
#define SHM_AVB_DATA_TX_BASE            (SHM_AVB_DATA_RX_BASE + SHM_AVB_DATA_RX_SIZE)
#define SHM_AVB_CNTL_RX_BASE            (SHM_AVB_DATA_TX_BASE + SHM_AVB_DATA_TX_SIZE)
#define SHM_AVB_CNTL_TX_BASE            (SHM_AVB_CNTL_RX_BASE + SHM_AVB_CNTL_RX_SIZE)
#define SHM_C7A53_SHARED_HEAP_BASE      (SHM_AVB_CNTL_TX_BASE + SHM_AVB_CNTL_TX_SIZE)
#define SHM_TUNING_PKT_BASE             (SHM_C7A53_SHARED_HEAP_BASE + SHM_C7A53_SHARED_HEAP_SIZE)
#define SHM_SHARED_LOG_BASE             (SHM_TUNING_PKT_BASE + SHM_TUNING_PKT_SIZE)
/*------------------ Shared Memory Definition : end ---------------------*/

/* All audio core must notify to gateway (mcu/r5f) */
#define REMOTE_ALIVE_NOTIFY         	(IPC_NOTIFY_CLIENT_ID_SYNC+1)

/* Ready ready between primary and secondary core */
#define REMOTE_AWE_DATA_NOTIFY      	(IPC_NOTIFY_CLIENT_ID_SYNC+2)

/* Notify secondary core to process tuning packet */
#define REMOTE_AWE_PACKET_READY     	(IPC_NOTIFY_CLIENT_ID_SYNC+3)

/* Tuning packet notify between MCU and Primary Audio Core */
#define AWE_TUNING_PCKT_READY		    (IPC_NOTIFY_CLIENT_ID_SYNC+4)

/* Layout Processing Time notify between MCU and Primary Audio Core */
#define LAYOUT_PROCESSING_TIME		    (IPC_NOTIFY_CLIENT_ID_SYNC+5)

/* AVB Control Command */
#define TI_AVB_CONTROL_CMD           	(IPC_NOTIFY_CLIENT_ID_SYNC+6)

/* AVB Control Response */
#define TI_AVB_CONTROL_RESP          	(IPC_NOTIFY_CLIENT_ID_SYNC+7)

/* AVB PCM Data */
#define TI_AVB_DATA_READY            	(IPC_NOTIFY_CLIENT_ID_SYNC+8)

#endif /* __TI_BSP_DEFS_H__ */
