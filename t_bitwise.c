#include "../bitwise.h"
#include "../stdio_x.h"
#include "../rand.h"
#include <stdlib.h>

int
test_countBits_16(int n)
{
  int ret = 0;
  for (int i = 0; i < n && ret == 0; i++) {
    const uint16_t x = int16_rand();
    const int c_ser = uint32_countBits_serial(x); /* serial loop */
    const int c_ltab = uint16_countBits(x); /* lookup tables */
    if (c_ser != c_ltab) {
      leprintf("error: x:%d, c_ser:%d != c_ltab:%d\n", x, c_ser, c_ltab);
      ret = -1;
    }
    /* if (ret == 0) lprintf("x:%zd, c_ser:%d, c_ltab:%d, c_par:%d\n", x, c_ser, c_ltab, c_par); */
  }
  return ret;
}

int
test_countBits_32(int n)
{
  int ret = 0;
  for (int i = 0; i < n && ret == 0; i++) {
    const uint32_t x = int32_rand();
    const int c_ser = uint32_countBits_serial(x); /* serial loop */
    const int c_ltab = uint32_countBits(x); /* lookup tables */
    const int c_par = uint32_countBits_parallel(x); /* parallel */
    if (c_ser != c_ltab) {
      leprintf("error: x:%d, c_ser:%d != c_ltab:%d\n", x, c_ser, c_ltab);
      ret = -1;
    }
    if (c_par != c_ltab) {
      leprintf("error: x:%d, c_par:%d != c_ltab:%d\n", x, c_par, c_ltab);
      ret = -1;
    }
    /* if (ret == 0) lprintf("x:0x%x, c_ser:%d, c_ltab:%d, c_par:%d\n", x, c_ser, c_ltab, c_par); */
  }
  return ret;
}

int
test_countBits_64(int n)
{
  int ret = 0;
  for (int i = 0; i < n && ret == 0; i++) {
    const uint64_t x = int64_rand();
    const int c_ser = uint64_countBits_serial(x); /* serial loop */
    const int c_ltab = uint64_countBits(x); /* lookup tables */
    const int c_par = uint64_countBits_parallel(x); /* parallel */
    if (c_ser != c_ltab) {
      leprintf("error: x:%zd, c_ser:%d != c_ltab:%d\n", x, c_ser, c_ltab);
      ret = -1;
    }
    if (c_par != c_ltab) {
      leprintf("error: x:%zd, c_par:%d != c_ltab:%d\n", x, c_par, c_ltab);
      ret = -1;
    }
    /* if (ret == 0) lprintf("x:0x%zx, c_ser:%d, c_ltab:%d, c_par:%d\n", x, c_ser, c_ltab, c_par); */
  }
  return ret;
}

int
main(int argc, char *argv[])
{
  size_t n = 1e4;
  test_countBits_16(n);
  test_countBits_32(n);
  test_countBits_64(n);
}
