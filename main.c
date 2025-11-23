#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

float sdot;

// Function defs
extern float asm_kernel(unsigned long long int n, float* a, float* b);
float c_kernel(unsigned long long int n, float *a, float *b);

// Vector utils
float *malloc_vector(int dims);
void populate_vector(float *vector, int dims);
void normalize_vector(float *vector, int dims);

// Main routine
int main()
{
	srand(time(NULL));

	unsigned long long int dims;
	printf("Number of vector dimensions: ");
	scanf("%llu", &dims);

	float *vector_a = malloc_vector(dims);
	float *vector_b = malloc_vector(dims);

	// Populate vectors
	// populate_vector(vector_a, dims);
	vector_a[0] = 12.0;
	vector_a[1] = 5.0;
	populate_vector(vector_b, dims);
	normalize_vector(vector_a, dims);
	normalize_vector(vector_b, dims);

	printf("vectora [0]: %f\nvectora [1]: %f\n", vector_a[0], vector_a[1]);

	float c_sdot = c_kernel(dims, vector_a, vector_b);
	float asm_sdot = asm_kernel(dims, vector_a, vector_b);

	printf("C kernel result: %f\nASM kernel result: %f\n", c_sdot, asm_sdot);

	return 0;
}

/**
 * Sanity check.
 * Same signature as the ASM kernel.
 */
float c_kernel(unsigned long long int n, float *a, float *b)
{
	float sdot = 0.0f;
	for (int i = 0; i < n; i++)
		sdot += a[i] * b[i];
	return sdot;
}

/**
 * Utility function to allocate a vector in memory.
 * `dims` refers to the number of dimensions that the vector has.
 *
 * @param dims
 */
float *malloc_vector(int dims)
{
	return (float *)malloc(dims * sizeof(float));
}

/**
 * Populates the vector with random values.
 * Note that by default we use the range [0, RAND_MAX].
 * If we want to normalize the vector (to prevent overflows/underflows), we use the norm util below.
 *
 * @param vector
 * @param dims
 */
void populate_vector(float *vector, int dims)
{
	for (int i = 0; i < dims; i++)
		vector[i] = (float)rand();
}

/**
 * Normalizes the vector.
 *
 * @param vector
 * @param dims
 */
void normalize_vector(float *vector, int dims)
{
	// Compute length of vector
	float len = 0.0f;
	for (int i = 0; i < dims; i++)
		len += vector[i] * vector[i];
	len = powf(len, 0.5);

	// After computing length, div each vector component by it
	for (int i = 0; i < dims; i++)
		vector[i] /= len;
}