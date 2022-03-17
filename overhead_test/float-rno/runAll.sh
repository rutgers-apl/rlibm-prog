#!/bin/bash

if [ -z ${ORACLEPATH}]; then
  ORACLEPATH=/filer/tmp1/jpl169/float32_oracle
fi

echo -e "Testing Rlibm-rno version speedup"
make -s clean
make -s

echo -e "Log"
taskset -c 12 ./log ${ORACLEPATH}/LogOracle

echo -e "Log2"
taskset -c 12 ./log2 ${ORACLEPATH}/Log2Oracle

echo -e "Log10"
taskset -c 12 ./log10 ${ORACLEPATH}/Log10Oracle

echo -e "Exp"
taskset -c 12 ./exp ${ORACLEPATH}/ExpOracle

echo -e "Exp2"
taskset -c 12 ./exp2 ${ORACLEPATH}/Exp2Oracle

echo -e "Exp10 version 1 (smaller polynomial)"
taskset -c 12 ./exp10 ${ORACLEPATH}/Exp10Oracle

echo -e "Sinpi"
taskset -c 12 ./sinpi ${ORACLEPATH}/SinpiOracle

echo -e "Cospi"
taskset -c 12 ./cospi ${ORACLEPATH}/CospiOracle

echo -e "Cosh version 1 (sinh smaller poly. cosh smaller poly.)"
taskset -c 12 ./cosh ${ORACLEPATH}/CoshOracle

echo -e "Sinh"
taskset -c 12 ./sinh ${ORACLEPATH}/SinhOracle

make -s clean
