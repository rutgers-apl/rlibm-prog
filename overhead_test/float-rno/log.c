#define __RLIBM_FAST_ELEM__ rlibm_fast_log
#define __RLIBM32_ELEM__ rlibm_log
#define __GLIBC_FLOAT_ELEM__ logf
#define __GLIBC_DOUBLE_ELEM__ log
#define __RLIBM_RNO_ELEM__ rlibm_rno_log
#define __RLIBM_PIE_ELEM__ rlibm_rno_log_piecewise
#define __RLIBM_34RO_ELEM__ rlibm34_log

#include "LibTestHelper.h"

int main(int argc, char** argv) {

    if (argc != 2) {
        printf("Usage  %s <oracle file>\n", argv[0]);
        exit(0);
    }
    
    RunTest(argv[1]);
    return 0;
}
