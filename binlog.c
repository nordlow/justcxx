#include "binlog.h"
#include "bitwise.h"

__attribute__ ((constructor))
void int_binlog_LUT_init(void)
{
  for (uint i = 0; i < 256; i++) {
    g_int8_binlog_LUT[i] = int_binlog((int8_t)i);
  }
}

uint8_t g_int8_binlog_LUT[256];
uint8_t g_int8_MSBPos_LUT[256];

int
int_binceil_slow(int a)
{
  const uint bitsize = sizeof(a) * 8;
  if (int_get_bit(a, bitsize-1)) { /* most signficant bit is set */
    return ((int)1 << (int_is_binpow(a) ? bitsize-1 : bitsize ));
  }
  int c = 1;			/* ceiling */
  while (c < a) { c <<= 1; }
  return c;
}

int64_t
int64_binceil_slow(int64_t a)
{
  const uint bitsize = sizeof(a) * 8;
  if (int64_get_bit(a, bitsize-1)) { /* most signficant bit is set */
    return ((int64_t)1 << (int64_is_binpow(a) ? bitsize-1 : bitsize ));
  }
  int64_t c = 1;		/* ceiling */
  while (c < a) { c <<= 1; }
  return c;
}

void test_binlog(void)
{
  for (int a = 0; a < 256; a++) {
    printf("a:%d ", a);
    printf("int_LSBSet():%d ", int_LSBSet(a));
    printf("int32_LSBPos():%d ", int32_LSBSetPos(a));
    printf("int8_LSBPos_LUT():%d ", int8_LSBSetPos_LUT(a));
    printf("\n");
  }

  for (uint i = 0; i < 32; i++) {
    int a = 1 << i;
    printf("i:%d a:%x ", i, a);
    printf("int_binlog():%d ", int_binlog(a));
    printf("int_binceil():%x ", int_binceil(a));
    printf("int_binceil_slow():%x ", int_binceil_slow(a));
    printf("int_LSBSet():%x ", int_LSBSet(a));
    printf("int32_LSBSetPos():%d ", int32_LSBSetPos(a));
    printf("\n");
  }
  for (uint i = 0; i < 64; i++) {
    int64_t a = 1LL << i;
    printf("i:%d a:%jx ", i, a);
    printf("int64_binlog():%d ", int64_binlog(a));
    printf("int64_binceil():%jx ", int64_binceil(a));
    printf("int64_binceil_slow():%jx ", int64_binceil_slow(a));
    printf("int_LSBSet():%jx ", int64_LSBSet(a));
    printf("int64_LSBSetPos():%d ", int64_LSBSetPos(a));
    printf("\n");
  }
}
