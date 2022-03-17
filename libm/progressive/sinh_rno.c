#include <math.h>
#include "progressive/float_prog_lib.h"
#include "progressive/sinhcoshconsts.h"

#define CONST64BYLN2 92.332482616893656768297660164535045623779296875
#define LN2BY64 0.01083042469624914509729318723429969395510852336883544921875

double rlibm_prog_rno_sinh(float x) {
  float_x fx;
  fx.f = x;
  unsigned long sign = (fx.x & 0x80000000) == 0 ? 0x0 : 0x8000000000000000;
  fx.x &= 0x7FFFFFFF;

  if (fx.x == 0) return 0.0;
  
  // Take care of special cases
  if (fx.x <= 971544424) {
    double_x dX;
    dX.d = (double)fx.f;
    long exp = (dX.x & 0x7FF0000000000000UL) >> 52UL;
    exp -= 1023L;
    long mantissaCount = exp + 149L;
    if (mantissaCount > 23) mantissaCount = 23;
    mantissaCount += 2L;
    unsigned long shiftAmount = (52L - mantissaCount);
    unsigned long sticky = 1UL << shiftAmount;
    dX.x |= sticky;
    dX.x |= sign;
    return dX.d;
  }
  
  if (fx.x >= 1119016189) {
    if (fx.x > 0x7F800000) return 0.0/0.0;
    if (fx.x == 0x7F800000) {
      if (x > 0.0f) return 1.0 / 0.0;
      else return -1.0 / 0.0;
    }

    if (x > 0.0f) return 3.40282361850336062550457001444955389952e+38;
    else return -3.40282361850336062550457001444955389952e+38;
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
  
  double sinhL;
  if (R == 8.3387088168791478892671875655651092529296875000000000000000000000000000e-03) {
    sinhL = 8.3388052000104226357812819969694828614592552185058593750000000000000000e-03;
  } else {
    sinhL = 8.3336375947769426170141926490941841620951890945434570312500000000000000e-03;
    sinhL *= R2;
    sinhL += 1.6666666665382681711271573021804215386509895324707031250000000000000000e-01;
    sinhL *= R2;
    sinhL += 9.9999999999999977795539507496869191527366638183593750000000000000000000e-01;
    sinhL *= R;
  }
  
  // Compute cosh component
  double coshL;
  if (R == 8.3387088168791478892671875655651092529296875000000000000000000000000000e-03) {
    coshL = 1.0000347367018735944554919115034863352775573730468750000000000000000000e+00;
  } else {
    coshL = 4.1667466122972861286566370608852594159543514251708984375000000000000000e-02;
    coshL *= R2;
    coshL += 4.9999999994849525508655574412841815501451492309570312500000000000000000e-01;
    coshL *= R2;
    coshL += 1.0000000000000002220446049250313080847263336181640625000000000000000000e+00;
  }
  
  // Perform output compensation
  double_x dX;
  dX.d = sinhHM * coshL + coshHM * sinhL;
  dX.x |= sign;
  return dX.d;
}
