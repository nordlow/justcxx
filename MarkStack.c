#include "MarkStack.h"

void mstack_init(MStack * mstack)
{
  mstack->markN = 0;
}

void mstack_clear(MStack * mstack)
{
  mstack->markN = 0;
}

int mstack_push(MStack * mstack, const Mark * mark)
{
  int ret = 0;
  if (mstack->markN < MSTACK_DEPTH) {
    mstack->marks[mstack->markN] = *mark;
    mstack->markN++;
    ret = 1;
  } else { PWARN("MStack full\n"); ret = -1; }
  return ret;
}

int mstack_pop_mark(MStack * mstack, Mark * mark_ret)
{
  int ret = 0;
  if (mstack->markN >= 1) {
    mstack->markN--;
    *mark_ret = mstack->marks[mstack->markN];
  } else {
    PWARN("MStack empty\n");
  }
  return ret;
}

int mstack_next(MStack * mstack,
                const char * buf, size_t off, size_t buf_len,
                char entry_ch, char exit_ch)
{
  return 0;
}
