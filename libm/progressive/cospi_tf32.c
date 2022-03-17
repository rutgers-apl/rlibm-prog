#include <math.h>
#include "progressive/float_prog_lib.h"
#include "progressive/sinpicospiconsts.h"

double rlibm_prog_tf32_cospi(float x) {
  float_x fX;
  fX.f = x;
  fX.x &= 0x7FFFFFFF;
  
  // Special cases:
  if (fX.x <= 0x3be66000) return 1.0f;
  
  if (fX.x >= 0x44800000) {
    if (fX.x >= 0x7F800000) return 0.0f/0.0f;
    if (fX.x >= 0x45000000) return 1.0f;
    if ((fX.x & 0x2000) == 0) return 1.0f;
    return -1.0f;
  }
  
  // Range Reduction
  double xp = fX.f * 512.0;
  unsigned N = (unsigned)xp;
  unsigned N2 = N & 0xFF;
  unsigned I = (N >> 8) + 1;
  double R, cospiM, sinpiM;
  unsigned long s = (I & 0x2) ? 0x8000000000000000 : 0;

  if (I & 1) {
    if (N2 == 0) {
      R = fX.f - N * 0.001953125;
      cospiM = 1.0;
      sinpiM = 0.0;
    }
    else {
      N2++;
      R = (N + 1) * 0.001953125 - fX.f;
      cospiM = sinpiMBy512[256 - N2];
      sinpiM = cospiMBy512[256 - N2];
    }
  } else {
    R = fX.f - N * 0.001953125;
    cospiM = sinpiMBy512[N2];
    sinpiM = cospiMBy512[N2];
  }
  
  double R2 = R * R;
  double cospiR, sinpiR;
      
  sinpiR = -5.16771279405369199366759858094155788421630859375; //tf32 bf16
  sinpiR *= R2;
  sinpiR += 3.14159265358979755689006196917034685611724853515625;
  sinpiR *= R;
  
  cospiR = -4.93480220469346253509002053760923445224761962890625; //tf32 bf16
  cospiR *= R2;
  cospiR += 1.0000000000000015543122344752191565930843353271484375;
  
  double_x dX;
  dX.d = cospiM * cospiR + sinpiM * sinpiR;
  dX.x ^= s;
    
  return dX.d;
}
