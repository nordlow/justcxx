/*!
 * \file bitget.h
 * \brief Bitwise Operations on C Arrays.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*! \name Indexing a Bit (Unary-Bit) in a Byte Array. */
/* @{ */

/*! Get the \p i:th Leftmost Bit in \p a.
 *
 * The Most Significant Bit (MSB) is the Leftmost element:
 * \return a[B0][7 6 5 4 3 2 1 0],  a[B1][7 6 5 4 3 2 1 0], ...
 */
static inline pure int int8array_get_bit_MSBleft(const uint8_t * a, size_t i)
{
  const size_t Bi = i >> 3;	/* Byte offset */
  const uint bi = i & 0x7;	/* Byte-local bit index [0...7] */
  return (a[Bi] >> (7-bi) & 1);
}

/*! Get the \p i:th Rightmost Bit in \p a.
 *
 * The Most Significant Bit (MSB) is the Rightmost element.
 * \return a[B0][0 1 2 3 4 5 6 7],  a[B1][0 1 2 3 4 5 6 7], ...
 */
static inline pure int int8array_get_bit_MSBright(const uint8_t * a, size_t i)
{
  const size_t Bi = i >> 3;	/* Byte offset */
  const uint bi = i & 0x7;	/* Byte-local bit index [0...7] */
  return (a[Bi] >> (bi) & 1);
}

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*! \name Indexing a Dit (Dual-Bit) in a Byte Array. */
/* @{ */

/*! Get the \p i:th Leftmost Dit (Dual-Bit) in \p a.
 *
 * The Most Significant Dual-Bit (Dit) (MSD) is the Leftmost element:
 * \return a[B0][3-3 2-2 1-1 0-0],  a[B1][7-7 6-6 5-5 4-4], ...
 */
static inline pure int int8array_getDit_MSDleft(const uint8_t * a, size_t i)
{
  const size_t Bi = i >> 2;	/* Byte offset */
  const uint di = i & 0x3;	/* Byte-local dit index [0...3] */
  return (a[Bi] >> 2*(1-di) & 0x7);
}

/*! Get the \p i:th Rightmost Dit (Dual-Bit) in \p a.
 *
 * The Most Significant Duad-Bit (Dit) (MSD) is the Rightmost element:
 * \return a[B0][0-0 1-1 2-2 3-3],  a[B1][4-4 5-5 6-6 7-7], ...
 */
static inline pure int int8array_getDit_MSDright(const uint8_t * a, size_t i)
{
  const size_t Bi = i >> 2;	/* Byte offset */
  const uint di = i & 0x3;	/* Byte-local dit index [0...3] */
  return (a[Bi] >> 2*(di) & 0x7);
}

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*! \name Indexing a Qit (Quad-Bit) in a Byte Array. */
/* @{ */

/*! Get the \p i:th Leftmost Qit (Quad-Bit) in \p a.
 *
 * The Most Significant Quad-Bit (Qit) (MSQ) is the Leftmost element:
 * \return a[B0][1-----1 0-----0],  a[B1][3-----3 2-----2], ...
 */
static inline pure int int8array_getQit_MSQleft(const uint8_t * a, size_t i)
{
  const size_t Bi = i >> 1;	/* Byte offset */
  const uint qi = i & 0x1;	/* Byte-local qit index [0...1] */
  return (a[Bi] >> 4*(1-qi) & 0xf);
}

/*! Get the \p i:th Rightmost Qit (Quad-Bit) in \p a.
 *
 * The Most Significant Quad-Bit (Qit) (MSQ) is the Rightmost element:
 * \return a[B0][0-----0 1-----1],  a[B1][2-----2 3-----3], ...
 */
static inline pure int int8array_getQit_MSQright(const uint8_t * a, size_t i)
{
  const size_t Bi = i >> 1;	/* Byte offset */
  const uint qi = i & 0x1;	/* Byte-local qit index [0...1] */
  return (a[Bi] >> 4*(qi) & 0xf);
}

/* @} */

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
