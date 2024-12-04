#!/bin/bash
g++ -c ../exact/Shoe/Shoe.cpp
g++ -c testShoe.cpp
g++ testShoe.o Shoe.o -o test
./test
rm testShoe.o Shoe.o test
