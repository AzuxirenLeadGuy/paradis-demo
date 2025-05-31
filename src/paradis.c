#include <paradis.h>
#include <stdlib.h>
#include <string.h>

#define BIN_VAL(data, bbl, mask) (((data) >> (bbl)) & (mask))

const data_t DATA_T_BITS = sizeof(data_t) * 8;

typedef struct radix_iteration_state_t radix_iteration_state_t;

/// Represents a state of the radix sort
struct radix_iteration_state_t {

	/// The total number of iterations required by
	/// the radix sort function
	size_t max_depth;

	/// The total number of bins that are being counted
	/// by the histogram
	size_t total_bins;

	/// The length of integer bits that is accounted within
	/// the histogram for radix sort
	size_t bin_bit_length;

	/// The size of a single element of array in bits
	size_t element_bit_length;

	/// The value of mask to use while extracting the integer
	/// bits for the radix sort
	data_t mask;

	/// This is the array for maintaining histogram. It is not
	/// allocated automatically, and must be manually assigned
	/// by the user
	size_t *histogram_bin;
};

c_error_t init_state(
	radix_iteration_state_t *state,
	const size_t element_bit_length,
	const byte_t bin_bit_length) {

	const byte_t bpb = 8;

	if (bin_bit_length == 0 || bin_bit_length >= element_bit_length ||
		element_bit_length == 0 || element_bit_length % bpb != 0) {
		return c_error_state_failure;
	}

	const size_t total_bins = (1 << bin_bit_length);
	// size_t bins[total_bins + 1];
	*state = (const radix_iteration_state_t){
		.max_depth = element_bit_length / bin_bit_length,
		.total_bins = total_bins,
		.bin_bit_length = bin_bit_length,
		.element_bit_length = element_bit_length,
		.mask = total_bins - 1,
		.histogram_bin = 0,
	};
	return c_error_no_error;
}

data_t get_hist_array_length(const radix_iteration_state_t *state) {
	if (state == 0) {
		return 0;
	}
	return state->total_bins + 1;
}

data_t get_bin_id(
	const radix_iteration_state_t *state,
	const data_t element,
	const size_t bin_shift) {

	if (state == 0) {
		return -1;
	}

	if (bin_shift >= state->max_depth) {
		return 0;
	}

	return BIN_VAL(
		element, bin_shift * state->bin_bit_length, state->mask);
}

/**
 * @brief Updates the histogram for the given array. NOTE: no error
 * checks are made for null pointers. Use with caution.
 * 
 * @param state The state to update. 
 * @param size The size of the array
 * @param data The array to prepare histogram for
 * @param bin_shift The number of iterations passed (and therefore, the
 * number of times the element is shifted by the bin_bit_length amount )
 * @return size_t The number of non-zero bins counted within the histogram
 */
size_t update_histogram(
	radix_iteration_state_t *state,
	const size_t size,
	const data_t *data,
	const size_t bin_shift) {

	size_t idx;
	size_t active_bins;
	data_t bin_id;

	// Reset histogram/cumulative counts
	for (idx = 0; idx <= state->total_bins; idx++) {
		state->histogram_bin[idx] = 0;
	}

	// Set values for histogram
	for (idx = 0; idx < size; idx++) {
		bin_id = get_bin_id(state, data[idx], bin_shift);
		state->histogram_bin[bin_id + 1]++;
	}

	// Count non-zero bins
	for (idx = 1, active_bins = 0; idx <= state->total_bins; idx++) {
		if (state->histogram_bin[idx] > 0) {
			active_bins++;
		}
	}

	// Set values for cumulative counts
	for (idx = 2; idx <= state->total_bins; idx++) {
		state->histogram_bin[idx] += state->histogram_bin[idx - 1];
	}

	return active_bins;
}

/**
 * @brief Uses radix sort with allocation of auxiallary array
 *
 * @param size The size of the array
 * @param data The array to sort
 * @param bin_bit_length The number of bits for bins in each
 * iteration.
 * @return c_error_t If successful, returns zero. Otherwise returns a
 * non-zero error code
 */
c_error_t perform_radix_auxillary_sort(
	const size_t size, data_t *data, const byte_t bin_bit_length) {

	radix_iteration_state_t state;
	c_error_t err = init_state(&state, DATA_T_BITS, bin_bit_length);

	if (err != 0) {
		return err;
	}

	if (data == 0) {
		return c_error_invalid_argument;
	}

	const size_t hist_len = get_hist_array_length(&state);
	size_t bins[hist_len];
	state.histogram_bin = bins;
	data_t *src_array = data;
	data_t *temp_array = calloc(size, state.element_bit_length);
	data_t *dest_array = temp_array;
	if (dest_array == 0) {
		return c_error_allocation_error;
	}
	for (size_t itr = 0; itr < state.max_depth; itr++) {

		// No need to check for error returned as we already checked
		// the data
		if (update_histogram(&state, size, src_array, itr) < 2) {
			dest_array = src_array;
			continue;
		}
		dest_array = src_array == data ? temp_array : data;

		// Copy values to temp array
		for (size_t idx = 0; idx < size; idx++) {
			data_t bin_id = get_bin_id(&state, src_array[idx], itr);
			size_t new_idx = state.histogram_bin[bin_id]++;
#if DEBUG
			if (new_idx > size) {

				free(temp_array);
				return c_error_sle;
			}
#endif
			dest_array[new_idx] = src_array[idx];
		}

		// Swap pointers
		src_array = dest_array;
	}

	if (dest_array != data) {
		memcpy(data, dest_array, size * sizeof(data_t));
	}
	free(temp_array);

	return c_error_no_error;
}

c_error_t radix_inplace_sort_recursive(
	const size_t size,
	data_t *data,
	const size_t depth,
	byte_t cutoff,
	const size_t element_bit_length,
	const size_t bin_bit_length) {

	if (size < 2) {
		return c_error_no_error;
	}
	if (size < cutoff) {
		// TODO: Do linear sorting here
		return c_error_invalid_argument;
	}

	radix_iteration_state_t state;
	init_state(&state, element_bit_length, bin_bit_length);

	if (depth > state.max_depth) {
		return c_error_no_error;
	}
	
	const size_t shifts = state.max_depth - depth;
	const size_t hist_len = get_hist_array_length(&state);
	size_t bins[hist_len];
	state.histogram_bin = bins;

	// Check if at least two non-zero bins
	if (update_histogram(&state, size, data, shifts) < 2) {
		return radix_inplace_sort_recursive(
			size,
			data,
			depth + 1,
			cutoff,
			element_bit_length,
			bin_bit_length);
	}

	// Copy values to their correct location
	size_t idx = 0;
	while (idx < size) {

		data_t bin_id = get_bin_id(&state, data[idx], shifts);
		size_t beg_idx = state.histogram_bin[bin_id];
		size_t end_idx = state.histogram_bin[bin_id + 1];
#if DEBUG
		if (idx > end_idx || beg_idx >= size || beg_idx > end_idx) {
			return c_error_sle;
		}
#endif
		if (idx >= beg_idx && idx < end_idx) {
			idx++;
		} else {
			data_t temp = data[beg_idx];
			while (get_bin_id(&state, temp, shifts) == bin_id) {
				temp = data[++beg_idx];
			}
			data[beg_idx] = data[idx];
			data[idx] = temp;
		}
	}

	if(depth == state.max_depth) {
		return c_error_no_error;
	}

	c_error_t err = 0;
	for (idx = 0; idx < state.total_bins; idx++) {
		size_t beg_idx = state.histogram_bin[idx];
		size_t end_idx = state.histogram_bin[idx + 1];
		size_t new_size = end_idx - beg_idx;
		if (new_size < 2) {
			continue;
		}
#if DEBUG
		if (end_idx < beg_idx || new_size > size) {
			return c_error_sle;
		}
#endif
		err |= radix_inplace_sort_recursive(
			new_size,
			data + beg_idx,
			depth + 1,
			cutoff,
			element_bit_length,
			bin_bit_length);
	}

	return err;
}

/**
 * @brief Performs radix sort without auxillary array
 *
 * @param data The array to sort
 * @param size The size of the array
 * @param bin_bit_length
 * @return c_error_t If successful, returns zero. Otherwise returns a
 * non-zero error code
 */
c_error_t perform_radix_inplace_sort(
	const size_t size,
	data_t *data,
	const byte_t bin_bit_length,
	const byte_t cutoff) {

	if (data == 0 || bin_bit_length == 0) {
		return c_error_invalid_argument;
	}

	if(size < 2) {
		return c_error_no_error;
	}

	return radix_inplace_sort_recursive(
		size, data, 0, cutoff, DATA_T_BITS, bin_bit_length);
}
