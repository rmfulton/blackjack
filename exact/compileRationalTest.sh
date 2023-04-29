#!/bin/bash
g++ -c utils.cpp
g++ -c Rational.cpp
g++ -c testRational.cpp
g++ testRational.o Rational.o utils.o -o test
./test
rm utils.o testRational.o Rational.o test
