#include "BatchNormalization.h"	
#include <iostream>

BatchNormalization::BatchNormalization() {
	this->channel = 0;
}

BatchNormalization::BatchNormalization(int channel) {
	this->channel = channel;

	for (int i = 0; i < this->channel; i++) {
		this->gamma[i]    = 0;
		this->beta[i]     = 0;
		this->mean[i]     = 0;
		this->variance[i] = 0;
	}
}

BatchNormalization::BatchNormalization(int channel, const float gamma[], const float beta[], const float mean[], const float variance[]) {
	this->channel = channel;

	for (int i = 0; i < this->channel; i++) {
		this->gamma[i] = gamma[i];
		this->beta[i] = beta[i];
		this->mean[i] = mean[i];
		this->variance[i] = variance[i];
	}
}

BatchNormalization::BatchNormalization(int channel, const float parameters[][4]) {
	this->channel = channel;

	for (int i = 0; i < this->channel; i++) {
		this->gamma[i] = parameters[i][0];
		this->beta[i] = parameters[i][1];
		this->mean[i] = parameters[i][2];
		this->variance[i] = parameters[i][3];
	}
}

BatchNormalization::~BatchNormalization() {
	//delete[] this->gamma;
	//delete[] this->beta;
	//delete[] this->mean;
	//delete[] this->variance;

}

void BatchNormalization::init(int channel, const float gamma[], const float beta[], const float mean[], const float variance[]) {
	if (this->channel != channel) {
		delete[] this->gamma;
		delete[] this->beta;
		delete[] this->mean;
		delete[] this->variance;

		this->channel = channel;
	}

	for (int i = 0; i < this->channel; i++) {
		this->gamma[i] = gamma[i];
		this->beta[i] = beta[i];
		this->mean[i] = mean[i];
		this->variance[i] = variance[i];
	}
}

void BatchNormalization::init(int channel, const float parameters[][4]) {
	this->channel = channel;

	for (int i = 0; i < this->channel; i++) {
		this->gamma[i] = parameters[i][0];
		this->beta[i] = parameters[i][1];
		this->mean[i] = parameters[i][2];
		this->variance[i] = parameters[i][3];
	}
}
