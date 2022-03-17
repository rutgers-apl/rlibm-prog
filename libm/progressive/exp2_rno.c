#include <math.h>
#include "progressive/float_prog_lib.h"
#include "progressive/expconsts.h"

double rlibm_prog_rno_exp2(float x) {
  float_x fx;
  fx.f = x;
  
  // Take care of special cases
  if ((fx.x & 0x7FFFFFFF) == 0) return 1.0;
  
  if (fx.x <= 876128826) {
    if (fx.x <= 867740218) return 1.0000000298023223876953125;
    return 1.0000000894069671630859375;
  }
  
  if (1124073472 <= fx.x && fx.x <= 3015223867) {
    if (fx.x < 0x80000000) {
      if (fx.x < 0x7F800000) return 3.40282361850336062550457001444955389952e+38;
      if (fx.x == 0x7F800000) return 1.0 / 0.0;
      return 0.0/0.0;
    }
    
    // negative counterpart
    if (fx.x <= 3006835259) return 0.99999998509883880615234375;
    
    return 0.99999995529651641845703125;
  }
  
  if (fx.x >= 3272998913) {
    if (fx.x == 0xFF800000) return 0.0;
    if (fx.x < 0xFF800000) return ldexp(1.0, -151);
    return 0.0/0.0;
  }
  
  // Perform range reduction
  double xp = x * 64;
  int N = (int)xp;
  int N2 = N % 64;
  if (N2 < 0) N2 += 64;
  int N1 = N - N2;
  
  int M = N1 / 64;
  int J = N2;
  double R = x - N * 0.015625;
  
  if (R == 0.0 && N2 == 0) {
    return ldexp(1.0, M);
  }
  
  double y = 2.4130489079041634106759239841721864650025963783264160156250000000000000e-03;
  y *= R;
  y += 1.3447329885844189287930072751464649627450853586196899414062500000000000e-03;
  y *= R;
  y += 9.6174549660996543759550192476126539986580610275268554687500000000000000e-03;
  y *= R;
  y += 5.5504106323778230891452523110274341888725757598876953125000000000000000e-02; // tf32
  y *= R;
  y += 2.4022650700583483840411247456358978524804115295410156250000000000000000e-01; // bf16
  y *= R;
  y += 6.9314718055997226464626237429911270737648010253906250000000000000000000e-01;
  y *= R;
  y += 9.9999999999999988897769753748434595763683319091796875000000000000000000e-01;

  // Perform output compensation
  return y * ldexp(exp2JBy64[J], M);
}
