/*! \file ceasar_cipher.hpp
 * \brief Ceasar Cipher.
 * \author Copyright (C) 2013 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2013-04-02 23:09
 * \see https://hbfs.wordpress.com/2013/04/02/caesars-cipher/
 */

#pragma once
#include <string>

/*! Encrypts Message \p m (in-place) using Ceasar's method. */
void ceasar_encrypt(int key, std::string& m )
{
    for (size_t i = 0; i < m.size(); i ++ )
        m[i] = 'a' + (ord(m[i]) + key) % 26;
}

/*! Decrypts \p m (in-place) using Ceasar's method. */
void ceasar_decrypt(int key, std::string& m)
{
    for (size_t i = 0; i < m.size(); i ++ )
        m[i] = 'a' + (ord(m[i]) + (26 - key)) % 26;
}
