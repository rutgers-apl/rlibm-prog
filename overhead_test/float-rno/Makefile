CC = gcc

funcs = log2 log log10 exp exp2 exp10 sinh cosh sinpi cospi

src = $(patsubst %, %.c, $(funcs))
obj = $(patsubst %, %, $(funcs))


all: $(obj)

%: %.c
	$(CC) -O3 -std=c11 -I../../include/float-rno/ -I../../include/ -I../../rlibm-32/include/  -I../../rlibm-rno/include/ $^ ../../libm/floatlibm.a ../../libm/floatrnolibm.a ../../rlibm-32/lib/floatMathLib.a ../../rlibm-rno/lib/float34ROMathLib.a -lgmp -lm -o $@

clean:
	rm -rf *.o $(funcs) *~ *.txt
