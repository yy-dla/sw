#include <iostream>
// #include "config.h"
#include <string>
#include "MobileNet.h"
#include "image.h"


#if defined __ARM__
#include "xparameters.h"
#include "xuartps.h"
#include "xil_printf.h"
#include "xtime_l.h"
#endif

#if defined __ARM__ && defined __DMA__
#include "AXI_DMA/axi_dma.h"
#endif

#if defined __ARM__
#define URAT_DEVICE_ID XPAR_XUARTPS_0_DEVICE_ID
#endif

using namespace std;

//static float map1[1024][224][224];
//static float map2[1024][224][224];
//static float globalAvgPoolMap[1024];
//static float denseMap[43];
// static float result[43];

int main()
{
#if defined __WINDOWS__

    float result[43];

    MobileNet net;

    net.init();

    net.invoke(224, 224, 3, image, result);

    for (int i = 0; i < 43; i++) {
        printf("%lf \n", result[i]);
    }

    return 0;
    
    
#endif
#if defined __ARM__ && defined __CAL__
    int Status;
    XUartPs Uart_Ps;
    XUartPs_Config *Config;
    MobileNet net;
    float result[43];
    XTime begin, end;
    double timeUsed;

    Config = XUartPs_LookupConfig(URAT_DEVICE_ID);

    Status = XUartPs_CfgInitialize(&Uart_Ps, Config, Config->BaseAddress);

    XUartPs_SetBaudRate(&Uart_Ps, 115200);

    // printf("hello world, this is mobilenet!, %lf", 0.01);

    cout << "begin" << endl;

    net.init();

    XTime_GetTime(&begin);

    net.invoke(224, 224, 3, image, result);

    XTime_GetTime(&end);

    timeUsed = ((end - begin)) / (666666687.0 / 2.0);

    cout << "Time elapsed: " << timeUsed << "s" << endl;

    for (int i = 0; i < 43; i++) {
        cout << result[i] << std::endl;
    }

    return 0;

#endif

#if defined __ARM__ && defined __DMA__

    AXI_DMA dma;

    int *a, *b;

    // MobileNet m;

    // m.writeReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET, 0xff);

    // cout << m.readReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET) << endl;

    a = new int[216];
    b = new int[220];

    for(int i = 0; i < 216; i++) {
        a[i] = i;
    }

    dma.resetDma();

    dma.setup();

    // dma.setInterruptInit(TX_INTR_ID, RX_INTR_ID);

    for (int j = 0; j < 2; j++){
        dma.writeReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET, 0x7);

        dma.trans_DMA_device(a, 216); // W

        dma.writeReg(MOBILENET_S00_AXI_SLV_CONFIG_REG_OFFSET, 0x5);

        dma.trans_device_DMA(b, 300); // R

//        for (int i = 0; i < 216; i++){
//            cout << b[i] << " ";
//            // a[i] += b[i + 4];
//        }
//
//        cout << endl;

        
    }

#endif

    return 0;
}
