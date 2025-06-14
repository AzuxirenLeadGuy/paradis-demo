#include "test_utils.h"

const data_t debug_array_size = 512;
const size_t DEFAULT_SIZE = 0x4000;
const size_t EPOCH_SIZE = 0x100;

int main() {
	const size_t size = DEFAULT_SIZE;
	data_t array[size];
	c_error_t err_value = 0;

	for (size_t itr = 0; itr < EPOCH_SIZE; itr++) {

		populate_array_rng(array, size);
		err_value |= perform_radix_auxillary_sort(size, array, 4);
		if (err_value != c_error_no_error) {
			break;
		}
		err_value |= check_sorted(array, size);
		if (err_value != c_error_no_error) {
			break;
		}
	}

	if (err_value != c_error_no_error) {
		return err_value;
	}

	for (size_t itr = 0; itr < EPOCH_SIZE; itr++) {

		populate_array_rng(array, size);
		err_value |= perform_radix_inplace_sort(size, array, 4, 0);
		if (err_value != c_error_no_error) {
			break;
		}
		err_value |= check_sorted(array, size);
		if (err_value != c_error_no_error) {
			break;
		}
	}

	return err_value;
}
