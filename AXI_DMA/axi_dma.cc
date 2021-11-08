#include "axi_dma.h"

static XAxiDma AxiDma;
static XScuGic INST;

static s32 rxIrq = 1;
static s32 txIrq = 1;

AXI_DMA::AXI_DMA() {}

AXI_DMA::~AXI_DMA() {}

int AXI_DMA::setInterruptInit(u16 txIntrId, u16 rxIntrId)
{
    XScuGic_Config *Config;
    int Status;

    Config = XScuGic_LookupConfig(INT_DEVICE_ID);

    Status = XScuGic_CfgInitialize(&INST, Config, Config->CpuBaseAddress);

    if (Status != XST_SUCCESS)
        return XST_FAILURE;

    // Status = XScuGic_Connect(InstancePtr, IntrID, (Xil_ExceptionHandler)CheckData, XAxiDmaPtr);
    Status = XScuGic_Connect(&INST, rxIntrId,
            (Xil_ExceptionHandler)rxIrqHandler,
            &AxiDma);

    Status = XScuGic_Connect(&INST, txIntrId,
            (Xil_ExceptionHandler)txIrqHandler,
            &AxiDma);
//
//    if (Status != XST_SUCCESS)
//        return XST_FAILURE;

    XScuGic_Enable(&INST, txIntrId);
    XScuGic_Enable(&INST, rxIntrId);

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

    Status = setInterruptInit(TX_INTR_ID, RX_INTR_ID);
    if (Status != XST_SUCCESS)
        return XST_FAILURE;
    // Disable all interrupt.
    XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK,
                       XAXIDMA_DEVICE_TO_DMA);
    XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK,
                       XAXIDMA_DMA_TO_DEVICE);
    
    // enable all interrupt.
    XAxiDma_IntrEnable(&AxiDma, XAXIDMA_IRQ_ALL_MASK,
                       XAXIDMA_DEVICE_TO_DMA);
    XAxiDma_IntrEnable(&AxiDma, XAXIDMA_IRQ_ALL_MASK,
                        XAXIDMA_DMA_TO_DEVICE);

    this->initDone = true;

    return XST_SUCCESS;
}

long int AXI_DMA::trans_DMA_device(u8 *sendBuffer, int length)
{

    int Status = 0;

    Xil_DCacheFlushRange((UINTPTR)sendBuffer, length);

    Status = XAxiDma_SimpleTransfer(&AxiDma, (UINTPTR)sendBuffer, length, XAXIDMA_DMA_TO_DEVICE);

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

long int AXI_DMA::trans_DMA_device(u32* sendBuffer, int length){

    length = length * 4;

    int Status = 0;

    Xil_DCacheFlushRange((UINTPTR)sendBuffer, length);

    Status = XAxiDma_SimpleTransfer(&AxiDma, (UINTPTR)sendBuffer, length, XAXIDMA_DMA_TO_DEVICE);

    if (Status != XST_SUCCESS)
    {
        throw "DMA to slaver is not success!\n";
        return XST_FAILURE;
    }

    while (1)
    {
        if(txIrq == 0)
            break;
    }
    txIrq = 1;

    return XST_SUCCESS;
}

long int AXI_DMA::trans_device_DMA(u32* receiveBuffer, int length){
    length = length * 4;

    int Status = 0;

    // Xil_DCacheFlushRange((UINTPTR)receiveBuffer, length);

    Status = XAxiDma_SimpleTransfer(&AxiDma, (UINTPTR)receiveBuffer, length, XAXIDMA_DEVICE_TO_DMA);

    if (Status != XST_SUCCESS)
    {
        throw "slaver to DMA is not success!\n";
        return XST_FAILURE;
    }

    // while (XAxiDma_Busy(&AxiDma, XAXIDMA_DEVICE_TO_DMA))
    while(rxIrq)
    {
        ;
    }
    rxIrq = 1;

    Xil_DCacheInvalidateRange((UINTPTR)receiveBuffer, length);

    return XST_SUCCESS;
}

void AXI_DMA::rxIrqHandler(void *CallBackRef){
    XAxiDma_IntrAckIrq(&AxiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA) ;
    rxIrq = 0;
}

void AXI_DMA::txIrqHandler(void *CallBackRef){
    XAxiDma_IntrAckIrq(&AxiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
    txIrq = 0;
}

void AXI_DMA::resetDma(){
    XAxiDma_Reset(&AxiDma);
    this->initDone = false;
}

bool AXI_DMA::isInited(){
    return this->initDone;
}
