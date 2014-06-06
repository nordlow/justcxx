#include "know_langC.hpp"
#include "know_common.hpp"
#include "patt.hpp"
#include "seq.hpp"
#include "sit.hpp"
#include "alt.hpp"
#include "rep.hpp"
#include "any.hpp"
#include "lit.hpp"
#include "lang.hpp"
#include "pmatchers.hpp"

/* ---------------------------- Group Separator ---------------------------- */

namespace semnet {
namespace patterns {

using namespace semnet::patterns::gen;

Alt* gp_EscSeq          = nullptr;

Lit* gp_Esc_Alert       = nullptr;
Lit* gp_Esc_Backspace   = nullptr;
Lit* gp_Esc_FormFeed    = nullptr;
Lit* gp_Esc_NewLine     = nullptr;
Lit* gp_Esc_CR          = nullptr;
Lit* gp_Esc_HT          = nullptr;
Lit* gp_Esc_VTFeed      = nullptr;
Lit* gp_Esc_SingleQuote = nullptr;
Lit* gp_Esc_DoubleQuote = nullptr;
Lit* gp_Esc_QMark       = nullptr;
Lit* gp_Esc_Backslash   = nullptr;
Lit* gp_Esc_Null        = nullptr;

// Numbers
Seq* gp_Esc_NumDec      = nullptr;
Seq* gp_Esc_NumOct      = nullptr;
Seq* gp_Esc_NumHex      = nullptr;

/* ---------------------------- Group Separator ---------------------------- */

namespace C {

Lit*   gp_LBrace                  = nullptr;
Lit*   gp_RBrace                  = nullptr;

Lit*   gp_LBracket                = nullptr;
Lit*   gp_RBracket                = nullptr;

Seq*   gp_ID                      = nullptr;
Seq*   gp_ID_GCC                  = nullptr;

Seq*   gp_STR                     = nullptr;
Alt*   gp_STR_CONTENTS_ATOM       = nullptr;
Rep*   gp_STR_CONTENTS            = nullptr;

Base  *gp_HEX_CONSTANT            = nullptr;
Base  *gp_OCT_CONSTANT            = nullptr;
Base  *gp_DEC_CONSTANT            = nullptr;
Base*  gp_CONSTANT                = nullptr;

Rep*   gp_STRING_LITERAL_CONTENTS = nullptr;
Seq*   gp_STRING_LITERAL          = nullptr;
Seq*   gp_SIZEOF                  = nullptr;

Rep*   gp_ISS                     = nullptr;

Alt*   gp_mul_ops                 = nullptr;
Lit*   gp_MUL_OP                  = nullptr;
Lit*   gp_DIV_OP                  = nullptr;
Lit*   gp_MOD_OP                  = nullptr;

Alt*   gp_add_ops                 = nullptr;
Lit*   gp_ADD_OP                  = nullptr;
Lit*   gp_SUB_OP                  = nullptr;

Lit*   gp_BITAND_OP               = nullptr;
Lit*   gp_BITOR_OP                = nullptr;
Lit*   gp_BITXOR_OP               = nullptr;

Alt*   gp_rel_ops                 = nullptr;
Lit*   gp_LT_OP                   = nullptr;
Lit*   gp_GT_OP                   = nullptr;
Lit*   gp_LE_OP                   = nullptr;
Lit*   gp_GE_OP                   = nullptr;

Lit*   gp_MIN_OP                  = nullptr;
Lit*   gp_MAX_OP                  = nullptr;

Lit*   gp_PTR_OP                  = nullptr;
Lit*   gp_INC_OP                  = nullptr;
Lit*   gp_DEC_OP                  = nullptr;

Alt*   gp_shift_ops               = nullptr;
Lit*   gp_LEFT_OP                 = nullptr;
Lit*   gp_RIGHT_OP                = nullptr;

Alt*   gp_eq_ops                  = nullptr;
Lit*   gp_EQ_OP                   = nullptr;
Lit*   gp_NE_OP                   = nullptr;

Lit*   gp_AND_OP                  = nullptr;
Lit*   gp_OR_OP                   = nullptr;

Alt*   gp_assignment_ops          = nullptr;
Lit*   gp_ASSIGN_OP               = nullptr;
Lit*   gp_MUL_ASSIGN_OP           = nullptr;
Lit*   gp_DIV_ASSIGN_OP           = nullptr;
Lit*   gp_MOD_ASSIGN_OP           = nullptr;
Lit*   gp_ADD_ASSIGN_OP           = nullptr;
Lit*   gp_SUB_ASSIGN_OP           = nullptr;
Lit*   gp_LEFT_ASSIGN_OP          = nullptr;
Lit*   gp_RIGHT_ASSIGN_OP         = nullptr;
Lit*   gp_AND_ASSIGN_OP           = nullptr;
Lit*   gp_XOR_ASSIGN_OP           = nullptr;
Lit*   gp_OR_ASSIGN_OP            = nullptr;
Lit*   gp_MIN_ASSIGN_OP           = nullptr;
Lit*   gp_MAX_ASSIGN_OP           = nullptr;

Seq*   gp_TYPEDEF                 = nullptr;
Seq*   gp_EXTERN                  = nullptr;
Seq*   gp_STATIC                  = nullptr;
Seq*   gp_AUTO                    = nullptr;
Seq*   gp_REGISTER                = nullptr;
Seq*   gp_INLINE                  = nullptr;
Seq*   gp_RESTRICT                = nullptr;

Seq*   gp_CHAR                    = nullptr;
Seq*   gp_SHORT                   = nullptr;
Seq*   gp_INT                     = nullptr;
Seq*   gp_LONG                    = nullptr;
Seq*   gp_SIGNED                  = nullptr;
Seq*   gp_UNSIGNED                = nullptr;
Seq*   gp_FLOAT                   = nullptr;
Seq*   gp_DOUBLE                  = nullptr;
Seq*   gp_CONST                   = nullptr;
Seq*   gp_VOLATILE                = nullptr;
Seq*   gp_VOID                    = nullptr;
Seq*   gp_BOOL                    = nullptr;
Seq*   gp_COMPLEX                 = nullptr;
Seq*   gp_IMAGINARY               = nullptr;
Seq*   gp_STRUCT                  = nullptr;
Seq*   gp_UNION                   = nullptr;
Seq*   gp_ENUM                    = nullptr;
Lit*   gp_ELLIPSIS                = nullptr;

Seq*   gp_Cxx_11_AUTO              = nullptr;

// Other
Seq*   gp_CASE                    = nullptr;
Seq*   gp_DEFAULT                 = nullptr;
Seq*   gp_IF                      = nullptr;
Seq*   gp_ELSE                    = nullptr;
Seq*   gp_SWITCH                  = nullptr;
Seq*   gp_WHILE                   = nullptr;
Seq*   gp_DO                      = nullptr;
Seq*   gp_FOR                     = nullptr;
Seq*   gp_GOTO                    = nullptr;
Seq*   gp_CONTINUE                = nullptr;
Seq*   gp_BREAK                   = nullptr;
Seq*   gp_RETURN                  = nullptr;

/* ---------------------------- Group Separator ---------------------------- */

// Parser Constructs
Alt*  gp_primary_expr                  = nullptr;
Alt*  gp_postfix_expr                  = nullptr;
Alt*  gp_arg_expr_list                 = nullptr;
Alt*  gp_unary_expr                    = nullptr;
Alt*  gp_unary_operator                = nullptr;
Alt*  gp_cast_expr                     = nullptr;
Alt*  gp_mult_expr                     = nullptr;
Alt*  gp_add_expr                      = nullptr;
Alt*  gp_shift_expr                    = nullptr;
Alt*  gp_rel_expr                      = nullptr;
Alt*  gp_eq_expr                       = nullptr;
Alt*  gp_and_expr                      = nullptr;
Alt*  gp_xor_expr                      = nullptr;
Alt*  gp_ior_expr                      = nullptr;
Alt*  gp_logical_and_expr              = nullptr;
Alt*  gp_logical_or_expr               = nullptr;
Alt*  gp_cond_expr                     = nullptr;
Alt*  gp_assignment_expr               = nullptr;
Alt*  gp_expr                          = nullptr;
Alt*  gp_constant_expr                 = nullptr;
Alt*  gp_declaration                   = nullptr;
Alt*  gp_declaration_specifiers        = nullptr;
Alt*  gp_init_declarator_list          = nullptr;
Alt*  gp_init_declarator               = nullptr;
Alt*  gp_storage_class_specifier       = nullptr;
Alt*  gp_type_specifier                = nullptr;
Alt*  gp_struct_or_union_specifier     = nullptr;
Alt*  gp_struct_or_union               = nullptr;
Alt*  gp_struct_declaration_list       = nullptr;
Seq*  gp_struct_declaration            = nullptr;
Alt*  gp_specifier_qualifier_list      = nullptr;
Alt*  gp_struct_declarator_list        = nullptr;
Alt*  gp_struct_declarator             = nullptr;
Alt*  gp_enum_specifier                = nullptr;
Alt*  gp_enumerator_list               = nullptr;
Alt*  gp_enumerator                    = nullptr;
Alt*  gp_type_qualifier                = nullptr;
Alt*  gp_function_specifier            = nullptr;
Alt*  gp_declarator                    = nullptr;
Alt*  gp_direct_declarator             = nullptr;
Alt*  gp_pointer                       = nullptr;
Alt*  gp_type_qualifier_list           = nullptr;
Alt*  gp_param_type_list               = nullptr;
Alt*  gp_param_list                    = nullptr;
Alt*  gp_param_declaration             = nullptr;
Alt*  gp_identifier_list               = nullptr;
Alt*  gp_type_name                     = nullptr;
Alt*  gp_abstract_declarator           = nullptr;
Alt*  gp_direct_abstract_declarator    = nullptr;
Alt*  gp_initializer                   = nullptr;
Alt*  gp_initializer_list              = nullptr;
Alt*  gp_designation                   = nullptr;
Alt*  gp_designator_list               = nullptr;
Alt*  gp_designator                    = nullptr;
Alt*  gp_stmt                          = nullptr;
Alt*  gp_labeled_stmt                  = nullptr;
Alt*  gp_compound_stmt                 = nullptr;
Alt*  gp_block_item_list               = nullptr;
Alt*  gp_block_item                    = nullptr;
Alt*  gp_expr_stmt                     = nullptr;
Alt*  gp_selection_stmt                = nullptr;
Alt*  gp_iteration_stmt                = nullptr;
Alt*  gp_jump_stmt                     = nullptr;
Alt*  gp_translation_unit              = nullptr;
Alt*  gp_external_declaration          = nullptr;
Alt*  gp_function_definition           = nullptr;
Rep*  gp_declaration_list              = nullptr;
Lang* gp_lang                          = nullptr;

Alt*  gp_not_Backslash_nor_DoubleQuote = nullptr;

Alt*  gp_bit_ops                       = nullptr;
Alt*  gp_step_ops                      = nullptr;
Alt*  gp_extreme_ops                   = nullptr;

/* ---------------------------- Group Separator ---------------------------- */

Lang* learn_C98_lang()
{
    if (gp_lang) { return gp_lang; }

    using namespace semnet::filesystem;
    using namespace semnet::patterns::gen;

    gp_mul_ops      = alt("Multiplication Operator");
    gp_MUL_OP       = gp_mul_ops->add(oplit('*', "Multiplication"));
    gp_DIV_OP       = gp_mul_ops->add(oplit('/', "Division"));
    gp_MOD_OP       = gp_mul_ops->add(oplit('%', "Modulus"));

    gp_add_ops      = alt("Additive Operator");
    gp_ADD_OP       = oplit('+', "Addition");
    gp_SUB_OP       = oplit('-', "Subtraction");

    gp_BITAND_OP    = oplit('&');
    gp_BITOR_OP     = oplit('|');
    gp_BITXOR_OP    = oplit('^');

    gp_shift_ops    = alt("Bitwise Shift Operator");
    gp_LEFT_OP      = gp_shift_ops->add(oplit("<<"));
    gp_RIGHT_OP     = gp_shift_ops->add(oplit(">>"));

    // TODO: Binary, LtoR
    (gp_bit_ops     = alt(gp_BITAND_OP,
                          gp_BITOR_OP,
                          gp_BITXOR_OP,
                          gp_LEFT_OP,
                          gp_RIGHT_OP))->set_name("Bitwise Operator");

    gp_PTR_OP       = oplit('*');

    gp_INC_OP       = oplit("++");
    gp_DEC_OP       = oplit("--");
    (gp_step_ops    = alt(gp_INC_OP,
                          gp_DEC_OP))->set_name("Additive Step Operator");

    gp_rel_ops      = alt("Relational Operator");
    gp_LT_OP        = gp_rel_ops->add(oplit('<'));
    gp_GT_OP        = gp_rel_ops->add(oplit('>'));
    gp_LE_OP        = gp_rel_ops->add(oplit("<="));
    gp_GE_OP        = gp_rel_ops->add(oplit(">="));

    gp_eq_ops       = alt("Equality Operator");
    gp_EQ_OP        = gp_eq_ops->add(oplit("=="));
    gp_NE_OP        = gp_eq_ops->add(oplit("!="));

    gp_AND_OP       = oplit("&&");
    gp_OR_OP        = oplit("||");

    gp_MIN_OP       = oplit("<?"); // TODO: Relate to GCC deprecated in favor of std::min
    gp_MAX_OP       = oplit(">?"); // TODO: Relate to GCC deprecated in favor of std::min
    (gp_extreme_ops = alt(gp_MIN_OP,
                            gp_MAX_OP))->set_name("Extreme Operator"); // TODO: Binary, RtoL

    gp_assignment_ops = alt("Assignment Operator");
    gp_ASSIGN_OP       = gp_assignment_ops->add(oplit('='));
    gp_MUL_ASSIGN_OP   = gp_assignment_ops->add(oplit("*="));
    gp_DIV_ASSIGN_OP   = gp_assignment_ops->add(oplit("/="));
    gp_MOD_ASSIGN_OP   = gp_assignment_ops->add(oplit("%="));
    gp_ADD_ASSIGN_OP   = gp_assignment_ops->add(oplit("+="));
    gp_SUB_ASSIGN_OP   = gp_assignment_ops->add(oplit("-="));
    gp_LEFT_ASSIGN_OP  = gp_assignment_ops->add(oplit("<<="));
    gp_RIGHT_ASSIGN_OP = gp_assignment_ops->add(oplit(">>="));
    gp_AND_ASSIGN_OP   = gp_assignment_ops->add(oplit("&="));
    gp_XOR_ASSIGN_OP   = gp_assignment_ops->add(oplit("^="));
    gp_OR_ASSIGN_OP    = gp_assignment_ops->add(oplit("|="));
    gp_MIN_ASSIGN_OP   = gp_assignment_ops->add(oplit("<?=")); // TODO: Relate to GCC deprecated
    gp_MAX_ASSIGN_OP   = gp_assignment_ops->add(oplit(">?=")); // TODO: Relate to GCC deprecated

    gp_TYPEDEF    = keyword("typedef");
    gp_EXTERN     = keyword("extern");
    gp_STATIC     = keyword("static");
    gp_AUTO       = keyword("auto");
    gp_REGISTER   = keyword("register");
    gp_INLINE     = keyword("inline");
    gp_RESTRICT   = keyword("restrict");

    gp_CONST      = keyword("const");
    gp_VOLATILE   = keyword("volatile");

    gp_Cxx_11_AUTO = keyword("char");

    gp_CHAR       = keyword("char");
    gp_SHORT      = keyword("short");
    gp_INT        = keyword("int");
    gp_LONG       = keyword("long");
    gp_SIGNED     = keyword("signed");
    gp_UNSIGNED   = keyword("unsigned");
    gp_FLOAT      = keyword("float");
    gp_DOUBLE     = keyword("double");
    gp_BOOL       = keyword("bool");
    gp_VOID       = keyword("void");

    // TODO: Types
    gp_COMPLEX    = keyword("complex");
    gp_IMAGINARY  = keyword("imaginary");

    // TODO: Types
    gp_STRUCT     = keyword("struct");
    gp_UNION      = keyword("union");
    gp_ENUM       = keyword("enum");

    gp_ELLIPSIS   = new Lit("...");

    // TODO: Group: Keywords
    gp_CASE       = keyword("case");
    gp_DEFAULT    = keyword("default");
    gp_IF         = keyword("if");
    gp_ELSE       = keyword("else");
    gp_SWITCH     = keyword("switch");
    gp_WHILE      = keyword("while");
    gp_DO         = keyword("do");
    gp_FOR        = keyword("for");
    gp_GOTO       = keyword("goto");
    gp_CONTINUE   = keyword("continue");
    gp_BREAK      = keyword("break");
    gp_RETURN     = keyword("return");

    (gp_LBrace = lit('{', "C Left Brace"))->tokenize()->set_dfmt(DFMT_C);
    (gp_RBrace = lit('}', "C Right Brace"))->tokenize()->set_dfmt(DFMT_C);

    (gp_LBracket = lit('[', "C Left Bracket"))->tokenize()->set_dfmt(DFMT_C);
    (gp_RBracket = lit(']', "C Right Bracket"))->tokenize()->set_dfmt(DFMT_C);

    (gp_ID = seq(bos(),
                 gp_Letter_EN_us,
                 gp_Letter_EN_us_dig09_0oM,
                 eos()))->tokenize()->set_name("C-style Identifier")->set_pmatch(cbuf_lmatch_CIdent)->set_dfmt(DFMT_C);

    (gp_ID_GCC = seq(bos(),
                     alt(gp_Letter_EN_us, gp_Dollar),
                     alt(gp_Letter_EN_us_dig09_0oM, gp_Dollar),
                     eos()))->tokenize()->set_name("GCC C-style Identifier")->set_pmatch(cbuf_lmatch_CIdent)->set_dfmt(DFMT_C);

    (gp_ISS = rep0o1(gp_lL))->set_name("C-style Integer Size Suffix")->set_dfmt(DFMT_C);

    {
        // 0[xX]{H}+{IS}?
        gp_HEX_CONSTANT = seq(gp_NumHex, gp_ISS)->tokenize()->set_name("C-style Hexadecimal Numerical Literal (Constant)");

        // 0{D}+{IS}?
        gp_OCT_CONSTANT = seq(gp_NumOct, gp_ISS)->tokenize()->set_name("C-style Octal Numerical Literal (Constant)");

        // {D}+{IS}?
        gp_DEC_CONSTANT = seq(gp_NumDec, gp_ISS)->tokenize()->set_name("C-style Decimal Numerical Literal (Constant)");

        /*
          L?'(\\.|[^\\'])+'	{ count(); return(CONSTANT); }
          {D}+{E}{FS}?		{ count(); return(CONSTANT); }
          {D}*"."{D}+({E})?{FS}?	{ count(); return(CONSTANT); }
          {D}+"."{D}*({E})?{FS}?	{ count(); return(CONSTANT); }
        */
        gp_CONSTANT = alt(gp_HEX_CONSTANT,
                          gp_OCT_CONSTANT,
                          gp_DEC_CONSTANT)->tokenize()->set_name("C-style Numerical Literal (Constant)")->set_dfmt(DFMT_C);
    }

    gp_SIZEOF = seq(bos(), lit_r("sizeof"), eos());

    gp_STRING_LITERAL_CONTENTS = rep0oM(alt(anybytes(1), gp_backslashed_DoubleQuote));
    gp_STRING_LITERAL = seq(gp_DoubleQuote,
                              gp_STRING_LITERAL_CONTENTS,
                              gp_DoubleQuote);
    gp_STRING_LITERAL->tokenize();

    /* All chars except backslash and double-quote */
    gp_not_Backslash_nor_DoubleQuote = alt(gp_Backslash, gp_DoubleQuote);
    gp_not_Backslash_nor_DoubleQuote->set_complement(true); /* make it a complementary set */

    /* C Escape Sequence */
    gp_Esc_Alert       = new Lit("\\a"); /* \a  	Alert  	Produces an audible or visible alert. */
    gp_Esc_Backspace   = new Lit("\\b"); /* \b  Backspace 	Moves the cursor back one position (non-destructive). */
    gp_Esc_FormFeed    = new Lit("\\f"); /* \f  Form Feed 	Moves the cursor to the first position of the next page. */
    gp_Esc_NewLine     = new Lit("\\n"); /* \n  New Line 	Moves the cursor to the first position of the next line. */
    gp_Esc_CR          = new Lit("\\r"); /* \r  Carriage Return 	Moves the cursor to the first position of the current line. */
    gp_Esc_HT          = new Lit("\\t"); /* \t  Horizontal Tab 	Moves the cursor to the next horizontal tabular position. */
    gp_Esc_VTFeed      = new Lit("\\v"); /* \v  Vertical Tab 	Moves the cursor to the next vertical tabular position. */
    gp_Esc_SingleQuote = new Lit("\\\'"); /* \'  Produces a single quote. */
    gp_Esc_DoubleQuote = new Lit("\\\""); /* \"   Produces a double quote. */
    gp_Esc_QMark       = new Lit("\\?"); /* \?   Produces a question mark. */
    gp_Esc_Backslash   = new Lit("\\\\"); /* \\  Produces a single backslash. */
    gp_Esc_Null        = new Lit("\\\0"); /* \0  Produces a null character. */

    gp_Esc_NumDec = seq(gp_Backslash, gp_NumDec); /* \ddd  Defines one character by the decal digits (base-8 number). Multiple characters may be defined in the same escape sequence, but the value is implementation-specific (see examples). */
    gp_Esc_NumOct = seq(gp_Backslash, gp_NumOct); /* \ddd  Defines one character by the octal digits (base-8 number). Multiple characters may be defined in the same escape sequence, but the value is implementation-specific (see examples). */
    gp_Esc_NumHex = seq(gp_Backslash, gp_NumHex); /* \xdd  Defines one character by the hexadecimal digit (base-16 number). */

    /* Examples: */
    /*   printf("\12");    Produces the decimal character 10 (x0A Hex). */
    /*   printf("\xFF");   Produces the decimal character -1 or 255 (depending on sign). */
    /*   printf("\x123");  Produces a single character (value is undefined). May cause errors. */
    /*   printf("\0222");  Produces two characters whose values are implementation-specific.  */

    gp_EscSeq = alt
        (gp_Esc_Alert,
         gp_Esc_Backspace,
         gp_Esc_FormFeed,
         gp_Esc_NewLine,
         gp_Esc_CR,
         gp_Esc_HT,
         gp_Esc_VTFeed,
         gp_Esc_SingleQuote,
         gp_Esc_DoubleQuote,
         gp_Esc_QMark,
         gp_Esc_Backslash,
         gp_Esc_Null,
         gp_Esc_NumDec,
         gp_Esc_NumOct,
         gp_Esc_NumHex
         );

    /* C String Literal Contents Atom */
    (gp_STR_CONTENTS_ATOM = alt(gp_not_Backslash_nor_DoubleQuote, gp_EscSeq))->
        set_name("C-style String Literal")->
        set_pmatch(cbuf_lmatch_CIdent)->
        set_dfmt(DFMT_C);

    /* C String Literal Contents */
    /* 0 or more Escape or letters in string; String case can be empty. */
    (gp_STR_CONTENTS = rep0oM(gp_STR_CONTENTS_ATOM))->
        set_name("C-style String Literal Contents")->
        set_pmatch(cbuf_lmatch_CIdent)->
        set_dfmt(DFMT_C);

    /* C String Literal */
    (gp_STR = seq(gp_DoubleQuote,
                    gp_STR_CONTENTS,
                    gp_DoubleQuote))->
        set_name("C-style String Literal")->
        set_pmatch(cbuf_lmatch_CIdent)->
        set_dfmt(DFMT_C);

    gp_expr = alt();

    (gp_primary_expr = alt
     (gp_ID,
      gp_CONSTANT,
      gp_STR,
      seq(gp_LParen, gp_expr, gp_RParen)
      ))->
        set_name("Primary Expression");

    (gp_type_name = alt())->set_name("Type Name");
    (gp_initializer_list = alt())->set_name("Initializer List");
    (gp_arg_expr_list = alt())->set_name("Argument Expression List");

    (gp_postfix_expr = alt())->
        set_name("Postfix Expression");
    gp_postfix_expr->add
        (gp_primary_expr,
         seq(gp_postfix_expr, gp_LBracket, gp_expr, gp_RBracket),
         seq(gp_postfix_expr, gp_LParen, gp_RParen),
         seq(gp_postfix_expr, gp_LParen, gp_arg_expr_list, gp_RParen),
         seq(gp_postfix_expr, gp_Dot, gp_ID),
         seq(gp_postfix_expr, gp_PTR_OP, gp_ID),
         seq(gp_postfix_expr, gp_INC_OP),
         seq(gp_postfix_expr, gp_DEC_OP),
         seq(gp_LParen, gp_type_name, gp_RParen, gp_LBrace, gp_initializer_list, gp_RBrace),
         seq(gp_LParen, gp_type_name, gp_RParen, gp_LBrace, gp_initializer_list, gp_Comma, gp_RBrace)
         );

    (gp_assignment_expr = alt())->set_name("Assignment Expression");

    gp_arg_expr_list->add
        (gp_assignment_expr,
         seq(gp_arg_expr_list, gp_Comma, gp_assignment_expr)
         );

    (gp_unary_operator = alt())->
        set_name("Unary Operator");
    (gp_unary_expr = alt())->
        set_name("Unary Expression");
    (gp_cast_expr = alt())->
        set_name("Cast Expression");

    gp_unary_expr->add
        (gp_postfix_expr,
         seq(gp_INC_OP, gp_unary_expr),
         seq(gp_DEC_OP, gp_unary_expr),
         seq(gp_unary_operator, gp_cast_expr),
         seq(gp_SIZEOF, gp_unary_expr),
         seq(gp_SIZEOF, gp_LParen, gp_type_name, gp_RParen)
         );

    gp_unary_operator->add
        (lit_r('&'),
         lit_r('*'),
         lit_r('+'),
         lit_r('-'),
         lit_r('~'),
         lit_r('!')
         );

    gp_cast_expr->add
        (gp_unary_expr,
         seq(gp_LParen, gp_type_name, gp_RParen, gp_cast_expr)
         );

    (gp_mult_expr = alt())->set_name("Multiplicative Expression");
    gp_mult_expr->add
        (gp_cast_expr,
         seq(gp_mult_expr, gp_mul_ops, gp_cast_expr)
         );

    (gp_add_expr = alt())->set_name("Additive Expression");
    gp_add_expr->add
        (gp_mult_expr,
         seq(gp_add_expr, gp_add_ops, gp_mult_expr)
         );

    (gp_shift_expr = alt())->set_name("Shift Expression");
    gp_shift_expr->add
        (gp_add_expr,
         seq(gp_shift_expr, gp_shift_ops, gp_add_expr)
         );

    (gp_rel_expr = alt())->set_name("Relational Expression");
    gp_rel_expr->add
        (gp_shift_expr,
         seq(gp_rel_expr, gp_rel_ops, gp_shift_expr)
         );

    (gp_eq_expr = alt())->set_name("Equality Expression");
    gp_eq_expr->add
        (gp_rel_expr,
         seq(gp_eq_expr, gp_eq_ops, gp_rel_expr)
         );

    (gp_and_expr = alt())->set_name("And Expression");
    gp_and_expr->add
        (gp_eq_expr,
         seq(gp_and_expr, gp_BITAND_OP, gp_eq_expr)
         );

    (gp_xor_expr = alt())->set_name("Exclusive Or (XOr) Expression");
    gp_xor_expr->add
        (gp_and_expr,
         seq(gp_xor_expr, gp_BITXOR_OP, gp_and_expr)
         );

    (gp_ior_expr = alt())->set_name("Inclusive Or (IOr) Expression");
    gp_ior_expr->add
        (gp_xor_expr,
         seq(gp_ior_expr, gp_BITOR_OP, gp_xor_expr)
         );

    (gp_logical_and_expr = alt())->set_name("Logical And Expression");
    gp_logical_and_expr->add
        (gp_ior_expr,
         seq(gp_logical_and_expr, gp_AND_OP, gp_ior_expr)
         );

    (gp_logical_or_expr = alt())->set_name("Logical Or Expression");
    gp_logical_or_expr->add
        (gp_logical_and_expr,
         seq(gp_logical_or_expr, gp_OR_OP, gp_logical_and_expr)
         );

    (gp_cond_expr = alt())->set_name("Conditional Expression");
    gp_cond_expr->add
        (gp_logical_or_expr,
         seq(gp_logical_or_expr,
             lit_r('?'),
             gp_expr,
             lit_r(':'),
             gp_cond_expr)
         );

    gp_assignment_expr->add
        (gp_cond_expr,
         seq(gp_unary_expr, gp_assignment_ops, gp_assignment_expr)
         );

    gp_expr->add(gp_assignment_expr,
                   seq(gp_expr, gp_Comma, gp_assignment_expr));

    (gp_constant_expr = alt
     (gp_cond_expr
      ))->set_name("Constant Expression");

    (gp_declaration_specifiers = alt())->
        set_name("Declaration Specifiers");
    (gp_init_declarator_list = alt())->
        set_name("Initialization Declarator List");

    (gp_declaration = alt
     (seq(gp_declaration_specifiers, lit_r(';')),
      seq(gp_declaration_specifiers, gp_init_declarator_list, lit_r(';'))
      ))->
        set_name("Declaration");

    (gp_function_specifier = alt())->
        set_name("Function Specifier");

    (gp_storage_class_specifier = alt())->set_name("Storage Class Specifier");
    (gp_type_specifier = alt())->set_name("Type Specifier");
    (gp_type_qualifier = alt())->set_name("Type Qualifier");

    gp_declaration_specifiers->add
        (gp_storage_class_specifier,
         seq(gp_storage_class_specifier, gp_declaration_specifiers),
         gp_type_specifier,
         seq(gp_type_specifier, gp_declaration_specifiers),
         gp_type_qualifier,
         seq(gp_type_qualifier, gp_declaration_specifiers),
         gp_function_specifier,
         seq(gp_function_specifier, gp_declaration_specifiers)
         );

    // gp_Cxx_11_auto_declaration_specifiers->add
    //     (seq(gp_AUTO_specifier, gp_declaration_specifiers),
    //      )->set_lang(gp_Cxx_11_lang);

    (gp_init_declarator = alt())->set_name("Initialization Declarator");

    gp_init_declarator_list->add
        (gp_init_declarator,
         seq(gp_init_declarator_list, gp_Comma, gp_init_declarator)
         );

    (gp_declarator = alt())->set_name("Declarator");
    (gp_initializer = alt())->set_name("Initializer");

    gp_init_declarator->add
        (gp_declarator,
         seq(gp_declarator, gp_ASSIGN_OP, gp_initializer)
         );

    gp_storage_class_specifier->add
        (gp_TYPEDEF,
         gp_EXTERN,
         gp_STATIC,
         gp_AUTO,
         gp_REGISTER
         );

    (gp_struct_or_union_specifier = alt())->set_name("Structure or Union Specifier");
    (gp_enum_specifier = alt())->set_name("Enumerator Specifier");

    gp_type_specifier->add
        (gp_VOID,
         gp_CHAR,
         gp_SHORT,
         gp_INT,
         gp_LONG,
         gp_FLOAT,
         gp_DOUBLE,
         gp_SIGNED,
         gp_UNSIGNED,
         gp_BOOL,
         gp_COMPLEX,
         gp_IMAGINARY,
         gp_struct_or_union_specifier,
         gp_enum_specifier,
         gp_type_name
         );

    (gp_struct_or_union = alt())->set_name("Structure or Union");
    (gp_struct_declaration_list = alt())->set_name("Structure Declaration List");

    gp_struct_or_union_specifier->add
        (seq(gp_struct_or_union,
             gp_ID,
             gp_LBrace,
             gp_struct_declaration_list,
             gp_RBrace),
         seq(gp_struct_or_union,
             gp_LBrace,
             gp_struct_declaration_list,
             gp_RBrace),
         seq(gp_struct_or_union, gp_ID)
         );

    gp_struct_or_union->add
        (gp_STRUCT,
         gp_UNION
         );

    (gp_struct_declaration = seq())->set_name("Structure Declaration");

    gp_struct_declaration_list->add
        (gp_struct_declaration,
         seq(gp_struct_declaration_list, gp_struct_declaration)
         );

    (gp_specifier_qualifier_list = alt())->set_name("Specifier Qualifier List");
    (gp_struct_declarator_list = alt())->set_name("Structure Declarator List");

    gp_struct_declaration->add
        (gp_specifier_qualifier_list,
         gp_struct_declarator_list,
         lit_r(';'));

    gp_specifier_qualifier_list->add
        (seq(gp_type_specifier, gp_specifier_qualifier_list),
         gp_type_specifier,
         seq(gp_type_qualifier, gp_specifier_qualifier_list),
         gp_type_qualifier
         );

    (gp_struct_declarator = alt())->set_name("Structure Declarator");

    gp_struct_declarator_list->add
        (gp_struct_declarator,
         seq(gp_struct_declarator_list,
             gp_Comma,
             gp_struct_declarator)
         );

    gp_struct_declarator->add
        (gp_declarator,
         seq(lit_r(':'), gp_constant_expr),
         seq(gp_declarator, lit_r(':'), gp_constant_expr)
         );

    (gp_enumerator_list = alt())->set_name("Enumerator List");

    gp_enum_specifier->add
        (seq(gp_ENUM, gp_LBrace, gp_enumerator_list, gp_RBrace),
         seq(gp_ENUM, gp_ID, gp_LBrace, gp_enumerator_list, gp_RBrace),
         seq(gp_ENUM, gp_LBrace, gp_enumerator_list, gp_Comma, gp_RBrace),
         seq(gp_ENUM, gp_ID, gp_LBrace, gp_enumerator_list, gp_Comma, gp_RBrace),
         seq(gp_ENUM, gp_ID)
         );

    (gp_enumerator = alt())->set_name("Enumerator");

    gp_enumerator_list->add
        (gp_enumerator,
         seq(gp_enumerator_list, gp_Comma, gp_enumerator)
         );

    gp_enumerator->add
        (gp_ID,
         seq(gp_ID, gp_ASSIGN_OP, gp_constant_expr)
         );

    gp_type_qualifier->add
        (gp_CONST,          /* constant keyword */
         gp_RESTRICT,
         gp_VOLATILE
         );

    gp_function_specifier->add
        (gp_INLINE
         );

    (gp_direct_declarator = alt())->set_name("Direct Declarator");

    (gp_pointer = alt())->set_name("Pointer");

    gp_declarator->add
        (seq(gp_pointer, gp_direct_declarator),
         seq(gp_direct_declarator)
         );

    (gp_type_qualifier_list = alt())->set_name("Type Qualifier List");
    (gp_param_type_list = alt())->set_name("Parameter Type list");
    (gp_identifier_list = alt())->set_name("Identifier List");

    gp_direct_declarator->add
        (gp_ID,
         seq(gp_LParen, gp_declarator, gp_RParen),
         seq(gp_direct_declarator, gp_LBracket, gp_type_qualifier_list, gp_assignment_expr, gp_RBracket),
         seq(gp_direct_declarator, gp_LBracket, gp_type_qualifier_list, gp_RBracket),
         seq(gp_direct_declarator, gp_LBracket, gp_assignment_expr, gp_RBracket),
         seq(gp_direct_declarator, gp_LBracket, gp_STATIC, gp_type_qualifier_list, gp_assignment_expr, gp_RBracket),
         seq(gp_direct_declarator, gp_LBracket, gp_type_qualifier_list, gp_STATIC, gp_assignment_expr, gp_RBracket),
         seq(gp_direct_declarator, gp_LBracket, gp_type_qualifier_list, lit_r('*'), gp_RBracket),
         seq(gp_direct_declarator, gp_LBracket, lit_r('*'), gp_RBracket),
         seq(gp_direct_declarator, gp_LBracket, gp_RBracket),
         seq(gp_direct_declarator, gp_LParen, gp_param_type_list, gp_RParen),
         seq(gp_direct_declarator, gp_LParen, gp_identifier_list, gp_RParen),
         seq(gp_direct_declarator, gp_LParen, gp_RParen)
         );

    gp_pointer->add
        (lit_r('*'),
         seq(lit_r('*'), gp_type_qualifier_list),
         seq(lit_r('*'), gp_pointer),
         seq(lit_r('*'), gp_type_qualifier_list, gp_pointer)
         );

    gp_type_qualifier_list->add
        (gp_type_qualifier,
         seq(gp_type_qualifier_list, gp_type_qualifier)
         );

    (gp_param_list = alt())->set_name("Parameter list");

    gp_param_type_list->add
        (gp_param_list,
         seq(gp_param_list, gp_Comma, gp_ELLIPSIS)
         );

    (gp_param_declaration = alt())->set_name("Parameter Declaration");

    gp_param_list->add
        (gp_param_declaration,
         seq(gp_param_list, gp_Comma, gp_param_declaration)
         );

    (gp_abstract_declarator = alt())->set_name("Abstract Declarator");

    gp_param_declaration->add
        (seq(gp_declaration_specifiers, gp_declarator),
         seq(gp_declaration_specifiers, gp_abstract_declarator),
         gp_declaration_specifiers
         );

    gp_identifier_list->add
        (gp_ID,
         seq(gp_identifier_list, gp_Comma, gp_ID)
         );

    gp_type_name->add
        (gp_specifier_qualifier_list,
         seq(gp_specifier_qualifier_list, gp_abstract_declarator)
         );

    (gp_direct_abstract_declarator = alt())->set_name("Direct Abstract Declarator");

    gp_abstract_declarator->add
        (gp_pointer,
         gp_direct_abstract_declarator,
         seq(gp_pointer, gp_direct_abstract_declarator)
         );

    gp_direct_abstract_declarator->add
        (seq(gp_LParen, gp_abstract_declarator, gp_RParen),
         seq(gp_LBracket, gp_RBracket),
         seq(gp_LBracket, gp_assignment_expr, gp_RBracket),
         seq(gp_direct_abstract_declarator, gp_LBracket, gp_RBracket),
         seq(gp_direct_abstract_declarator, gp_LBracket, gp_assignment_expr, gp_RBracket),
         seq(gp_LBracket, lit_r('*'), gp_RBracket),
         seq(gp_direct_abstract_declarator, gp_LBracket, lit_r('*'), gp_RBracket),
         seq(gp_LParen, gp_RParen),
         seq(gp_LParen, gp_param_type_list, gp_RParen),
         seq(gp_direct_abstract_declarator, gp_LParen, gp_RParen),
         seq(gp_direct_abstract_declarator, gp_LParen, gp_param_type_list, gp_RParen)
         );

    gp_initializer->add
        (gp_assignment_expr,
         seq(gp_LBrace, gp_initializer_list, gp_RBrace),
         seq(gp_LBrace, gp_initializer_list, gp_Comma, gp_RBrace)
         );

    (gp_designation = alt())->set_name("Designation");

    gp_initializer_list->add
        (gp_initializer,
         seq(gp_designation, gp_initializer),
         seq(gp_initializer_list, gp_Comma, gp_initializer),
         seq(gp_initializer_list, gp_Comma, gp_designation, gp_initializer)
         );

    (gp_designator_list = alt())->set_name("Designator List");

    gp_designation->add
        (seq(gp_designator_list, gp_ASSIGN_OP)
         );

    (gp_designator = alt())->set_name("Designator");

    gp_designator_list->add
        (seq(gp_designator),
         seq(gp_designator_list, gp_designator)
         );

    gp_designator->add
        (seq(gp_LBracket, gp_constant_expr, gp_RBracket),
         seq(gp_Dot, gp_ID)
         );

    (gp_labeled_stmt = alt())->set_name("Labeled Statement");
    (gp_compound_stmt = alt())->set_name("Compound Statement");
    (gp_expr_stmt = alt())->set_name("Expression Statement");
    (gp_selection_stmt = alt())->set_name("Selection Statement");
    (gp_iteration_stmt = alt())->set_name("Iteration Statement");
    (gp_jump_stmt = alt())->set_name("Jump Statement");

    (gp_stmt = alt
     (gp_labeled_stmt,
      gp_compound_stmt,
      gp_expr_stmt,
      gp_selection_stmt,
      gp_iteration_stmt,
      gp_jump_stmt
      ))->set_name("Statement");

    gp_labeled_stmt->add
        (seq(gp_ID, lit_r(':'), gp_stmt),
         seq(gp_CASE, gp_constant_expr, lit_r(':'), gp_stmt),
         seq(gp_DEFAULT, lit_r(':'), gp_stmt)
         );

    (gp_block_item_list = alt())->set_name("Block Item List");

    gp_compound_stmt->add
        (seq(gp_LBrace, gp_RBrace),
         seq(gp_LBrace, gp_block_item_list, gp_RBrace)
         );

    (gp_block_item = alt())->set_name("Block Item");

    gp_block_item_list->add
        (gp_block_item,
         seq(gp_block_item_list, gp_block_item)
         );

    gp_block_item->add
        (gp_declaration,
         gp_stmt
         );

    gp_expr_stmt->add
        (lit_r(';'),
         seq(gp_expr, lit_r(';'))
         );

    gp_selection_stmt->add
        (seq(gp_IF, gp_LParen, gp_expr, gp_RParen, gp_stmt),
         seq(gp_IF, gp_LParen, gp_expr, gp_RParen, gp_stmt, gp_ELSE, gp_stmt),
         seq(gp_SWITCH, gp_LParen, gp_expr, gp_RParen, gp_stmt)
         );

    gp_iteration_stmt->add
        (seq(gp_WHILE, gp_LParen, gp_expr, gp_RParen, gp_stmt),
         seq(gp_DO, gp_stmt, gp_WHILE, gp_LParen, gp_expr, gp_RParen, lit_r(';')),
         seq(gp_FOR, gp_LParen, gp_expr_stmt, gp_expr_stmt, gp_RParen, gp_stmt),
         seq(gp_FOR, gp_LParen, gp_expr_stmt, gp_expr_stmt, gp_expr, gp_RParen, gp_stmt),
         seq(gp_FOR, gp_LParen, gp_declaration, gp_expr_stmt, gp_RParen, gp_stmt),
         seq(gp_FOR, gp_LParen, gp_declaration, gp_expr_stmt, gp_expr, gp_RParen, gp_stmt)
         );

    gp_jump_stmt->add
        (seq(gp_GOTO, gp_ID, lit_r(';')),
         seq(gp_CONTINUE, lit_r(';')),
         seq(gp_BREAK, lit_r(';')),
         seq(gp_RETURN, lit_r(';')),
         seq(gp_RETURN, gp_expr, lit_r(';'))
         );

    (gp_external_declaration = alt())->set_name("External_Declaration");

    gp_translation_unit = alt("Translation Unit");
    gp_translation_unit->add
        (gp_external_declaration,
         seq(gp_translation_unit, gp_external_declaration)
         );

    (gp_function_definition = alt())->set_name("Function Definition");

    gp_external_declaration->add
        (gp_function_definition,
         gp_declaration
         );

    (gp_declaration_list = rep1oM(gp_declaration))->
        set_name("Declaration List");

    gp_function_definition->add
        (seq(gp_declaration_specifiers, gp_declarator, gp_declaration_list, gp_compound_stmt),
         seq(gp_declaration_specifiers, gp_declarator, gp_compound_stmt)
         );

    return gp_lang = lang("C98", gp_declaration_list);
}

}
}
}
