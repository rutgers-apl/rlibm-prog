#include <math.h>
#include "progressive/float_prog_lib.h"
#include "progressive/expconsts.h"
#include "progressive/exp2.h"

double rlibm_prog_tf32_exp2(float x) {
  float_x fx;
  fx.f = x;
  
  // If x =< -150, then 2^x rounds to 0
  if (x <= -150) return 0.0;
  
  // If x is between these values, 2^x rounds to 1.0
  if (-4.2995662141720458748750388622283935546875e-08 <= x &&
      x <= 8.59913171780135598964989185333251953125e-08)
    return 1.0;
  
  // If x >= 128, then 2^x rounds to infinity
  if (128 <= x) return 1.0/0.0;
  
  // NaN ressults in NaN
  if ((fx.x & 0x7FFFFFFF) > 0x7F800000) return 0.0/0.0;
  
  // Perform range reduction
  double xp = x * 64;
  int N = (int)xp;
  int N2 = N % 64;
  if (N2 < 0) N2 += 64;
  int N1 = N - N2;
  
  int M = N1 / 64;
  int J = N2;
  double R = x - N * 0.015625;
  
  double y = 5.5504106323778230891452523110274341888725757598876953125000000000000000e-02; // tf32
  y *= R;
  y += 2.4022650700583483840411247456358978524804115295410156250000000000000000e-01; // bf16
  y *= R;
  y += 6.9314718055997226464626237429911270737648010253906250000000000000000000e-01;
  y *= R;
  y += 9.9999999999999988897769753748434595763683319091796875000000000000000000e-01;

  // Perform output compensation
  return y * ldexp(exp2JBy64[J], M);
}
