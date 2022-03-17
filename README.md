# RLIBM-PROG

RLIBM-PROG is a math library generator and collection of correctly
rounded elementary functions. Given an elementary function, a single
polynomial approximation in RLIBM-ALL produces correctly rounded
results for all inputs with multiple representations and rounding
modes and has progressive performance with bfloat16 and tensorfloat32

If you are interested in the technical details of RLIBM-PROG, the
following resources will be helful.

1. [RLIBM-PROG: Progressive Polynomial Approximations for Fast Correctly Rounded Elementary Functions](https://arxiv.org/pdf/2111.12852.pdf)
2. [The POPL 2022 paper on RLIBM-ALL](https://people.cs.rutgers.edu/~sn349/papers/rlibmall-popl-2022.pdf)
3. [The extended Rutgers DCS Technical Report DCS-TR-757 on RLIBM-ALL](https://arxiv.org/pdf/2108.06756.pdf)
4. [The RLIBM project page](https://people.cs.rutgers.edu/~sn349/rlibm/)

Currently, RLIBM-PROG supports a number of elementary functions for the
floating point representation.

## List of floating point functions supported by RLIBM-ALL
1. log(x), log2(x), log10(x)
2. exp(x), exp2(x), exp10(x)
3. sinh(x), cosh(x)
4. sinpi(x), cospi(x)


## Pre-requisites

1. Reduced Intervals from the RLIBM-ALL project to generate polynomial approximations.

## Building RLIBM-PROG libraries

RLIBM-PROG has multiple correctly rounded elementary functions in the libm folder

1. float subdirectory has correctly rounded elementary functions for
32-bit floats with round-to-nearest-ties-to-even mode.

2. float-rno subdirectory has polynomial approximations that produce correctly
rounded results for all inputs, rounding modes, and multiple
representations from a 10-bit FP to a 32-bit float.

3. progressive subdirectory has progressive polynomial approximations
that produce float-rno results but with better performance for
bfloat16 and tensorfloat32.


### Building the libraries.

Go to libm folder and type make.


## Building the polynomial generator

The polynomial generator can be found in polynomial_gen directory. It
needs the SOPLEX solver installed and appropriately set the following variables in
polynomial_gen/Makefile

1. SOPLEX_INCLUDE

2. SOPLEX_LIB

To build the polynomial generator, type make in the polynomial_gen directory


* Running the polynomial generator

The reduced intervals take up substantial storage space. Hence, it
needs to generated using the RLIBM-ALL prototype.  The Log2 reduced
interval file is available here:
https://people.cs.rutgers.edu/~sn349/Float34ROLog2Intervals.gz


```
wget https://people.cs.rutgers.edu/~sn349/Float34ROLog2Intervals.gz
gunzip Float34ROLog2Intervals.gz
cd polynomial_gen
./piecewise-polygen ../Float34ROLog2Intervals
```

It will print out the resultant polynomial (if the system of linear
inequalities is full rank)