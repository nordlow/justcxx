/*!
 * \file mt19937.h
 * \brief MT19937 Random Number Generator.
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

/*! Initializes mt[N] with a seed. */
  void init_genrand(ulong s);

/*!
 * Initialize by an array with array-length
 *
 * \param[out] init_key is the array for initializing keys
 * \param[in] key_length is its length.
 */
  void init_by_array(ulong init_key[], ulong key_length);

/*!
 * Generates a random number on [0,0xffffffff]-interval.
 */
  ulong mt19937_genrand_int32(void);

/*!
 * Generates a random number on [0,0x7fffffff]-interval.
 */
  long mt19937_genrand_int31(void);

/*!
 * Generates a random number on [0,1]-real-interval.
 */
  double mt19937_genrand_real1(void);

/*!
 * Generates a random number on [0,1)-real-interval.
 */
  double mt19937_genrand_real2(void);

/*!
 * Generates a random number on (0,1)-real-interval.
 */
  double mt19937_genrand_real3(void);

/*!
 * Generates a random number on [0,1) with 53-bit resolution.
 */
  double mt19937_genrand_res53(void);

  int mt19937_test(void);

#ifdef __cplusplus
}
#endif
