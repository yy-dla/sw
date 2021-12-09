#ifndef __MOBILENET_H__
#define __MOBILENET_H__

#include <string>
#include "BatchNormalization.h"

#if defined __ARM__
#include "xil_types.h"
#include "xstatus.h"
#include "AXI_DMA/axi_dma.h"
#include "dla/dla.h"

#endif

class MobileNet
{
private:

    bool hasBeenInit = false;

public:

    float layer_1_weight[32][3][9];
    float layer_1_bias[32];

    float dw_weight[1024][3][3];
    float dw_bias[1024];

    float conv2d_weight[1024][1024];
    float conv2d_bias[1024];

    float dense_weight[43][1024];
    float dense_bias[43];

    BatchNormalization BN;

    MobileNet();
    ~MobileNet();

    /**
     * @brief Used to compute the traditional conv.
     * 
     * @param f_w feature map width
     * @param f_h feature map height
     * @param k_w kernel width
     * @param k_h kernel height
     * @param channel channel size
     * @param stride 
     * @param padding "same" or "valid"
     * @param N amount of kernel
     * @param k[][][] kernel array
     * @param b[] bias array
     * @param f[][][] input feature map
     * @param o[][][] map after compute
    */
    void conv2d(int f_w, int f_h, int k_w, int k_h, int channel, int stride, std::string padding, int N, float*** k, float* b, float f[3][224][224], float o[1024][224][224]);
    void conv2d(int f_w, int f_h, int k_w, int k_h, int channel, int stride, std::string padding, int N, float f[3][224][224], float o[1024][224][224]);
#if defined __ARM__
    void conv2d(int f_w, int f_h, int k_w, int k_h, int channel, int stride, int N, 
        AXI_DMA *DMA, 
        dla *dla, 
        u32* param_addr,
        u32* fmap_addr,
        u32* result_addr
    );

    void conv2d(int f_w, int f_h, int k_w, int k_h, int channel, int stride, int N,
        AXI_DMA *DMA,
        dla *dla,
        u32* param_addr,
        u32* fmap_addr    
    );
#endif

    /**
     * @brief Used to compute depthwise Conv.
     * 
     * @param f_w feature map width
     * @param f_h feature map height
     * @param k_w kernel width
     * @param k_h kernel height
     * @param channel channel size
     * @param stride 
     * @param padding "same" or "valid"
     * @param k[][][] kernel array
     * @param b[] bias array
     * @param f[][][] input feature map
     * @param o[][][] map after compute
    */
    void depthwiseConv2d(int f_w, int f_h, int k_w, int k_h, int channel, int stride, std::string padding, float*** k, float* b, float f[1024][224][224], float o[1024][224][224]);
    void depthwiseConv2d(int f_w, int f_h, int k_w, int k_h, int channel, int stride, std::string padding, float f[1024][224][224], float o[1024][224][224]);
#if defined __ARM__
    void depthwiseConv2d(int f_w, int f_h, int k_w, int k_h, int channel, int stride, int N, 
        AXI_DMA *DMA, dla *dla, 
        u32* param_addr,
        u32* fmap_addr,
        u32* result_addr
    );
    void depthwiseConv2d(int f_w, int f_h, int k_w, int k_h, int channel, int stride, int N, 
        AXI_DMA *DMA, dla *dla, 
        u32* param_addr
    );
#endif

    /**
     * @brief Used to compute pointwise Conv.
     * 
     * @param f_w feature map width
     * @param f_h feature map height
     * @param channel channel size
     * @param N amount of kernel
     * @param k[][] kernel array
     * @param b[] bias array
     * @param f[][][] input feature map
     * @param o[][][] map after compute
    */
    void pointwiseConv2d(int f_w, int f_h, int channel, int N, float** k, float* b, float f[1024][224][224], float o[1024][224][224]);
    void pointwiseConv2d(int f_w, int f_h, int channel, int N, float f[1024][224][224], float o[1024][224][224]);
#if defined __ARM__
    void pointwiseConv2d(int f_w, int f_h, int channel, int N, 
        AXI_DMA *DMA, dla *dla,
        u32* param_addr,
        u32* fmap_addr,
        u32* result_addr
    );

    void pointwiseConv2d(int f_w, int f_h, int channel, int N, 
        AXI_DMA *DMA, dla *dla,
        u32* param_addr
    );
#endif

    /**
     * @brief Batch Normalization.
     * 
     * @param f_w input feature map width
     * @param f_h input feature map height
     * @param channel channel size
     * @param gamma[channel] gamma array
     * @param beta[channel]  beta array
     * @param mean[channel]  the mean of the data after train
     * @param variance[channel] the variance of the data after train
     * @param r the redundant part, default 0.003
     * @param f[][][] feature map
    */
    void batchNormalization(int f_w, int f_h, int channel, float f[1024][224][224], float* gamma, float* beta, float* mean, float* variance, float r);

    /**
    * @brief Batch Normalization
    * 
    * @param f_W input feature map width
    * @param f_h input feature map height
    * @param f[][][] feature map
    * @param BN the batch normalization class
    */
    void batchNormalization(int f_w, int f_h, int channel, float f[1024][224][224], BatchNormalization BN);

    /**
    * @brief The relu activity function.
    * 
    * @param f_w input feature map width
    * @param f_h input feature map height
    * @param channel input feature map channel
    * @param f[][][] feature map
    */
    void relu(int f_w, int f_h, int channel, float f[1024][224][224]);

    /**
     * @brief Compute the global average pooling.
     * 
     * @param f_w feature map width
     * @param f_h feature map height
     * @param channel channel size
     * @param f[][][] input feature map
     * @param o[] map after compute
    */
    void globalAvgPool2D(int f_w, int f_h, int channel, float f[1024][224][224], float o[]);
#if defined __ARM__
    void globalAvgPool2D(int f_w, int f_h, int channel, 
        AXI_DMA *DMA,
        dla *dla);
#endif

    /**
     * @brief Flatten the feature into single dimension.
     * 
     * @warning In MobileNet, there is no need to use this function.
    */
    void flatten();

    /**
     * @brief Compute the fully connect layer.
     * 
     * @param l_i input map length
     * @param l_o output map length
     * @param k[l_o][l_i] kernel array
     * @param b[] bias
     * @param i[] input array
     * @param o[] output array
    */
    void dense(int l_i, int l_o, float** k, float* b, float i[], float o[]);
    void dense(int l_i, int l_o, float i[], float o[]);

    /**
     * @brief Compute the Softmax.
     * 
     * @param l map length
     * @param i[] input array
     * @param o[] output array
    */
    void softmax(int l, float i[], float o[]);

    /**
     * @brief Start prediction.
    */
    void invoke(int w, int h, int channel, float image[3][224][224], float result[43]);

#if defined __ARM__
    void getResult(int f_w, int f_h, int channel, int N, AXI_DMA *DMA, dla *dla, u32* result);
#endif

    void init();
};

#endif // __MOBILENET_H__