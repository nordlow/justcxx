/*!
 * \file readline_utils.h
 * \brief Readline Utilities
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2010-03-16 16:43
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Read line using \c readline() with prompt \p prompt. Entry is added
 * to readline history if it is non-empty.
 */
char *readline_with_prompt(const char *prompt);

#ifdef __cplusplus
}
#endif
