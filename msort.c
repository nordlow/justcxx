#include "msort.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MID(lo, hi) (lo + ((hi - lo) >> 1))

static void
msort1 (void *array, void *buf, size_t low, size_t high, size_t size,
        int (* cmp) (const void *, const void *))
{
  char *a1, *al, *am, *ah, *ls, *hs, *lo, *hi, *b;
  size_t copied = 0;
  size_t mid;
    
  mid = MID (low, high);
    
  if (mid + 1 < high)
    msort1 (array, buf, mid + 1, high, size, cmp);
    
  if (mid > low)
    msort1 (array, buf, low, mid, size, cmp);
    
  ah = ((char *) array) + ((high + 1) * size);
  am = ((char *) array) + ((mid + 1) * size);
  a1 = al = ((char *) array) + (low * size);
    
  b = (char *) buf;
  lo = al;
  hi = am;
    
  do {
    ls = lo;
    hs = hi;
        
    if (lo > al || hi > am) {
      /* our last loop already compared lo & hi and found lo <= hi */
      lo += size;
    }
        
    while (lo < am && cmp (lo, hi) <= 0)
      lo += size;
        
    if (lo < am) {
      if (copied == 0) {
        /* avoid copying the leading items */
        a1 = lo;
        ls = lo;
      }
            
      /* our last cmp tells us hi < lo */
      hi += size;
            
      while (hi < ah && cmp (hi, lo) < 0)
        hi += size;
            
      if (lo > ls) {
        memcpy (b, ls, lo - ls);
        copied += (lo - ls);
        b += (lo - ls);
      }
            
      memcpy (b, hs, hi - hs);
      copied += (hi - hs);
      b += (hi - hs);
    } else if (copied) {
      memcpy (b, ls, lo - ls);
      copied += (lo - ls);
      b += (lo - ls);
            
      /* copy everything we needed to re-order back into array */
      memcpy (a1, buf, copied);
      return;
    } else {
      /* everything already in order */
      return;
    }
  } while (hi < ah);
    
  if (lo < am) {
    memcpy (b, lo, am - lo);
    copied += (am - lo);
  }
    
  memcpy (a1, buf, copied);
}

int
msort (void *base, size_t nmemb, size_t size,
       int (* cmp) (const void *, const void *))
{
  void *tmp;

  if (nmemb < 2)
    return 0;

  if (!(tmp = malloc (nmemb * size))) {
    errno = ENOMEM;
    return -1;
  }

  msort1 (base, tmp, 0, nmemb - 1, size, cmp);

  free (tmp);

  return 0;
}
