#include <time.h>
#include <stdio.h>
#include <x86intrin.h>
#include "simd.h"

long long int sum(int vals[NUM_ELEMS]) {
	clock_t start = clock();

	long long int sum = 0;
	for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
		for(unsigned int i = 0; i < NUM_ELEMS; i++) {
			if(vals[i] >= 128) {
				sum += vals[i];
			}
		}
	}
	clock_t end = clock();
	printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
	return sum;
}

long long int sum_unrolled(int vals[NUM_ELEMS]) {
	clock_t start = clock();
	long long int sum = 0;

	for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
		for(unsigned int i = 0; i < NUM_ELEMS / 4 * 4; i += 4) {
			if(vals[i] >= 128) sum += vals[i];
			if(vals[i + 1] >= 128) sum += vals[i + 1];
			if(vals[i + 2] >= 128) sum += vals[i + 2];
			if(vals[i + 3] >= 128) sum += vals[i + 3];
		}

		//This is what we call the TAIL CASE
		//For when NUM_ELEMS isn't a multiple of 4
		//NONTRIVIAL FACT: NUM_ELEMS / 4 * 4 is the largest multiple of 4 less than NUM_ELEMS
		for(unsigned int i = NUM_ELEMS / 4 * 4; i < NUM_ELEMS; i++) {
			if (vals[i] >= 128) {
				sum += vals[i];
			}
		}
	}
	clock_t end = clock();
	printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
	return sum;
}

long long int sum_unrolled_optimal(int vals[NUM_ELEMS]) {
	clock_t start = clock();
	long long int sum = 0;

	for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {

		long long int sum1=0, sum2=0, sum3=0, sum4=0;

		for(unsigned int i = 0; i < NUM_ELEMS / 4 * 4; i += 4) {
			if(vals[i] >= 128) sum1 += vals[i];
			if(vals[i + 1] >= 128) sum2 += vals[i + 1];
			if(vals[i + 2] >= 128) sum3 += vals[i + 2];
			if(vals[i + 3] >= 128) sum4 += vals[i + 3];
		}

		//This is what we call the TAIL CASE
		//For when NUM_ELEMS isn't a multiple of 4
		//NONTRIVIAL FACT: NUM_ELEMS / 4 * 4 is the largest multiple of 4 less than NUM_ELEMS
		for(unsigned int i = NUM_ELEMS / 4 * 4; i < NUM_ELEMS; i++) {
			if (vals[i] >= 128) {
				sum += vals[i];
			}
		}
		sum += sum1 + sum2 + sum3 + sum4;
	}
	clock_t end = clock();
	printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
	return sum;
}


long long int sum_simd(int vals[NUM_ELEMS]) {
	clock_t start = clock();
	__m128i _127 = _mm_set1_epi32(127);		// This is a vector with 127s in it... Why might you need this?
	long long int result = 0;				   // This is where you should put your final result!
	/* DO NOT DO NOT DO NOT DO NOT WRITE ANYTHING ABOVE THIS LINE. */
	
	for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
		/* YOUR CODE GOES HERE */

		/* You'll need a tail case. */

		__m128i temp = _mm_setzero_si128(); 
		
		for(unsigned int i = 0; i < NUM_ELEMS / 4 * 4; i += 4) {
			__m128i vec = _mm_loadu_si128((__m128i*)(vals + i)); // Load 4 integers into a 128-bit vector 
			__m128i mask = _mm_cmpgt_epi32(vec, _127); // Compare each element with 127, creating a mask 

			__m128i filtered_vec = _mm_and_si128(vec, mask); // Zero out elements that are not greater than 127
			temp = _mm_add_epi32(temp, filtered_vec); // Accumulate the sum in temp
		}

		for(unsigned int i = NUM_ELEMS / 4 * 4; i < NUM_ELEMS; i++) {
			if (vals[i] >= 128) {
				result += vals[i];
			}
		}

		// store __mm128i result into int array 
		int temp_array[4];
		_mm_storeu_si128((__m128i*)temp_array, temp);

		// sum up the elements in temp_array and add to result 
		for(int j = 0; j < 4; j++) {
			result += temp_array[j];
		}
	}
	clock_t end = clock();
	printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
	return result;
}

long long int sum_simd_unrolled(int vals[NUM_ELEMS]) {
	clock_t start = clock();
	__m128i _127 = _mm_set1_epi32(127);
	long long int result = 0;
	for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
		/* COPY AND PASTE YOUR sum_simd() HERE */
		/* MODIFY IT BY UNROLLING IT */

		/* You'll need 1 or maybe 2 tail cases here. */
		__m128i temp1 = _mm_setzero_si128();
		__m128i temp2 = _mm_setzero_si128();
		__m128i temp3 = _mm_setzero_si128();
		__m128i temp4 = _mm_setzero_si128();

		for(unsigned int i = 0; i < NUM_ELEMS / 16 * 16; i += 16) {
			__m128i vec1 = _mm_loadu_si128((__m128i*)(vals + i)); // Load first 4 integers
			__m128i vec2 = _mm_loadu_si128((__m128i*)(vals + i + 4)); // Load next 4 integers
			__m128i vec3 = _mm_loadu_si128((__m128i*)(vals + i + 8)); // Load next 4 integers
			__m128i vec4 = _mm_loadu_si128((__m128i*)(vals + i + 12)); // Load next 4 integers

			__m128i mask1 = _mm_cmpgt_epi32(vec1, _127); // Compare first vector with 127
			__m128i mask2 = _mm_cmpgt_epi32(vec2, _127); // Compare second vector with 127
			__m128i mask3 = _mm_cmpgt_epi32(vec3, _127); // Compare third vector with 127
			__m128i mask4 = _mm_cmpgt_epi32(vec4, _127); // Compare fourth vector with 127

			__m128i filtered_vec1 = _mm_and_si128(vec1, mask1); // Filter first vector
			__m128i filtered_vec2 = _mm_and_si128(vec2, mask2); // Filter second vector
			__m128i filtered_vec3 = _mm_and_si128(vec3, mask3); // Filter third vector
			__m128i filtered_vec4 = _mm_and_si128(vec4, mask4); // Filter fourth vector

			temp1 = _mm_add_epi32(temp1, filtered_vec1); // Accumulate first vector
			temp2 = _mm_add_epi32(temp2, filtered_vec2); // Accumulate second vector
			temp3 = _mm_add_epi32(temp3, filtered_vec3); // Accumulate third vector
			temp4 = _mm_add_epi32(temp4, filtered_vec4); // Accumulate fourth vector
		}

		for(unsigned int i = NUM_ELEMS / 16 * 16; i < NUM_ELEMS; i++) {
			if (vals[i] >= 128) {
				result += vals[i];
			}
		}

		int temp_array1[4], temp_array2[4], temp_array3[4], temp_array4[4];
		_mm_storeu_si128((__m128i*)temp_array1, temp1); // Store the first vector
		_mm_storeu_si128((__m128i*)temp_array2, temp2); // Store the second vector
		_mm_storeu_si128((__m128i*)temp_array3, temp3); // Store the third vector
		_mm_storeu_si128((__m128i*)temp_array4, temp4); // Store the last vector

		for(int j = 0; j < 4; j++) {
			result += temp_array1[j];
			result += temp_array2[j];
			result += temp_array3[j];
			result += temp_array4[j];
		}

	}
	clock_t end = clock();
	printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
	return result;
}