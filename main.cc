#include <iostream>
#include <string>
#include "MobileNet.h"
#include "utils.h"

#if defined __WINDOWS__
#include "parameters/image.h"
#include <time.h>
#include <windows.h>
#include <iomanip>
#endif

#ifdef __ARM__
#include "parameters/image_origin.h"
#include "parameters/config_quant.h"
#include "parameters/fc.h"
#endif

#if defined __CAL__
#include "parameters/image.h"
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

#if defined __ARM__ && defined __DMA__
void initParameters(u32* param_addr);
#endif

using namespace std;

int main()
{
#if defined __WINDOWS__

    float result[43];

    LARGE_INTEGER num;

    long long start, end, freq;

    MobileNet net;

    double counter = 1.0;
    double sum = 0.0;

    net.init();

    QueryPerformanceFrequency(&num);
    freq = num.QuadPart;

    while (1) {
        QueryPerformanceCounter(&num);
        start = num.QuadPart;

        net.invoke(224, 224, 3, image, result);

        QueryPerformanceCounter(&num);
        end = num.QuadPart;

        sum += (double)(end - start) * 1000.0 / freq;

        cout << setprecision(12) << sum / counter << endl;

        counter += 1.0;
    }

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

    XTime begin, end;
    double timeUsed;

    AXI_DMA dma;

    dla MobileNet_dla;

    MobileNet model;

    u8* res_p;

    u32 *param, *fmap, *param_base;

    u32 *result, *result_2;

    u8(*result_u8_2w)[32];
    u8(*result_u8_2w_1)[32];
    u8(*result_u8_2w_2)[32];

    param = new u32[834672];

    param_base = param;

    fmap = new u32[50176];
    result = new u32[64];

    initParameters(param_base);

    result_u8_2w = (u8(*)[32])(result + 4);

    float f[43], q[43];

    model.init();

    dma.resetDma();

    dma.setup();

    MobileNet_dla.enable();

    MobileNet_dla.ramInvert(true);

    MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, 36);

    __init_array(50176, image_orgin, fmap);

    while (1)
    {
        param = param_base;
        XTime_GetTime(&begin);
        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, t_conv_shift);
        
        model.conv2d(224, 224, 3, 3, 3, 2, 32, &dma, &MobileNet_dla, param, fmap);
        __addr_offset(280, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_0_shift);
        model.depthwiseConv2d(112, 112, 3, 3, 32, 1, 32, &dma, &MobileNet_dla, param);
        __addr_offset(32 * 9 / 4 + 32 + 32, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_0_shift);
        model.pointwiseConv2d(112, 112, 32, 64, &dma, &MobileNet_dla, param);
        __addr_offset(640, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_1_shift);
        model.depthwiseConv2d(112, 112, 3, 3, 64, 2, 64, &dma, &MobileNet_dla, param);
        __addr_offset(64 * 9 / 4 + 64 + 64, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_1_shift);
        model.pointwiseConv2d(56, 56, 64, 128, &dma, &MobileNet_dla, param);
        __addr_offset(64 * 128 / 4 + 128 + 128, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_2_shift);
        model.depthwiseConv2d(56, 56, 3, 3, 128, 1, 128, &dma, &MobileNet_dla, param);
        __addr_offset(128 * 9 / 4 + 128 + 128, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_2_shift);
        model.pointwiseConv2d(56, 56, 128, 128, &dma, &MobileNet_dla, param);
        __addr_offset(128 * 128 / 4 + 128 + 128, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_3_shift);
        model.depthwiseConv2d(56, 56, 3, 3, 128, 2, 128, &dma, &MobileNet_dla, param);
        __addr_offset(128 * 9 / 4 + 128 + 128, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_3_shift);
        model.pointwiseConv2d(28, 28, 128, 256, &dma, &MobileNet_dla, param);
        __addr_offset(128 * 256 / 4 + 256 + 256, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_4_shift);
        model.depthwiseConv2d(28, 28, 3, 3, 256, 1, 256, &dma, &MobileNet_dla, param);
        __addr_offset(256 * 9 / 4 + 256 + 256, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_4_shift);
        model.pointwiseConv2d(28, 28, 256, 256, &dma, &MobileNet_dla, param);
        __addr_offset(256 * 256 / 4 + 256 + 256, param);
        
        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_5_shift);
        model.depthwiseConv2d(28, 28, 3, 3, 256, 2, 256, &dma, &MobileNet_dla, param);
        __addr_offset(256 * 9 / 4 + 256 + 256, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_5_shift);
        model.pointwiseConv2d(14, 14, 256, 512, &dma, &MobileNet_dla, param);
        __addr_offset(256 * 512 / 4 + 512 + 512, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_6_shift);
        model.depthwiseConv2d(14, 14, 3, 3, 512, 1, 512, &dma, &MobileNet_dla, param);
        __addr_offset(512 * 9 / 4 + 512 + 512, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_6_shift);
        model.pointwiseConv2d(14, 14, 512, 512, &dma, &MobileNet_dla, param);
        __addr_offset(512 * 512 / 4 + 512 + 512, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_7_shift);
        model.depthwiseConv2d(14, 14, 3, 3, 512, 1, 512, &dma, &MobileNet_dla, param);
        __addr_offset(512 * 9 / 4 + 512 + 512, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_7_shift);
        model.pointwiseConv2d(14, 14, 512, 512, &dma, &MobileNet_dla, param);
        __addr_offset(512 * 512 / 4 + 512 + 512, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_8_shift);
        model.depthwiseConv2d(14, 14, 3, 3, 512, 1, 512, &dma, &MobileNet_dla, param);
        __addr_offset(512 * 9 / 4 + 512 + 512, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_8_shift);
        model.pointwiseConv2d(14, 14, 512, 512, &dma, &MobileNet_dla, param);
        __addr_offset(512 * 512 / 4 + 512 + 512, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_9_shift);
        model.depthwiseConv2d(14, 14, 3, 3, 512, 1, 512, &dma, &MobileNet_dla, param);
        __addr_offset(512 * 9 / 4 + 512 + 512, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_9_shift);
        model.pointwiseConv2d(14, 14, 512, 512, &dma, &MobileNet_dla, param);
        __addr_offset(512 * 512 / 4 + 512 + 512, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_10_shift);
        model.depthwiseConv2d(14, 14, 3, 3, 512, 1, 512, &dma, &MobileNet_dla, param);
        __addr_offset(512 * 9 / 4 + 512 + 512, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_10_shift);
        model.pointwiseConv2d(14, 14, 512, 512, &dma, &MobileNet_dla, param);
        __addr_offset(512 * 512 / 4 + 512 + 512, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_11_shift);
        model.depthwiseConv2d(14, 14, 3, 3, 512, 2, 512, &dma, &MobileNet_dla, param);
        __addr_offset(512 * 9 / 4 + 512 + 512, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_11_shift);
        model.pointwiseConv2d(7, 7, 512, 1024, &dma, &MobileNet_dla, param);
        __addr_offset(512 * 1024 / 4 + 1024 + 1024, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_12_shift);
        model.depthwiseConv2d(7, 7, 3, 3, 1024, 1, 1024, &dma, &MobileNet_dla, param);
        __addr_offset(1024 * 9 / 4 + 1024 + 1024, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_12_shift);
        model.pointwiseConv2d(7, 7, 1024, 1024, &dma, &MobileNet_dla, param);
        __addr_offset(1024 * 1024 / 4 + 1024 + 1024, param);

        model.globalAvgPool2D(7, 7, 1024, &dma, &MobileNet_dla);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dense_shift);
        model.dense(0, 0, &dma, &MobileNet_dla, param);

        XTime_GetTime(&end);

        model.getResult(1, 1, 1, 1024, &dma, &MobileNet_dla, result);

        for (int i = 0; i < 43; i++) {
            f[i] = 0.2866767346858978  * (((u8*)result_u8_2w)[i] - 38);
        }
    
        model.softmax(43, f, q);

        for (int i = 0; i < 43; i++)
            printf("%.5lf\n", q[i]);
        cout << endl;
        
        timeUsed = ((end - begin)) / (666666687.0 / 2.0);

        printf("Time elapsed: %.5lfms\n", timeUsed * 1000);

    }

    while (1);

#endif

    return 0;
}

#if defined __ARM__ && defined __DMA__
void initParameters(u32* param_addr){

    __init_array(280, t_conv_quant, param_addr);
    param_addr += 280;
    
    __init_array(32 * 9 / 4 + 32 + 32 , dw_conv_0_quant, param_addr);
    param_addr += 32 * 9 / 4 + 32 + 32 ;
    __init_array(640, pw_conv_0_quant, param_addr);
    param_addr += 640;
    __init_array(64 * 9 / 4 + 64 + 64 , dw_conv_1_quant, param_addr);
    param_addr += 64 * 9 / 4 + 64 + 64 ;
    __init_array(64 * 128 / 4 + 128 + 128, pw_conv_1_quant, param_addr);
    param_addr += 64 * 128 / 4 + 128 + 128;
    __init_array(128 * 9 / 4 + 128 + 128 , dw_conv_2_quant, param_addr);
    param_addr += 128 * 9 / 4 + 128 + 128 ;
    __init_array(128 * 128 / 4 + 128 + 128 , pw_conv_2_quant, param_addr);
    param_addr += 128 * 128 / 4 + 128 + 128 ;
    __init_array(128 * 9 / 4 + 128 + 128, dw_conv_3_quant, param_addr);
    param_addr += 128 * 9 / 4 + 128 + 128;
    __init_array(128 * 256 / 4 + 256 + 256 , pw_conv_3_quant, param_addr);
    param_addr += 128 * 256 / 4 + 256 + 256 ;
    __init_array(256 * 9 / 4 + 256 + 256 , dw_conv_4_quant, param_addr);
    param_addr += 256 * 9 / 4 + 256 + 256 ;
    __init_array(256 * 256 / 4 + 256 + 256 , pw_conv_4_quant, param_addr);
    param_addr += 256 * 256 / 4 + 256 + 256 ;
    __init_array(256 * 9 / 4 + 256 + 256 , dw_conv_5_quant, param_addr);
    param_addr += 256 * 9 / 4 + 256 + 256 ;
    __init_array(256 * 512 / 4 + 512 + 512 , pw_conv_5_quant, param_addr);
    param_addr += 256 * 512 / 4 + 512 + 512 ;
    __init_array(512 * 9 / 4 + 512 + 512 , dw_conv_6_quant, param_addr);
    param_addr += 512 * 9 / 4 + 512 + 512 ;
    __init_array(512 * 512 / 4 + 512 + 512 , pw_conv_6_quant, param_addr);
    param_addr += 512 * 512 / 4 + 512 + 512 ;
    __init_array(512 * 9 / 4 + 512 + 512 , dw_conv_7_quant, param_addr);
    param_addr += 512 * 9 / 4 + 512 + 512 ;
    __init_array(512 * 512 / 4 + 512 + 512 , pw_conv_7_quant, param_addr);
    param_addr += 512 * 512 / 4 + 512 + 512 ;
    __init_array(512 * 9 / 4 + 512 + 512 , dw_conv_8_quant, param_addr);
    param_addr += 512 * 9 / 4 + 512 + 512 ;
    __init_array(512 * 512 / 4 + 512 + 512 , pw_conv_8_quant, param_addr);
    param_addr += 512 * 512 / 4 + 512 + 512 ;
    __init_array(512 * 9 / 4 + 512 + 512 , dw_conv_9_quant, param_addr);
    param_addr += 512 * 9 / 4 + 512 + 512 ;
    __init_array(512 * 512 / 4 + 512 + 512 , pw_conv_9_quant, param_addr);
    param_addr += 512 * 512 / 4 + 512 + 512 ;
    __init_array(512 * 9 / 4 + 512 + 512, dw_conv_10_quant, param_addr);
    param_addr += 512 * 9 / 4 + 512 + 512;
    __init_array(512 * 512 / 4 + 512 + 512 , pw_conv_10_quant, param_addr);
    param_addr += 512 * 512 / 4 + 512 + 512 ;
    __init_array(512 * 9 / 4 + 512 + 512 , dw_conv_11_quant, param_addr);
    param_addr += 512 * 9 / 4 + 512 + 512 ;
    __init_array(512 * 1024 / 4 + 1024 + 1024 , pw_conv_11_quant, param_addr);
    param_addr += 512 * 1024 / 4 + 1024 + 1024 ;
    __init_array(1024 * 9 / 4 + 1024 + 1024, dw_conv_12_quant, param_addr);
    param_addr += 1024 * 9 / 4 + 1024 + 1024;
    __init_array(1024 * 1024 / 4 + 1024 + 1024 , pw_conv_12_quant, param_addr);
    param_addr += 1024 * 1024 / 4 + 1024 + 1024 ;

    __init_array(64 * 1024 / 4 + 64 + 64, dense_quant, param_addr);

    // int amount = 
    //     280 + 
    //     32 * 9 / 4 + 32 + 32 +
    //     640 +
    //     64 * 9 / 4 + 64 + 64 +
    //     64 * 128 / 4 + 128 + 128 + 
    //     128 * 9 / 4 + 128 + 128 +
    //     128 * 128 / 4 + 128 + 128 +
    //     128 * 9 / 4 + 128 + 128 + 
    //     128 * 256 / 4 + 256 + 256 +
    //     256 * 9 / 4 + 256 + 256 +
    //     256 * 256 / 4 + 256 + 256 +
    //     256 * 9 / 4 + 256 + 256 +
    //     256 * 512 / 4 + 512 + 512 +
    //     512 * 9 / 4 + 512 + 512 +
    //     512 * 512 / 4 + 512 + 512 +
    //     512 * 9 / 4 + 512 + 512 +
    //     512 * 512 / 4 + 512 + 512 +
    //     512 * 9 / 4 + 512 + 512 +
    //     512 * 512 / 4 + 512 + 512 +
    //     512 * 9 / 4 + 512 + 512 +
    //     512 * 512 / 4 + 512 + 512 +
    //     512 * 9 / 4 + 512 + 512 + 
    //     512 * 512 / 4 + 512 + 512 +
    //     512 * 9 / 4 + 512 + 512 +
    //     512 * 1024 / 4 + 1024 + 1024 +
    //     1024 * 9 / 4 + 1024 + 1024 +
    //     1024 * 1024 / 4 + 1024 + 1024 +
    //     64 * 1024 / 4 + 64 + 64;
}

#endif
