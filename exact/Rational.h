#ifndef rat
#define rat

class Rational {
    public:
        int p;
        int q;
        Rational();
        Rational(int n);
        Rational(int x, int y);
};

void print(Rational r);

#endif