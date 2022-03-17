#!/bin/bash

if [ -z ${ORACLEPATH}]; then
  ORACLEPATH=/filer/tmp1/jpl169/float32_oracle
fi

echo -e "Testing Rlibm-fast speedup"
make -s clean
make -s

echo -e "Log"
taskset -c 12 ./Log ${ORACLEPATH}/LogOracle

echo -e "Log2"
taskset -c 12 ./Log2 ${ORACLEPATH}/Log2Oracle

echo -e "Log10"
taskset -c 12 ./Log10 ${ORACLEPATH}/Log10Oracle

echo -e "Exp"
taskset -c 12 ./Exp ${ORACLEPATH}/ExpOracle

echo -e "Exp2"
taskset -c 12 ./Exp2 ${ORACLEPATH}/Exp2Oracle

echo -e "Exp10 version 1 (smaller polynomial)"
taskset -c 12 ./Exp10 ${ORACLEPATH}/Exp10Oracle

echo -e "Sinpi"
taskset -c 12 ./Sinpi ${ORACLEPATH}/SinpiOracle

echo -e "Cospi"
taskset -c 12 ./Cospi ${ORACLEPATH}/CospiOracle

echo -e "Cosh version 1 (sinh smaller poly. cosh smaller poly.)"
taskset -c 12 ./Cosh ${ORACLEPATH}/CoshOracle

echo -e "Sinh"
taskset -c 12 ./Sinh ${ORACLEPATH}/SinhOracle


echo -e "Exp10 version 2 (no violations)"
taskset -c 12 ./Exp10_v2 ${ORACLEPATH}/Exp10Oracle

echo -e "Cosh version 2 (sinh no violation. cosh smaller poly.)"
taskset -c 12 ./Cosh_v2 ${ORACLEPATH}/CoshOracle

echo -e "Cosh version 3 (sinh smaller poly. cosh no violation.)"
taskset -c 12 ./Cosh_v3 ${ORACLEPATH}/CoshOracle

echo -e "Cosh version 4 (sinh no violation. cosh no violation.)"
taskset -c 12 ./Cosh_v4 ${ORACLEPATH}/CoshOracle

make -s clean
