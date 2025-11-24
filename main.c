#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

float sdot;
clock_t timestamp;

// Function defs
extern float asm_kernel(unsigned long long int n, float* a, float* b);
float c_kernel(unsigned long long int n, float *a, float *b);

// Vector utils
float *malloc_vector(int dims);
void free_vector(float *vector);
void populate_vector(float *vector, int dims);
void normalize_vector(float *vector, int dims);

// Timing utils
void start_timer();
double end_timer();

// Testing utils
double benchmark_asm(float *va, float *vb, int dims);
double benchmark_c(float *va, float *vb, int dims);

// Main routine
int main()
{
	// Fixed seed so we things are deterministic
	srand(0);

	// Allocate vars we'll need
	// Change these before we submit
	int cycles = 20;
	int test_dims_length = 5;
	int test_dims[] = {1 << 16, 1 << 20, 1 << 24, 1 << 28, 1 << 30};
	double test_results[2][test_dims_length][cycles];


	// Small test to see if the functions are correct
	float small_a[] = {3.2, 5.6, 10.0, 2.5, 1.7};
	float small_b[] = {6.2, -7.9, 2.3, 7.0, 4.6};

	printf("[TESTING RESULT] Small ASM: %f, Small C: %f\n",
		asm_kernel(5, small_a, small_b),
		c_kernel(5, small_a, small_b)
	);

	// For each vector dimension count
	for (int i = 0; i < test_dims_length; i++)
	{

		// Allocate vectors
		int n = test_dims[i];
		float *vector_a = malloc_vector(n);
		float *vector_b = malloc_vector(n);

		// Log
		printf("[!] ==============================\n");
		printf("[!] RUNNING BENCHMARK FOR n=%d\n", n);
		printf("[!] ==============================\n\n");

		// Populate and normalize vectors; this makes it faster since its outside the cycles loop
		printf(" -  Populating vectors...\n");
		populate_vector(vector_a, n);
		populate_vector(vector_b, n);
		normalize_vector(vector_a, n);
		normalize_vector(vector_b, n);
		printf(" !  Done populating vectors.\n\n");

		// We test `cycles` times
		for (int j = 0; j < cycles; j++)
		{
			printf(" -  Running benchmarks (cycle %d)...\n", j);
			test_results[1][i][j] = benchmark_asm(vector_a, vector_b, n);
			test_results[0][i][j] = benchmark_c(vector_a, vector_b, n);
			printf(" !  Cycle finished (cycle %d).\n\n", j);
		}

		// Free vectors
		free_vector(vector_a);
		free_vector(vector_b);
	}

	// Log
	printf("[!] ==============================\n");
	printf("[!] LOGGING FINAL TEST RESULTS\n");
	printf("[!] ==============================\n\n");

	// Display test results
	for (int i = 0; i < test_dims_length; i++)
	{
		double ave_t_c = 0, sd_t_c = 0;
		double ave_t_asm = 0, sd_t_asm = 0;

		// Compute average
		for (int j = 0; j < cycles; j++)
		{
			ave_t_c += test_results[0][i][j];
			ave_t_asm += test_results[1][i][j];
		}

		ave_t_c /= cycles;
		ave_t_asm /= cycles;

		// Compute sd
		for (int j = 0; j < cycles; j++)
		{
			sd_t_c += (test_results[0][i][j] - ave_t_c) * (test_results[0][i][j] - ave_t_c);
			sd_t_asm += (test_results[1][i][j] - ave_t_asm) * (test_results[1][i][j] - ave_t_asm);
		}

		sd_t_c = powf(sd_t_c / (cycles - 1), 0.5);
		sd_t_asm = powf(sd_t_asm / (cycles - 1), 0.5);

		// Display results
		printf("Average runtime for C (%d): %lf\n", test_dims[i], ave_t_c);
		printf("S.D. of runtime for C (%d): %lf\n", test_dims[i], sd_t_c);
		printf("Average runtime for ASM (%d): %lf\n", test_dims[i], ave_t_asm);
		printf("S.D. of runtime for ASM (%d): %lf\n\n", test_dims[i], sd_t_asm);
	}

	return 0;
}

/**
 * Performs a benchmark of the ASM func, given the two ves.
 *
 * @param va
 * @param vb
 * @param dims
 */
double benchmark_asm(float *va, float *vb, int dims)
{
	float asm_sdot;
	double t;

	// Time the execution
	start_timer();
	asm_sdot = asm_kernel(dims, va, vb);
	t = end_timer();

	// Log result, ew this function has a side effect
	// Very not-functional D:
	printf("    ASM kernel result: %f (%lfs)\n", asm_sdot, t);

	return t;
}

/**
 * Performs a benchmark of the C func, given the two ves.
 *
 * @param va
 * @param vb
 * @param dims
 */
double benchmark_c(float *va, float *vb, int dims)
{
	float c_sdot;
	double t;

	// Time the execution
	start_timer();
	c_sdot = c_kernel(dims, va, vb);
	t = end_timer();

	// Log result, ew this function has a side effect
	// Very not-functional D:
	printf("    C kernel result: %f (%lfs)\n", c_sdot, t);

	return t;
}

/**
 * Sanity check.
 * Same signature as the ASM kernel.
 */
float c_kernel(unsigned long long int n, float *a, float *b)
{
	float sdot = 0.0f;
	for (int i = 0; i < n; i++)
		sdot += (float)(a[i] * b[i]);
	return sdot;
}

/**
 * Starts the timer and inits the time var.
 * I hate globals yeah but fck it.
 */
void start_timer()
{
	timestamp = clock();
}

/**
 * Ends the timer and returns the number of seconds elapsed since the last call to start_timer().
 * Don't joke man.
 *
 * @returns
 */
double end_timer()
{
	clock_t end = clock();
	return (double)(end - timestamp) / CLOCKS_PER_SEC;
}

/**
 * Utility function to allocate a vector in memory.
 * `dims` refers to the number of dimensions that the vector has.
 * Must call free_vector after use.
 *
 * @param dims
 */
float *malloc_vector(int dims)
{
	return (float *)malloc(dims * sizeof(float));
}

/**
 * Frees the vector memory.
 *
 * @param vector
 */
void free_vector(float *vector)
{
	free(vector);
}

/**
 * Populates the vector with random values.
 * Note that by default we use the range [0, 100] to keep things small.
 * If we want to normalize the vector (to prevent overflows/underflows), we use the norm util below.
 *
 * @param vector
 * @param dims
 */
void populate_vector(float *vector, int dims)
{
	for (int i = 0; i < dims; i++)
		vector[i] = (float)(rand() % 100);
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
