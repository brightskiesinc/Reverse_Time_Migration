//
// Created by amr on 24/06/2020.
//
#include "noise_filtering.h"
#include <iostream>
#include <cmath>

float laplace_3x3 [9] = {
		 1, 1, 1,
		 1,-8, 1,
		 1, 1, 1};
float laplace_9x9 [81] = {
		0, 1, 1, 2, 2, 2, 1, 1, 0,
		1, 2, 4, 5, 5, 5, 4, 2, 1,
		1, 4, 5, 3, 0, 3, 5, 4, 1,
		2, 5, 3,-12,-24,-12, 3, 5, 2,
		2, 5, 0,-24,-40,-24, 0, 5, 2,
		2, 5, 3,-12,-24,-12, 3, 5, 2,
		1, 4, 5, 3, 0, 3, 5, 4, 1,
		1, 2, 4, 5, 5, 5, 4, 2, 1,
		0, 1, 1, 2, 2, 2, 1, 1, 0,
};
float sobel_x [9] = {
		 1, 0, -1,
		 2, 0, -2,
		 1, 0, -1};

float sobel_y [9] = {
		 1, 2, 1,
		 0, 0, 0,
		-1,-2,-1};

float bandpass_3x3 [9] = {
		 -1, 2,-1,
		  2,-4, 2,
		 -1, 2,-1,
};

float gauss_5x5 [25] = {
		  1, 4, 7, 4, 1,
		  4,16,26,16, 4,
		  7,25,41,25, 7,
		  4,16,26,16, 4,
		  1, 4, 7, 4, 1,
};

void convolution(float* mat, float* dst,float* kernel, int kernel_size, uint nz, uint nx)
{
	int hl = kernel_size / 2;
	for(uint row = hl; row < nz - hl; row++){
		for(uint col = hl; col < nx - hl; col++)
		{
			float value = 0;
			for(int i = 0; i < kernel_size; i++)
			{
				for(int j = 0; j < kernel_size; j++)
				{
					value += kernel[i * kernel_size + j] * mat[(row + i - hl) * nx + (col + j - hl)];
				}
			}
			dst[row * nx + col] = value;
		}
	}
}

void magnitude(float* Gx, float* Gy, float* G, uint size)
{
	  for (uint idx = 0; idx < size; idx++) {
		  G[idx] = sqrt((Gx[idx] * Gx[idx]) + (Gy[idx] * Gy[idx]));
	  }
}

void row_normalize(float* data, float *output, size_t nx, size_t nz) {
    /* normalizes each row so that the new value is ranging from 0 to 1, it skips the boundaries in seek for min and max value */
    for (int iz=0; iz<nz; iz++) { // normalize for each row (i.e. for each depth)
        float min = data[iz*nx];
        float max = min;
        for (int ix=0; ix<nx; ix++) { // finds the max and min per row
            min = fminf(min,data[iz*nx+ix]);
            max = fmaxf(max,data[iz*nx+ix]);
        }
        float range = max - min;
        for (int ix=0; ix<nx; ix++) { // subtract min value and divide by max-min, so that new value ranges from 0 to 1 (except for boundaries)
            if (range != 0) {
                output[iz * nx + ix] = (data[iz * nx + ix] - min) / range;
            } else {
                output[iz * nx + ix] = 0;
            }
        }
    }
}

void z_normalize(float* input, float* output, uint nz, uint nx)
{
	float sum = 0.0;
	float mean;
	float standardDeviation = 0.0;
	float item_count = nx*nz;

	for(int i = 0; i < item_count; ++i)
	{
		sum += input[i];
	}

	mean = sum/item_count;
	printf("mean = %f\n", mean);

	for(int i = 0; i < item_count; ++i)
	{
		standardDeviation += pow(input[i] - mean, 2);
	}

	standardDeviation = sqrt(standardDeviation/item_count);

	printf("standardDeviation = %f\n", standardDeviation);
	for(int i = 0; i < item_count; i++)
	{
		output[i] = (input[i] - mean)/ standardDeviation;
	}

}

void normalize(float* input, uint size, float max, float min)
{
	for(int i = 0; i < size; i++)
	{
		input[i] = 1023 * ((input[i] - min) / (max - min));
	}
}

void denormalize(float*input, uint size, float max, float min)
{
	for(int i = 0; i < size ;i++)
	{
		input[i] = (input[i]/1023) * (max - min) + min;
	}
}

void histogram_equalize(float* input, float* output, uint nz, uint nx)
{
	int item_count = nx*nz;
    float min = input[0];
    float max = min;

    float hist[1024] = { 0 };
    float new_gray_level[1024] = { 0 };
    float cumulative_frequency = 0;


	for(int i = 0; i < item_count; ++i)
	{
        min = fminf(min,input[i]);
        max = fmaxf(max,input[i]);
	}

	// scale to 0-1023
	normalize(input, item_count, max, min);

	for(int i = 0; i < item_count; i++)
	{
		hist[(int)input[i]]++;
	}

	for(int i = 0; i<1024; i++)
	{
		cumulative_frequency += hist[i];
		new_gray_level[i] = round(((cumulative_frequency) * 1023) / item_count);
	}

	for(int i =0; i< item_count; i++ )
	{
		output[i] = new_gray_level[(int)input[i]];
	}

	denormalize(output, item_count, max, min);

}

void filter_stacked_correlation(float *input_frame, float *output_frame,
        unsigned int nx, unsigned int nz, unsigned int ny,
        float dx, float dz, float dy) {

	convolution(input_frame, output_frame, laplace_9x9, 9, nz, nx);
}
