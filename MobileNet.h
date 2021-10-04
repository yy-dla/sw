#ifndef __MOBILENET_H__
#define __MOBILENET_H__

#include <string>
#include "BatchNormalization.h"

class MobileNet
{
private:
    bool hasBeenInit = false;

    // Heap
    //float map1[1024][224][224];
    //float map2[1024][224][224];
    //float globalAvgPoolMap[1024];
    //float denseMap[43];
    
   //  float map2[1024][224][224];
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

    /*float*** conv2d_weight;
    float*** depthwise_conv2d_weight;
    float** conv2d_1_weight;
    float*** depthwise_conv2d_1_weight;
    float** conv2d_2_weight;
    float*** depthwise_conv2d_2_weight;
    float** conv2d_3_weight;
    float*** depthwise_conv2d_3_weight;
    float** conv2d_4_weight;
    float*** depthwise_conv2d_4_weight;
    float** conv2d_5_weight;
    float*** depthwise_conv2d_5_weight;
    float** conv2d_6_weight;
    float*** depthwise_conv2d_6_weight;
    float** conv2d_7_weight;
    float*** depthwise_conv2d_7_weight;
    float** conv2d_8_weight;
    float*** depthwise_conv2d_8_weight;
    float** conv2d_9_weight;
    float*** depthwise_conv2d_9_weight;
    float** conv2d_10_weight;
    float*** depthwise_conv2d_10_weight;
    float** conv2d_11_weight;
    float*** depthwise_conv2d_11_weight;
    float** conv2d_12_weight;
    float*** depthwise_conv2d_12_weight;
    float** conv2d_13_weight;
    float* conv2d_bias;
    float* depthwise_conv2d_bias;
    float* conv2d_1_bias;
    float* depthwise_conv2d_1_bias;
    float* conv2d_2_bias;
    float* depthwise_conv2d_2_bias;
    float* conv2d_3_bias;
    float* depthwise_conv2d_3_bias;
    float* conv2d_4_bias;
    float* depthwise_conv2d_4_bias;
    float* conv2d_5_bias;
    float* depthwise_conv2d_5_bias;
    float* conv2d_6_bias;
    float* depthwise_conv2d_6_bias;
    float* conv2d_7_bias;
    float* depthwise_conv2d_7_bias;
    float* conv2d_8_bias;
    float* depthwise_conv2d_8_bias;
    float* conv2d_9_bias;
    float* depthwise_conv2d_9_bias;
    float* conv2d_10_bias;
    float* depthwise_conv2d_10_bias;
    float* conv2d_11_bias;
    float* depthwise_conv2d_11_bias;
    float* conv2d_12_bias;
    float* depthwise_conv2d_12_bias;
    float* conv2d_13_bias;*/

   /* BatchNormalization BN_0;
    BatchNormalization BN_1;
    BatchNormalization BN_2;
    BatchNormalization BN_3;
    BatchNormalization BN_4;
    BatchNormalization BN_5;
    BatchNormalization BN_6;
    BatchNormalization BN_7;
    BatchNormalization BN_8;
    BatchNormalization BN_9;
    BatchNormalization BN_10;
    BatchNormalization BN_11;
    BatchNormalization BN_12;
    BatchNormalization BN_13;
    BatchNormalization BN_14;
    BatchNormalization BN_15;
    BatchNormalization BN_16;
    BatchNormalization BN_17;
    BatchNormalization BN_18;
    BatchNormalization BN_19;
    BatchNormalization BN_20;
    BatchNormalization BN_21;
    BatchNormalization BN_22;
    BatchNormalization BN_23;
    BatchNormalization BN_24;
    BatchNormalization BN_25;
    BatchNormalization BN_26;*/
    
public:
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

    /**
     * @brief Batch Normalization.
     * 
     * @param f_w input feature map width
     * @param f_h input feature map height
     * @param channel channel size
     * @param gamma[channel] gamma array (¦Ã)
     * @param beta[channel]  beta array (¦Â)
     * @param mean[channel]  the mean of the data after train (¦Ì)
     * @param variance[channel] the variance of the data after train (¦Ò^2)
     * @param r the redundant part (¦Å), default 0.003
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

    void init();
};

#endif // __MOBILENET_H__