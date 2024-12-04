#!/bin/bash
g++ -c testUtils.cpp
g++ -c ../exact/Utils/Utils.cpp
g++ testUtils.o utils.o -o test
./test
rm utils.o testUtils.o test
