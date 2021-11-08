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

class AXI_DMA
{
public:
    u16 Device_id = DMA_DEV_ID;

private:
    bool initDone = false;
public:
    AXI_DMA();
    ~AXI_DMA();

    int setInterruptInit(u16 txIntrId, u16 rxIntrId);

    long int setup();

    void resetDma();

    bool isInited();

    long int trans_DMA_device(u8* sendBuffer, int length);

    long int trans_DMA_device(u32* sendBuffer, int length);

    long int trans_device_DMA(u32* receiveBuffer, int length);

    static void rxIrqHandler(void *CallBackRef);

    static void txIrqHandler(void *CallBackRef);
};

#endif
#endif
