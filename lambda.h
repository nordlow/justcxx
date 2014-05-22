/*!
 * \file lambda.h
 * \brief C Lambda Macro
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-06-23 12:04
 * \see http://www.velocityreviews.com/forums/t730159-lambdas-in-gcc.html
 */

#pragma once

/* Macro to create lambda expressions in GCC, however it only works in
 * GCC no other compilers support it (not even clang).
 *
 * Use it as
 *   int (*max)(int, int) = lambda (int , (int x, int y) { return x > y ? x : y; });
 * or with \c qsort() as
 *   qsort (array, array_num, sizeof (int), lambda (int, (const void * a, const void * b) { return *(int*)a - *(int*)b; }));
 */
#define lambda(return_type, body_and_args)      \
  ({                                            \
    return_type __fn__ body_and_args            \
      __fn__;                                   \
  })
