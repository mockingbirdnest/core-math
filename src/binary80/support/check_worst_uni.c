/* Check correctness of univariate long double function on worst cases.

Copyright (c) 2022-2024 Stéphane Glondu and Paul Zimmermann, Inria.

This file is part of the CORE-MATH project
(https://core-math.gitlabpages.inria.fr/).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define _POSIX_C_SOURCE 200809L  /* for getline */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fenv.h>
#ifndef CORE_MATH_NO_OPENMP
#include <omp.h>
#endif

#include "function_under_test.h"

long double cr_function_under_test (long double);
long double ref_function_under_test (long double);
int ref_fesetround (int);
void ref_init (void);

int rnd1[] = { FE_TONEAREST, FE_TOWARDZERO, FE_UPWARD, FE_DOWNWARD };

int rnd = 0;

typedef union {long double f; struct {uint64_t m; uint16_t e;};} b80u80_t;

static void
readstdin(long double **result, int *count)
{
  char *buf = NULL;
  size_t buflength = 0;
  ssize_t n;
  int allocated = 512;

  *count = 0;
  if (NULL == (*result = malloc(allocated * sizeof(long double)))) {
    fprintf(stderr, "malloc failed\n");
    exit(1);
  }

  while ((n = getline(&buf, &buflength, stdin)) >= 0) {
    if (n > 0 && buf[0] == '#') continue;
    if (*count >= allocated) {
      int newsize = 2 * allocated;
      long double *newresult = realloc(*result, newsize * sizeof(long double));
      if (NULL == newresult) {
        fprintf(stderr, "realloc(%d) failed\n", newsize);
        exit(1);
      }
      allocated = newsize;
      *result = newresult;
    }
    long double *item = *result + *count;
    // special code for snan, since glibc does not read them
    if (strncmp (buf, "snan", 4) == 0 || strncmp (buf, "+snan", 5) == 0)
    {
      b80u80_t v;
      // +snan has encoding m=2^63+1, e=32767 (for example)
      v.e = 0x7fff;
      v.m = 0x8000000000000001ul;
      *item = v.f;
      (*count)++;
    }
    else if (strncmp (buf, "-snan", 5) == 0)
    {
      b80u80_t v;
      // -snan has encoding m=2^63+1, e=65535 (for example)
      v.e = 0xffff;
      v.m = 0x8000000000000001ul;
      *item = v.f;
      (*count)++;
    }
    else if (sscanf(buf, "%La", item) == 1)
    {
      (*count)++;
    }
  }
}

static int
is_nan (long double x)
{
  b80u80_t v = {.f = x};
  return ((v.e & 0x7ffful) == 0x7fff && (v.m != (1ul << 63)));
}

static inline int
is_equal (long double x, long double y)
{
  if (is_nan (x))
    return is_nan (y);
  if (is_nan (y))
    return is_nan (x);
  b80u80_t v = {.f = x}, w = {.f = y};
  return v.e == w.e && v.m == w.m; // ensures +0 and -0 differ
}

void
doloop(void)
{
  long double *items;
  int count, tests = 0, failures = 0;

  readstdin(&items, &count);

#ifndef CORE_MATH_NO_OPENMP
#pragma omp parallel for reduction(+: failures,tests)
#endif
  for (int i = 0; i < count; i++) {
    ref_init();
    ref_fesetround(rnd);
    fesetround(rnd1[rnd]);
    long double x = items[i];
    long double z1 = ref_function_under_test(x);
    long double z2 = cr_function_under_test(x);
    tests ++;
    /* Note: the test z1 != z2 would not distinguish +0 and -0. */
    if (is_equal (z1, z2) == 0) {
      printf("FAIL x=%La ref=%La z=%La\n", x, z1, z2);
      fflush(stdout);
#ifdef DO_NOT_ABORT
      failures ++;
#else
      exit(1);
#endif
    }
#ifdef WORST_SYMMETRIC
    x = -x;
    z1 = ref_function_under_test(x);
    z2 = cr_function_under_test(x);
    tests ++;
    if (is_equal (z1, z2) == 0) {
      printf("FAIL x=%La ref=%La z=%La\n", x, z1, z2);
      fflush(stdout);
#ifdef DO_NOT_ABORT
      failures ++;
#else
      exit(1);
#endif
    }
#endif /* WORST_SYMMETRIC */
  }

  free(items);
  printf("%d tests passed, %d failure(s)\n", tests, failures);
}

// check the "long double" type is the double-extended format
static void
check_long_double (void)
{
  fesetround (FE_TONEAREST);
  long double x = 1.0, y = 1.0;
  int p = 0;
  while (x + y != x)
    y = y * 0.5, p ++;
  if (p != 64)
  {
    printf ("The long-double format is not the double-extended format\n");
    printf ("It has a precision of %d bits\n", p);
    exit (1);
  }
}

int
main (int argc, char *argv[])
{
  while (argc >= 2)
    {
      if (strcmp (argv[1], "--rndn") == 0)
        {
          rnd = 0;
          argc --;
          argv ++;
        }
      else if (strcmp (argv[1], "--rndz") == 0)
        {
          rnd = 1;
          argc --;
          argv ++;
        }
      else if (strcmp (argv[1], "--rndu") == 0)
        {
          rnd = 2;
          argc --;
          argv ++;
        }
      else if (strcmp (argv[1], "--rndd") == 0)
        {
          rnd = 3;
          argc --;
          argv ++;
        }
      else
        {
          fprintf (stderr, "Error, unknown option %s\n", argv[1]);
          exit (1);
        }
    }

  check_long_double ();

  doloop();
}
