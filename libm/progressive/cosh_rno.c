#include <math.h>
#include "progressive/float_prog_lib.h"
#include "progressive/sinhcoshconsts.h"

#define CONST64BYLN2 92.332482616893656768297660164535045623779296875
#define LN2BY64 0.01083042469624914509729318723429969395510852336883544921875

double rlibm_prog_rno_cosh(float x) {
  float_x fx;
  fx.f = x;
  fx.x &= 0x7FFFFFFF;
  
  // Take care of special cases
  if (fx.x <= 973078527) {
    if (fx.x == 0) return 1.0;
    if (fx.x <= 968164595) return 1.0000000298023223876953125;
    return 1.0000000894069671630859375;
  }
  
  if (fx.x >= 1119016189) {
      if (fx.x > 0x7F800000) return 0.0/0.0;
  if (fx.x == 0x7F800000) return 1.0 / 0.0;
      return 3.40282361850336062550457001444955389952e+38;
  }
  
  // Perform range reduction
  double xp = fx.f * CONST64BYLN2;
  int N = (int)xp;
  int N2 = N % 64;
  if (N2 < 0) N2 += 64;
  int N1 = N - N2;
  int I = N1 / 64;
  double R = fx.f - N * LN2BY64;
  double R2 = R * R;
  
  double sinhHigh = sinhKLn2[I];
  double coshHigh = coshKLn2[I];
  double sinhMid = sinhKLn2By64[N2];
  double coshMid = coshKLn2By64[N2];
  
  double sinhHM = sinhHigh * coshMid + coshHigh * sinhMid;
  double coshHM = sinhHigh * sinhMid + coshHigh * coshMid;
  
  // Compute sinh component

  double sinhL = 8.3362525934194634663620959713625779841095209121704101562500000000000000e-03;
  sinhL *= R2;
  sinhL += 1.6666666640246388664259313827642472460865974426269531250000000000000000e-01; // tf32 bf16
  sinhL *= R2;
  sinhL += 1.0000000000000017763568394002504646778106689453125000000000000000000000e+00;
  sinhL *= R;
  
  // Compute cosh component
  double coshL = 4.1668689285602612815129219825394102372229099273681640625000000000000000e-02;
  coshL *= R2;
  coshL += 4.9999999983317883200939490961900446563959121704101562500000000000000000e-01; // tf32 bf16
  coshL *= R2;
  coshL += 1.0000000000000004440892098500626161694526672363281250000000000000000000e+00;
  
  // Perform output compensation
  return sinhHM * sinhL + coshHM * coshL;
}
