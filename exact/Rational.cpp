#include "Rational.h"
#include "utils.h"
#include <iostream>

Rational::Rational(){
    Rational tmp = Rational(1);
    this->p = tmp.p;
    this->q = tmp.q;
}

Rational::Rational(int n){
    Rational tmp = Rational(n, 1);
    this->p = tmp.p;
    this->q = tmp.q;
}

Rational::Rational(int x, int y){
    int z = gcd(x,y);
    int flipSign = y < 0;
    this->p = (flipSign ? -1 : 1)*(x/z);
    this->q = (flipSign ? -1 : 1)*(y/z);
}

bool Rational::operator==(Rational const &other) {
    return this->p == other.p && this->q == other.q;
}

bool Rational::operator!=(Rational const &other) {
    return this->p != other.p || this->q != other.q;
}

Rational Rational::operator+( Rational const &other){
    int numerator = this->p*other.q + other.p*this->q;
    int denominator = this->q*other.q;
    return Rational(numerator, denominator);
}

Rational Rational::operator*( Rational const &other){
    int numerator = this->p*other.p;
    int denominator = this->q*other.q;
    return Rational(numerator, denominator);
}

Rational Rational::operator-( Rational const &other){
    int numerator = this->p*other.q - other.p*this->q;
    int denominator = this->q*other.q;
    return Rational(numerator, denominator);
}

Rational Rational::operator/( Rational const &other){
    int numerator = this->p*other.q;
    int denominator = this->q*other.p;
    return Rational(numerator, denominator);
}

float Rational::toFloat(){
    return float(this->p)/float(this->q);
}

std::ostream& operator<<(std::ostream& os, const Rational& r)
{
    os << r.p << " / " << r.q;
    return os;
}