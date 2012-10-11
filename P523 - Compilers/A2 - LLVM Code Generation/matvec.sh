#! /bin/bash
llvm-as matvec.ll
llc -disable-cfi matvec.bc
gcc matvec.s matvec.c -o matvec
./matvec