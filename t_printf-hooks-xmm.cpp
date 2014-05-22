#include <stdio.h>
#include <inttypes.h>
#include <emmintrin.h>
#include <printf.h>
#include <string.h>
#include <stdarg.h>

static int pa_xmm;


static void
xmm_va (void *mem, va_list *ap)
{
  __m128 v = va_arg (*ap, __m128);
  memcpy (mem, &v, sizeof (v));
}


static int
xmm_ais (const struct printf_info *info, size_t n, int *argtype, int *size)
{
  argtype[0] = pa_xmm;
  size[0] = sizeof (__m128);
  return 1;
}

static int mod_v4f;
static int mod_v2d;
static int mod_v16i;
static int mod_v8i;
static int mod_v4i;
static int mod_v2i;

static int
xmm_printf_f (FILE *fp, const struct printf_info *info, const void *const *args)
{
  if (info->user & mod_v4f)
    {
      union
      {
        __m128 v;
        float f[4];
      } u;
      memcpy (&u, *((void ***)args)[0], sizeof (__m128));
      char *buf;
      asprintf (&buf, "{%f, %f, %f, %f}", u.f[0], u.f[1], u.f[2], u.f[3]);
      fputs (buf, fp);
      free (buf);
    }
  else if (info->user & mod_v2d)
    {
      union
      {
        __m128d v;
        double d[2];
      } u;
      memcpy (&u, *((void ***)args)[0], sizeof (__m128d));
      char *buf;
      asprintf (&buf, "{%f, %f}", u.d[0], u.d[1]);
      fputs (buf, fp);
      free (buf);
    }
  else
    return -2;

  return 0;
}


static int
xmm_printf_x (FILE *fp, const struct printf_info *info, const void *const *args)
{
  if (info->user & mod_v16i)
    {
      union
      {
        __m128i v;
        uint8_t i[16];
      } u;
      memcpy (&u, *((void***)args)[0], sizeof (__m128i));
      char *buf;
      asprintf (&buf, "{%" PRIx8 ", %" PRIx8 ", %" PRIx8 ", %" PRIx8 ", %" PRIx8 ", %" PRIx8 ", %" PRIx8 ", %" PRIx8 ", "
		"%" PRIx8 ", %" PRIx8 ", %" PRIx8 ", %" PRIx8 ", %" PRIx8 ", %" PRIx8 ", %" PRIx8 ", %" PRIx8 "}",
		u.i[0], u.i[1], u.i[2], u.i[3], u.i[4], u.i[5], u.i[6], u.i[7],
		u.i[8], u.i[9], u.i[10], u.i[11], u.i[12], u.i[13], u.i[14], u.i[15]);
      fputs (buf, fp);
      free (buf);
    }
  else if (info->user & mod_v8i)
    {
      union
      {
        __m128i v;
        uint16_t i[8];
      } u;
      memcpy (&u, *((void***)args)[0], sizeof (__m128i));
      char *buf;
      asprintf (&buf, "{%" PRIx16 ", %" PRIx16 ", %" PRIx16 ", %" PRIx16 ", %" PRIx16 ", %" PRIx16 ", %" PRIx16 ", %" PRIx16 "}",
		u.i[0], u.i[1], u.i[2], u.i[3], u.i[4], u.i[5], u.i[6], u.i[7]);
      fputs (buf, fp);
      free (buf);
    }
  else if (info->user & mod_v4i)
    {
      union
      {
        __m128i v;
        uint32_t i[4];
      } u;
      memcpy (&u, *((void***)args)[0], sizeof (__m128i));
      char *buf;
      asprintf (&buf, "{%" PRIx32 ", %" PRIx32 ", %" PRIx32 ", %" PRIx32 "}",
		u.i[0], u.i[1], u.i[2], u.i[3]);
      fputs (buf, fp);
      free (buf);
    }
  else if (info->user & mod_v2i)
    {
      union
      {
        __m128i v;
        uint64_t i[2];
      } u;
      memcpy (&u, *((void***)args)[0], sizeof (__m128i));
      char *buf;
      asprintf (&buf, "{%" PRIx64 ", %" PRIx64 "}",
		u.i[0], u.i[1]);
      fputs (buf, fp);
      free (buf);
    }
  else
    return -2;

  return 0;
}


int
main (void)
{
  pa_xmm = register_printf_type (xmm_va);
  mod_v4f = register_printf_modifier (L"v4f");
  mod_v2d = register_printf_modifier (L"v2d");
  mod_v16i = register_printf_modifier (L"v16i");
  mod_v8i = register_printf_modifier (L"v8i");
  mod_v4i = register_printf_modifier (L"v4i");
  mod_v2i = register_printf_modifier (L"v2i");

  register_printf_specifier ('f', xmm_printf_f, xmm_ais);
  register_printf_specifier ('x', xmm_printf_x, xmm_ais);

  __m128 f = _mm_set_ps (1.0, 2.0, 3.0, 4.0);
  __m128d d = _mm_set_pd (1.0, 2.0);
  __m128i i = _mm_set_epi8 (1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

  printf ("%f\n", 1.0f);
  printf ("%f\n", 2.0);
  printf ("%v4ff\n", f);
  printf ("%v2df\n", d);
  printf ("%x\n", 1);
  printf ("%v16ix\n", i);
  printf ("%v8ix\n", i);
  printf ("%v4ix\n", i);
  printf ("%v2ix\n", i);

  return 0;
}
