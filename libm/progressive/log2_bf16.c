#include <math.h>
#include "progressive/float_prog_lib.h"
#include "progressive/log2consts.h"
#include "progressive/log2.h"

double rlibm_prog_bf16_log2(float x) {
  float_x fix, fit;
  fix.f = x;
  int m = 0;
  
  if (fix.x < 0x800000 || fix.x >= 0x7F800000) {
    if (x == 0) return -1.0 / 0.0;
    if (fix.x > 0x7FFFFFFF) return 0.0 / 0.0;
    if (fix.x >= 0x7F800000) return x + x;
      
    fix.f *= 8.388608e+06;
    m -= 23;
  }
  
  m += fix.x >> 23;
  m -= 127;
  fix.x &= 0x007FFFFF;
  fix.x |= 0x3F800000;
  
  fit.x = fix.x & 0x007F0000;
  int FIndex = fit.x >> 16;
    
  return __log2_lut[FIndex] + m;
}
