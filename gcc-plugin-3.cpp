// GCC header includes to get the parse tree declarations. The order
// is important and doesn't follow any kind of logic.
//

#include <stdlib.h>
#include <gmp.h>

#include <cstdlib> // Include before GCC poison some declarations.

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

#include <set>
#include <string>

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

typedef std::multiset<tree, decl_comparator> decl_set;

void
collect (tree ns, decl_set& set)
{
  tree decl;
  cp_binding_level* level (NAMESPACE_LEVEL (ns));

  // Collect declarations.
  //
  for (decl = level->names; decl != 0; decl = TREE_CHAIN (decl))
  {
    if (DECL_IS_BUILTIN (decl))
      continue;

    set.insert (decl);
  }

  // Traverse namespaces.
  //
  for(decl = level->namespaces; decl != 0; decl = TREE_CHAIN (decl))
  {
    if (DECL_IS_BUILTIN (decl))
      continue;

    collect (decl, set);
  }
}

string
decl_scope (tree decl)
{
  string s, tmp;

  for (tree scope (CP_DECL_CONTEXT (decl));
       scope != global_namespace;
       scope = CP_DECL_CONTEXT (scope))
  {
    if (TREE_CODE (scope) == RECORD_TYPE)
      scope = TYPE_NAME (scope);

    tree id (DECL_NAME (scope));

    tmp = "::";
    tmp += (id != 0 ? IDENTIFIER_POINTER (id) : "<unnamed>");
    tmp += s;
    s.swap (tmp);
  }

  return s;
}

void
print_decl (tree decl);

enum access_spec
{
  public_, protected_, private_
};

const char* access_spec_str[] =
{
  "public", "protected", "private"
};

void
print_class (tree type)
{
  type = TYPE_MAIN_VARIANT (type);

  tree decl (TYPE_NAME (type));
  tree id (DECL_NAME (decl));
  const char* name (IDENTIFIER_POINTER (id));

  cerr << "class " << decl_scope (decl) << "::" << name
       << " at " << DECL_SOURCE_FILE (decl)
       << ":" << DECL_SOURCE_LINE (decl) << endl;

  // We are done if this is an incomplete
  // class declaration.
  //
  if (!COMPLETE_TYPE_P (type))
    return;

  // Traverse base information.
  //
  tree biv (TYPE_BINFO (type));
  size_t n (biv ? BINFO_N_BASE_BINFOS (biv) : 0);

  for (size_t i (0); i < n; i++)
  {
    tree bi (BINFO_BASE_BINFO (biv, i));

    // Get access specifier.
    //
    access_spec a (public_);

    if (BINFO_BASE_ACCESSES (biv))
    {
      tree ac (BINFO_BASE_ACCESS (biv, i));

      if (ac == 0 || ac == access_public_node)
        a = public_;
      else if (ac == access_protected_node)
        a = protected_;
      else
        a = private_;
    }

    bool virt (BINFO_VIRTUAL_P (bi));
    tree b_type (TYPE_MAIN_VARIANT (BINFO_TYPE (bi)));
    tree b_decl (TYPE_NAME (b_type));
    tree b_id (DECL_NAME (b_decl));
    const char* b_name (IDENTIFIER_POINTER (b_id));

    cerr << "\t" << access_spec_str[a] << (virt ? " virtual" : "")
         << " base " << decl_scope (b_decl) << "::" << b_name << endl;
  }

  // Traverse members.
  //
  decl_set set;

  for (tree d (TYPE_FIELDS (type)); d != 0; d = TREE_CHAIN (d))
  {
    switch (TREE_CODE (d))
    {
    case TYPE_DECL:
      {
        if (!DECL_SELF_REFERENCE_P (d))
          set.insert (d);
        break;
      }
    case FIELD_DECL:
      {
        if (!DECL_ARTIFICIAL (d))
          set.insert (d);
        break;
      }
    default:
      {
        set.insert (d);
        break;
      }
    }
  }

  for (tree d (TYPE_METHODS (type)); d != 0; d = TREE_CHAIN (d))
  {
    if (!DECL_ARTIFICIAL (d))
      set.insert (d);
  }

  for (decl_set::iterator i (set.begin ()), e (set.end ());
       i != e; ++i)
  {
    print_decl (*i);
  }
}

void
print_decl (tree decl)
{
  tree type (TREE_TYPE (decl));
  int dc (TREE_CODE (decl));
  int tc;

  if (type)
  {
    tc = TREE_CODE (type);

    if (dc == TYPE_DECL && tc == RECORD_TYPE)
    {
      // If DECL_ARTIFICIAL is true this is a class
      // declaration. Otherwise this is a typedef.
      //
      if (DECL_ARTIFICIAL (decl))
      {
        print_class (type);
        return;
      }
    }
  }

  tree id (DECL_NAME (decl));
  const char* name (id
                    ? IDENTIFIER_POINTER (id)
                    : "<unnamed>");

  cerr << tree_code_name[dc] << " "
       << decl_scope (decl) << "::" << name;

  if (type)
    cerr << " type " << tree_code_name[tc];

  cerr << " at " << DECL_SOURCE_FILE (decl)
       << ":" << DECL_SOURCE_LINE (decl) << endl;
}

void
traverse (tree ns)
{
  decl_set set;
  collect (ns, set);

  for (decl_set::iterator i (set.begin ()), e (set.end ());
       i != e; ++i)
  {
    print_decl (*i);
  }
}

extern "C" void
gate_callback (void*, void*)
{
  // If there were errors during compilation,
  // let GCC handle the exit.
  //
  if (errorcount || sorrycount)
    return;

  int r (0);

  //
  // Process AST. Issue diagnostics and set r
  // to 1 in case of an error.
  //
  cerr << "processing " << main_input_filename << endl;
  traverse (global_namespace);

  exit (r);
}

extern "C" int
plugin_init (plugin_name_args* info,
             plugin_gcc_version*)
{
  int r (0);

  cerr << "starting " << info->base_name << endl;

  //
  // Parse options if any.
  //

  // Disable assembly output.
  //
  asm_file_name = HOST_BIT_BUCKET;

  // Register callbacks.
  //
  register_callback (info->base_name,
                     PLUGIN_OVERRIDE_GATE,
                     &gate_callback,
                     0);
  return r;
}
