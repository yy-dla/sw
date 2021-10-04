#pragma once
#ifndef __BATCH_NORMALIZATION_H__
#define __BATCH_NORMALIZATION_H__

class BatchNormalization {
public:
	int channel;
	float r = 0.001; // reference: https://github.com/tensorflow/tensorflow/blob/7f07ae89c0f16aeafa087b2a3a51e8b2b8bc7c89/tensorflow/python/keras/layers/normalization/batch_normalization.py#L165
	float gamma[1024];
	float beta[1024];
	float mean[1024];
	float variance[1024];

public:
	BatchNormalization();
	BatchNormalization(int);
	BatchNormalization(int, const float gamma[], const float beta[], const float mean[], const float varience[]);
	BatchNormalization(int, const float parameters[][4]);
	~BatchNormalization();

	void init(int, const float gamma[], const float beta[], const float mean[], const float varience[]);
	void init(int, const float parameters[][4]);


};

#endif // !__BATCH_NORMALIZATION_H__

