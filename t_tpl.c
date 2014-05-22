#include <tpl.h>

struct ms_t {
  int i;
  char c[3];
  double f;
};

void test_tpl_pack(void)
{
  tpl_node *tn;
  struct ms_t ms = {1, {'a','b','c'}, 3.14};

  tn = tpl_map( "S(ic#f)", &ms, 3);
  tpl_pack( tn, 0 );
  tpl_dump( tn, TPL_FILE, "test_struct.tpl" );
  tpl_free( tn );
}

#include "tpl.h"

void test_tpl_unpack(void)
{
  tpl_node *tn;
  struct ms_t ms;

  tn = tpl_map( "S(*)", &ms);
  tpl_load( tn, TPL_FILE, "test_struct.tpl" );
  tpl_unpack( tn, 0 );
  tpl_free( tn );
}

int main(int argc, char *argv[])
{
  test_tpl_pack();
  test_tpl_unpack();
  return 0;
}
