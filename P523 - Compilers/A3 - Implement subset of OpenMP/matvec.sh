#! /bin/bash
llvm-as matvec.ll
llc -disable-cfi matvec.bc
gcc matvec.s mat.c -o matvec
./matvec
