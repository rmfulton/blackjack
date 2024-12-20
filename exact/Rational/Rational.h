#ifndef RATIONAL
#define RATIONAL

#include <iostream>

class Rational {
    public:
        int p;
        int q;
        Rational();
        Rational(int n);
        Rational(int x, int y);
        bool operator==(Rational const &other);
        bool operator!=(Rational const &other);
        Rational operator+(Rational const &other);
        Rational operator-(Rational const &other);
        Rational operator*(Rational const &other);
        Rational operator/(Rational const &other);
        float toFloat();
};

std::ostream& operator<<(std::ostream& os, const Rational& r);

#endif