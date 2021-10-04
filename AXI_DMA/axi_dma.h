#ifndef __AXI_DMA_H__
#define __AXI_DMA_H__

#define __ARM__

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
public:
    AXI_DMA();
    ~AXI_DMA();

    int setInterruptInit(XScuGic *InstancePtr, u16 IntrID, XAxiDma *XAxiDmaPtr);

    long int setup(u16 Device_id);

    long int trans_DMA_device(u8* sendBuffer, int length);
};

/*
static int CheckData(void)
{
    u8 *RxPacket;
    int Index = 0;
    u8 Value;

    RxPacket =  RxBufferPtr;
    Value = TEST_START_VALUE;

     * Invalidate the DestBuffer before receiving the data, in case the
     * Data Cache is enabled
     *
#ifndef __aarch64__
    Xil_DCacheInvalidateRange((UINTPTR)RxPacket, MAX_PKT_LEN);
#endif

    for(Index = 0; Index < MAX_PKT_LEN; Index++) {
        if (RxPacket[Index] != Value) {
            xil_printf("Data error %d: %x/%x\r\n",
            Index, (unsigned int)RxPacket[Index],
                (unsigned int)Value);

            return XST_FAILURE;
        }
        Value = (Value + 1) & 0xFF;
    }

    return XST_SUCCESS;
}
*/

#endif
#endif