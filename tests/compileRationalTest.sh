#!/bin/bash
g++ -c ../exact/Utils/Utils.cpp
g++ -c ../exact/Rational/Rational.cpp
g++ -c testRational.cpp
g++ testRational.o Rational.o utils.o -o test
./test
rm utils.o testRational.o Rational.o test
