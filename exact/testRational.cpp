#include "Rational.h"
#include <iostream>

using namespace std;

bool test1(){
    Rational x = Rational(3,2);
    cout << "Rational(3,2) = ";
    print(x);
    return true;
}

bool test2(){
    Rational x = Rational(6,4);
    cout << "Rational(6,4) = ";
    print(x);
    return true;
}

bool test3(){
    Rational x = Rational(6,-4);
    cout << "Rational(6,-4) = ";
    print(x);
    return true;
}


int main() {
    bool allTestsPass = true;
    allTestsPass = allTestsPass && test1();
    allTestsPass = allTestsPass && test2();
    allTestsPass = allTestsPass && test3();

    cout << (allTestsPass ? "All" : "Not all") << " tests pass" << endl;
    return 0;
}