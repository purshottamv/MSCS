#! /bin/bash
llvm-as llvmexample.ll
llc -disable-cfi llvmexample.bc
gcc llvmexample.s example.c -o example
./example
