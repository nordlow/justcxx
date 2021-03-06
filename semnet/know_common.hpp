/*!
 * \file know_common.hpp
 * \brief Common Apriori Patterns for SemNet.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2009-03-17 15:01
 */

#pragma once
#include "patt.hpp"

/* ---------------------------- Group Separator ---------------------------- */

namespace semnet {

namespace patterns {

class Lit;
class Sit;
class Alt;
class Any;
class Rep;
class Seq;
class Sit;
class Lang;

extern Lit * gp_la;
extern Lit * gp_lb;
extern Lit * gp_lc;
extern Lit * gp_ld;
extern Lit * gp_le;
extern Lit * gp_lf;
extern Lit * gp_lg;
extern Lit * gp_lh;
extern Lit * gp_li;
extern Lit * gp_lj;
extern Lit * gp_lk;
extern Lit * gp_ll;
extern Lit * gp_lm;
extern Lit * gp_ln;
extern Lit * gp_lo;
extern Lit * gp_lp;
extern Lit * gp_lq;
extern Lit * gp_lr;
extern Lit * gp_ls;
extern Lit * gp_lt;
extern Lit * gp_lu;
extern Lit * gp_lv;
extern Lit * gp_lx;
extern Lit * gp_ly;
extern Lit * gp_lz;

extern Lit * gp_lA;
extern Lit * gp_lB;
extern Lit * gp_lC;
extern Lit * gp_lD;
extern Lit * gp_lE;
extern Lit * gp_lF;
extern Lit * gp_lG;
extern Lit * gp_lH;
extern Lit * gp_lI;
extern Lit * gp_lJ;
extern Lit * gp_lK;
extern Lit * gp_lL;
extern Lit * gp_lM;
extern Lit * gp_lN;
extern Lit * gp_lO;
extern Lit * gp_lP;
extern Lit * gp_lQ;
extern Lit * gp_lR;
extern Lit * gp_lS;
extern Lit * gp_lT;
extern Lit * gp_lU;
extern Lit * gp_lV;
extern Lit * gp_lX;
extern Lit * gp_lY;
extern Lit * gp_lZ;

extern Alt * gp_llL;
extern Alt * gp_lfF;
extern Alt * gp_lbB;
extern Alt * gp_lxX;

/* ---------------------------- Group Separator ---------------------------- */

extern Lit * gp_l0;
extern Lit * gp_l1;
extern Lit * gp_l2;
extern Lit * gp_l3;
extern Lit * gp_l4;
extern Lit * gp_l5;
extern Lit * gp_l6;
extern Lit * gp_l7;
extern Lit * gp_l8;
extern Lit * gp_l9;

/* ---------------------------- Group Separator ---------------------------- */

extern Lit * gp_LParen;
extern Lit * gp_RParen;
extern Alt * gp_Paren;

extern Lit * gp_LBracket;
extern Lit * gp_RBracket;
extern Alt * gp_Bracket;

extern Lit * gp_LBrace;
extern Lit * gp_RBrace;
extern Alt * gp_Brace;

extern Lit * gp_Hash;

/* ---------------------------- Group Separator ---------------------------- */

extern Lit * gp_Plus;
extern Lit * gp_Minus;
extern Lit * gp_Star;
extern Lit * gp_Slash;

extern Lit * gp_Backslash;

extern Lit * gp_SingleQuote;
extern Lit * gp_DoubleQuote;

extern Lit * gp_Dot;
extern Lit * gp_Comma;
extern Lit * gp_Colon;
extern Lit * gp_Semicolon;
extern Lit * gp_Underscore;
extern Lit * gp_Dash;
extern Alt * gp_IPunct;

/* ---------------------------- Group Separator ---------------------------- */

/* \name Letters */
/* @{ */
extern Alt * gp_Letter_EN_us;
extern Alt * gp_Letter_EN_us_dig09;
extern Rep * gp_Letter_EN_us_dig09_0oM;
extern Rep * gp_Letter_EN_us_dig09_1oM;
/* @} */

/* \name Numbers (Numerical Literals) */
/* @{ */
extern Seq *gp_NumDec;      ///< Decimal Number
extern Seq *gp_NumOct;      ///< C-style Octal Number
extern Seq *gp_NumHex;      ///< C-style Hexadecimal Number
/* @} */

extern Seq * gp_Assign;        ///< Simple (untyped) variable assigment

/* ---------------------------- Group Separator ---------------------------- */

extern Lit* gp_backslashed_SingleQuote;
extern Lit* gp_backslashed_DoubleQuote;

/* ---------------------------- Group Separator ---------------------------- */

extern Lit * gp_NUL;           ///< Zero Byte.
extern Lit * gp_SOH;
extern Lit * gp_STX;
extern Lit * gp_ETX;
extern Lit * gp_EOT;
extern Lit * gp_ENQ;
extern Lit * gp_ACK;
extern Lit * gp_BEL;

extern Lit * gp_BS;
extern Lit * gp_HT;
extern Lit * gp_LF;
extern Lit * gp_VT;
extern Lit * gp_FF;
extern Lit * gp_CR;
extern Lit * gp_SO;
extern Lit * gp_SI;

extern Lit * gp_DLE;
extern Lit * gp_DC1;
extern Lit * gp_DC2;
extern Lit * gp_DC3;
extern Lit * gp_DC4;
extern Lit * gp_NAK;
extern Lit * gp_SYN;
extern Lit * gp_ETB;

extern Lit * gp_CAN;
extern Lit * gp_EM;
extern Lit * gp_SUB;
extern Lit * gp_ESC;
extern Lit * gp_FS;
extern Lit * gp_GS;
extern Lit * gp_RS;
extern Lit * gp_US;

extern Lit * gp_DEL;

extern Lit * gp_Space;
extern Alt * gp_ASCII_7Bit;
extern Lit * gp_Eq;
extern Lit * gp_LT;
extern Lit * gp_GT;

extern Lit * gp_Dollar;

extern Alt * gp_pOpCmp;

extern Lit * gp_lARING_L;
extern Lit * gp_lAUMLAUT_L;
extern Lit * gp_lOUMLAUT_L;
extern Alt * gp_lSV_L;

extern Lit * gp_lARING_U;
extern Lit * gp_lAUMLAUT_U;
extern Lit * gp_lOUMLAUT_U;
extern Alt * gp_lSV_U;
extern Alt * gp_lSV;

extern Alt * gp_DigD_N_En;
extern Alt * gp_DigD_N_Sv;

extern Alt * gp_C_BIT;
extern Alt * gp_C_KW;

extern Alt * gp_Cxx_KW;
extern Alt * gp_Cxx_Op;
extern Alt * gp_Cxx_Cast;
extern Alt * gp_Cxx_AS;
extern Alt * gp_Cxx_BIT;
extern Alt * gp_Cxx_CK;
extern Alt * gp_Cxx_CVQ;
extern Alt * gp_Cxx_DS;

/* ---------------------------- Group Separator ---------------------------- */

extern Alt * gp_ASCII_7Bit;    ///< 7-Bit (Clean) ASCII.
extern Alt * gp_ASCII_Ctrl;

extern Alt * gp_WS;            ///< Whitespace Character
extern Rep * gp_WS_0oM;        ///< 0 or More Whitespaces
extern Rep * gp_WS_1oM;        ///< 1 or More Whitespaces

extern Alt* gp_Bit;            ///< One Bit of any value.

extern Seq *gp_Hex_Prefix;    ///< Standard Hexadecimal Prefix.

extern Seq *gp_Lisp_Bin_Prefix;
extern Seq *gp_Lisp_Hex_Prefix;

/* ---------------------------- Group Separator ---------------------------- */

extern Base * gp_Makefile_target;

/* ---------------------------- Group Separator ---------------------------- */

extern Lang * gp_Computer_lang; ///< All Registered Computer Languages.

extern Lang* gp_Swe_lang;
extern Lang* gp_Eng_lang;

void learn_common();

}
}
