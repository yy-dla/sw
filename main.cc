#include <iostream>
// #include "config.h"
#include <string>
#include "MobileNet.h"
#include "utils.h"

#if defined __WINDOWS__
#include "parameters/image.h"
#endif

#ifdef __ARM__
#include "parameters/image_origin.h"
#include "parameters/config_quant.h"
#endif

#if defined __ARM__
#include "xparameters.h"
#include "xuartps.h"
#include "xil_printf.h"
#include "xtime_l.h"
#endif

#if defined __ARM__ && defined __DMA__
#include "AXI_DMA/axi_dma.h"
#include "dla/dla.h"
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

    for (int i = 0; i < 43; i++)
    {
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

    for (int i = 0; i < 43; i++)
    {
        cout << result[i] << std::endl;
    }

    return 0;

#endif

#if defined __ARM__ && defined __DMA__

    AXI_DMA dma;

    dla MobileNet_dla;

    MobileNet model;

    u32 *param, *fmap;

    u32 *result, *result_2;

    u8(*result_u8_2w)[32];
    u8(*result_u8_2w_1)[32];

    param = new u32[280];
    fmap = new u32[50176];
    result = new u32[100352 + 4];
    result_2 = new u32[100352 + 4];

    // for(int i = 0; i < 280; i++) {
    //     param[i] = t_conv_config_quant[i];
    // }

    // for(int i = 0; i < 50176; i++){
    //     fmap[i] = image_orgin[i];
    // }

    result_u8_2w = (u8(*)[32])(result + 4);
    result_u8_2w_1 = (u8(*)[32])(result_2 + 4);

    model.init();

    dma.resetDma();

    dma.setup();

    MobileNet_dla.enable();
    while (1)
    {
        __init_array(280, t_conv_config_quant, param);
        __init_array(50176, image_orgin, fmap);

        model.conv2d(224, 224, 3, 3, 3, 2, 32, &dma, &MobileNet_dla, param, fmap, result);

        for (int j = 0; j < 32; j++)
        {
            cout << (int)result_u8_2w[0][j] << ", ";
        }
        cout << endl;

        __init_array(32 * 9 / 4 + 32 + 32, dw_conv_config_quant, param);

        model.depthwiseConv2d(112, 112, 3, 3, 32, 1, 32, &dma, &MobileNet_dla, param, result + 4, result_2);

        for (int j = 0; j < 32; j++)
        {
            cout << (int)result_u8_2w_1[0][j] << ", ";
        }
        cout << endl;
    }

    while (1);

        //    cout << hex << *result_offset4 << "  addr:" << result_offset4 <<endl;
        //    cout << hex << *result << "  addr:" << result<< endl;
        //    cout << hex << *(result + 4) << endl;

#endif

    return 0;
}
