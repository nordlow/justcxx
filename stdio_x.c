#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "stdio_x.h"
#include "pnw_types.h"
#include "bitwise.h"
#include "pathops.h"

void spaces(size_t n) {
  for (size_t i = 0; i < n; i++) {
    putc(' ', stdout);
  }
}
void fspaces(FILE * stream, size_t n) {
  for (size_t i = 0; i < n; i++) {
    putc(' ', stream);
  }
}

void chararray_fprint_hex(FILE* stream, const uchar *buf, size_t len) {
  for (size_t i = 0; i < len; i++) {
    fprintf(stream, "%02x", buf[i]);
  }
}
void chararray_print_hex(const uchar *buf, size_t len) {
  chararray_fprint_hex(stdout, buf, len);
}
void chararray_print_humanely(const uchar *buf, size_t len) {
  int string_flag = 1;
  for (size_t i = 0; i < len; i++) {
    const int ch = buf[i];
#ifdef VXWORKS
    if (!((ch >= 'a' && ch <= 'z') ||
          (ch >= '0' && ch <= '9'))) {
      string_flag = 0;
    }
#else
    if (!(isgraph(ch) && !isspace(ch))) {
      string_flag = 0;
    }
#endif
  }
  if (string_flag && len >= 2) {
    printf("\"%.*s\" ", (unsigned int) len, buf); /* print all character as one whole string */
  } else {
    for (size_t i = 0; i < len; i++) {
      const int ch = buf[i];
#ifdef VXWORKS
      if ((ch >= 'a' && ch <= 'z') ||
          (ch >= '0' && ch <= '9')) {
        printf("'%c' ", ch);
      }
#else
      if (isgraph(ch) && !isspace(ch)) {
        printf("'%c' ", ch);
      }
#endif
      else {
        printf("0x%02x ", (uchar)ch);
      }
    }
  }
}

void int_binfprint(FILE * stream, int a) {
  int n = sizeof(a) * 8;
  /* print exponent bits */
  for (int i = 0; i < n; i++) {
    const char ch = int_get_bit(a, i) ? '1' : '0';
    putc(ch, stream);
  }
}

int float_binfprint(FILE * stream, float a) {
  char ch;
  uint32_t b; memcpy(&b, &a, 4);
  /* print exponent bits */
  for (int i = 31; i >= 24; i--) {
    ch = int_get_bit(*((uint32_t*) &b), i) ? '1' : '0'; putc(ch, stream);
  }
  /* print separator */
  ch = '_'; putc(ch, stream);
  /* print mantissa bits */
  for (int i = 23; i >= 0; i--) {
    ch = int_get_bit(*((uint32_t*) &b), i) ? '1' : '0'; putc(ch, stream);
  }
  return 0;
}

void double_fsiunit_show(FILE * stream, double a) {
  /* TODO: Convert this to a common reusable function that returns scaling
     exponent K and character  */
  /* if      (x >= (T)1e+18) os << ((T)1e-18 * x) << "E"; // exa */
  /* else if (x >= (T)1e+15) os << ((T)1e-15 * x) << "P"; // peta */
  /* else if (x >= (T)1e+12) os << ((T)1e-12 * x) << "T"; // tera */
  /* else if (x >= (T)1e+9)  os << ((T)1e-9  * x) << "G"; // giga */
  /* else if (x >= (T)1e+6)  os << ((T)1e-6  * x) << "M"; // mega */
  /* else if (x >= (T)1e+3)  os << ((T)1e-3  * x) << "k"; // kilo */
  /* else if (x >= (T)1e+0)  os << ((T)1e+0  * x); // none */
  /* else if (x >= (T)1e-3)  os << ((T)1e+3  * x) << "m"; // milli */
  /* else if (x >= (T)1e-6)  os << ((T)1e+6  * x) << "u"; // micro */
  /* else if (x >= (T)1e-9)  os << ((T)1e+9  * x) << "n"; // nano */
  /* else                    os << ((T)1e+12 * x) << "p"; // pico */
  if (a >= 1e-0) {
    fprintf(stream, "%Fs", 1e+0 * a);	/* seconds */
  } else if (a >= 1e-3) {
    fprintf(stream, "%Fms", 1e+3 * a);	/* milli seconds */
  } else if (a >= 1e-6) {
    fprintf(stream, "%Fus", 1e+6 * a);	/* micro seconds */
  } else if (a >= 1e-9) {
    fprintf(stream, "%Fns", 1e+9 * a);	/* nano seconds */
  } else
    fprintf(stream, "%Fps", 1e+12 * a);	/* pico seconds */
}

size_t
find_file_new_COPY(const char * cdir, size_t clen,
		   const char * lsub, size_t llen,
		   char * copy_lsub, size_t copy_lsub_room)
{
  size_t copy_lsub_len = 0;
  const char * hit = strchr(lsub, '.');
  if (hit) {			/* extension found at hit */
  } else {
    /* Example: "README" => "README (copy 1)" */
    for (size_t i = 0; ; i++) {
      copy_lsub_len = snprintf(copy_lsub, copy_lsub_room,
				 "%s (copy %zd)", lsub, i+1);

      char * full = cbuf_pathadd(cdir, clen, copy_lsub, copy_lsub_len, NULL);
      FILE * file = fopen(full, "rb"); /* try opening it readonly */
      free(full);

      if (file) {
	fclose(file);		/* already exists */
      } else {
	break;		  /* no file with that name => we are done! */
      }
    }
  }
  return copy_lsub_len;
}

char *
fload(const char *path, char *buf, size_t *len_ret)
{
  char * ret = NULL;
  FILE *f = fopen(path, "rb");
  if (f == NULL) { return NULL; }
  fseek(f, 0, SEEK_END);
  *len_ret = ftell(f);
  buf = (char*)realloc(buf, *len_ret);
  rewind(f);
  size_t fret = fread(buf, *len_ret, 1, f); /* \TODO: Handle \c ret. */
  if (fret <= 0) { ret = 0; }
  else { ret = buf; }
  fclose(f);
  return ret;
}

int fload_alt(const char * path, char ** buf_ret, size_t * len_ret)
{
  int ret = 0;
  const int fd = open((char*)path, O_RDONLY);  /* open */
  struct stat st; fstat(fd, &st); /* read status (and thereby size) */
  *len_ret = st.st_size;	      /* read size */
  *buf_ret = (char*)malloc(*len_ret);	      /*  */
  ret = read(fd, *buf_ret, *len_ret);
  close(fd);
  return ret;
}
