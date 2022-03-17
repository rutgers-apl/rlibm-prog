#define __RLIBM_FAST_ELEM__ rlibm_fast_exp2
#define __RLIBM32_ELEM__ rlibm_exp2
#define __GLIBC_FLOAT_ELEM__ exp2f
#define __GLIBC_DOUBLE_ELEM__ exp2
#include "LibTestHelper.h"

int main(int argc, char** argv) {

    if (argc != 2) {
        printf("UsageL %s <oracle file>\n", argv[0]);
        exit(0);
    }
    
    RunTest(argv[1]);
    return 0;
}
