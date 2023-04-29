#include "Rational.h"
#include "utils.h"
#include <iostream>

using namespace std;


Rational::Rational(){
    Rational(1);
}

Rational::Rational(int n){
    Rational(n, 1);
}

Rational::Rational(int x, int y){
    int z = gcd(x,y);
    int flipSign = y < 0;
    this->p = (flipSign ? -1 : 1)*(x/z);
    this->q = (flipSign ? -1 : 1)*(y/z);
}

void print(Rational r) {
    cout << r.p << " / " << r.q << endl;
}