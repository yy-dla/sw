# This is an implementation of MobileNet by C++

## Why I make this?
Before implementing on FPGA or ASIC, we should build the model by software. And in order to evaluate the performance of the computation on Cortex-A9 in 666.6MHz, this is a necessary work to be done.  

## Some notice
|Platform|pre-define|
|:-|:-|
|Windows|`__WINDOWS__`|
|Cortex-A9|`__ARM__`|  

If you use Cortex-A9 platform, do not forget to adjust the heap size to `0x1fffffff` in file `lscript.ld` to prevent the overflow.

If you want to know how I train the MobileNet, please go to this link:    
[94.79% Accuracy by Q_MobileNet_v1](https://www.kaggle.com/mzc6838/94-79-accuracy-by-q-mobilenet-v1)  

## reference

> Main structure of the MobileNet model is refered from the article:  
> [MobileNets: Efficient Convolutional Neural Networks for Mobile Vision Applications](https://arxiv.org/abs/1704.04861)  

> The dataset used here is from Kaggle, the link of the dataset is:  
> [GTSRB - German Traffic Sign Recognition Benchmark](https://www.kaggle.com/meowmeowmeowmeowmeow/gtsrb-german-traffic-sign)  

