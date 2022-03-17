#include <math.h>
#include "progressive/float_prog_lib.h"
#include "progressive/logconsts.h"

#define LN2HIGH 0.69314718055994528622676398299518041312694549560546875

double rlibm_prog_bf16_log(float x) {
  float_x fix, fit;
  fix.f = x;
  int m = 0;
  
  if (fix.x < 0x800000 || fix.x >= 0x7F800000) {
    if ((fix.x & 0x7FFFFFFF) == 0) return -1.0 / 0.0;
    if (fix.x > 0x7F800000) return 0.0 / 0.0;
    if (fix.x == 0x7F800000) return 1.0 / 0.0;
      
    fix.f *= 8.388608e+06;
    m -= 23;
  }
  
  m += fix.x >> 23;
  m -= 127;
  fix.x &= 0x007FFFFF;
  fix.x |= 0x3F800000;
  
  fit.x = fix.x & 0x007F0000;
  int FIndex = fit.x >> 16;
  fit.x |= 0x3F800000;
  
  double f = fix.f - fit.f;
  f *= log_oneByF[FIndex];
  
  double y = 0;
  y += ln_lutHIGH[FIndex];
  y += m * LN2HIGH;
  
  return y;
}
