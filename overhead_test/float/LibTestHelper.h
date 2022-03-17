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

// just flushing L1
#define FLUSH_SIZE 1024 * 64

size_t  flush_cache(int* temp_value){


  for(int i = 0; i < FLUSH_SIZE; i+=64){
    temp_value[i] = rand();
  }
  size_t ret_val = 0;

  for(int i = 0; i< FLUSH_SIZE; i+=64){
    ret_val = ret_val + temp_value[i];
  }
  return ret_val;
}

void RunTest(char* FileName) {

  int* temp_value = calloc(FLUSH_SIZE, sizeof(int));
  
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
  
  float x, rlibm32_res, glibc_double_res, glibc_float_res, rlibm_fast_res;
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

  size_t temp = 0;
  for (count = 0x0; count < 0x100000000; count++) {
    xbase.x = count;
    x = xbase.f;

    // Check rlibm32 speed
    do {
      rlibm32_t1 = __rdtscp(&dummy);
      rlibm32_res = __RLIBM32_ELEM__(x);
      rlibm32_t2 = __rdtscp(&dummy);
    } while(rlibm32_t2 <= rlibm32_t1);
    rlibm32_totalTime += (rlibm32_t2 - rlibm32_t1);

    //    temp = temp + flush_cache(temp_value);
    // Check glibc_float speed
    do {
      glibc_float_t1 = __rdtscp(&dummy);
      glibc_float_res = __GLIBC_FLOAT_ELEM__(x);
      glibc_float_t2 = __rdtscp(&dummy);
    } while(glibc_float_t2 <= glibc_float_t1);
    glibc_float_totalTime += (glibc_float_t2 - glibc_float_t1);

    //    temp = temp + flush_cache(temp_value);
    // Check glibc_double speed
    do {
      glibc_double_t1 = __rdtscp(&dummy);
      glibc_double_res = __GLIBC_DOUBLE_ELEM__(x);
      glibc_double_t2 = __rdtscp(&dummy);
    } while(glibc_double_t2 <= glibc_double_t1);
    glibc_double_totalTime += (glibc_double_t2 - glibc_double_t1);

    //    temp = temp + flush_cache(temp_value);
    // Check rlibm_fast speed
    do {
      rlibm_fast_t1 = __rdtscp(&dummy);
      rlibm_fast_res = __RLIBM_FAST_ELEM__(x);
      rlibm_fast_t2 = __rdtscp(&dummy);
    } while(rlibm_fast_t2 <= rlibm_fast_t1);
    rlibm_fast_totalTime += (rlibm_fast_t2 - rlibm_fast_t1);

    float oracleResult = oracle[count];

    // Now check if the two values are exactly the same
    if ((oracle[count] != rlibm32_res) &&
        (oracleResult == oracleResult || rlibm32_res == rlibm32_res))
      rlibm32_wrongResult++;
    if ((oracle[count] != glibc_float_res) &&
        (oracleResult == oracleResult || glibc_float_res == glibc_float_res))
      glibc_float_wrongResult++;
    if ((oracle[count] != glibc_double_res) &&
        (oracleResult == oracleResult || glibc_double_res == glibc_double_res))
      glibc_double_wrongResult++;
    if ((oracle[count] != rlibm_fast_res) &&
        (oracleResult == oracleResult || rlibm_fast_res == rlibm_fast_res))
      rlibm_fast_wrongResult++;
    
    if (count % 10000000 == 0) {
      //      temp = temp + flush_cache(temp_value);
      fprintf(stderr, "count = %lu (%lu)\n", count, rlibm_fast_wrongResult);
      fflush(stdout);
    }

    //    temp = temp + flush_cache(temp_value);
  }

  // Un-mmap oracle file
  munmap(oracle, file_size);
  close(fd);

  
  printf("Wrong results: \n");
  printf("rlibm32      wrong result: %lu\n", rlibm32_wrongResult);
  printf("glibc float  wrong result: %lu\n", glibc_float_wrongResult);
  printf("glibc double wrong result: %lu\n", glibc_double_wrongResult);
  printf("rlibm-fast   wrong result: %lu\n", rlibm_fast_wrongResult);
  
  printf("Speedup (baseline: rlibm-fast. Above 1.0 means rlibm-fast is better)\n");
  printf("vs. rlibm32      : %.5f\n", 1.0 * rlibm32_totalTime / rlibm_fast_totalTime);
  printf("vs. glibc float  : %.5f\n", 1.0 * glibc_float_totalTime / rlibm_fast_totalTime);
  printf("vs. glibc double : %.5f\n", 1.0 * glibc_double_totalTime / rlibm_fast_totalTime);

  free(temp_value);
}
