#define _GNU_SOURCE
#include <stdio.h>
#include "float_math.h"
#include "float_lib.h"
#include "math.h"
#include <x86intrin.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv){

  struct stat s;
  unsigned long count = 0;
  unsigned long rlibm32_wrongResult = 0, rlibm32_totalTime = 0;
  unsigned long rlibm32_t1, rlibm32_t2;
  unsigned long glibc_float_wrongResult = 0, glibc_float_totalTime = 0;
  unsigned long glibc_float_t1, glibc_float_t2;
  unsigned long glibc_double_wrongResult = 0, glibc_double_totalTime = 0;
  unsigned long glibc_double_t1, glibc_double_t2;
  unsigned long rlibm_fast_wrongResult = 0, rlibm_fast_totalTime = 0;
  unsigned long rlibm_fast_t1, rlibm_fast_t2;
  unsigned int dummy;
  
  float x, rlibm32_res[10], glibc_double_res[10], glibc_float_res[10], rlibm_fast_res[10];
  floatX xbase;

  
  for (count = 0x0; count < 0x100000000; count++) {

    
    xbase.x = count;
    x = xbase.f;

    // Check rlibm32 speed
    do {
      rlibm32_t1 = __rdtscp(&dummy);
      
      rlibm32_res[0] = rlibm_log2(x);
      rlibm32_res[1] = rlibm_log10(x);
      rlibm32_res[2] = rlibm_log(x);
      rlibm32_res[3] = rlibm_exp2(x);
      rlibm32_res[4] = rlibm_exp10(x);
      rlibm32_res[5] = rlibm_exp(x);
      rlibm32_res[6] = rlibm_sinpi(x);
      rlibm32_res[7] = rlibm_cospi(x);
      rlibm32_res[8] = rlibm_sinh(x);
      rlibm32_res[9] = rlibm_cosh(x);
      
      rlibm32_t2 = __rdtscp(&dummy);
    } while(rlibm32_t2 <= rlibm32_t1);
    rlibm32_totalTime += (rlibm32_t2 - rlibm32_t1);

#if 0    
    
    // Check glibc_float speed
    do {
      glibc_float_t1 = __rdtscp(&dummy);
      
      glibc_float_res[0] = log2f(x);
      glibc_float_res[1] = log10f(x);
      glibc_float_res[2] = logf(x);
      glibc_float_res[3] = exp2f(x);
      glibc_float_res[4] = exp10f(x);
      glibc_float_res[5] = expf(x);
      glibc_float_res[6] = sinpif(x);
      glibc_float_res[7] = cospif(x);
      glibc_float_res[8] = sinhf(x);
      glibc_float_res[9] = coshf(x);
  
      glibc_float_t2 = __rdtscp(&dummy);
    } while(glibc_float_t2 <= glibc_float_t1);
    glibc_float_totalTime += (glibc_float_t2 - glibc_float_t1);

    
    // Check glibc_double speed
    do {
      glibc_double_t1 = __rdtscp(&dummy);

      glibc_double_res[0] = log2(x);
      glibc_double_res[1] = log10(x);
      glibc_double_res[2] = log(x);
      glibc_double_res[3] = exp2(x);
      glibc_double_res[4] = exp10(x);
      glibc_double_res[5] = exp(x);
      glibc_double_res[6] = sinpi(x);
      glibc_double_res[7] = cospi(x);
      glibc_double_res[8] = sinh(x);
      glibc_double_res[9] = cosh(x);
      
      glibc_double_t2 = __rdtscp(&dummy);
    } while(glibc_double_t2 <= glibc_double_t1);
    glibc_double_totalTime += (glibc_double_t2 - glibc_double_t1);

#endif    
    // Check rlibm_fast speed
    do {
      rlibm_fast_t1 = __rdtscp(&dummy);
            
      rlibm_fast_res[0] = rlibm_fast_log2(x);
      rlibm_fast_res[1] = rlibm_fast_log10(x);
      rlibm_fast_res[2] = rlibm_fast_log(x);
      rlibm_fast_res[3] = rlibm_fast_exp2(x);
      rlibm_fast_res[4] = rlibm_fast_exp10(x);
      rlibm_fast_res[5] = rlibm_fast_exp(x);
      rlibm_fast_res[6] = rlibm_fast_sinpi(x);
      rlibm_fast_res[7] = rlibm_fast_cospi(x);
      rlibm_fast_res[8] = rlibm_fast_sinh(x);
      rlibm_fast_res[9] = rlibm_fast_cosh(x);
     
      
      rlibm_fast_t2 = __rdtscp(&dummy);
    } while(rlibm_fast_t2 <= rlibm_fast_t1);
    rlibm_fast_totalTime += (rlibm_fast_t2 - rlibm_fast_t1);


    // Now check if the two values are exactly the same
    for(int i = 0; i< 10; i ++){
      if ((rlibm_fast_res[i] != rlibm32_res[i]) &&
	  (rlibm32_res[i] == rlibm32_res[i]))
	rlibm32_wrongResult++;
#if 0      
      if ((rlibm_fast_res[i] != glibc_float_res[i]) &&
	  ((rlibm_fast_res[i] == rlibm_fast_res[i]) || (glibc_float_res[i] == glibc_float_res[i])))
	glibc_float_wrongResult++;
      if ((rlibm_fast_res[i] != glibc_double_res[i]) &&
	  ((rlibm_fast_res[i] == rlibm_fast_res[i]) || glibc_double_res[i] == glibc_double_res[i]))
	glibc_double_wrongResult++;
#endif
    }
    
    if (count % 10000000 == 0) {
      printf("count = %lu \r", count);
      fflush(stdout);
    }
  }
  
  printf("Wrong results: \n");
  printf("rlibm32      wrong result: %lu\n", rlibm32_wrongResult);
  //  printf("glibc float  wrong result: %lu\n", glibc_float_wrongResult);
  //  printf("glibc double wrong result: %lu\n", glibc_double_wrongResult);
  
  printf("Speedup (baseline: rlibm-fast. Above 1.0 means rlibm-fast is better)\n");
  printf("vs. rlibm32      : %.5f\n", 1.0 * rlibm32_totalTime / rlibm_fast_totalTime);
  //  printf("vs. glibc float  : %.5f\n", 1.0 * glibc_float_totalTime / rlibm_fast_totalTime);
  //  printf("vs. glibc double : %.5f\n", 1.0 * glibc_double_totalTime / rlibm_fast_totalTime);
}
