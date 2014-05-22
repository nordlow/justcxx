/*! \file gcc-ast.cpp
 * \brief GCC Abstract Syntax Tree (AST) Sample Plugin.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * \todo Integrate gcc-plugin-3.cpp
 * \see http://codesynthesis.com/~boris/blog/2010/05/17/parsing-cxx-with-gcc-plugin-part-3/
 */

#include <stdlib.h>
#include <gmp.h>
#include <cstdlib> // Include before GCC poison some declarations.
#include <set>
#include <string>
#include <iostream>

// GCC header includes to get the parse tree declarations. The order is
// important and doesn't follow any kind of logic.
extern "C"
{
#include "gcc-plugin.h"

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tree.h"
#include "intl.h"

#include "tm.h"

#include "diagnostic.h"
#include "c-common.h"
#include "c-pragma.h"
#include "cp/cp-tree.h"
}

using namespace std;

int plugin_is_GPL_compatible;

struct decl_comparator
{
    bool
    operator() (tree x, tree y) const
    {
        location_t xl (DECL_SOURCE_LOCATION (x));
        location_t yl (DECL_SOURCE_LOCATION (y));

        return xl < yl;
    }
};

typedef std::set<tree, decl_comparator> decl_set;

void
collect (tree ns, decl_set& set)
{
    cp_binding_level* level (NAMESPACE_LEVEL (ns));
    level->

    // Collect top declarations.
    for (tree decl = level->names; decl != 0; decl = TREE_CHAIN (decl))
    {
        if (DECL_IS_BUILTIN (decl)) continue; // skip builtins
        set.insert (decl);
    }

    // Traverse top namespaces.
    for (tree decl = level->namespaces; decl != 0; decl = TREE_CHAIN (decl))
    {
        if (DECL_IS_BUILTIN (decl)) continue; // skip builtins
        collect (decl, set);
    }
}

std::string
decl_namespace (tree decl)
{
    std::string s;
    for (tree scope (CP_DECL_CONTEXT (decl));
         scope != global_namespace;
         scope = CP_DECL_CONTEXT (scope))
    {
        tree id (DECL_NAME (scope));
        std::string tmp = "::";
        tmp += (id != 0 ? IDENTIFIER_POINTER (id) : "<unnamed>");
        tmp += s;
        s.swap (tmp);
    }
    return s;
}

// All GENERIC trees have two fields in common. First, \c TREE_CHAIN is a
// pointer that can be used as a singly-linked list to other trees. The other is
// \c TREE_TYPE. Many trees store the type of an expression or declaration in
// this field.
void
traverse_tree_decl (tree decl, int depth = 0)
{
    // \see http://gcc.gnu.org/onlinedocs/gccint/GENERIC.html#GENERIC
    // \see http://gcc.gnu.org/onlinedocs/gccint/Tree-overview.html#Tree-overview


    //if (DECL_IS_BUILTIN (decl)) return; // skip builtins

    const int tc (TREE_CODE (decl));  // tree code
    const tree id (DECL_NAME (decl)); // lookup id name
    const char* name (id ? IDENTIFIER_POINTER (id) : "<unnamed>");

    for (int i = 0; i < 2 * depth; i++) { cout << ' '; } // indent

    cout << tree_code_name[tc] << " "
         << decl_namespace (decl) << "::" << name
         << " at "
         << DECL_SOURCE_FILE (decl) << ":"
         << DECL_SOURCE_LINE (decl) << " ";

    // Oftentimes macros that only apply to a specific kind of nodes have their
    // names start with the corresponding prefix, for example, macro @c
    // DECL_NAME can only be used on *_DECL nodes and macro \c TYPE_NAME can
    // only be used on *_TYPE nodes.
    switch (tc)
    {
    case NAMESPACE_DECL: {
        // \see http://gcc.gnu.org/onlinedocs/gccint/Namespaces.html
        tree name = DECL_NAME(decl);
        cout << "decl_name:" << DECL_NAME(decl) << " ";
        cout << "alias:" << DECL_NAMESPACE_ALIAS(decl) << " ";
        cout << "level:" << NAMESPACE_LEVEL(decl) << " ";
        tree decls = cp_namespace_decls(decl);
        if (decls != NULL_TREE) {
            cout << endl;
            for (; decls != 0; decls = TREE_CHAIN (decls)) {
                traverse_tree_decl(decls, depth+1);
            }
        }
        break;
    }
    case TYPE_DECL:             // \see http://gcc.gnu.org/onlinedocs/gccint/Types.html#Types
        cout << "decl_name:" << DECL_NAME(decl) << " ";
        cout << "type_name:" << TYPE_NAME(decl) << " ";
        cout << "size:" << TYPE_SIZE(decl) << " ";
        cout << "align:" << TYPE_ALIGN(decl) << " ";
        break;
    case VAR_DECL:              // variable
        cout << "decl_name:" << DECL_NAME(decl) << " ";
        break;
    case FUNCTION_DECL:
        // \see http://gcc.gnu.org/onlinedocs/gccint/Functions.html#Functions
        // \see http://gcc.gnu.org/onlinedocs/gccint/Functions-for-C_002b_002b.html#Functions-for-C_002b_002b
        cout << "decl_name:" << DECL_NAME(decl) << " ";
        break;
    case METHOD_TYPE:
        // \see http://gcc.gnu.org/onlinedocs/gccint/Types.html#Types
        break;
    case ARRAY_TYPE:
        // \see http://gcc.gnu.org/onlinedocs/gccint/Types.html#Types
        break;
    case RECORD_TYPE: {
        // \see http://gcc.gnu.org/onlinedocs/gccint/Types.html#Types
        // \see http://gcc.gnu.org/onlinedocs/gccint/Classes.html#Classes

        // TYPE_FIELDS chain of FIELD_DECLs for the fields of the struct, and
        // VAR_DECLs, TYPE_DECLs and CONST_DECLs for record-scope variables,
        tree fields = TYPE_FIELDS(decl); // chain of FIELD_DECLs
        for (; fields != 0; fields = TREE_CHAIN (fields)) {
            traverse_tree_decl(fields, depth+1);
        }
        tree methods = TYPE_METHODS(decl);
        for (; methods != 0; methods = TREE_CHAIN (methods)) {
            traverse_tree_decl(methods, depth+1);
        }
        break;
    }
    case UNION_TYPE:            // union
        // \see http://gcc.gnu.org/onlinedocs/gccint/Types.html#Types
        TYPE_FIELDS(decl);
        break;
    }

    cout << endl;
}

void
traverse_tree_root (tree ns)
{
    cp_binding_level* level (NAMESPACE_LEVEL (ns));

    // Traverse declarations.
    for (tree decl = level->names; decl != 0; decl = TREE_CHAIN (decl))
    {
        if (DECL_IS_BUILTIN (decl)) continue;
        traverse_tree_decl (decl);
    }

    // Traverse namespaces.
    for (tree decl = level->namespaces; decl != 0; decl = TREE_CHAIN (decl))
    {
        if (DECL_IS_BUILTIN (decl)) continue;
        traverse_tree_decl (decl);
    }
}

void
order_traverse_tree_decl (tree ns)
{
    decl_set set;
    collect (ns, set);
    for (auto i (set.begin ()), e (set.end ());
         i != e; ++i)
    {
        traverse_tree_decl (*i);
    }
}

extern "C" void
gate_callback (void*, void*)
{
    // If there were errors during compilation, let GCC handle the exit.
    if (errorcount || sorrycount) {
        return;
    }
    int r (0);

    // Process AST. Issue diagnostics and set r to 1 in case of an error.
    cout << "processing " << main_input_filename << endl;
    traverse_tree_root (global_namespace);
    //order_traverse_tree_decl (global_namespace);
    exit (r);
}

extern "C" int
plugin_init (plugin_name_args* info,
             plugin_gcc_version* ver)
{
    int r (0);

    cout << "starting " << info->base_name << endl;

    // Parse options if any.

    // Disable assembly output.
    asm_file_name = HOST_BIT_BUCKET;

    // Register callbacks.
    register_callback (info->base_name,
                       PLUGIN_OVERRIDE_GATE,
                       &gate_callback,
                       0);
    return r;
}
