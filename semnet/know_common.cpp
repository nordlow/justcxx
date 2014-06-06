#include "preg.hpp"
#include "know_common.hpp"
#include "know_langC.hpp"
#include "know_langCxx.hpp"
#include "know_dfmts.hpp"
#include "alt.hpp"
#include "ran.hpp"
#include "seq.hpp"
#include "lit.hpp"
#include "sit.hpp"
#include "rep.hpp"
#include "any.hpp"
#include "filetype.hpp"
#include "dirtype.hpp"
#include "../ASCII.h"
#include "pmatchers.hpp"
#include "../make_array.hpp"
#include "lang.hpp"

/* ---------------------------- Group Separator ---------------------------- */

namespace semnet {

namespace patterns {

Lit * gp_la = nullptr;
Lit * gp_lb = nullptr;
Lit * gp_lc = nullptr;
Lit * gp_ld = nullptr;
Lit * gp_le = nullptr;
Lit * gp_lf = nullptr;
Lit * gp_lg = nullptr;
Lit * gp_lh = nullptr;
Lit * gp_li = nullptr;
Lit * gp_lj = nullptr;
Lit * gp_lk = nullptr;
Lit * gp_ll = nullptr;
Lit * gp_lm = nullptr;
Lit * gp_ln = nullptr;
Lit * gp_lo = nullptr;
Lit * gp_lp = nullptr;
Lit * gp_lq = nullptr;
Lit * gp_lr = nullptr;
Lit * gp_ls = nullptr;
Lit * gp_lt = nullptr;
Lit * gp_lu = nullptr;
Lit * gp_lv = nullptr;
Lit * gp_lx = nullptr;
Lit * gp_ly = nullptr;
Lit * gp_lz = nullptr;

Lit * gp_lA = nullptr;
Lit * gp_lB = nullptr;
Lit * gp_lC = nullptr;
Lit * gp_lD = nullptr;
Lit * gp_lE = nullptr;
Lit * gp_lF = nullptr;
Lit * gp_lG = nullptr;
Lit * gp_lH = nullptr;
Lit * gp_lI = nullptr;
Lit * gp_lJ = nullptr;
Lit * gp_lK = nullptr;
Lit * gp_lL = nullptr;
Lit * gp_lM = nullptr;
Lit * gp_lN = nullptr;
Lit * gp_lO = nullptr;
Lit * gp_lP = nullptr;
Lit * gp_lQ = nullptr;
Lit * gp_lR = nullptr;
Lit * gp_lS = nullptr;
Lit * gp_lT = nullptr;
Lit * gp_lU = nullptr;
Lit * gp_lV = nullptr;
Lit * gp_lX = nullptr;
Lit * gp_lY = nullptr;
Lit * gp_lZ = nullptr;

Alt * gp_llL = nullptr;
Alt * gp_lfF = nullptr;
Alt * gp_lbB = nullptr;
Alt * gp_lxX = nullptr;

/* ---------------------------- Group Separator ---------------------------- */

Lit * gp_l0 = nullptr;
Lit * gp_l1 = nullptr;
Lit * gp_l2 = nullptr;
Lit * gp_l3 = nullptr;
Lit * gp_l4 = nullptr;
Lit * gp_l5 = nullptr;
Lit * gp_l6 = nullptr;
Lit * gp_l7 = nullptr;
Lit * gp_l8 = nullptr;
Lit * gp_l9 = nullptr;

/* ---------------------------- Group Separator ---------------------------- */

Alt * gp_laf = nullptr;
Alt * gp_lAF = nullptr;
Alt * gp_lHex = nullptr;

/* ---------------------------- Group Separator ---------------------------- */

Lit * gp_LParen = nullptr;
Lit * gp_RParen = nullptr;
Alt * gp_Paren = nullptr;

Lit * gp_LBracket = nullptr;
Lit * gp_RBracket = nullptr;
Alt * gp_Bracket = nullptr;

Lit * gp_LBrace = nullptr;
Lit * gp_RBrace = nullptr;
Alt * gp_Brace = nullptr;

Lit * gp_Hash = nullptr;

/* ---------------------------- Group Separator ---------------------------- */

Lit * gp_Plus = nullptr;
Lit * gp_Minus = nullptr;
Lit * gp_Star = nullptr;
Lit * gp_Slash = nullptr;

Lit * gp_Backslash = nullptr;
Lit * gp_SingleQuote = nullptr;
Lit * gp_DoubleQuote = nullptr;

Lit * gp_Dot = nullptr;
Lit * gp_Comma = nullptr;
Lit * gp_Colon = nullptr;
Lit * gp_Semicolon = nullptr;
Lit * gp_Underscore = nullptr;
Lit * gp_Dash = nullptr;
Alt * gp_IPunct = nullptr;

/* ---------------------------- Group Separator ---------------------------- */

Lit* gp_backslashed_SingleQuote = nullptr;
Lit* gp_backslashed_DoubleQuote = nullptr;

/* ---------------------------- Group Separator ---------------------------- */

Alt * gp_a_to_z = nullptr;
Alt * gp_A_to_Z = nullptr;

Alt * gp_Letter_EN = nullptr;

Alt * gp_Letter_EN_us = nullptr;
Alt * gp_Letter_EN_us_dig09 = nullptr;
Rep * gp_Letter_EN_us_dig09_0oM = nullptr;
Rep * gp_Letter_EN_us_dig09_1oM = nullptr;

/* ---------------------------- Group Separator ---------------------------- */

Alt * gp_DigB = nullptr;
Alt * gp_DigO = nullptr;
Alt * gp_DigD = nullptr;
Alt * gp_DigH = nullptr;

Alt * gp_Dig1t9 = nullptr;

/* ---------------------------- Group Separator ---------------------------- */

Seq *gp_NumDec = nullptr;
Seq *gp_NumOct = nullptr;
Seq *gp_NumHex = nullptr;

/* ---------------------------- Group Separator ---------------------------- */

Alt * gp_ASCII_Ctrl = nullptr;

Alt * gp_WS = nullptr;
Rep * gp_WS_0oM = nullptr;
Rep * gp_WS_1oM = nullptr;

Alt* gp_Bit = nullptr;

/* ---------------------------- Group Separator ---------------------------- */

Rep *gp_DigD_0o2 = nullptr;    ///< 0 or 2 Decimal Digits
Rep *gp_DigD_1o2 = nullptr;    ///< 1 or 2 Decimal Digits

/* Octal Digits */
Rep *gp_DigO_1o3 = nullptr;    ///< 1 or 3 Octal Digits

/* Hexadecimal Digits */
Rep *gp_DigH_1o2 = nullptr; ///< 1 or 2 Hexadecimal Digits (1 byte)
Rep *gp_DigH_3o4 = nullptr; ///< 3 or 4 Hexadecimal Digits (2 bytes)
Rep *gp_DigH_5o6 = nullptr; ///< 5 or 6 Hexadecimal Digits (3 bytes)
Rep *gp_DigH_7o8 = nullptr; ///< 7 or 8 Hexadecimal Digits (4 bytes)

Seq *gp_Bin_Prefix = nullptr;
Seq *gp_Hex_Prefix = nullptr;
Seq *gp_Lisp_Bin_Prefix = nullptr;
Seq *gp_Lisp_Hex_Prefix = nullptr;

Lang * gp_Computer_lang = nullptr;

Seq * gp_Assign = nullptr;

/* ---------------------------- Group Separator ---------------------------- */

Lit * gp_Byte0 = nullptr;

/* ---------------------------- Group Separator ---------------------------- */

Lit * gp_NUL = nullptr;
Lit * gp_SOH = nullptr;
Lit * gp_STX = nullptr;
Lit * gp_ETX = nullptr;
Lit * gp_EOT = nullptr;
Lit * gp_ENQ = nullptr;
Lit * gp_ACK = nullptr;
Lit * gp_BEL = nullptr;

Lit * gp_BS = nullptr;
Lit * gp_HT = nullptr;
Lit * gp_LF = nullptr;
Lit * gp_VT = nullptr;
Lit * gp_FF = nullptr;
Lit * gp_CR = nullptr;
Lit * gp_SO = nullptr;
Lit * gp_SI = nullptr;

Lit * gp_DLE = nullptr;
Lit * gp_DC1 = nullptr;
Lit * gp_DC2 = nullptr;
Lit * gp_DC3 = nullptr;
Lit * gp_DC4 = nullptr;
Lit * gp_NAK = nullptr;
Lit * gp_SYN = nullptr;
Lit * gp_ETB = nullptr;

Lit * gp_CAN = nullptr;
Lit * gp_EM = nullptr;
Lit * gp_SUB = nullptr;
Lit * gp_ESC = nullptr;
Lit * gp_FS = nullptr;
Lit * gp_GS = nullptr;
Lit * gp_RS = nullptr;
Lit * gp_US = nullptr;

Lit * gp_DEL = nullptr;

Lit * gp_Space = nullptr;
Alt * gp_ASCII_7Bit = nullptr;
Lit * gp_Eq = nullptr;
Lit * gp_LT = nullptr;
Lit * gp_GT = nullptr;

Lit * gp_Dollar = nullptr;

Alt * gp_pOpCmp = nullptr;

Lit * gp_lARING_L = nullptr;
Lit * gp_lAUMLAUT_L = nullptr;
Lit * gp_lOUMLAUT_L = nullptr;
Alt * gp_lSV_L = nullptr;

Lit * gp_lARING_U = nullptr;
Lit * gp_lAUMLAUT_U = nullptr;
Lit * gp_lOUMLAUT_U = nullptr;
Alt * gp_lSV_U = nullptr;
Alt * gp_lSV = nullptr;

Alt * gp_DigD_N_En = nullptr;
Alt * gp_DigD_N_Sv = nullptr;

Alt * gp_C_BIT = nullptr;
Alt * gp_C_KW = nullptr;

Alt * gp_Cxx_KW = nullptr;
Alt * gp_Cxx_Op = nullptr;
Alt * gp_Cxx_Cast = nullptr;
Alt * gp_Cxx_AS = nullptr;
Alt * gp_Cxx_BIT = nullptr;
Alt * gp_Cxx_CK = nullptr;
Alt * gp_Cxx_CVQ = nullptr;
Alt * gp_Cxx_DS = nullptr;

Lang* gp_Swe_lang = nullptr;
Lang* gp_Eng_lang = nullptr;

/* ---------------------------- Group Separator ---------------------------- */

void learn_common()
{
    using namespace semnet::filesystem::gen;
    using namespace semnet::patterns::gen;

    // load these first so we can override or fintune them afterwards
    PTODO("Load pPatts from magic_dir_path\n");

    /* --- ASCII Control Codes --- */

    (gp_ASCII_Ctrl = alt())->set_name("ASCII Control Code");

    gp_Byte0 = new Lit(0, "Zero Byte"); gp_ASCII_Ctrl->add(gp_Byte0);

    gp_NUL = new Lit(ASCII_NUL, "Null"); gp_ASCII_Ctrl->add(gp_NUL);
    gp_SOH = new Lit(ASCII_SOH, "Start of heading (A)"); gp_ASCII_Ctrl->add(gp_SOH);
    gp_STX = new Lit(ASCII_STX, "Start of text (B)"); gp_ASCII_Ctrl->add(gp_STX);
    gp_ETX = new Lit(ASCII_ETX, "End of text (C)"); gp_ASCII_Ctrl->add(gp_ETX);
    gp_EOT = new Lit(ASCII_EOT, "End of transmit (D)"); gp_ASCII_Ctrl->add(gp_EOT);
    gp_ENQ = new Lit(ASCII_ENQ, "Enquiry (E)"); gp_ASCII_Ctrl->add(gp_ENQ);
    gp_ACK = new Lit(ASCII_ACK, "Acknowledge (F)"); gp_ASCII_Ctrl->add(gp_ACK);
    gp_BEL = new Lit(ASCII_BEL, "Audible bell (G)"); gp_ASCII_Ctrl->add(gp_BEL);

    gp_BS  = new Lit(ASCII_BS, "Backspace (H)"); gp_ASCII_Ctrl->add(gp_BS);
    gp_HT  = new Lit(ASCII_HT, "Horizontal tab (I)"); gp_ASCII_Ctrl->add(gp_HT);
    gp_LF  = new Lit(ASCII_LF, "Line feed (newline) (J)"); gp_ASCII_Ctrl->add(gp_LF);
    gp_VT  = new Lit(ASCII_VT, "Vertical tab (K)"); gp_ASCII_Ctrl->add(gp_VT);
    gp_FF  = new Lit(ASCII_FF, "Form feed, NP (L)"); gp_ASCII_Ctrl->add(gp_FF);
    gp_CR  = new Lit(ASCII_CR, "Carriage return (M)"); gp_ASCII_Ctrl->add(gp_CR);
    gp_SO  = new Lit(ASCII_SO, "Shift out (N)"); gp_ASCII_Ctrl->add(gp_SO);
    gp_SI  = new Lit(ASCII_SI, "Shift in (O)"); gp_ASCII_Ctrl->add(gp_SI);

    gp_DLE = new Lit(ASCII_DLE, "Data link escape (P)"); gp_ASCII_Ctrl->add(gp_DLE);
    gp_DC1 = new Lit(ASCII_DC1, "Device control 1, XON (Q)"); gp_ASCII_Ctrl->add(gp_DC1);
    gp_DC2 = new Lit(ASCII_DC2, "Device control 2 (R)"); gp_ASCII_Ctrl->add(gp_DC2);
    gp_DC3 = new Lit(ASCII_DC3, "Device control 3, XOFF (S)"); gp_ASCII_Ctrl->add(gp_DC3);
    gp_DC4 = new Lit(ASCII_DC4, "Device control 4 (T)"); gp_ASCII_Ctrl->add(gp_DC4);
    gp_NAK = new Lit(ASCII_NAK, "Negative acknowledge (U)"); gp_ASCII_Ctrl->add(gp_NAK);
    gp_SYN = new Lit(ASCII_SYN, "Synchronous idle (V)"); gp_ASCII_Ctrl->add(gp_SYN);
    gp_ETB = new Lit(ASCII_ETB, "End transmit block (W)"); gp_ASCII_Ctrl->add(gp_ETB);

    gp_CAN = new Lit(ASCII_CAN, "Cancel (X)"); gp_ASCII_Ctrl->add(gp_CAN);
    gp_EM  = new Lit(ASCII_EM, "End of medium (Y)"); gp_ASCII_Ctrl->add(gp_EM);
    gp_SUB = new Lit(ASCII_SUB, "Substitution (Z)"); gp_ASCII_Ctrl->add(gp_SUB);
    gp_ESC = new Lit(ASCII_ESC, "Escape ( )"); gp_ASCII_Ctrl->add(gp_ESC);
    gp_FS  = new Lit(ASCII_FS, "File separator ( )"); gp_ASCII_Ctrl->add(gp_FS);
    gp_GS  = new Lit(ASCII_GS, "Group separator ( )"); gp_ASCII_Ctrl->add(gp_GS);
    gp_RS  = new Lit(ASCII_RS, "Record separator ( )"); gp_ASCII_Ctrl->add(gp_RS);
    gp_US  = new Lit(ASCII_US, "Unit separator ( )"); gp_ASCII_Ctrl->add(gp_US);

    gp_DEL = new Lit(ASCII_DEL, "Delete"); gp_ASCII_Ctrl->add(gp_DEL);

    (gp_ASCII_7Bit = alt(gp_NUL, gp_DEL, true))->set_name("ASCII Character");

    gp_Eq = new Lit(ASCII_EQ, "Equality Sign");

    gp_Dollar = new Lit(ASCII_DOLLAR, "Equality Dollar");

    gp_Space = new Lit(ASCII_SPACE, "Space");

    /* --- ASCII Whitespace --- */
    (gp_WS = alt(gp_Space, gp_HT, gp_LF, gp_VT, gp_CR))->set_name("Whitespace");
    (gp_WS_0oM = rep0oM(gp_WS))->set_name("Whitespace 0oM");
    (gp_WS_1oM = rep1oM(gp_WS))->set_name("Whitespace 1oM");

    /* --- ASCII Interpunct --- */

    gp_Dot = new Lit(ASCII_DOT, "Dot");
    gp_Comma = new Lit(ASCII_COMMA, "Comma");
    gp_Colon = new Lit(ASCII_COLON, "Colon");
    gp_Semicolon = new Lit(ASCII_SEMICOLON, "Semicolon");
    gp_Underscore = new Lit(ASCII_UNDERSCORE, "Underscore");
    gp_Dash = new Lit(ASCII_DASH, "Dash");

    (gp_IPunct = alt(gp_Dot, gp_Comma, gp_Colon, gp_Semicolon, gp_Underscore, gp_Dash))->set_name("Interpunctuation Operators");


    /* --- ASCII Operators --- */

    gp_Plus = new Lit('\\', "Plus");
    gp_Minus = new Lit('\\', "Minus");
    gp_Star = new Lit('\\', "Star");
    gp_Slash = new Lit('\\', "Slash");

    gp_Backslash = new Lit('\\', "Backslash");

    gp_SingleQuote = new Lit('\'', "SingleQuote");
    gp_DoubleQuote = new Lit('"', "DoubleQuote");

    /* --- ASCII Brackets --- */

    gp_LBracket = new Lit('[', "Left Bracket");
    gp_RBracket = new Lit(']', "Left Bracket");

    (gp_Bracket = alt(gp_LBracket, gp_RBracket))->set_name("Left or Right Bracket");

    /* --- ASCII Brackets --- */

    gp_LT = new Lit('<', "Left Than");
    gp_GT = new Lit('>', "Larger Than");

    (gp_pOpCmp = alt(gp_LT, gp_GT))->set_name("Comparison Operators");


    /* --- ASCII Parens --- */

    gp_LParen = new Lit('(', "Left Parenthesis");
    gp_RParen = new Lit(')', "Left Parenthesis");

    (gp_Paren = alt(gp_LParen, gp_RParen))->set_name("Parenthesis");


    /* --- ASCII Braces --- */

    gp_LBrace = new Lit('{', "Left Brace");
    gp_RBrace = new Lit('}', "Left Brace");

    (gp_Brace = alt(gp_LBrace, gp_RBrace))->set_name("Brace");


    /* --- Others --- */

    gp_Hash = new Lit('#', "Hash");

    /* --- Letters --- */

    gp_la = new Lit('a', "Lower Letter a");
    gp_lb = new Lit('b', "Lower Letter b");
    gp_lc = new Lit('c', "Lower Letter c");
    gp_ld = new Lit('d', "Lower Letter d");
    gp_le = new Lit('e', "Lower Letter e");
    gp_lf = new Lit('f', "Lower Letter f");
    gp_lg = new Lit('g', "Lower Letter g");
    gp_lh = new Lit('h', "Lower Letter h");
    gp_li = new Lit('i', "Lower Letter i");
    gp_lj = new Lit('j', "Lower Letter j");
    gp_lk = new Lit('k', "Lower Letter k");
    gp_ll = new Lit('l', "Lower Letter l");
    gp_lm = new Lit('m', "Lower Letter m");
    gp_ln = new Lit('n', "Lower Letter n");
    gp_lo = new Lit('o', "Lower Letter o");
    gp_lp = new Lit('p', "Lower Letter p");
    gp_lq = new Lit('q', "Lower Letter q");
    gp_lr = new Lit('r', "Lower Letter r");
    gp_ls = new Lit('s', "Lower Letter s");
    gp_lt = new Lit('t', "Lower Letter t");
    gp_lu = new Lit('u', "Lower Letter u");
    gp_lv = new Lit('v', "Lower Letter v");
    gp_lx = new Lit('x', "Lower Letter x");
    gp_ly = new Lit('y', "Lower Letter y");
    gp_lz = new Lit('z', "Lower Letter z");

    gp_lA = new Lit('A', "Upper Letter A");
    gp_lB = new Lit('B', "Upper Letter B");
    gp_lC = new Lit('C', "Upper Letter C");
    gp_lD = new Lit('D', "Upper Letter D");
    gp_lE = new Lit('E', "Upper Letter E");
    gp_lF = new Lit('F', "Upper Letter F");
    gp_lG = new Lit('G', "Upper Letter G");
    gp_lH = new Lit('H', "Upper Letter H");
    gp_lI = new Lit('I', "Upper Letter I");
    gp_lJ = new Lit('J', "Upper Letter J");
    gp_lK = new Lit('K', "Upper Letter K");
    gp_lL = new Lit('L', "Upper Letter L");
    gp_lM = new Lit('M', "Upper Letter M");
    gp_lN = new Lit('N', "Upper Letter N");
    gp_lO = new Lit('O', "Upper Letter O");
    gp_lP = new Lit('P', "Upper Letter P");
    gp_lQ = new Lit('Q', "Upper Letter Q");
    gp_lR = new Lit('R', "Upper Letter R");
    gp_lS = new Lit('S', "Upper Letter S");
    gp_lT = new Lit('T', "Upper Letter T");
    gp_lU = new Lit('U', "Upper Letter U");
    gp_lV = new Lit('V', "Upper Letter V");
    gp_lX = new Lit('X', "Upper Letter X");
    gp_lY = new Lit('Y', "Upper Letter Y");
    gp_lZ = new Lit('Z', "Upper Letter Z");

    /* --- Case-Insensitive Letters --- */

    gp_llL = alt(gp_ll, gp_lL);
    gp_lfF = alt(gp_lf, gp_lF);
    gp_lbB = alt(gp_lB, gp_lB);
    gp_lxX = alt(gp_lx, gp_lX);

    /* --- Digits --- */

    gp_l0 = new Lit('0', "Digit 0");
    gp_l1 = new Lit('1', "Digit 1");
    gp_l2 = new Lit('2', "Digit 2");
    gp_l3 = new Lit('3', "Digit 3");
    gp_l4 = new Lit('4', "Digit 4");
    gp_l5 = new Lit('5', "Digit 5");
    gp_l6 = new Lit('6', "Digit 6");
    gp_l7 = new Lit('7', "Digit 7");
    gp_l8 = new Lit('8', "Digit 8");
    gp_l9 = new Lit('9', "Digit 9");

    /* --- Ranges of Letters --- */

    (gp_laf = ran(gp_la, gp_lf, true))->set_name("Letter a to f")->set_pmatch(cbuf_lmatch_LETTER_a_to_f);
    (gp_lAF = ran(gp_lA, gp_lF, true))->set_name("Letter A to F")->set_pmatch(cbuf_lmatch_LETTER_A_to_F);

    /* --- Ranges --- */

    (gp_lHex = alt(gp_laf, gp_lAF))->set_name("Hexadecimal Letter")->set_pmatch(cbuf_lmatch_LETTER_HEX);

    /* --- Swedish Letters --- */

    /* - Swedish Lower Letters - */

    gp_lARING_L = new Lit(ISO8859_ARING_L, "Swedish Only Lower Letter å");
    gp_lAUMLAUT_L = new Lit(ISO8859_AUMLAUT_L, "Swedish Only Lower Letter ä");
    gp_lOUMLAUT_L = new Lit(ISO8859_OUMLAUT_L, "Swedish Only Lower Letter ö");

    (gp_lSV_L = alt(gp_lARING_L, gp_lAUMLAUT_L, gp_lOUMLAUT_L))->set_name("Swedish Only Lower Letter");

    /* - Swedish Upper Letters - */

    gp_lARING_U = new Lit(ISO8859_ARING_U, "Swedish Only Upper Letter Å");
    gp_lAUMLAUT_U = new Lit(ISO8859_AUMLAUT_U, "Swedish Only Upper Letter Ä");
    gp_lOUMLAUT_U = new Lit(ISO8859_OUMLAUT_U, "Swedish Only Upper Letter Ö");

    (gp_lSV_U = alt(gp_lARING_U, gp_lAUMLAUT_U, gp_lOUMLAUT_U))->set_name("Swedish Only Upper Letter");


    /* - Swedish Lower and Upper Letters - */

    (gp_lSV = alt(gp_lSV_L, gp_lSV_U))->
        set_name("Swedish Only Lower or Upper Letter")->
        set_pmatch(cbuf_lmatch_LETTER_ISO8859_SVonly);

    gp_Swe_lang = lang("Swedish", gp_lSV);

    gp_backslashed_SingleQuote = new Lit("\\\'");
    gp_backslashed_DoubleQuote = new Lit("\\\"");

    /* --- Ranges --- */

    (gp_a_to_z = ran(gp_la, gp_lz, true))->
        set_name("English Lower Letter")->
        set_pmatch(cbuf_lmatch_LETTER_EN_LOWER);

    (gp_A_to_Z = ran(gp_lA, gp_lZ, true))->
        set_name("English Upper Letter")->
        set_pmatch(cbuf_lmatch_LETTER_EN_UPPER);

    gp_Eng_lang = lang("English",
                       ((gp_Letter_EN = alt(gp_a_to_z, gp_A_to_Z))->
                        set_name("English Letter")->
                        set_pmatch(cbuf_lmatch_LETTER_EN)));

    /* --- Digit Name --- */

    (gp_DigD_N_En = alt())->
        set_name("Decimal Digit Name");

    Lit *digA_EN[10];               /* digit array */
    const char *digN_EN[10] = {
        "Zero", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine"
    };
    for (size_t i = 0; i < 10; i++) {
        Lit * dX = digA_EN[i] = new Lit('0'+i, digN_EN[i]);
        gp_DigD_N_En->add(dX);
    }
    (gp_DigD_N_Sv = alt())->
        set_name("Decimal Digit Name");

    Lit *digA_SV[10];               /* digit array */
    const char *digN_SV[10] = {
        "Noll", "Ett", "Två", "Tree", "Fyra", "Fem", "Sex", "Sju", "Åtta", "Nio"
    };
    for (size_t i = 0; i < 10; i++) {
        Lit * dX = digA_SV[i] = new Lit('0'+i, digN_SV[i]);
        gp_DigD_N_Sv->add(dX);
    }

    /* --- Digits (Ranges) --- */

    (gp_DigB = alt(gp_l0, gp_l1, true))->set_name("Binary Digit")->set_pmatch(cbuf_lmatch_DIGIT_BIN);
    (gp_DigO = ran(gp_l0, gp_l7, true))->set_name("Octal Digit")->set_pmatch(cbuf_lmatch_DIGIT_OCT);
    (gp_DigD = ran(gp_l0, gp_l9, true))->set_name("Decimal Digit")->set_pmatch(cbuf_lmatch_DIGIT_DEC);

    (gp_DigH = alt(gp_DigD,
                   gp_laf,
                   gp_lAF))->set_name("HexaDecimal Digit");
    gp_DigH->set_pmatch(cbuf_lmatch_DIGIT_HEX);

    (gp_Dig1t9 = ran(gp_l1, gp_l9, true))->set_name("Digit 1 to 9");
    gp_Dig1t9->set_pmatch(cbuf_lmatch_DIGIT_BIN);

    /* --- Sequences of Digits --- */

    /* Decimal Digits */
    gp_DigD_0o2 = rep0o2(gp_DigD); /* 0 or 2 Decimal Digits */
    gp_DigD_1o2 = rep1o2(gp_DigD); /* 1 or 2 Decimal Digits */

    /* Octal Digits */
    gp_DigO_1o3 = rep1o3(gp_DigO); /* 1 or 3 Octal Digits */

    /* Hexadecimal Digits */
    gp_DigH_1o2 = rep1o2(gp_DigH); /* 1 or 2 Hexadecimal Digits (1 byte) */
    gp_DigH_3o4 = rep3o4(gp_DigH); /* 3 or 4 Hexadecimal Digits (2 bytes) */
    gp_DigH_5o6 = rep5o6(gp_DigH); /* 5 or 6 Hexadecimal Digits (3 bytes) */
    gp_DigH_7o8 = rep7o8(gp_DigH); /* 7 or 8 Hexadecimal Digits (4 bytes) */

    /* - Number Literals Prefixes - */
    gp_Bin_Prefix = seq(bos(), gp_l0, gp_lbB);
    gp_Hex_Prefix = seq(bos(), gp_l0, gp_lxX);
    (gp_Lisp_Bin_Prefix = seq(gp_Hash, gp_l0, gp_lbB))->set_name("Lisp-style Binary Integer Literal Prefix");
    (gp_Lisp_Hex_Prefix = seq(gp_Hash, gp_l0, gp_lxX))->set_name("Lisp-style Hexadecimal Integer Literal Prefix");

    /* - Number Literals Suffixes - */

    /* --- C-style Number Literals --- */

    gp_NumDec = seq(gp_Dig1t9, gp_DigD_0o2);
    gp_NumOct = seq(gp_l0, gp_DigO_1o3);
    gp_NumHex = seq(gp_Hex_Prefix, gp_DigH_1o2);

    /* Regexp: [a-zA-Z_] */
    (gp_Letter_EN_us = alt(gp_Letter_EN, gp_Underscore))->set_name("English Letter or Underscore");

    /* Regexp: [a-zA-Z_0-9] */
    (gp_Letter_EN_us_dig09 = alt(gp_Letter_EN_us, gp_DigD))->set_name("English Letter, Underscore or Decimal Digit");

    /* Regexp: [a-zA-Z_0-9]* */
    (gp_Letter_EN_us_dig09_0oM = rep0oM(gp_Letter_EN_us_dig09))->set_name("English Letter, Underscore or Decimal Digit 0oM");

    /* Regexp: [a-zA-Z_0-9]+ */
    (gp_Letter_EN_us_dig09_1oM = rep1oM(gp_Letter_EN_us_dig09))->set_name("English Letter, Underscore or Decimal Digit 1oM");

    /* C Builtin Type Keyword */
    const char *CBIT_cstrA[] = { "char", "long", "int", "short", "char", "float", "double" };
    const size_t CBIT_N = ARRAY_LENGTH(CBIT_cstrA);
    //auto CBIT = boost::make_array("char", "long", "int", "short", "char", "float", "double");
    (gp_C_BIT = alt_cstrA(CBIT_cstrA, nullptr, CBIT_N, "C Builtin Type Keyword"))->set_name("C Builtin Type Keyword");
    gp_C_BIT->set_dfmt(DFMT_C);
    /* gp_C_BIT->set_grammar_class("<keyword>"); */

    /* C++ Operators */
    const char *CxxOp_cstrA[] = {
        "->.",
        "and", "or", "not"
    };
    const size_t CxxOp_N = ARRAY_LENGTH(CxxOp_cstrA);
    // TODO: Replace with generalization of operator sub-categories.
    (gp_Cxx_Op = alt_cstrA(CxxOp_cstrA, nullptr, CxxOp_N, "C++ Operator"))->set_name("C++ Operators");
    gp_Cxx_Op->set_dfmt(DFMT_CXX);
    /* gp_Cxx_Op->set_grammar_class("<operator>"); */

    /* C++ Casters */
    const char *CxxCast_cstrA[] = {
        "static_cast",
        "const_cast",
        "reinterpret_cast",
        "dynamic_cast"
    };
    const size_t CxxCast_N = ARRAY_LENGTH(CxxCast_cstrA);
    (gp_Cxx_Cast = alt_cstrA(CxxCast_cstrA, nullptr, CxxCast_N, "C++ Cast Operator"))->set_name("C++ Cast Operators");
    gp_Cxx_Cast->set_dfmt(DFMT_CXX);
    /* gp_Cxx_Cast->set_grammar_class("<operator>"); */

    /* C++ Access Specifiers */
    const char *CxxAS_cstrA[] = { "private", "protected", "public" };
    const size_t CxxAS_N = ARRAY_LENGTH(CxxAS_cstrA);
    (gp_Cxx_AS = alt_cstrA(CxxAS_cstrA, nullptr, CxxAS_N, "C++ Access Specifier"))->set_name("C++ Access Specifiers");
    gp_Cxx_AS->set_dfmt(DFMT_CXX);
    /* gp_Cxx_AS->set_grammar_class("<keyword>"); */

    /* C++ Builtin Types */
    const char *CxxBIT_cstrA[] = { "bool", "char", "double", "float", "int", "long", "short",
                                   "signed", "unsigned", "void", "wchar_t" };
    const size_t CxxBIT_N = ARRAY_LENGTH(CxxBIT_cstrA);
    (gp_Cxx_BIT = alt_cstrA(CxxBIT_cstrA, nullptr, CxxBIT_N, "C++ Builtin Type"))->set_name("C++ Builtin Types");
    gp_Cxx_BIT->set_dfmt(DFMT_CXX);
    /* gp_Cxx_BIT->set_grammar_class("<built_in_id>"); */

    /* C++ Class Keys */
    const char *CxxCK_cstrA[] = { "class", "enum", "namespace", "struct", "typename", "union" };
    const size_t CxxCK_N = ARRAY_LENGTH(CxxCK_cstrA);
    (gp_Cxx_CK = alt_cstrA(CxxCK_cstrA, nullptr, CxxCK_N, "C++ Class Key"))->set_name("C++ Class Keys");
    gp_Cxx_CK->set_dfmt(DFMT_CXX);
    /* gp_Cxx_CK->set_grammar_class("<class_key>"); */

    /* C++ CV-Qualifiers */
    const char *CxxCVQ_cstrA[] = { "const", "volatile" };
    const size_t CxxCVQ_N = ARRAY_LENGTH(CxxCVQ_cstrA);
    (gp_Cxx_CVQ = alt_cstrA(CxxCVQ_cstrA, nullptr, CxxCVQ_N, "C++ Keyword Qualifier"))->set_name("C++ Keyword Qualifiers");
    gp_Cxx_CVQ->set_dfmt(DFMT_CXX);
    /* gp_Cxx_CVQ->set_grammar_class("<cv_qualifiers>"); */

    /* C++ Declaration Specifiers */
    const char *CxxDS_cstrA[] = { "auto", "explicit", "export", "extern", "friend", "inline", "mutable", "register",
                                  "static", "template", "typedef", "using", "virtual" };
    const size_t CxxDS_N = ARRAY_LENGTH(CxxDS_cstrA);
    (gp_Cxx_DS = alt_cstrA(CxxDS_cstrA, nullptr, CxxDS_N, "C++ Declaration Specifier"))->set_name("C++ Declaration Specifiers");
    gp_Cxx_DS->set_dfmt(DFMT_CXX);
    /* gp_Cxx_DS->set_grammar_class("<decl_specifier_id>"); */

    // =================================================

    /* C Keywords. \todo Reuse C Keyword Groups such as gp_C_BIT */
    const char *C_KW_cstrA[] = { "auto", "_Bool", "break", "case", "char", "_Complex",
                                "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if",
                                "_Imaginary", "inline", "int", "long", "register", "restrict", "return",
                                "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while" };
    const size_t C_KW_N = ARRAY_LENGTH(C_KW_cstrA);
    (gp_C_KW = alt_cstrA(C_KW_cstrA, nullptr, C_KW_N, "C Keyword"))->set_name("C Keywords");
    gp_C_KW->set_dfmt(DFMT_C);
    /* gp_C_KW->set_grammar_class("<keyword>"); */

    /* C++ Keywords. \todo Reuse C Keywords and C++ Specific Keyword Groups */
    const char *Cxx_KW_cstrA[] = { "private", "protected", "public",
                                  "bool", "char", "double", "float", "int", "long", "short",
                                  "signed", "unsigned", "void", "wchar_t",
                                  "class", "enum", "namespace", "struct", "typename", "union",
                                  "const", "volatile",
                                  "auto", "explicit", "export", "extern", "friend", "inline", "mutable", "register", "static",
                                  "template", "typedef", "using", "virtual",
                                  "asm", "break", "case", "catch", "const_cast", "continue", "default", "delete", "do", "dynamic_cast",
                                  "else", "false", "for", "goto", "if", "new", "operator", "reinterpret_cast", "return",
                                  "sizeof", "static_cast", "switch", "this", "throw", "true", "try", "typeid", "while",
    };
    const size_t Cxx_KW_N = ARRAY_LENGTH(Cxx_KW_cstrA);
    (gp_Cxx_KW = alt_cstrA(Cxx_KW_cstrA, nullptr, Cxx_KW_N, "C++ Keyword"))->set_name("C++ Keywords");
    gp_Cxx_KW->set_dfmt(DFMT_CXX);
    /* gp_Cxx_KW->set_grammar_class("<keyword>"); */

    // =================================================

    /* simple (untyped) variable assigment */
    (gp_Assign = seq(bol(),
                     gp_WS_0oM, /* whitespace */
                     gp_Letter_EN_us_dig09_1oM, /* word */
                     gp_WS_0oM, /* whitespace */
                     gp_Eq))->set_name("Simple Variable Assigment");
}

}
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 