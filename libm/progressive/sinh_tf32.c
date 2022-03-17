#include <math.h>
#include "progressive/float_prog_lib.h"
#include "progressive/sinhcoshconsts.h"

#define CONST64BYLN2 92.332482616893656768297660164535045623779296875
#define LN2BY64 0.01083042469624914509729318723429969395510852336883544921875

double rlibm_prog_tf32_sinh(float x) {
  float_x fx;
  fx.f = x;
  unsigned long sign = (fx.x & 0x80000000) == 0 ? 0x0 : 0x8000000000000000;
  fx.x &= 0x7FFFFFFF;
  
  // Take care of special cases
  if (fx.x <= 0x3d388000) return x;
  
  if (fx.x >= 0x42b2e000) {
      if (fx.x > 0x7F800000) return 0.0f/0.0f;
      return x / 0.0f;
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
    sinhL = 1.6666666665382681711271573021804215386509895324707031250000000000000000e-01;
    sinhL *= R2;
    sinhL += 9.9999999999999977795539507496869191527366638183593750000000000000000000e-01;
    sinhL *= R;
  }
  
  // Compute cosh component
  double coshL;
  if (R == 8.3387088168791478892671875655651092529296875000000000000000000000000000e-03) {
    coshL = 1.0000347367018735944554919115034863352775573730468750000000000000000000e+00;
  } else {
    coshL = 4.9999999994849525508655574412841815501451492309570312500000000000000000e-01;
    coshL *= R2;
    coshL += 1.0000000000000002220446049250313080847263336181640625000000000000000000e+00;
  }
    
  // Perform output compensation
  double_x dX;
  dX.d = sinhHM * coshL + coshHM * sinhL;
  dX.x |= sign;
  return dX.d;
}
