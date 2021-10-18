#include "MobileNet.h"
#include "math.h"
#include "config.h"
#include <iostream>

#if defined __ARM__
#include "xil_io.h"
#endif

#ifndef E
#define E 2.7182818284590452353602874713527
#endif

#define __get_name(x,y,arr) arr[x][y]

#define __init_array(x,source,arr)	\
    for(int i = 0; i < x; i++)		\
        arr[i] = source[i];

#define __init_array_2d(x,y,source,arr) \
    for(int i = 0; i < x; i++)			\
        for(int j = 0; j < y; j++)		\
            arr[i][j] = source[i][j];	

#define __init_array_3d(x,y,z,source,arr)		\
    for(int i = 0; i < x; i++)					\
        for (int j = 0; j < y; j++)				\
            for (int k = 0; k < z; k++)			\
                arr[i][j][k] = source[i][j][k];


#define __allocate_mem(x,arr)\
    arr = new float[x];


#define __allocate_mem_2d(x,y,arr)\
    arr = new float*[x]; \
    for (int i = 0; i < x; i++) \
        arr[i] = new float[y]; 

#define __allocate_mem_3d(x,y,z,arr) \
    arr = new float** [x];	\
    for (int i = 0; i < x; i++) { \
        arr[i] = new float * [y]; \
        for (int j = 0; j < y; j++) \
            arr[i][j] = new float[z]; \
    }

#define __free_mem(arr) \
    delete[] arr;

#define __free_mem_2d(x,arr)	\
    for (int i = 0; i < x; i++)	\
        delete[] arr[i];		\
    delete[] arr;

#define __free_mem_3d(x,y,arr)  \
    for (int i = 0; i < x; i++) { \
        for (int j = 0; j < y; j++) {\
            delete[] arr[i][j];\
        }\
        delete[] arr[i];\
    }\
    delete[] arr;


static float map1[1024][224][224];
static float map2[1024][224][224];
static float globalAvgPoolMap[1024];
static float denseMap[43];

static int i, j, k;

MobileNet::MobileNet(/* args */)
{
}

MobileNet::~MobileNet()
{
}

void MobileNet::conv2d(int f_w, int f_h, int k_w, int k_h, int channel, int stride, std::string padding, int N, float*** k, float* b, float f[3][224][224], float o[1024][224][224])
{
    /**
    ** kernel shape is (N, channel, k_w*k_h)
    ** feature map shape is (channel, x, y)
    ** output map shape is (channel, x, y)
    */
    if (padding == "same")
    {
        // padding
        int f_w_padding = f_w + k_w - stride;
        int f_h_padding = f_h + k_h - stride;

        float local_sum = 0.0;

        for (int n = 0; n < N; n++)
        { // Kernel number
            for (int f_x = 0; f_x < f_w; f_x += stride)
            {
                for (int f_y = 0; f_y < f_h; f_y += stride)
                {
                    local_sum = 0.0;
                    for (int c = 0; c < channel; c++)
                    {
                        for (int k_x = 0; k_x < k_w; k_x++)
                        {
                            for (int k_y = 0; k_y < k_h; k_y++)
                            {
                                if ((f_x + k_x) != -1 && (f_y + k_y) != -1 && (f_x + k_x) < f_w && (f_y + k_y) < f_h)
                                    local_sum += k[n][c][k_x * 3 + k_y] * f[c][f_x + k_x][f_y + k_y];
                                else
                                    local_sum += 0.0; // padding add zero
                            }
                        }
                    }
                    local_sum += b[n];
                    // o[n][f_x >> 1][f_y >> 1] = (local_sum > 0) ? local_sum : 0;
                    o[n][f_x >> 1][f_y >> 1] = local_sum;
                }
            }
        }
    }
    else
    {
        return;
    }
}

void MobileNet::conv2d(int f_w, int f_h, int k_w, int k_h, int channel, int stride, std::string padding, int N, float f[3][224][224], float o[1024][224][224])
{
    /**
    ** kernel shape is (N, channel, k_w*k_h)
    ** feature map shape is (channel, x, y)
    ** output map shape is (channel, x, y)
    */
    if (padding == "same")
    {
        // padding
        int f_w_padding = f_w + k_w - stride;
        int f_h_padding = f_h + k_h - stride;

        float local_sum = 0.0;

        for (int n = 0; n < N; n++)
        { // Kernel number
            for (int f_x = 0; f_x < f_w; f_x += stride)
            {
                for (int f_y = 0; f_y < f_h; f_y += stride)
                {
                    local_sum = 0.0;
                    for (int c = 0; c < channel; c++)
                    {
                        for (int k_x = 0; k_x < k_w; k_x++)
                        {
                            for (int k_y = 0; k_y < k_h; k_y++)
                            {
                                if ((f_x + k_x) != -1 && (f_y + k_y) != -1 && (f_x + k_x) < f_w && (f_y + k_y) < f_h)
                                    local_sum += this->layer_1_weight[n][c][k_x * 3 + k_y] * f[c][f_x + k_x][f_y + k_y];
                                else
                                    local_sum += 0.0; // padding add zero
                            }
                        }
                    }
                    local_sum += this->layer_1_bias[n];
                    // o[n][f_x >> 1][f_y >> 1] = (local_sum > 0) ? local_sum : 0;
                    o[n][f_x >> 1][f_y >> 1] = local_sum;
                }
            }
        }
    }
    else
    {
        return;
    }
}

void MobileNet::depthwiseConv2d(int f_w, int f_h, int k_w, int k_h, int channel, int stride, std::string padding, float*** k, float* b, float f[1024][224][224], float o[1024][224][224])
{
    /**
    ** kernel shape is (channel, k_w, k_h)
    ** feature map shape is (channel, x, y)
    ** output map shape is (channel, x, y)
    */

    float local_sum = 0.0;

    if (padding == "same")
    {
        if (stride == 1)
        {
            // padding
            /*f_w += 2;
            f_h += 2;*/

            for (int c = 0; c < channel; c++)
            {
                for (int f_x = 0; f_x < f_w; f_x += stride)
                {
                    for (int f_y = 0; f_y < f_h; f_y += stride)
                    {
                        local_sum = 0.0;
                        for (int k_x = 0; k_x < k_w; k_x++)
                        {
                            for (int k_y = 0; k_y < k_h; k_y++)
                            {
                                if ((f_x - 1 + k_x) != -1 && (f_y - 1 + k_y) != -1 && (f_x - 1 + k_x) < f_w && (f_y - 1 + k_y) < f_h)
                                    local_sum += k[c][k_x][k_y] * f[c][f_x - 1 + k_x][f_y - 1 + k_y];
                                else
                                    local_sum += 0.0; // padding zero
                            }
                        }
                        local_sum += b[c];
                        o[c][f_x][f_y] = local_sum;
                        // o[c][f_x][f_y] = (local_sum > 0) ? local_sum : 0;
                    }
                }
            }
        }
        else if (stride == 2)
        {
            // padding
            /*f_w += 1;
            f_h += 1;*/

            for (int c = 0; c < channel; c++)
            {
                for (int f_x = 0; f_x < f_w; f_x += stride)
                {
                    for (int f_y = 0; f_y < f_h; f_y += stride)
                    {
                        local_sum = 0.0;
                        for (int k_x = 0; k_x < k_w; k_x++)
                        {
                            for (int k_y = 0; k_y < k_h; k_y++)
                            {
                                if ((f_x + k_x) != -1 && (f_y + k_y) != -1 && (f_x + k_x) < f_w && (f_y + k_y) < f_h)
                                    local_sum += k[c][k_x][k_y] * f[c][f_x + k_x][f_y + k_y];
                                else
                                    local_sum += 0.0; // padding zero
                            }
                        }
                        local_sum += b[c];
                        o[c][f_x >> 1][f_y >> 1] = local_sum;
                        // o[c][f_x][f_y] = (local_sum > 0) ? local_sum : 0;
                    }
                }
            }
        }

        

        
    }
    else if (padding == "valid")
    {
        return;
    }
}

void MobileNet::depthwiseConv2d(int f_w, int f_h, int k_w, int k_h, int channel, int stride, std::string padding, float f[1024][224][224], float o[1024][224][224])
{
    /**
    ** kernel shape is (channel, k_w, k_h)
    ** feature map shape is (channel, x, y)
    ** output map shape is (channel, x, y)
    */

    float local_sum = 0.0;

    if (padding == "same")
    {
        if (stride == 1)
        {
            // padding
            /*f_w += 2;
            f_h += 2;*/

            for (int c = 0; c < channel; c++)
            {
                for (int f_x = 0; f_x < f_w; f_x += stride)
                {
                    for (int f_y = 0; f_y < f_h; f_y += stride)
                    {
                        local_sum = 0.0;
                        for (int k_x = 0; k_x < k_w; k_x++)
                        {
                            for (int k_y = 0; k_y < k_h; k_y++)
                            {
                                if ((f_x - 1 + k_x) != -1 && (f_y - 1 + k_y) != -1 && (f_x - 1 + k_x) < f_w && (f_y - 1 + k_y) < f_h)
                                    local_sum += this->dw_weight[c][k_x][k_y] * f[c][f_x - 1 + k_x][f_y - 1 + k_y];
                                else
                                    local_sum += 0.0; // padding zero
                            }
                        }
                        local_sum += dw_bias[c];
                        o[c][f_x][f_y] = local_sum;
                        // o[c][f_x][f_y] = (local_sum > 0) ? local_sum : 0;
                    }
                }
            }
        }
        else if (stride == 2)
        {
            // padding
            /*f_w += 1;
            f_h += 1;*/

            for (int c = 0; c < channel; c++)
            {
                for (int f_x = 0; f_x < f_w; f_x += stride)
                {
                    for (int f_y = 0; f_y < f_h; f_y += stride)
                    {
                        local_sum = 0.0;
                        for (int k_x = 0; k_x < k_w; k_x++)
                        {
                            for (int k_y = 0; k_y < k_h; k_y++)
                            {
                                if ((f_x + k_x) != -1 && (f_y + k_y) != -1 && (f_x + k_x) < f_w && (f_y + k_y) < f_h)
                                    local_sum += this->dw_weight[c][k_x][k_y] * f[c][f_x + k_x][f_y + k_y];
                                else
                                    local_sum += 0.0; // padding zero
                            }
                        }
                        local_sum += this->dw_bias[c];
                        o[c][f_x >> 1][f_y >> 1] = local_sum;
                        // o[c][f_x][f_y] = (local_sum > 0) ? local_sum : 0;
                    }
                }
            }
        }




    }
    else if (padding == "valid")
    {
        return;
    }
}

void MobileNet::pointwiseConv2d(int f_w, int f_h, int channel, int N, float** k, float* b, float f[1024][224][224], float o[1024][224][224])
{
    /**
    ** kernel shape is (N, channel)
    ** feature map shape is (channel, x, y)
    ** output map shape is (channel, x, y)
    */

    float local_sum = 0.0;

    for (int n = 0; n < N; n++)
    {
        for (int f_x = 0; f_x < f_w; f_x++)
        {
            for (int f_y = 0; f_y < f_h; f_y++)
            {
                local_sum = 0.0;
                for (int c = 0; c < channel; c++)
                {
                    local_sum += k[n][c] * f[c][f_x][f_y];
                }
                local_sum += b[n];
                o[n][f_x][f_y] = local_sum;
                // o[n][f_x][f_y] = (local_sum > 0) ? local_sum : 0;
            }
        }
    }

    return;
}

void MobileNet::pointwiseConv2d(int f_w, int f_h, int channel, int N, float f[1024][224][224], float o[1024][224][224])
{
    /**
    ** kernel shape is (N, channel)
    ** feature map shape is (channel, x, y)
    ** output map shape is (channel, x, y)
    */

    float local_sum = 0.0;

    for (int n = 0; n < N; n++)
    {
        for (int f_x = 0; f_x < f_w; f_x++)
        {
            for (int f_y = 0; f_y < f_h; f_y++)
            {
                local_sum = 0.0;
                for (int c = 0; c < channel; c++)
                {
                    local_sum += this->conv2d_weight[n][c] * f[c][f_x][f_y];
                }
                local_sum += this->conv2d_bias[n];
                o[n][f_x][f_y] = local_sum;
                // o[n][f_x][f_y] = (local_sum > 0) ? local_sum : 0;
            }
        }
    }

    return;
}

void MobileNet::batchNormalization(int f_w, int f_h, int channel, float f[1024][224][224], float* gamma, float* beta, float* mean, float* variance, float r = 0.003) {
    for (int c = 0; c < channel; c++) {
        for (int x = 0; x < f_w; x++) {
            for (int y = 0; y < f_h; y++) {
                f[c][x][y] = gamma[c] * ((f[c][x][y] - mean[c]) / sqrt(variance[c] + r)) + beta[c];
            }
        }
    }
}

void MobileNet::batchNormalization(int f_w, int f_h, int channel, float f[1024][224][224], BatchNormalization BN) {
    /**
    * BN includes 4 parts:
    *       BN.channel : int
    *       BN.gamma[] : float*
    *       BN.beta[]  : float*
    *       BN.mean[]  : float*
    *       BN.variance[] : float*
    */

    for (int c = 0; c < channel; c++) {
        for (int x = 0; x < f_w; x++) {
            for (int y = 0; y < f_h; y++) {
                f[c][x][y] = BN.gamma[c] * ((f[c][x][y] - BN.mean[c]) / sqrt(BN.variance[c] + BN.r)) + BN.beta[c];
            }
        }
    }
}

void MobileNet::relu(int f_w, int f_h, int channel, float f[1024][224][224]) {
    for (int c = 0; c < channel; c++)
        for (int w = 0; w < f_w; w++)
            for (int h = 0; h < f_h; h++)
                f[c][w][h] = (f[c][w][h] < 0) ? 0.0 : f[c][w][h];	// ReLU
}

void MobileNet::globalAvgPool2D(int f_w, int f_h, int channel, float f[1024][224][224], float o[])
{
    /**
    ** feature map shape is (channel, x, y)
    ** output map shape is (channel, )
    */

    float local_sum = 0.0;

    for (int c = 0; c < channel; c++)
    {
        local_sum = 0.0;
        for (int f_x = 0; f_x < f_w; f_x++)
        {
            for (int f_y = 0; f_y < f_h; f_y++)
            {
                local_sum += f[c][f_x][f_y];
            }
        }
        o[c] = local_sum / (f_w * f_h);
    }

    return;
}

void MobileNet::dense(int l_i, int l_o, float** k, float* b, float i[], float o[]) {
    /**
    ** kernel shape is (l_i, l_o)
    ** feature map shape is (l_i, )
    ** output map shape is (l_o, )
    */

    float local_sum = 0.0;

    for (int c = 0; c < l_o; c++) {
        local_sum = 0.0;
        for (int i_c = 0; i_c < l_i; i_c++) {
            local_sum += i[i_c] * k[c][i_c];
        }
        local_sum += b[c];
        o[c] = local_sum;
    }

    return;
}

void MobileNet::dense(int l_i, int l_o, float i[], float o[]) {
    /**
    ** kernel shape is (l_i, l_o)
    ** feature map shape is (l_i, )
    ** output map shape is (l_o, )
    */

    float local_sum = 0.0;

    for (int c = 0; c < l_o; c++) {
        local_sum = 0.0;
        for (int i_c = 0; i_c < l_i; i_c++) {
            local_sum += i[i_c] * this->dense_weight[c][i_c];
        }
        local_sum += this->dense_bias[c];
        o[c] = local_sum;
    }

    return;
}

void MobileNet::softmax(int l, float i[], float o[]) {
    double sum = 0.0;
    double t_f = 0.0;

    float* t = new float[l];

    for (int pos = 0; pos < l; pos++) {
        t_f = pow(E, i[pos]);
        sum += t_f;
        t[pos] = t_f;
    }

    for (int pos = 0; pos < l; pos++) {
        o[pos] = t[pos] / sum;
    }

    delete[] t;
}

void MobileNet::invoke(int w, int h, int channel, float image[3][224][224], float result[43])
{
    if (this->hasBeenInit == false) {
        throw std::string("The net is not initialized yet, please call the function \"init()\" first.");
    }

    if (w != 224 || h != 224 || channel != 3) {
        throw std::string("The image should as the shape (3, 224, 224) which means (channel, w, h)");
    }

    // Start the Invoke process:
    // 
    
    // std::cout << "proceed 1" << std::endl;

    //__allocate_mem_3d(32, 3, 9, this->conv2d_weight);
    //__allocate_mem(32, this->conv2d_bias);
    __init_array_3d(32, 3, 9, quant_conv2d_weight, this->layer_1_weight);
    __init_array(32, quant_conv2d_bias, this->layer_1_bias);
    BN.init(32, quant_batch_normalization_parameters);
    this->conv2d(224, 224, 3, 3, 3, 2, "same", 32, image, map1);
    this->batchNormalization(112, 112, 32, map1, this->BN);
    this->relu(112, 112, 32, map1);
    //__free_mem_3d(32, 3, this->conv2d_weight);
    //__free_mem(this->conv2d_bias);

    // std::cout << "proceed 2" << std::endl;
    //__allocate_mem_3d(32, 3, 3, this->depthwise_conv2d_weight);
    //__allocate_mem(32, this->depthwise_conv2d_bias);
    //__allocate_mem_2d(64, 32, this->conv2d_1_weight);
    //__allocate_mem(64, this->conv2d_1_bias);
    __init_array_3d(32, 3, 3, quant_depthwise_conv2d_weight, this->dw_weight);
    __init_array(32, quant_depthwise_conv2d_bias, this->dw_bias);
    __init_array_2d(64, 32, quant_conv2d_1_weight, this->conv2d_weight);
    __init_array(64, quant_conv2d_1_bias, this->conv2d_bias);
    BN.init(32, quant_batch_normalization_1_parameters);
    this->depthwiseConv2d(112, 112, 3, 3, 32, 1, "same", map1, map2);
    this->batchNormalization(112, 112, 32, map2, this->BN);
    this->relu(112, 112, 32, map2);
    BN.init(64, quant_batch_normalization_2_parameters);
    this->pointwiseConv2d(112, 112, 32, 64, map2, map1);
    this->batchNormalization(112, 112, 64, map1, this->BN);
    this->relu(112, 112, 64, map1);
    //__free_mem_3d(32, 3, this->depthwise_conv2d_weight);
    //__free_mem(this->depthwise_conv2d_bias);
    //__free_mem_2d(64, this->conv2d_1_weight);
    //__free_mem(this->conv2d_1_bias);

    // std::cout << "proceed 3" << std::endl;
    //__allocate_mem_3d(64, 3, 3, this->depthwise_conv2d_1_weight);
    //__allocate_mem(64, this->depthwise_conv2d_1_bias);
    //__allocate_mem_2d(128, 64, this->conv2d_2_weight);
    //__allocate_mem(128, this->conv2d_2_bias);
    __init_array_3d(64, 3, 3, quant_depthwise_conv2d_1_weight, this->dw_weight);
    __init_array(64, quant_depthwise_conv2d_1_bias, this->dw_bias);
    __init_array_2d(128, 64, quant_conv2d_2_weight, this->conv2d_weight);
    __init_array(128, quant_conv2d_2_bias, this->conv2d_bias);
    BN.init(64, quant_batch_normalization_3_parameters);
    this->depthwiseConv2d(112, 112, 3, 3, 64, 2, "same", map1, map2);
    this->batchNormalization(56, 56, 64, map2, this->BN);
    this->relu(56, 56, 64, map2);
    BN.init(128, quant_batch_normalization_4_parameters);
    this->pointwiseConv2d(56, 56, 64, 128, map2, map1);
    this->batchNormalization(56, 56, 128, map1, this->BN);
    this->relu(56, 56, 128, map1);
    //__free_mem_3d(64, 3, this->depthwise_conv2d_1_weight);
    //__free_mem(this->depthwise_conv2d_1_bias);
    //__free_mem_2d(128, this->conv2d_2_weight);
    //__free_mem(this->conv2d_2_bias);

    // std::cout << "proceed 4" << std::endl;
    //__allocate_mem_3d(128, 3, 3, this->depthwise_conv2d_2_weight);
    //__allocate_mem(128, this->depthwise_conv2d_2_bias);
    //__allocate_mem_2d(128, 128, this->conv2d_3_weight);
    //__allocate_mem(128, this->conv2d_3_bias);
    __init_array_3d(128, 3, 3, quant_depthwise_conv2d_2_weight, this->dw_weight);
    __init_array(128, quant_depthwise_conv2d_2_bias, this->dw_bias);
    __init_array_2d(128, 128, quant_conv2d_3_weight, this->conv2d_weight);
    __init_array(128, quant_conv2d_3_bias, this->conv2d_bias);
    BN.init(128, quant_batch_normalization_5_parameters);
    this->depthwiseConv2d(56, 56, 3, 3, 128, 1, "same", map1, map2);
    this->batchNormalization(56, 56, 128, map2, this->BN);
    this->relu(56, 56, 128, map2);
    BN.init(128, quant_batch_normalization_6_parameters);
    this->pointwiseConv2d(56, 56, 128, 128, map2, map1);
    this->batchNormalization(56, 56, 128, map1, this->BN);
    this->relu(56, 56, 128, map1);
    //__free_mem_3d(128, 3, this->depthwise_conv2d_2_weight);
    //__free_mem(this->depthwise_conv2d_2_bias);
    //__free_mem_2d(128, this->conv2d_3_weight);
    //__free_mem(this->conv2d_3_bias);

    // std::cout << "proceed 5" << std::endl;
    //__allocate_mem_3d(128, 3, 3, this->depthwise_conv2d_3_weight);
    //__allocate_mem(128, this->depthwise_conv2d_3_bias);
    //__allocate_mem_2d(256, 128, this->conv2d_4_weight);
    //__allocate_mem(256, this->conv2d_4_bias);
    __init_array_3d(128, 3, 3, quant_depthwise_conv2d_3_weight, this->dw_weight);
    __init_array(128, quant_depthwise_conv2d_3_bias, this->dw_bias);
    __init_array_2d(256, 128, quant_conv2d_4_weight, this->conv2d_weight);
    __init_array(256, quant_conv2d_4_bias, this->conv2d_bias);
    BN.init(128, quant_batch_normalization_7_parameters);
    this->depthwiseConv2d(56, 56, 3, 3, 128, 2, "same", map1, map2);
    this->batchNormalization(28, 28, 128, map2, this->BN);
    this->relu(28, 28, 128, map2);
    BN.init(256, quant_batch_normalization_8_parameters);
    this->pointwiseConv2d(28, 28, 128, 256, map2, map1);
    this->batchNormalization(28, 28, 256, map1, this->BN);
    this->relu(28, 28, 256, map1);
    //__free_mem_3d(128, 3, this->depthwise_conv2d_3_weight);
    //__free_mem(this->depthwise_conv2d_3_bias);
    //__free_mem_2d(256, this->conv2d_4_weight);
    //__free_mem(this->conv2d_4_bias);

    // std::cout << "proceed 6" << std::endl;
    //__allocate_mem_3d(256, 3, 3, this->depthwise_conv2d_4_weight);
    //__allocate_mem(256, this->depthwise_conv2d_4_bias);
    //__allocate_mem_2d(256, 256, this->conv2d_5_weight);
    //__allocate_mem(256, this->conv2d_5_bias);
    __init_array_3d(256, 3, 3, quant_depthwise_conv2d_4_weight, this->dw_weight);
    __init_array(256, quant_depthwise_conv2d_4_bias, this->dw_bias);
    __init_array_2d(256, 256, quant_conv2d_5_weight, this->conv2d_weight);
    __init_array(256, quant_conv2d_5_bias, this->conv2d_bias);
    BN.init(256, quant_batch_normalization_9_parameters);
    this->depthwiseConv2d(28, 28, 3, 3, 256, 1, "same", map1, map2);
    this->batchNormalization(28, 28, 256, map2, this->BN);
    this->relu(28, 28, 256, map2);
    BN.init(256, quant_batch_normalization_10_parameters);
    this->pointwiseConv2d(28, 28, 256, 256, map2, map1);
    this->batchNormalization(28, 28, 256, map1, this->BN);
    this->relu(28, 28, 256, map1);
    //__free_mem_3d(256, 3, this->depthwise_conv2d_4_weight);
    //__free_mem(this->depthwise_conv2d_4_bias);
    //__free_mem_2d(256, this->conv2d_5_weight);
    //__free_mem(this->conv2d_5_bias);

    // std::cout << "proceed 7" << std::endl;
    //__allocate_mem_3d(256, 3, 3, this->depthwise_conv2d_5_weight);
    //__allocate_mem(256, this->depthwise_conv2d_5_bias);
    //__allocate_mem_2d(512, 256, this->conv2d_6_weight);
    //__allocate_mem(512, this->conv2d_6_bias);
    __init_array_3d(256, 3, 3, quant_depthwise_conv2d_5_weight, this->dw_weight);
    __init_array(256, quant_depthwise_conv2d_5_bias, this->dw_bias);
    __init_array_2d(512, 256, quant_conv2d_6_weight, this->conv2d_weight);
    __init_array(512, quant_conv2d_6_bias, this->conv2d_bias);
    BN.init(256, quant_batch_normalization_11_parameters);
    this->depthwiseConv2d(28, 28, 3, 3, 256, 2, "same", map1, map2);
    this->batchNormalization(14, 14, 256, map2, this->BN);
    this->relu(14, 14, 256, map2);
    BN.init(512, quant_batch_normalization_12_parameters);
    this->pointwiseConv2d(14, 14, 256, 512, map2, map1);
    this->batchNormalization(14, 14, 512, map1, this->BN);
    this->relu(14, 14, 512, map1);
    //__free_mem_3d(256, 3, this->depthwise_conv2d_5_weight);
    //__free_mem(this->depthwise_conv2d_5_bias);
    //__free_mem_2d(512, this->conv2d_6_weight);
    //__free_mem(this->conv2d_6_bias);

    // std::cout << "proceed 8" << std::endl;
    //__allocate_mem_3d(512, 3, 3, this->depthwise_conv2d_6_weight);
    //__allocate_mem(512, this->depthwise_conv2d_6_bias);
    //__allocate_mem_2d(512, 512, this->conv2d_7_weight);
    //__allocate_mem(512, this->conv2d_7_bias);
    __init_array_3d(512, 3, 3, quant_depthwise_conv2d_6_weight, this->dw_weight);
    __init_array(512, quant_depthwise_conv2d_6_bias, this->dw_bias);
    __init_array_2d(512, 512, quant_conv2d_7_weight, this->conv2d_weight);
    __init_array(512, quant_conv2d_7_bias, this->conv2d_bias);
    BN.init(512, quant_batch_normalization_13_parameters);
    this->depthwiseConv2d(14, 14, 3, 3, 512, 1, "same", map1, map2);
    this->batchNormalization(14, 14, 512, map2, this->BN);
    this->relu(14, 14, 512, map2);
    BN.init(512, quant_batch_normalization_14_parameters);
    this->pointwiseConv2d(14, 14, 512, 512, map2, map1);
    this->batchNormalization(14, 14, 512, map1, this->BN);
    this->relu(14, 14, 512, map1);
    //__free_mem_3d(512, 3, this->depthwise_conv2d_6_weight);
    //__free_mem(this->depthwise_conv2d_6_bias);
    //__free_mem_2d(512, this->conv2d_7_weight);
    //__free_mem(this->conv2d_7_bias);

    // std::cout << "proceed 9" << std::endl;
    //__allocate_mem_3d(512, 3, 3, this->depthwise_conv2d_7_weight);
    //__allocate_mem(512, this->depthwise_conv2d_7_bias);
    //__allocate_mem_2d(512, 512, this->conv2d_8_weight);
    //__allocate_mem(512, this->conv2d_8_bias);
    __init_array_3d(512, 3, 3, quant_depthwise_conv2d_7_weight, this->dw_weight);
    __init_array(512, quant_depthwise_conv2d_7_bias, this->dw_bias);
    __init_array_2d(512, 512, quant_conv2d_8_weight, this->conv2d_weight);
    __init_array(512, quant_conv2d_8_bias, this->conv2d_bias);
    BN.init(512, quant_batch_normalization_15_parameters);
    this->depthwiseConv2d(14, 14, 3, 3, 512, 1, "same", map1, map2);
    this->batchNormalization(14, 14, 512, map2, this->BN);
    this->relu(14, 14, 512, map2);
    BN.init(512, quant_batch_normalization_16_parameters);
    this->pointwiseConv2d(14, 14, 512, 512, map2, map1);
    this->batchNormalization(14, 14, 512, map1, this->BN);
    this->relu(14, 14, 512, map1);
    //__free_mem_3d(512, 3, this->depthwise_conv2d_7_weight);
    //__free_mem(this->depthwise_conv2d_7_bias);
    //__free_mem_2d(512, this->conv2d_8_weight);
    //__free_mem(this->conv2d_8_bias);

    // std::cout << "proceed 10" << std::endl;
    //__allocate_mem_3d(512, 3, 3, this->depthwise_conv2d_8_weight);
    //__allocate_mem(512, this->depthwise_conv2d_8_bias);
    //__allocate_mem_2d(512, 512, this->conv2d_9_weight);
    //__allocate_mem(512, this->conv2d_9_bias);
    __init_array_3d(512, 3, 3, quant_depthwise_conv2d_8_weight, this->dw_weight);
    __init_array(512, quant_depthwise_conv2d_8_bias, this->dw_bias);
    __init_array_2d(512, 512, quant_conv2d_9_weight, this->conv2d_weight);
    __init_array(512, quant_conv2d_9_bias, this->conv2d_bias);
    BN.init(512, quant_batch_normalization_17_parameters);
    this->depthwiseConv2d(14, 14, 3, 3, 512, 1, "same", map1, map2);
    this->batchNormalization(14, 14, 512, map2, this->BN);
    this->relu(14, 14, 512, map2);
    BN.init(512, quant_batch_normalization_18_parameters);
    this->pointwiseConv2d(14, 14, 512, 512, map2, map1);
    this->batchNormalization(14, 14, 512, map1, this->BN);
    this->relu(14, 14, 512, map1);
    //__free_mem_3d(512, 3, this->depthwise_conv2d_8_weight);
    //__free_mem(this->depthwise_conv2d_8_bias);
    //__free_mem_2d(512, this->conv2d_9_weight);
    //__free_mem(this->conv2d_9_bias);

    // std::cout << "proceed 11" << std::endl;
    //__allocate_mem_3d(512, 3, 3, this->depthwise_conv2d_9_weight);
    //__allocate_mem(512, this->depthwise_conv2d_9_bias);
    //__allocate_mem_2d(512, 512, this->conv2d_10_weight);
    //__allocate_mem(512, this->conv2d_10_bias);
    __init_array_3d(512, 3, 3, quant_depthwise_conv2d_9_weight, this->dw_weight);
    __init_array(512, quant_depthwise_conv2d_9_bias, this->dw_bias);
    __init_array_2d(512, 512, quant_conv2d_10_weight, this->conv2d_weight);
    __init_array(512, quant_conv2d_10_bias, this->conv2d_bias);
    BN.init(512, quant_batch_normalization_19_parameters);
    this->depthwiseConv2d(14, 14, 3, 3, 512, 1, "same", map1, map2);
    this->batchNormalization(14, 14, 512, map2, this->BN);
    this->relu(14, 14, 512, map2);
    BN.init(512, quant_batch_normalization_20_parameters);
    this->pointwiseConv2d(14, 14, 512, 512, map2, map1);
    this->batchNormalization(14, 14, 512, map1, this->BN);
    this->relu(14, 14, 512, map1);
    //__free_mem_3d(512, 3, this->depthwise_conv2d_9_weight);
    //__free_mem(this->depthwise_conv2d_9_bias);
    //__free_mem_2d(512, this->conv2d_10_weight);
    //__free_mem(this->conv2d_10_bias);

    // std::cout << "proceed 12" << std::endl;
    //__allocate_mem_3d(512, 3, 3, this->depthwise_conv2d_10_weight);
    //__allocate_mem(512, this->depthwise_conv2d_10_bias);
    //__allocate_mem_2d(512, 512, this->conv2d_11_weight);
    //__allocate_mem(512, this->conv2d_11_bias);
    __init_array_3d(512, 3, 3, quant_depthwise_conv2d_10_weight, this->dw_weight);
    __init_array(512, quant_depthwise_conv2d_10_bias, this->dw_bias);
    __init_array_2d(512, 512, quant_conv2d_11_weight, this->conv2d_weight);
    __init_array(512, quant_conv2d_11_bias, this->conv2d_bias);
    BN.init(512, quant_batch_normalization_21_parameters);
    this->depthwiseConv2d(14, 14, 3, 3, 512, 1, "same", map1, map2);
    this->batchNormalization(14, 14, 512, map2, this->BN);
    this->relu(14, 14, 512, map2);
    BN.init(512, quant_batch_normalization_22_parameters);
    this->pointwiseConv2d(14, 14, 512, 512, map2, map1);
    this->batchNormalization(14, 14, 512, map1, this->BN);
    this->relu(14, 14, 512, map1);
    //__free_mem_3d(512, 3, this->depthwise_conv2d_10_weight);
    //__free_mem(this->depthwise_conv2d_10_bias);
    //__free_mem_2d(512, this->conv2d_11_weight);
    //__free_mem(this->conv2d_11_bias);

    // std::cout << "proceed 13" << std::endl;
    //__allocate_mem_3d(512, 3, 3, this->depthwise_conv2d_11_weight);
    //__allocate_mem(512, this->depthwise_conv2d_11_bias);
    //__allocate_mem_2d(1024, 512, this->conv2d_12_weight);
    //__allocate_mem(1024, this->conv2d_12_bias);
    __init_array_3d(512, 3, 3, quant_depthwise_conv2d_11_weight, this->dw_weight);
    __init_array(512, quant_depthwise_conv2d_11_bias, this->dw_bias);
    __init_array_2d(1024, 512, quant_conv2d_12_weight, this->conv2d_weight);
    __init_array(1024, quant_conv2d_12_bias, this->conv2d_bias);
    BN.init(512, quant_batch_normalization_23_parameters);
    this->depthwiseConv2d(14, 14, 3, 3, 512, 2, "same", map1, map2);
    this->batchNormalization(7, 7, 512, map2, this->BN);
    this->relu(7, 7, 512, map2);
    BN.init(1024, quant_batch_normalization_24_parameters);
    this->pointwiseConv2d(7, 7, 512, 1024, map2, map1);
    this->batchNormalization(7, 7, 1024, map1, this->BN);
    this->relu(7, 7, 1024, map1);
    //__free_mem_3d(512, 3, this->depthwise_conv2d_11_weight);
    //__free_mem(this->depthwise_conv2d_11_bias);
    //__free_mem_2d(1024, this->conv2d_12_weight);
    //__free_mem(this->conv2d_12_bias);

    // std::cout << "proceed 14" << std::endl;
    //__allocate_mem_3d(1024, 3, 3, this->depthwise_conv2d_12_weight);
    //__allocate_mem(1024, this->depthwise_conv2d_12_bias);
    //__allocate_mem_2d(1024, 1024, this->conv2d_13_weight);
    //__allocate_mem(1024, this->conv2d_13_bias);
    __init_array_3d(1024, 3, 3, quant_depthwise_conv2d_12_weight, this->dw_weight);
    __init_array(1024, quant_depthwise_conv2d_12_bias, this->dw_bias);
    __init_array_2d(1024, 1024, quant_conv2d_13_weight, this->conv2d_weight);
    __init_array(1024, quant_conv2d_13_bias, this->conv2d_bias);
    BN.init(1024, quant_batch_normalization_25_parameters);
    this->depthwiseConv2d(7, 7, 3, 3, 1024, 1, "same", map1, map2);
    this->batchNormalization(7, 7, 1024, map2, this->BN);
    this->relu(7, 7, 1024, map2);
    BN.init(1024, quant_batch_normalization_26_parameters);
    this->pointwiseConv2d(7, 7, 1024, 1024, map2, map1);
    this->batchNormalization(7, 7, 1024, map1, this->BN);
    this->relu(7, 7, 1024, map1);
    //__free_mem_3d(1024, 3, this->depthwise_conv2d_12_weight);
    //__free_mem(this->depthwise_conv2d_12_bias);
    //__free_mem_2d(1024, this->conv2d_13_weight);
    //__free_mem(this->conv2d_13_bias);

    // std::cout << "avg pool" << std::endl;
    this->globalAvgPool2D(7, 7, 1024, map1, globalAvgPoolMap);

    // std::cout << "dense" << std::endl;
    //__allocate_mem_2d(43, 1024, this->dense_weight);
    //__allocate_mem(43, this->dense_bias);
    __init_array_2d(43, 1024, quant_dense_weight, this->dense_weight);
    __init_array(43, quant_dense_bias, this->dense_bias);
    this->dense(1024, 43, globalAvgPoolMap, denseMap);
    //__free_mem_2d(43, this->dense_weight);
    //__free_mem(this->dense_bias);

    // std::cout << "softmax" << std::endl;
    this->softmax(43, denseMap, result);

}

void MobileNet::init()
{
    /**
    map1[1024][224][224];
    map2[1024][224][224];
    globalAvgPoolMap[1024];
    denseMap[43];
    */
    for (i = 0; i < 1024; i++) {
        for (j = 0; j < 224; j++) {
            for (k = 0; k < 224; k++) {
                map1[i][j][k] = 0.0;
                map2[i][j][k] = 0.0;
            }
        }

        globalAvgPoolMap[i] = 0.0;
    }

    for (i = 0; i < 43; i++) {
        denseMap[i] = 0.0;
    }

    std::cout << "Init done!" << std::endl;

    this->hasBeenInit = true;
}

#if defined __ARM__

void MobileNet::writeReg(int offset, int data){
    Xil_Out32((MOBILENET_BASEADDR + offset), (unsigned int)data);
}

int MobileNet::readReg(int offset){
    return Xil_In32(MOBILENET_BASEADDR + offset);
}

#endif
