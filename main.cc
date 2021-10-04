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
#if defined __ARM__
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
}
