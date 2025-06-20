#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdlib.h>
#include <time.h>
#define DEBUG 1
#include <paradis.h>

c_error_t populate_array_rng(data_t *array, size_t size) {
	unsigned int seed = time(0);
	unsigned int idx;
	for (idx = 0; idx < size; idx++) {
		array[idx] = rand_r(&seed);
	}
	return 0;
}

c_error_t check_sorted(const data_t *array, size_t size) {
	if (size <= 0) {
		return c_error_allocation_error;
	}

	for (size_t idx = 1; idx < size; idx++) {
		if (array[idx - 1] > array[idx]) {
			return c_error_invalid_argument;
		}
	}
	return c_error_no_error;
}

#endif
