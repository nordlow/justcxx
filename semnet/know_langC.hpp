/*!
 * \file know_langC.hpp
 * \brief Apriori Patterns for C Language.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * Use these along with a \c Iter to scan for example a \c RegFile.
 */

#pragma once

namespace semnet {
namespace patterns {

class Alt;
class Seq;
class Rep;
class Lit;
class Base;
class Lang;

/* ---------------------------- Group Separator ---------------------------- */

extern Alt* gp_EscSeq;      ///< C Escape Sequence

extern Lit* gp_Esc_Alert;
extern Lit* gp_Esc_Backspace;
extern Lit* gp_Esc_FormFeed;
extern Lit* gp_Esc_NewLine;
extern Lit* gp_Esc_CR;
extern Lit* gp_Esc_HT;
extern Lit* gp_Esc_VTFeed;
extern Lit* gp_Esc_SingleQuote;
extern Lit* gp_Esc_DoubleQuote;
extern Lit* gp_Esc_QMark;
extern Lit* gp_Esc_Backslash;
extern Lit* gp_Esc_Null;

extern Seq* gp_Esc_NumDec;
extern Seq* gp_Esc_NumOct;
extern Seq* gp_Esc_NumHe;

namespace C {

/*!
 * \name Language Tokens (Atomic).
 * Note: SemNet does not separate tokens from higher level structures.
 */
/* @{ */

extern Lit* gp_LBrace;
extern Lit* gp_RBrace;

extern Lit* gp_LBracket;
extern Lit* gp_RBracket;

extern Seq* gp_ID;
extern Seq* gp_ID_GCC;

extern Seq* gp_STR; ///< String Literal
extern Alt* gp_STR_CONTENTS_ATOM; ///< String Literal Contents Atom
extern Rep* gp_STR_CONTENTS; ///< String Literal Contents (inside double-quotes)
extern Base* gp_CONSTANT; ///< Numerical Constant: 0, 1, 0x2, 1.2, 1.2f

extern Rep* gp_STRING_LITERAL_CONTENTS;
extern Seq* gp_STRING_LITERAL;
extern Seq* gp_SIZEOF;
extern Rep* gp_ISS;

extern Alt* gp_add_ops;
extern Lit* gp_ADD_OP;
extern Lit* gp_SUB_OP;

extern Alt* gp_mul_ops;
extern Lit* gp_MUL_OP;
extern Lit* gp_DIV_OP;
extern Lit* gp_MOD_OP;

extern Lit* gp_BITAND_OP;
extern Lit* gp_BITOR_OP;
extern Lit* gp_BITXOR_OP;

extern Alt* gp_rel_ops;
extern Lit* gp_LT_OP;
extern Lit* gp_GT_OP;
extern Lit* gp_LE_OP;
extern Lit* gp_GE_OP;

extern Lit* gp_MIN_OP;
extern Lit* gp_MAX_OP;

extern Lit* gp_PTR_OP;
extern Lit* gp_INC_OP;
extern Lit* gp_DEC_OP;

extern Alt* gp_shift_ops;
extern Lit* gp_LEFT_OP;
extern Lit* gp_RIGHT_OP;

extern Alt* gp_eq_ops;
extern Lit* gp_EQ_OP;
extern Lit* gp_NE_OP;

extern Lit* gp_AND_OP;
extern Lit* gp_OR_OP;

// Assignments
extern Alt* gp_assignment_ops;
extern Lit* gp_ASSIGN_OP;
extern Lit* gp_MIN_ASSIGN_OP;
extern Lit* gp_MAX_ASSIGN_OP;
extern Lit* gp_MUL_ASSIGN_OP;
extern Lit* gp_DIV_ASSIGN_OP;
extern Lit* gp_MOD_ASSIGN_OP;
extern Lit* gp_ADD_ASSIGN_OP;
extern Lit* gp_SUB_ASSIGN_OP;
extern Lit* gp_LEFT_ASSIGN_OP;
extern Lit* gp_RIGHT_ASSIGN_OP;
extern Lit* gp_AND_ASSIGN_OP;
extern Lit* gp_XOR_ASSIGN_OP;
extern Lit* gp_OR_ASSIGN_OP;

// Qualifiers
extern Seq* gp_TYPEDEF;
extern Seq* gp_EXTERN;
extern Seq* gp_STATIC;
extern Seq* gp_AUTO;
extern Seq* gp_REGISTER;
extern Seq* gp_INLINE;
extern Seq* gp_RESTRICT;

// Type
extern Seq* gp_CHAR;
extern Seq* gp_SHORT;
extern Seq* gp_INT;
extern Seq* gp_LONG;
extern Seq* gp_SIGNED;
extern Seq* gp_UNSIGNED;
extern Seq* gp_FLOAT;
extern Seq* gp_DOUBLE;
extern Seq* gp_CONST;
extern Seq* gp_VOLATILE;
extern Seq* gp_VOID;
extern Seq* gp_BOOL;
extern Seq* gp_COMPLEX;
extern Seq* gp_IMAGINARY;
extern Seq* gp_STRUCT;
extern Seq* gp_UNION;
extern Seq* gp_ENUM;
extern Lit* gp_ELLIPSIS;

// Other
extern Seq* gp_CASE;
extern Seq* gp_DEFAULT;
extern Seq* gp_IF;
extern Seq* gp_ELSE;
extern Seq* gp_SWITCH;
extern Seq* gp_WHILE;
extern Seq* gp_DO;
extern Seq* gp_FOR;
extern Seq* gp_GOTO;
extern Seq* gp_CONTINUE;
extern Seq* gp_BREAK;
extern Seq* gp_RETURN;

/* @} */

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Load the Grammar for C Language version 98 (year 1998).
 */
Lang* learn_C98_lang();

/* ---------------------------- Group Separator ---------------------------- */

extern Alt* gp_bit_ops;
extern Alt* gp_step_ops;
extern Alt* gp_extreme_ops;

extern Alt* gp_primary_expr; /* primary_expression */
extern Alt* gp_postfix_expr; /* postfix_expression */
extern Alt* gp_arg_expr_list; /* argument_expression_list */
extern Alt* gp_unary_expr;    /* unary_expression */
extern Alt* gp_unary_operator; /* unary_operator */
extern Alt* gp_cast_expr;      /* cast_expression */
extern Alt* gp_mult_expr;      /* multiplicative_expression */
extern Alt* gp_additive_expr; /* additive_expression */
extern Alt* gp_shift_expr;    /* shift_expression */
extern Alt* gp_rel_expr;      /* relational_expression */
extern Alt* gp_eq_expr;       /* equality_expression */
extern Alt* gp_and_expr;      /* and_expression */
extern Alt* gp_xor_expr;      /* exclusive_or_expression */
extern Alt* gp_ior_expr;      /* inclusive_or_expression */
extern Alt* gp_logical_and_expr; /* logical_and_expression */
extern Alt* gp_logical_or_expr;  /* logical_or_expression */
extern Alt* gp_cond_expr;        /* conditional_expression */
extern Alt* gp_assignment_expr;  /* assignment_expression */
extern Alt* gp_expr;                /* expression */
extern Alt* gp_constant_expr;       /* constant_expression */
extern Alt* gp_declaration;         /* declaration */
extern Alt* gp_declaration_specifiers; /* declaration_specifiers */
extern Alt* gp_init_declarator_list;   /* init_declarator_list */
extern Alt* gp_init_declarator;        /* init_declarator */
extern Alt* gp_storage_class_specifier; /* storage_class_specifier */
extern Alt* gp_type_specifier;          /* type_specifier */
extern Alt* gp_struct_or_union_specifier; /* struct_or_union_specifier */
extern Alt* gp_struct_or_union;           /* struct_or_union */
extern Alt* gp_struct_declaration_list; /* struct_declaration_list */
extern Seq* gp_struct_declaration; /* struct_declaration */
extern Alt* gp_specifier_qualifier_list; /* specifier_qualifier_list */
extern Alt* gp_struct_declarator_list; /* struct_declarator_list */
extern Alt* gp_struct_declarator;   /* struct_declarator */
extern Alt* gp_enum_specifier;      /* enum_specifier */
extern Alt* gp_enumerator_list;     /* enumerator_list */
extern Alt* gp_enumerator;          /* enumerator */
extern Alt* gp_type_qualifier;      /* type_qualifier */
extern Alt* gp_function_specifier;  /* function_specifier */
extern Alt* gp_declarator;          /* declarator */
extern Alt* gp_direct_declarator;   /* direct_declarator */
extern Alt* gp_pointer;             /* pointer */
extern Alt* gp_type_qualifier_list; /* type_qualifier_list */
extern Alt* gp_param_type_list;   /* parameter_type_list */
extern Alt* gp_param_list;        /* parameter_list */
extern Alt* gp_param_declaration; /* parameter_declaration */
extern Alt* gp_identifier_list;     /* identifier_list */
extern Alt* gp_type_name;           /* type_name */
extern Alt* gp_abstract_declarator; /* abstract_declarator */
extern Alt* gp_direct_abstract_declarator; /* direct_abstract_declarator */
extern Alt* gp_initializer;                /* initializer */
extern Alt* gp_initializer_list;    /* initializer_list */
extern Alt* gp_designation;         /* designation */
extern Alt* gp_designator_list;     /* designator_list */
extern Alt* gp_designator;          /* designator */
extern Alt* gp_stmt;                /* statement */
extern Alt* gp_labeled_stmt;        /* labeled_statement */
extern Alt* gp_compound_stmt;       /* compound_statement */
extern Alt* gp_block_item_list;     /* block_item_list */
extern Alt* gp_block_item;          /* block_item */
extern Alt* gp_expr_stmt;            /* expression_statement */
extern Alt* gp_selection_stmt;       /* selection_statement */
extern Alt* gp_iteration_stmt;       /* iteration_statement */
extern Alt* gp_jump_stmt;            /* jump_statement */
extern Alt* gp_translation_unit;     /* translation_unit */
extern Alt* gp_external_declaration; /* external_declaration */
extern Alt* gp_function_definition;  /* function_definition */
extern Rep* gp_declaration_list; ///< Note: Top-Most Entry for C language.

extern Lang* gp_lang;

}
}
}
