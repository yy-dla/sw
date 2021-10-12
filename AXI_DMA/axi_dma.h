#ifndef __AXI_DMA_H__
#define __AXI_DMA_H__

// #define __ARM__

#if defined __ARM__

#include "xaxidma.h"
#include "xscugic.h"
#include "xil_exception.h"

#define DMA_DEV_ID		  XPAR_AXIDMA_0_DEVICE_ID
#define INT_DEVICE_ID     XPAR_SCUGIC_SINGLE_DEVICE_ID
#define INTR_ID           XPAR_FABRIC_AXI_DMA_0_S2MM_INTROUT_INTR


class AXI_DMA
{
private:
    XAxiDma AxiDma;
    XScuGic INST;
    u16 Device_id = DMA_DEV_ID;
public:
    AXI_DMA();
    ~AXI_DMA();

    int setInterruptInit(u16 IntrID);

    long int setup();

    long int trans_DMA_device(u8* sendBuffer, int length);

    long int trans_DMA_device(int* sendBuffer, int length);
};

#endif
#endif