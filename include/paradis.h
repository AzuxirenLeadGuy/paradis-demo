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
	 * @brief The data array argument is NULL
	 * 
	 */
	c_error_null_argument,

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
	 * @brief DEBUG error. Should not occur in Release
	 * 
	 */
	c_error_sle,
} c_error_t;

/**
 * @brief The parameters needed for radix sorting
 *
 */
typedef struct {
	/**
	 * @brief The array of integers to sort
	 *
	 */
	data_t *data_array;

	/**
	 * @brief The size of the integer array.
	 *
	 */
	const size_t array_size;
	
	/**
	 * @brief The size of radix bins (in bits)
	 * 
	 */
	const byte_t bit_bin_length;

	/**
	 * @brief The number of data_t elements that a single integer is
	 * composed of
	 *
	 */
	const byte_t single_int_size;
} radix_sort_params;

/**
 * @brief The interface for radix sorting
 *
 */
typedef struct {
	/**
	 * @brief The radix sorting function
	 *
	 */
	const c_error_t (*sort_fn)(radix_sort_params);
} sort_interface_t;

/**
 * @brief Get the radix naive sort object initialized within
 * the pointer of sort_interface_t
 *
 * @param sif The interface to initialize
 * @param use_aux Use non-zero value to indicate that the method must
 * use auxillary array allocation. Otherwise, the sorting takes place
 * in-place without an auxiallary array (i.e no heap memory required).
 * @return byte_t
 */
c_error_t get_radix_naive_sort(sort_interface_t *sif, byte_t use_aux);

/**
 * @brief Get the radix sequential sort object initialized within
 * the pointer of sort_interface_t
 *
 * @param sif The interface to initialize
 * @return byte_t
 */
c_error_t get_radix_se_sort(sort_interface_t *sif);

/**
 * @brief Get the radix parallel sort object initialized within
 * the pointer of sort_interface_t
 *
 * @param sif The interface to initialize
 * @return byte_t
 */
c_error_t get_radix_ax_sort(sort_interface_t *sif);

/**
 * @brief Get the paradis sort object initialized within
 * the pointer of sort_interface_t
 *
 * @param sif The interface to initialize
 * @return byte_t
 */
c_error_t get_paradis_sort(sort_interface_t *sif);

#endif
