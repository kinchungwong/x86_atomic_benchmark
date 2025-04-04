#!/bin/bash -e

mkdir -p ./build
g++ -std=c++11 -g -O3 -DNDEBUG -o ./build/main.out ./main/main.cpp -I. -I./atomic_impl/include -I./atomic_impl/src -fPIC -flto
objdump -d -C -M intel ./build/main.out > ./build/main.s
