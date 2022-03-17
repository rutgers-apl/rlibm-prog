SOPLEX_INCLUDE=/filer/tmp1/sn349/soplex-4.0.1/src/
SOPLEX_LIB=/filer/tmp1/sn349/soplex-4.0.1/build/lib/libsoplex.a


all: piecewise piecewise-progressive

piecewise: rlibm-prog.h piecewise-small.cpp
	g++ -O2 -I$(SOPLEX_INCLUDE) -std=c++11 piecewise-small.cpp  $(SOPLEX_LIB) -o piecewise-polygen -lgmp -lz -lm

piecewise-progressive: rlibm-prog.h progressive-piecewise-small.cpp
	g++ -O3 -I$(SOPLEX_INCLUDE) -std=c++11 progressive-piecewise-small.cpp  $(SOPLEX_LIB) -o progressive-piecewise-polygen -lgmp -lz -lm


clean:
	rm -f *~ piecewise-polygen piecewise-progressive-polygen
 
