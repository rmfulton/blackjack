#include "utils.h"

/*
Assume that a and b are both non-negative
*/
int gcd(int a, int b){
    while (b > 0){
        int tmp = a;
        if (a < b){
            a = b;
            b = tmp;
        } else {
            a = b;
            b = (tmp % b);
        }
    }
    return a;
}