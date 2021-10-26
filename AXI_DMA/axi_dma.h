#ifndef __AXI_DMA_H__
#define __AXI_DMA_H__

// #define __ARM__

#if defined __ARM__

#include "xaxidma.h"
#include "xscugic.h"
#include "xil_exception.h"

#define DMA_DEV_ID		  XPAR_AXIDMA_0_DEVICE_ID
#define INT_DEVICE_ID     XPAR_SCUGIC_SINGLE_DEVICE_ID
#define RX_INTR_ID        XPAR_FABRIC_AXI_DMA_0_S2MM_INTROUT_INTR
#define TX_INTR_ID        XPAR_FABRIC_AXI_DMA_0_MM2S_INTROUT_INTR


#define MOBILENET_S00_AXI_SLV_STATE_REG_OFFSET      0
#define MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET     4
#define MOBILENET_S00_AXI_SLV_F_W_OFFSET            8
#define MOBILENET_S00_AXI_SLV_F_H_OFFSET            12
#define MOBILENET_S00_AXI_SLV_STRIDE_OFFSET         16
#define MOBILENET_S00_AXI_SLV_K_W_OFFSET            20
#define MOBILENET_S00_AXI_SLV_K_H_OFFSET            24
#define MOBILENET_S00_AXI_SLV_CHANNEL_OFFSET        28
#define MOBILENET_S00_AXI_SLV_N_OFFSET              32

#define MOBILENET_BASEADDR                          XPAR_MOBILENET_0_S00_AXI_BASEADDR

class AXI_DMA
{
public:
    u16 Device_id = DMA_DEV_ID;
public:
    AXI_DMA();
    ~AXI_DMA();

    int setInterruptInit(u16 txIntrId, u16 rxIntrId);

    long int setup();

    void resetDma();

    long int trans_DMA_device(u8* sendBuffer, int length);

    long int trans_DMA_device(int* sendBuffer, int length);

    long int trans_device_DMA(int* receiveBuffer, int length);

    /**
     ** For ARM-9 registers reading and writing.
    */
    #if defined __ARM__

    void writeReg(int offset, int data);

    int readReg(int offset);

    #endif

    static void rxIrqHandler(void *CallBackRef);

    static void txIrqHandler(void *CallBackRef);
};

#endif
#endif
