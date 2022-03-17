#define _GNU_SOURCE
#include <stdio.h>
#include "float34RO_math.h"
#include "float_rno_lib.h"
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

void RunTest(char* FileName) {

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

  unsigned long rlibm_34RO_wrongResult = 0, rlibm_34RO_totalTime = 0;
  unsigned long rlibm_34RO_t1, rlibm_34RO_t2;

  unsigned long rlibm_rno_wrongResult = 0, rlibm_rno_totalTime = 0;
  unsigned long rlibm_rno_t1, rlibm_rno_t2;
  
  unsigned long rlibm_pie_wrongResult = 0, rlibm_pie_totalTime = 0;
  unsigned long rlibm_pie_t1, rlibm_pie_t2;

  unsigned int dummy;
  
  float x, rlibm_rno_res, rlibm_34RO_res, rlibm_pie_res, rlibm32_res, glibc_double_res, glibc_float_res, rlibm_fast_res;
  floatX xbase;

  int fd = open(FileName, O_RDONLY);

  // Get Size of oracle file
  int status = fstat(fd, &s);
  size_t file_size = s.st_size;

  // MMap oracle file
  float* oracle = mmap(0, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (oracle == MAP_FAILED) {
    printf("mmap %s failed: %s\n", FileName, strerror(errno));
    exit(0);
  }
  
  
  float rinps[50000000];
  for (int i = 0; i < 50000000; i++) {
    while (1) {
      unsigned v = rand();
      float f = *(float *)&v;
      if (f != f) continue;
      rinps[i] = f;
      break;
    }
  }
  
  printf("done\n");
  exit(0);
  
  for (count = 0x0; count < 0x100000000; count++) {
    xbase.x = count;
    x = xbase.f;
    
    // Check new rlibm-rno speed
    do {
      rlibm_rno_t1 = __rdtscp(&dummy);
      rlibm_rno_res = __RLIBM_RNO_ELEM__(x);
      rlibm_rno_t2 = __rdtscp(&dummy);
    } while(rlibm_rno_t2 <= rlibm_rno_t1);
    rlibm_rno_totalTime += (rlibm_rno_t2 - rlibm_rno_t1);
    
    // Check new rlibm-pie speed
    do {
      rlibm_pie_t1 = __rdtscp(&dummy);
      rlibm_pie_res = __RLIBM_PIE_ELEM__(x);
      rlibm_pie_t2 = __rdtscp(&dummy);
    } while(rlibm_pie_t2 <= rlibm_pie_t1);
    rlibm_pie_totalTime += (rlibm_pie_t2 - rlibm_pie_t1);
    
    
    // Check rlibm-allspeed
    do {
      rlibm_34RO_t1 = __rdtscp(&dummy);
      rlibm_34RO_res = __RLIBM_34RO_ELEM__(x);
      rlibm_34RO_t2 = __rdtscp(&dummy);
    } while(rlibm_34RO_t2 <= rlibm_34RO_t1);
    rlibm_34RO_totalTime += (rlibm_34RO_t2 - rlibm_34RO_t1);
    
    
    // Check rlibm32 speed
    do {
      rlibm32_t1 = __rdtscp(&dummy);
      rlibm32_res = __RLIBM32_ELEM__(x);
      rlibm32_t2 = __rdtscp(&dummy);
    } while(rlibm32_t2 <= rlibm32_t1);
    rlibm32_totalTime += (rlibm32_t2 - rlibm32_t1);
    
    
    // Check glibc_float speed
    do {
      glibc_float_t1 = __rdtscp(&dummy);
      glibc_float_res = __GLIBC_FLOAT_ELEM__(x);
      glibc_float_t2 = __rdtscp(&dummy);
    } while(glibc_float_t2 <= glibc_float_t1);
    glibc_float_totalTime += (glibc_float_t2 - glibc_float_t1);
    
    
    // Check glibc_double speed
    do {
      glibc_double_t1 = __rdtscp(&dummy);
      glibc_double_res = __GLIBC_DOUBLE_ELEM__(x);
      glibc_double_t2 = __rdtscp(&dummy);
    } while(glibc_double_t2 <= glibc_double_t1);
    glibc_double_totalTime += (glibc_double_t2 - glibc_double_t1);
    
    
    // Check rlibm_fast speed
    do {
      rlibm_fast_t1 = __rdtscp(&dummy);
      rlibm_fast_res = __RLIBM_FAST_ELEM__(x);
      rlibm_fast_t2 = __rdtscp(&dummy);
    } while(rlibm_fast_t2 <= rlibm_fast_t1);
    rlibm_fast_totalTime += (rlibm_fast_t2 - rlibm_fast_t1);
    
    float oracleResult = oracle[count];
    
    // Now check if the two values are exactly the same
    if ((oracleResult != rlibm_rno_res) &&
        (oracleResult == oracleResult || rlibm_rno_res == rlibm_rno_res))
      rlibm_rno_wrongResult++;
    
    if ((oracleResult != rlibm_34RO_res) &&
        (oracleResult == oracleResult || rlibm_34RO_res == rlibm_34RO_res))
      rlibm_34RO_wrongResult++;
    
    if ((oracleResult != rlibm_pie_res) &&
        (oracleResult == oracleResult || rlibm_pie_res == rlibm_pie_res))
      rlibm_pie_wrongResult++;
    
    if ((oracleResult != rlibm32_res) &&
        (oracleResult == oracleResult || rlibm32_res == rlibm32_res))
      rlibm32_wrongResult++;
    if ((oracleResult != glibc_float_res) &&
        (oracleResult == oracleResult || glibc_float_res == glibc_float_res))
      glibc_float_wrongResult++;
    if ((oracleResult != glibc_double_res) &&
        (oracleResult == oracleResult || glibc_double_res == glibc_double_res))
      glibc_double_wrongResult++;
    if ((oracleResult != rlibm_fast_res) &&
        (oracleResult == oracleResult || rlibm_fast_res == rlibm_fast_res))
      rlibm_fast_wrongResult++;
    
    if (count % 10000000 == 0) {
      fprintf(stderr, "count = %lu (%lu)\r", count, rlibm_fast_wrongResult);
      fflush(stdout);
    }
  }
  
  
  
  
  
  
  
  for (count = 0x0; count < 0x100000000; count++) {
    xbase.x = count;
    x = xbase.f;

    // Check new rlibm-rno speed
    do {
      rlibm_rno_t1 = __rdtscp(&dummy);
      rlibm_rno_res = __RLIBM_RNO_ELEM__(x);
      rlibm_rno_t2 = __rdtscp(&dummy);
    } while(rlibm_rno_t2 <= rlibm_rno_t1);
    rlibm_rno_totalTime += (rlibm_rno_t2 - rlibm_rno_t1);
    
    // Check new rlibm-pie speed
    do {
      rlibm_pie_t1 = __rdtscp(&dummy);
      rlibm_pie_res = __RLIBM_PIE_ELEM__(x);
      rlibm_pie_t2 = __rdtscp(&dummy);
    } while(rlibm_pie_t2 <= rlibm_pie_t1);
    rlibm_pie_totalTime += (rlibm_pie_t2 - rlibm_pie_t1);


    // Check rlibm-allspeed
    do {
      rlibm_34RO_t1 = __rdtscp(&dummy);
      rlibm_34RO_res = __RLIBM_34RO_ELEM__(x);
      rlibm_34RO_t2 = __rdtscp(&dummy);
    } while(rlibm_34RO_t2 <= rlibm_34RO_t1);
    rlibm_34RO_totalTime += (rlibm_34RO_t2 - rlibm_34RO_t1);


    // Check rlibm32 speed
    do {
      rlibm32_t1 = __rdtscp(&dummy);
      rlibm32_res = __RLIBM32_ELEM__(x);
      rlibm32_t2 = __rdtscp(&dummy);
    } while(rlibm32_t2 <= rlibm32_t1);
    rlibm32_totalTime += (rlibm32_t2 - rlibm32_t1);

    
    // Check glibc_float speed
    do {
      glibc_float_t1 = __rdtscp(&dummy);
      glibc_float_res = __GLIBC_FLOAT_ELEM__(x);
      glibc_float_t2 = __rdtscp(&dummy);
    } while(glibc_float_t2 <= glibc_float_t1);
    glibc_float_totalTime += (glibc_float_t2 - glibc_float_t1);

    
    // Check glibc_double speed
    do {
      glibc_double_t1 = __rdtscp(&dummy);
      glibc_double_res = __GLIBC_DOUBLE_ELEM__(x);
      glibc_double_t2 = __rdtscp(&dummy);
    } while(glibc_double_t2 <= glibc_double_t1);
    glibc_double_totalTime += (glibc_double_t2 - glibc_double_t1);

    
    // Check rlibm_fast speed
    do {
      rlibm_fast_t1 = __rdtscp(&dummy);
      rlibm_fast_res = __RLIBM_FAST_ELEM__(x);
      rlibm_fast_t2 = __rdtscp(&dummy);
    } while(rlibm_fast_t2 <= rlibm_fast_t1);
    rlibm_fast_totalTime += (rlibm_fast_t2 - rlibm_fast_t1);

    float oracleResult = oracle[count];
    
    // Now check if the two values are exactly the same
    if ((oracleResult != rlibm_rno_res) &&
        (oracleResult == oracleResult || rlibm_rno_res == rlibm_rno_res))
      rlibm_rno_wrongResult++;

    if ((oracleResult != rlibm_34RO_res) &&
        (oracleResult == oracleResult || rlibm_34RO_res == rlibm_34RO_res))
      rlibm_34RO_wrongResult++;
    
    if ((oracleResult != rlibm_pie_res) &&
        (oracleResult == oracleResult || rlibm_pie_res == rlibm_pie_res))
      rlibm_pie_wrongResult++;
    
    if ((oracleResult != rlibm32_res) &&
        (oracleResult == oracleResult || rlibm32_res == rlibm32_res))
      rlibm32_wrongResult++;
    if ((oracleResult != glibc_float_res) &&
        (oracleResult == oracleResult || glibc_float_res == glibc_float_res))
      glibc_float_wrongResult++;
    if ((oracleResult != glibc_double_res) &&
        (oracleResult == oracleResult || glibc_double_res == glibc_double_res))
      glibc_double_wrongResult++;
    if ((oracleResult != rlibm_fast_res) &&
        (oracleResult == oracleResult || rlibm_fast_res == rlibm_fast_res))
      rlibm_fast_wrongResult++;
    
    if (count % 10000000 == 0) {
      fprintf(stderr, "count = %lu (%lu)\r", count, rlibm_fast_wrongResult);
      fflush(stdout);
    }
  }

  // Un-mmap oracle file
  munmap(oracle, file_size);
  close(fd);
  
  printf("Wrong results: \n");
  printf("rlibm-rno    wrong result: %lu\n", rlibm_rno_wrongResult);
  printf("rlibm-pie    wrong result: %lu\n", rlibm_pie_wrongResult);
  printf("rlibm-all   wrong result: %lu\n", rlibm_34RO_wrongResult);
  printf("rlibm32      wrong result: %lu\n", rlibm32_wrongResult);
  printf("glibc float  wrong result: %lu\n", glibc_float_wrongResult);
  printf("glibc double wrong result: %lu\n", glibc_double_wrongResult);
  printf("rlibm-fast   wrong result: %lu\n", rlibm_fast_wrongResult);
  
  printf("Speedup (baseline: rlibm-rno. Above 1.0 means rlibm-rno is better)\n");
  printf("vs. rlibm-rno     : %.5f\n", 1.0 * rlibm_rno_totalTime / rlibm_pie_totalTime);
  printf("vs. rlibm-all     : %.5f\n", 1.0 * rlibm_34RO_totalTime / rlibm_pie_totalTime);
  printf("vs. rlibm-fast      : %.5f\n", 1.0 * rlibm_fast_totalTime / rlibm_pie_totalTime);
  printf("vs. rlibm32      : %.5f\n", 1.0 * rlibm32_totalTime / rlibm_pie_totalTime);
  printf("vs. glibc float  : %.5f\n", 1.0 * glibc_float_totalTime / rlibm_pie_totalTime);
  printf("vs. glibc double : %.5f\n", 1.0 * glibc_double_totalTime / rlibm_pie_totalTime);
}
