#include "crypto.h"

/*
 * All operations must be reversible:
 - integer add and sub
 - xor

 - A decoder performs the encoder's reverse operations in reverse order.

 * Sites:
 - http://www.pgp.com
 - http://hem.fyristorg.com/emailsynaren/
 - http://go.to/safeit

 * Excellent site by Hans Husman concerning both crytography and
 compression:
 - http://www.docs.uu.se/~tv98hah/kryptering/inledning.html

 * Factorization, Discrete logarithmation
 */

/* Designed for 32-bit processors.
 * See "Programmers Vault" section "Technical". Feistel type routine.
 */

const int tea_nloops = 32;
const ulong tea_delta = 0x9e3779b9;	/* A key schedule constant */

void
tea_encode(ulong *v, const ulong *keys)
{
  int j;
  ulong y = v[0], z = v[1];
  ulong a = keys[0], b = keys[1], c = keys[2], d = keys[3];
  ulong sum = 0;
  for (j = 0; j < tea_nloops; j++) {
    sum += tea_delta;
    y += (z << 4) + (z ^ a) + (sum ^ (z >> 5)) + b;
    z += (y << 4) + (y ^ c) + (sum ^ (y >> 5)) + d;
  }
  v[0] = y;
  v[1] = z;
}

void
tea_decode(ulong *v, const ulong *keys)
{
  int j;
  ulong y = v[0], z = v[1];
  ulong a = keys[0], b = keys[1], c = keys[2], d = keys[3];
  ulong sum = tea_delta * tea_nloops;
  for (j = 0; j < tea_nloops; j++) {
    z -= (y << 4) + (y ^ c) + (sum ^ (y >> 5)) + d;
    y -= (z << 4) + (z ^ a) + (sum ^ (z >> 5)) + b;
    sum -= tea_delta;
  }
  v[0] = y;
  v[1] = z;
}

/* DES, Data Encryption Standard - 56-bit */

/* Rijndael, Data Encryption Standard "2" - 128-bits */

/* 3DES, Triple DES */

/* RSA, Rivest, Shamir, Adleman */
