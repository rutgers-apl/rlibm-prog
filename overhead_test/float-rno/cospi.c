#define __RLIBM_FAST_ELEM__ rlibm_fast_cospi
#define __RLIBM32_ELEM__ rlibm_cospi
#define __GLIBC_FLOAT_ELEM__ cosf
#define __GLIBC_DOUBLE_ELEM__ cos
#define __RLIBM_RNO_ELEM__ rlibm_rno_cospi
#define __RLIBM_PIE_ELEM__ rlibm_rno_cospi
#define __RLIBM_34RO_ELEM__ rlibm34_cospi

#include "LibTestHelper.h"

int main(int argc, char** argv) {

    if (argc != 2) {
        printf("Usage  %s <oracle file>\n", argv[0]);
        exit(0);
    }
    
    RunTest(argv[1]);
    return 0;
}
