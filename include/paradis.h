#ifndef PARADIS_H
#define PARADIS_H

#include <stddef.h>
/**
 * @brief The data type for exactly one byte
 *
 */
typedef unsigned char byte_t;

/**
 * @brief The data type for integer elements
 *
 */
typedef unsigned int data_t;

/**
 * @brief The type of errors
 *
 */
typedef enum {

	/**
	 * @brief No errors encountered
	 *
	 */
	c_error_no_error = 0,

	/**
	 * @brief The data array argument is NULL or invalid
	 *
	 */
	c_error_invalid_argument,

	/**
	 * @brief The auxillary data could not be allocated
	 *
	 */
	c_error_allocation_error,

	/**
	 * @brief The function is not implemented
	 *
	 */
	c_error_not_implemented,

	/**
	 * @brief Failure to prepare a common data state for the sort functions
	 * 
	 */
	c_error_state_failure,

	/**
	 * @brief DEBUG error. Should not occur in Release
	 *
	 */
	c_error_sle,
} c_error_t;

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
	size_t size, 
	data_t *data, 
	byte_t bin_bit_length);

/**
 * @brief Performs radix sort without auxillary array
 *
 * @param data The array to sort
 * @param size The size of the array
 * @param bin_bit_length The number of bits for bins in each
 * iteration.
 * @param cutoff The minimum size of the array to not invoke linear sort
 * @return c_error_t If successful, returns zero. Otherwise returns a
 * non-zero error code
 */
 c_error_t perform_radix_inplace_sort(
	size_t size,
	data_t *data,
	byte_t bin_bit_length,
	byte_t cutoff);

#endif
