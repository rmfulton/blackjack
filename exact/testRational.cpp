#include "Rational.h"
#include "utils.h"
#include <iostream>

using namespace std;

bool genericConstructorTest(int numer, int denom){
    int divisor = denom > 0 ? gcd(numer,denom) : -gcd(numer, denom);
    Rational x = Rational(numer,denom);
    bool result = (x.p == numer/divisor) && (x.q == denom/divisor);
    return result;
}
bool constructorTest1(){
    return genericConstructorTest(3,2);
}
bool constructorTest2(){
    return genericConstructorTest(6,4);
}
bool constructorTest3(){
    return genericConstructorTest(6,-4);
}

bool genericEqualityTest(Rational r1, Rational r2, bool expected){
    bool actual = r1 == r2;
    return actual == expected;
}

bool equalityTest1(){
    return genericEqualityTest( Rational(3,2), Rational(-6,-4), true);
}
bool equalityTest2(){
    return genericEqualityTest( Rational(0,1), Rational(0,-4), true);
}
bool equalityTest3(){
    return genericEqualityTest( Rational(4,2), Rational(-6,-4), false);
}


bool additionTest1() {
    bool allTrue = true;
    for (int i = 0; i < 100; ++i){
        for (int j = 1000; j < 1200; ++j){
            allTrue = allTrue && Rational(i) + Rational(j) == Rational(i+j);
            allTrue = allTrue && Rational(i) + Rational(j) != Rational(i+j+1);
        }
    }
    return allTrue;
}
bool additionTest2() {
    bool allTrue = true;
    for (int i = )
}
bool constructorTests() {

    bool allTestsPass = true;
    allTestsPass = allTestsPass && constructorTest1();
    allTestsPass = allTestsPass && constructorTest2();
    allTestsPass = allTestsPass && constructorTest3();
    return allTestsPass;
}
bool operatorEqualTests() {    

    bool allTestsPass = true;
    allTestsPass = allTestsPass && equalityTest1();
    allTestsPass = allTestsPass && equalityTest2();
    allTestsPass = allTestsPass && equalityTest3();
    return allTestsPass;
}
bool operatorAddTests() {
    bool allTestsPass = true;
    allTestsPass = allTestsPass && additionTest1();
    return allTestsPass;
}

int main() {
    bool allTestsPass = true;
    allTestsPass = allTestsPass && constructorTests();
    allTestsPass = allTestsPass && operatorEqualTests();
    allTestsPass = allTestsPass && operatorAddTests();

    cout << (allTestsPass ? "All" : "Not all") << " tests pass" << endl;
    return 0;
}