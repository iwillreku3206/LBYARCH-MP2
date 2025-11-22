#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

extern float asm_kernel(unsigned long long int n, float* a, float* b);
extern float c_kernel(unsigned long long int n, float* a, float* b) {
	float sdot = 0.0f;

	for (int i = 0; i < n; i++) {
		sdot += a[i] * b[i];
	}

	return sdot;
}

float sdot;

int main() {
	unsigned long long int vector_length;
	srand(time(NULL));

	printf("Size of vector: ");
	scanf("%llu", &vector_length);

	float* vector_a = (float*)malloc(vector_length * sizeof(float));
	float* vector_b = (float*)malloc(vector_length * sizeof(float));

	for (int i = 0; i < vector_length; i++) {
		vector_a[i] = (float)rand() / 5.0f;
		vector_b[i] = (float)rand() / 5.0f;
	}

	float c_sdot = c_kernel(vector_length, vector_a, vector_b);
	sdot = c_sdot;
	float asm_sdot = asm_kernel(vector_length, vector_a, vector_b);

	printf("C kernel result: %f\nASM kernel result: %f\n", c_sdot, asm_sdot);

	return 0;
}
