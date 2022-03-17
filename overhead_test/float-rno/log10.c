#define __RLIBM_FAST_ELEM__ rlibm_fast_log10
#define __RLIBM32_ELEM__ rlibm_log10
#define __GLIBC_FLOAT_ELEM__ log10f
#define __GLIBC_DOUBLE_ELEM__ log10
#define __RLIBM_RNO_ELEM__ rlibm_rno_log10
#define __RLIBM_PIE_ELEM__ rlibm_rno_log10_piecewise
#define __RLIBM_34RO_ELEM__ rlibm34_log10

#include "LibTestHelper.h"

int main(int argc, char** argv) {

    if (argc != 2) {
        printf("UsageL %s <oracle file>\n", argv[0]);
        exit(0);
    }
    
    RunTest(argv[1]);
    return 0;
}
