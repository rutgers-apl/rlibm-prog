#include <math.h>
#include "progressive/float_prog_lib.h"
#include "progressive/sinpicospiconsts.h"

#define PI 3.141592653589793115997963468544185161590576171875

double rlibm_prog_tf32_sinpi(float x) {
  float_x fX;
  fX.f = x;
  unsigned s = fX.x & 0x80000000;
  fX.x &= 0x7FFFFFFF;
  
  // Special cases:
  if (fX.x <= 0x3a9a6000) {
      return PI * (double)x;
  }
  
  if (fX.x >= 0x44800000) {
      if (fX.x >= 0x7F800000) {
          return 0.0f/0.0f;
      }
      return 0.0f;
  }
  
  double xp = fX.f * 512;
  unsigned N = (unsigned)xp;
  unsigned N2 = N & 0xFF;
  unsigned I = N >> 8;
  double R;
  
  if (I & 0x1) {
      N2 = 255 - N2;
      R = (N + 1) * 0.001953125 - fX.f;
  } else R = fX.f - N * 0.001953125;
  
  if (I & 0x2) s ^= 0x80000000;
  
  double R2 = R * R;
  
  double cospiR, sinpiR;
  sinpiR = -5.1677127873202506336269834719132632017135620117187500000000000000000000e+00;
  sinpiR *= R2;
  sinpiR += 3.1415926535897931159979634685441851615905761718750000000000000000000000e+00;
  sinpiR *= R;
  
  cospiR = -4.9348022065162915339442406548187136650085449218750000000000000000000000e+00;
  cospiR *= R2;
  cospiR += 1.0000000000000011102230246251565404236316680908203125000000000000000000e+00;
  
  fX.f = sinpiMBy512[N2] * cospiR + cospiMBy512[N2] * sinpiR;
  fX.x ^= s;
  
  return fX.f;
}
