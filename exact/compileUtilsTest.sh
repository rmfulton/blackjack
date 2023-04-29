#!/bin/bash
g++ -c testUtils.cpp
g++ -c utils.cpp
g++ testUtils.o utils.o -o test
./test
rm utils.o testUtils.o test
