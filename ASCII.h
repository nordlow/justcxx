/*!
 * \file ASCII.h
 * \brief American Standard Code for Information Interchange (ASCII) Constants.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * The first 32 ASCII Control Codes.
 */
#define ASCII_NUL (0x00)	/**< Null */
#define ASCII_SOH (0x01)	/**< Start of heading (A) */
#define ASCII_STX (0x02)	/**< Start of text (B) */
#define ASCII_ETX (0x03)	/**< End of text (C) */
#define ASCII_EOT (0x04)	/**< End of transmit (D) */
#define ASCII_ENQ (0x05)	/**< Enquiry (E) */
#define ASCII_ACK (0x06)	/**< Acknowledge (F) */
#define ASCII_BEL (0x07)	/**< Audible bell (G) */

#define ASCII_BS (0x08)		/**< Backspace (H) */
#define ASCII_HT (0x09)		/**< Horizontal tab (I) */
#define ASCII_LF (0x0a)		/**< Line feed (newline) (J) */
#define ASCII_VT (0x0b)		/**< Vertical tab (K) */
#define ASCII_FF (0x0c)		/**< Form feed, NP (L) */
#define ASCII_CR (0x0d)		/**< Carriage return (M) */
#define ASCII_SO (0x0e)		/**< Shift out (N) */
#define ASCII_SI (0x0f)		/**< Shift in (O) */

#define ASCII_DLE (0x10)	/**< Data link escape (P) */
#define ASCII_DC1 (0x11)	/**< Device control 1, XON (Q) */
#define ASCII_DC2 (0x12)	/**< Device control 2 (R) */
#define ASCII_DC3 (0x13)	/**< Device control 3, XOFF (S) */
#define ASCII_DC4 (0x14)	/**< Device control 4 (T) */
#define ASCII_NAK (0x15)	/**< Negative acknowledge (U) */
#define ASCII_SYN (0x16)	/**< Synchronous idle (V) */
#define ASCII_ETB (0x17)	/**< End transmit block (W) */

#define ASCII_CAN (0x18)	/**< Cancel (X) */
#define ASCII_EM (0x19)		/**< End of medium (Y) */
#define ASCII_SUB (0x1a)	/**< Substitution (Z) */
#define ASCII_ESC (0x1b)	/**< Escape ( ) */
#define ASCII_FS (0x1c)		/**< File separator ( ) */
#define ASCII_GS (0x1d)		/**< Group separator ( ) */
#define ASCII_RS (0x1e)		/**< Record separator ( ) */
#define ASCII_US (0x1f)		/**< Unit separator ( ) */

#define ASCII_SPACE (' ')	/**< Space Character. */

#define ASCII_DEL ('\x7f')	/**< Delete Character. */

/* ========================================================================= */

/*!
 * ASCII Control Code Aliases.
 */
#define ASCII_CTRL_A (1)
#define ASCII_CTRL_B (2)
#define ASCII_CTRL_C (3)
#define ASCII_CTRL_D (4)
#define ASCII_CTRL_E (5)
#define ASCII_CTRL_F (6)
#define ASCII_CTRL_G (7)
#define ASCII_CTRL_H (8)
#define ASCII_CTRL_I (9)
#define ASCII_CTRL_J (10)
#define ASCII_CTRL_K (11)
#define ASCII_CTRL_L (12)
#define ASCII_CTRL_M (13)
#define ASCII_CTRL_N (14)
#define ASCII_CTRL_O (15)
#define ASCII_CTRL_P (16)
#define ASCII_CTRL_Q (17)
#define ASCII_CTRL_R (18)
#define ASCII_CTRL_S (19)
#define ASCII_CTRL_T (20)
#define ASCII_CTRL_U (21)
#define ASCII_CTRL_V (22)
#define ASCII_CTRL_W (23)
#define ASCII_CTRL_X (24)
#define ASCII_CTRL_Y (25)
#define ASCII_CTRL_Z (26)

/* ========================================================================= */

#define ASCII_a ('a')
#define ASCII_b ('b')
#define ASCII_c ('c')
#define ASCII_d ('d')
#define ASCII_e ('e')
#define ASCII_f ('f')
#define ASCII_g ('g')
#define ASCII_h ('h')
#define ASCII_i ('i')
#define ASCII_j ('j')
#define ASCII_k ('k')
#define ASCII_l ('l')
#define ASCII_m ('m')
#define ASCII_n ('n')
#define ASCII_o ('o')
#define ASCII_p ('p')
#define ASCII_q ('q')
#define ASCII_r ('r')
#define ASCII_s ('s')
#define ASCII_t ('t')
#define ASCII_u ('u')
#define ASCII_v ('v')
#define ASCII_w ('w')
#define ASCII_x ('x')
#define ASCII_y ('y')
#define ASCII_z ('z')

#define ASCII_A ('A')
#define ASCII_B ('B')
#define ASCII_C ('C')
#define ASCII_D ('D')
#define ASCII_E ('E')
#define ASCII_F ('F')
#define ASCII_G ('G')
#define ASCII_H ('H')
#define ASCII_I ('I')
#define ASCII_J ('J')
#define ASCII_K ('K')
#define ASCII_L ('L')
#define ASCII_M ('M')
#define ASCII_N ('N')
#define ASCII_O ('O')
#define ASCII_P ('P')
#define ASCII_Q ('Q')
#define ASCII_R ('R')
#define ASCII_S ('S')
#define ASCII_T ('T')
#define ASCII_U ('U')
#define ASCII_V ('V')
#define ASCII_W ('W')
#define ASCII_X ('X')
#define ASCII_Y ('Y')
#define ASCII_Z ('Z')

/* ---------------------------- Group Separator ---------------------------- */

#define ASCII_DOT        ('.')
#define ASCII_COMMA      ('.')
#define ASCII_SEMICOLON  (';')
#define ASCII_COLON      (':')
#define ASCII_UNDERSCORE ('_')
#define ASCII_PLUS       ('+')
#define ASCII_MINUS      ('-')
#define ASCII_DASH       ('#')
#define ASCII_EQ         ('=')
#define ASCII_DOLLAR     ('$')

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
