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
#include "parameters/fc.h"
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

    XTime begin, end;
    double timeUsed;

    AXI_DMA dma;

    dla MobileNet_dla;

    MobileNet model;

    u8* res_p;

    u32 *param, *fmap, *param2;

    u32 *result, *result_2;

    u8(*result_u8_2w)[32];
    u8(*result_u8_2w_1)[32];
    u8(*result_u8_2w_2)[32];

    param = new u32[264192];

    fmap = new u32[50176];
    result = new u32[100352 + 4];
    result_2 = new u32[100352 + 4];

    result_u8_2w = (u8(*)[32])(result + 4);
    result_u8_2w_1 = (u8(*)[32])(result_2 + 4);
    res_p = (u8*)(result + 4);

    int64_t final_result[43];
    float f[43], q[43];

    int64_t pool_result[1024];

    for(int i = 0; i < 43; i++)
        final_result[i] = 0;

    for(int i = 0; i < 1024; i++)
        pool_result[i] = 0;

    model.init();

    dma.resetDma();

    dma.setup();

    MobileNet_dla.enable();

    MobileNet_dla.ramInvert(true);

    MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, 36);
    while (1)
    {
        // XTime_GetTime(&begin);
        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, t_conv_shift);
        __init_array(280, t_conv_quant, param);
        __init_array(50176, image_orgin, fmap);
        model.conv2d(224, 224, 3, 3, 3, 2, 32, &dma, &MobileNet_dla, param, fmap);

        // model.getResult(112, 112, 32, 64, &dma, &MobileNet_dla, result);
        // for (int j = 0; j < 32; j++)
        // {
        //     cout << (int)result_u8_2w[1][j] << ", ";
        // }
        // cout << endl;

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_0_shift);
        __init_array(32 * 9 / 4 + 32 + 32, dw_conv_0_quant, param);
        model.depthwiseConv2d(112, 112, 3, 3, 32, 1, 32, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_0_shift);
        __init_array(640, pw_conv_0_quant, param);
        model.pointwiseConv2d(112, 112, 32, 64, &dma, &MobileNet_dla, param);

        // model.getResult(112, 112, 32, 64, &dma, &MobileNet_dla, result);
        // for (int j = 0; j < 32; j++)
        // {
        //     cout << (int)result_u8_2w[0][j] << ", ";
        // }
        // cout << endl;

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_1_shift);
        __init_array(64 * 9 / 4 + 64 + 64, dw_conv_1_quant, param);
        model.depthwiseConv2d(112, 112, 3, 3, 64, 2, 64, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_1_shift);
        __init_array(64 * 128 / 4 + 128 + 128, pw_conv_1_quant, param);
        model.pointwiseConv2d(56, 56, 64, 128, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_2_shift);
        __init_array(128 * 9 / 4 + 128 + 128, dw_conv_2_quant, param);
        model.depthwiseConv2d(56, 56, 3, 3, 128, 1, 128, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_2_shift);
        __init_array(128 * 128 / 4 + 128 + 128, pw_conv_2_quant, param);
        model.pointwiseConv2d(56, 56, 128, 128, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_3_shift);
        __init_array(128 * 9 / 4 + 128 + 128, dw_conv_3_quant, param);
        model.depthwiseConv2d(56, 56, 3, 3, 128, 2, 128, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_3_shift);
        __init_array(128 * 256 / 4 + 256 + 256, pw_conv_3_quant, param);
        model.pointwiseConv2d(28, 28, 128, 256, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_4_shift);
        __init_array(256 * 9 / 4 + 256 + 256, dw_conv_4_quant, param);
        model.depthwiseConv2d(28, 28, 3, 3, 256, 1, 256, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_4_shift);
        __init_array(256 * 256 / 4 + 256 + 256, pw_conv_4_quant, param);
        model.pointwiseConv2d(28, 28, 256, 256, &dma, &MobileNet_dla, param);
        
        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_5_shift);
        __init_array(256 * 9 / 4 + 256 + 256, dw_conv_5_quant, param);
        model.depthwiseConv2d(28, 28, 3, 3, 256, 2, 256, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_5_shift);
        __init_array(256 * 512 / 4 + 512 + 512, pw_conv_5_quant, param);
        model.pointwiseConv2d(14, 14, 256, 512, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_6_shift);
        __init_array(512 * 9 / 4 + 512 + 512, dw_conv_6_quant, param);
        model.depthwiseConv2d(14, 14, 3, 3, 512, 1, 512, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_6_shift);
        __init_array(512 * 512 / 4 + 512 + 512, pw_conv_6_quant, param);
        model.pointwiseConv2d(14, 14, 512, 512, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_7_shift);
        __init_array(512 * 9 / 4 + 512 + 512, dw_conv_7_quant, param);
        model.depthwiseConv2d(14, 14, 3, 3, 512, 1, 512, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_7_shift);
        __init_array(512 * 512 / 4 + 512 + 512, pw_conv_7_quant, param);
        model.pointwiseConv2d(14, 14, 512, 512, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_8_shift);
        __init_array(512 * 9 / 4 + 512 + 512, dw_conv_8_quant, param);
        model.depthwiseConv2d(14, 14, 3, 3, 512, 1, 512, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_8_shift);
        __init_array(512 * 512 / 4 + 512 + 512, pw_conv_8_quant, param);
        model.pointwiseConv2d(14, 14, 512, 512, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_9_shift);
        __init_array(512 * 9 / 4 + 512 + 512, dw_conv_9_quant, param);
        model.depthwiseConv2d(14, 14, 3, 3, 512, 1, 512, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_9_shift);
        __init_array(512 * 512 / 4 + 512 + 512, pw_conv_9_quant, param);
        model.pointwiseConv2d(14, 14, 512, 512, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_10_shift);
        __init_array(512 * 9 / 4 + 512 + 512, dw_conv_10_quant, param);
        model.depthwiseConv2d(14, 14, 3, 3, 512, 1, 512, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_10_shift);
        __init_array(512 * 512 / 4 + 512 + 512, pw_conv_10_quant, param);
        model.pointwiseConv2d(14, 14, 512, 512, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_11_shift);
        __init_array(512 * 9 / 4 + 512 + 512, dw_conv_11_quant, param);
        model.depthwiseConv2d(14, 14, 3, 3, 512, 2, 512, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_11_shift);
        __init_array(512 * 1024 / 4 + 1024 + 1024, pw_conv_11_quant, param);
        model.pointwiseConv2d(7, 7, 512, 1024, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, dw_conv_12_shift);
        __init_array(1024 * 9 / 4 + 1024 + 1024, dw_conv_12_quant, param);
        model.depthwiseConv2d(7, 7, 3, 3, 1024, 1, 1024, &dma, &MobileNet_dla, param);

        MobileNet_dla.writeReg(MOBILENET_S00_AXI_SLV_SHIFT_OFFSET, pw_conv_12_shift);
        __init_array(1024 * 1024 / 4 + 1024 + 1024, pw_conv_12_quant, param);
        model.pointwiseConv2d(7, 7, 1024, 1024, &dma, &MobileNet_dla, param);

        model.getResult(7, 7, 1024, 1024, &dma, &MobileNet_dla, result);
        // for (int k = 0; k < 16; k++){
        //     for (int j = 0; j < 32; j++)
        //     {
        //         cout << (int)result_u8_2w[k][j] << ", ";
        //     }
        //     cout << endl;
        // }

        for (int c = 0; c < 1024; c++){
            for (int j = 0; j < 7; j++){
                for (int k = 0; k < 7; k++){
                    pool_result[c] += (uint64_t)result_u8_2w[(c / 32) * 49 + j * 7 + k][c % 32];
                }
            }
            pool_result[c] = pool_result[c] * 1530651995LL >> 34; 
            // cout << pool_result[c] << ", ";
        }
        cout << endl;
        cout << "next" << endl;
        

        model.globalAvgPool2D(7, 7, 1024, &dma, &MobileNet_dla);

        model.getResult(1, 1, 1, 1024, &dma, &MobileNet_dla, result);

        // for(int i = 0; i < 32; i++){
        //     cout << (int)res_p[i] << ", ";
        // }
        // cout << endl;

        for(int j = 0; j < 43; j++){
            for(int k = 0; k < 1024; k++){
                final_result[j] += fc_quant[j][k] * pool_result[k];
            }
            final_result[j] += fc_quant_bias[j];
            final_result[j] *= 1542039082LL;
            final_result[j] >>= 43;

            cout << final_result[j] << endl;
        }

        for (int i = 0; i < 43; i++) {
            f[i] = 0.2866767346858978  * (final_result[i] - 38);
            // cout << f[i] << endl;
        }
        
        
        model.softmax(43, f, q);

        for (int i = 0; i < 43; i++)
            cout << q[i] << ", ";
        cout << endl;
        
        // for (int k = 0; k < 32; k++){
        //     for (int j = 0; j < 32; j++)
        //     {
        //         cout << (int)result_u8_2w[k][j] << ", ";
        //     }
        //     cout << endl;
        // }


        // XTime_GetTime(&end);
        // timeUsed = ((end - begin)) / (666666687.0 / 2.0);

        // cout << "Time elapsed: " << timeUsed * 1000 << "ms" << endl;

    }

    while (1);

#endif

    return 0;
}
