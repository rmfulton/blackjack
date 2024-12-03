#include "Rational.h"
#include "utils.h"
#include <iostream>

using namespace std;

bool constructorTestHelper(int numer, int denom){
    int divisor = denom > 0 ? gcd(numer,denom) : -gcd(numer, denom);
    Rational x = Rational(numer,denom);
    bool result = (x.p == numer/divisor) && (x.q == denom/divisor);
    return result;
}

bool constructorTest(){
    int maxValue = 10;
    for (int denom = -maxValue; denom<maxValue + 1; ++denom){
        if (denom == 0){
            continue;
        }
        for (int numer = -maxValue; numer < maxValue + 1; ++numer){
            bool pass = constructorTestHelper(numer, denom);
            if (!pass){
                std::cout << "ConstructorTest fails on numer " << numer << " and denom " << denom << std::endl;
                return false;
            }
        }
    }
    std::cout << "ConstructorTest passes" << std::endl;
    return true;
}

bool genericEqualityTest(Rational r1, Rational r2, bool expected){
    bool actual = r1 == r2;
    return actual == expected;
}

bool equalityTest1(){
    bool result = genericEqualityTest( Rational(3,2), Rational(-6,-4), true);
    cout << "equalityTest1" << (result ? "Does" : "Does NOT") << "pass" << endl;
    return result;
    
}
bool equalityTest2(){
    bool result = genericEqualityTest( Rational(0,1), Rational(0,-4), true);
    cout << "equalityTest2" << (result ? "Does" : "Does NOT") << "pass" << endl;
    return result;
}
bool equalityTest3(){
    bool result = genericEqualityTest( Rational(4,2), Rational(-6,-4), false);
    cout << "equalityTest3" << (result ? "Does" : "Does NOT") << "pass" << endl;
    return result;
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
    for (int numer1 = -8; numer1 < 8; ++numer1){
        for (int numer2 = -8; numer2 < 8;  ++numer2){
            for(int denom1 = -8; denom1 < 8; ++denom1){
                if(denom1 == 0){
                    continue;
                }
                for(int denom2 = -8; denom2 < 8; ++denom2){
                    if(denom2 == 0){
                        continue;
                    }
                    Rational actual = Rational(numer1,denom1) + Rational(numer2, denom2);
                    Rational expected = Rational(numer1*denom2 + denom1*numer2, denom1*denom2);
                    allTrue = allTrue && actual == expected;
                }
            }
        }
    }
    return allTrue;
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
    allTestsPass = allTestsPass && additionTest2();
    return allTestsPass;
}

int main() {
    bool allTestsPass = true;
    allTestsPass = allTestsPass && constructorTest();
    allTestsPass = allTestsPass && operatorEqualTests();
    allTestsPass = allTestsPass && operatorAddTests();

    cout << (allTestsPass ? "All" : "Not all") << " tests pass" << endl;
    return 0;
}