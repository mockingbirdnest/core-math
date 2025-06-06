/* Correctly-rounded cubic root of binary32 value.

Copyright (c) 2023 Alexei Sibidanov.

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

#include <stdint.h>

// Warning: clang also defines __GNUC__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

#pragma STDC FENV_ACCESS ON

#define INEXACTFLAG 0
#if INEXACTFLAG!=0
#  include <x86intrin.h> /* for the x86 architecture with SSE to rise the inexact flag only when the root is indeed inexact */
#endif

typedef union {float f; uint32_t u;} b32u32_u;
typedef union {double f; uint64_t u;} b64u64_u;

float cr_cbrtf (float x){
  static const double escale[3] = {1.0, 0x1.428a2f98d728bp+0/* 2^(1/3) */, 0x1.965fea53d6e3dp+0/* 2^(2/3) */};
#if INEXACTFLAG!=0
  volatile uint32_t flag = _mm_getcsr(); /* store MXCSR Control/Status Register */
#endif
  b32u32_u t = {.f = x};
  uint32_t u = t.u, au = u<<1, sgn = u>>31, e = au>>24;
  if(__builtin_expect(au<1u<<24 || au>=0xffu<<24, 0)){
    if(au>=0xffu<<24) return x; /* inf, nan */
    if(au==0) return x; /* +-0 */
    int nz = __builtin_clz(au) - 7;  /* subnormal */
    au <<= nz;
    e -= nz-1;
  }
  uint32_t mant = au&0xffffff;
  b64u64_u cvt1 = {.u = (uint64_t)mant<<28|(0x3fful<<52)};
  e += 899;
  uint32_t et = e/3, it = e%3;
  uint64_t isc = ((const uint64_t*)escale)[it];
  isc += (long)(et - 342)<<52;
  isc |= (long)sgn<<63;
  b64u64_u cvt2 = {.u = isc};
  static const double c[] =
    {0x1.2319d352ea5d5p-1, 0x1.67ad8ee258d1ap-1, -0x1.9342edf9cbad9p-2, 0x1.b6388fc510a75p-3,
     -0x1.6002455599e2fp-4, 0x1.7b096936192c4p-6, -0x1.e5577187e8bf8p-9, 0x1.169ef81d6c34ep-12};
  double z = cvt1.f, r0 = -0x1.9931c6c2d19d1p-6/z, z2 = z*z, z4 = z2*z2;
  double f = ((c[0] + z*c[1]) + z2*(c[2] + z*c[3])) + z4*((c[4] + z*c[5]) + z2*(c[6] + z*c[7])) + r0;
  double r = f * cvt2.f;
  float ub = r, lb = r - cvt2.f*1.4182e-9;
  if(__builtin_expect(ub==lb, 1)) return ub;
  const double u0 = -0x1.ab16ec65d138fp+3;
  double h = f*f*f - z;
  f -= (f*r0*u0)*h;
  r = f * cvt2.f;
  cvt1.f = r;
  ub = r;
  long m0 = cvt1.u<<19, m1 = m0>>63;
  if(__builtin_expect((m0^m1)<(1l<<31),0)){
    cvt1.u = (cvt1.u + (1ul<<31))&0xffffffff00000000ul;
    ub = cvt1.f;
#if INEXACTFLAG!=0
    _mm_setcsr(flag); /* restore MXCSR Control/Status Register for exact roots to get rid of the inexact flag if risen inside the function */
#endif
  }
  return ub;
}
