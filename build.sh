#!/bin/bash -e

INCLUDE_DIR="-I."
INCLUDE_DIR="-I./main"
INCLUDE_DIR="${INCLUDE_DIR} -I./atomic_impl/include"
INCLUDE_DIR="${INCLUDE_DIR} -I./atomic_impl/src"
INCLUDE_DIR="${INCLUDE_DIR} -I./nano_timer/include"
INCLUDE_DIR="${INCLUDE_DIR} -I./nano_timer/src"

mkdir -p ./build
g++ -std=c++11 -g -O3 -DNDEBUG -o ./build/main.out ./main/main.cpp ${INCLUDE_DIR} -fPIC -flto
objdump -d -C -M intel ./build/main.out > ./build/main.s
