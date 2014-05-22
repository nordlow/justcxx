/*!
 * \file sha1-sse3.h
 * \brief SHA-1 Calculations using SSE3 Instructions.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2010-11-17 16:20
 *
 * This code implements two interfaces of SHA-1 update function: 1) working on a single
 * 64-byte block and 2) working on a buffer of multiple 64-bit blocks. Multiple blocks
 * version of code is software pipelined and faster overall, it is a default. Assemble
 * with -DINTEL_SHA1_SINGLEBLOCK to select single 64-byte block function interface.
 *
 * Function name 'sha1_update_intel' can be changed in the source or via macro:
 * -DINTEL_SHA1_UPDATE_FUNCNAME=my_sha1_update_func_name
 *
 * \see http://software.intel.com/en-us/articles/improving-the-performance-of-the-secure-hash-algorithm-1/
 */

#pragma once

#ifndef INTEL_SHA1_SINGLEBLOCK
// Updates 20-byte SHA-1 record in \p hash for \p num_blocks consequtive 64-byte blocks.
extern "C" void sha1_update_intel(int *hash, const char* input, size_t num_blocks );
#else
// Updates 20-byte SHA-1 record in \p hash for one 64-byte block pointed by \p input.
extern "C" void sha1_update_intel(int *hash, const char* input);
#endif
