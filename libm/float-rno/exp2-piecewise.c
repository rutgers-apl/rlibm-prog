#include <math.h>
#include "float-rno/float_rno_lib.h"
#include "exp2.h"

double rlibm_rno_exp2_piecewise(float x) {
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

  double y = 0.0;
  
  if (R < -1.1579314929122119792737066745758056640625e-07) {
    y = 2.3124617654531440420762056220382874016650021076202392578125e-03;
    y *= R;
    y += 9.65120837185145553915877059125705272890627384185791015625e-03;
    y *= R;
    y += 5.5504482021450037010357192457377095706760883331298828125e-02;
    y *= R;
    y += 2.40226508538220417943165330143528990447521209716796875e-01;
    y *= R;
    y += 6.9314718056170143700711605561082251369953155517578125e-01;
    y *= R;
    y += 9.9999999999999988897769753748434595763683319091796875e-01;
    
  } else {
    y = 1.6866084696630545720907878859406991978175938129425048828125e-03;
    y *= R;
    y += 9.60637228768240931942035132351520587690174579620361328125e-03;
    y *= R;
    y += 5.55042469539323291005672444953233934938907623291015625e-02;
    y *= R;
    y += 2.4022650627551767055223308489075861871242523193359375e-01;
    y *= R;
    y += 6.93147180561226594619483876158483326435089111328125e-01;
    y *= R;
    y += 9.99999999999999555910790149937383830547332763671875e-01;
  }

  // Perform output compensation
  return y * ldexp(exp2JBy64[J], M);
}
