CC = gcc

funcs = Log Log2 Log10 Exp Exp2 Cosh Cosh_v2 Cosh_v3 Cosh_v4 Sinh Cospi Sinpi Exp10 Exp10_v2

src = $(patsubst %, %.c, $(funcs))
obj = $(patsubst %, %, $(funcs))


all: $(obj) combined

combined: CombinedFunctionTester.c
	gcc -O3 -std=c11  -I../../include/ -I../../rlibm-32/include/  CombinedFunctionTester.c -o combined-test  ../../libm/floatlibm.a ../../rlibm-32/lib/floatMathLib.a -lgmp -lm 

%: %.c
	$(CC) -O3 -std=c11 -I../../include/ -I../../rlibm-32/include/  $^ ../../libm/floatlibm.a ../../rlibm-32/lib/floatMathLib.a -lgmp -lm -o $@

clean:
	rm -rf *.o $(funcs) *~ *.txt combined-test
