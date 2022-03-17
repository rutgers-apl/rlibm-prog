#include <math.h>
#include "progressive/float_prog_lib.h"
#include "progressive/log10consts.h"

#define LOG102HIGH 0.30102999566398114250631579125183634459972381591796875
#define LOG102LOW  5.27074231034726570126349709198449199648263806413338306011695522101945243775844573974609375e-17

double rlibm_prog_bf16_log10(float x) {
  float_x fix, fit;
  fix.f = x;
  int m = 0;
  
  if (fix.x < 0x800000 || fix.x >= 0x7F800000) {
    if ((fix.x & 0x7FFFFFFF) == 0) return -1.0 / 0.0;
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
  fit.x |= 0x3F800000;
  
  double f = fix.f - fit.f;
  f *= log_oneByF[FIndex];
    
  double y = 0;
    
  y += m * LOG102LOW;
  y += log10_lut[FIndex];
  y += m * LOG102HIGH;
  return y;
}
