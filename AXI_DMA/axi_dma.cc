#include "axi_dma.h"

AXI_DMA::AXI_DMA() {}

AXI_DMA::~AXI_DMA() {}

int AXI_DMA::setInterruptInit(u16 IntrID)
{
    XScuGic_Config *Config;
    int Status;

    Config = XScuGic_LookupConfig(INT_DEVICE_ID);

    Status = XScuGic_CfgInitialize(&INST, Config, Config->CpuBaseAddress);

    if (Status != XST_SUCCESS)
        return XST_FAILURE;

//    Status = XScuGic_Connect(InstancePtr, IntrID, (Xil_ExceptionHandler)CheckData, XAxiDmaPtr);
//
//    if (Status != XST_SUCCESS)
//        return XST_FAILURE;

    XScuGic_Enable(&INST, IntrID);

    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler,
                                 &INST);
    Xil_ExceptionEnable();

    return XST_SUCCESS;
}

long int AXI_DMA::setup()
{
    XAxiDma_Config *CfgPtr;
    int Status;
    u8 Value;
    /* Initialize the XAxiDma device.
     */
    CfgPtr = XAxiDma_LookupConfig(Device_id);
    if (!CfgPtr)
    {
        xil_printf("No config found for %d\r\n", Device_id);
        return XST_FAILURE;
    }

    Status = XAxiDma_CfgInitialize(&AxiDma, CfgPtr);
    if (Status != XST_SUCCESS)
    {
        xil_printf("Initialization failed %d\r\n", Status);
        return XST_FAILURE;
    }

    if (XAxiDma_HasSg(&AxiDma))
    {
        xil_printf("Device configured as SG mode \r\n");
        return XST_FAILURE;
    }

    Status = setInterruptInit(INTR_ID);
    if (Status != XST_SUCCESS)
        return XST_FAILURE;

    // Disable MM2S interrupt, Enable S2MM interrupt.
    XAxiDma_IntrEnable(&AxiDma, XAXIDMA_IRQ_IOC_MASK,
                       XAXIDMA_DEVICE_TO_DMA);
    XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK,
                        XAXIDMA_DMA_TO_DEVICE);

    return XST_SUCCESS;
}

long int AXI_DMA::trans_DMA_device(u8 *sendBuffer, int length)
{

    int Status = 0;

    Xil_DCacheFlushRange((UINTPTR)sendBuffer, length);

    Status = XAxiDma_SimpleTransfer(&this->AxiDma, (UINTPTR)sendBuffer, length, XAXIDMA_DMA_TO_DEVICE);

    if (Status != XST_SUCCESS)
    {
        throw "DMA to slaver is not success!\n";
        return XST_FAILURE;
    }

    while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE))
    {
        ;
    }

    return XST_SUCCESS;
}

long int AXI_DMA::trans_DMA_device(int* sendBuffer, int length){

    length = length * 4;

    int Status = 0;

    Xil_DCacheFlushRange((UINTPTR)sendBuffer, length);

    Status = XAxiDma_SimpleTransfer(&this->AxiDma, (UINTPTR)sendBuffer, length, XAXIDMA_DMA_TO_DEVICE);

    if (Status != XST_SUCCESS)
    {
        throw "DMA to slaver is not success!\n";
        return XST_FAILURE;
    }

    while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE))
    {
        ;
    }

    return XST_SUCCESS;
}
