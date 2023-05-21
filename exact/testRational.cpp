#include "Rational.h"
#include "utils.h"
#include <iostream>

using namespace std;

bool genericConstructorTest(int numer, int denom){
    int divisor = denom > 0 ? gcd(numer,denom) : -gcd(numer, denom);
    Rational x = Rational(numer,denom);
    bool result = (x.p == numer/divisor) && (x.q == denom/divisor);
    cout << "Rational(" << numer << ", "<< denom << ") = " << x;
    cout << " : " << (result ? "TRUE": "FALSE") << endl;
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

bool equalityTest1(){
    Rational x = Rational(3,2);
    Rational y = Rational(-6,-4);
    bool expected = true;
    bool result;
    if ((x == y) == expected){
        cout << "==Test2 Passed: ";
        result = true;
    } else {
        cout << "==Test2 FAIL: ";
        result = false;
    }
    cout << "Rational(3,2) == Rational(-6,-4) is " << (x == y) << ", expected " << expected << endl;
    return result;
}
bool equalityTest2(){
    Rational x = Rational(0,1);
    Rational y = Rational(0,-4);
    bool expected = true;
    bool result;
    if ((x == y) == expected){
        cout << "==Test1 Passed: ";
        result = true;
    } else {
        cout << "==Test1 FAIL: ";
        result = false;
    }
    cout << "Rational(0,1) == Rational(0,-4) is " << (x == y) << ", expected " << expected << endl;
    return result;
}
bool equalityTest3(){
    Rational x = Rational(4,2);
    Rational y = Rational(-6,-4);
    bool expected = false;
    bool result;
    if ((x == y) == expected){
        cout << "==Test3 Passed: ";
        result = true;
    } else {
        cout << "==Test3 FAIL: ";
        result = false;
    }
    cout << "Rational(4,2) == Rational(-6,-4) is " << (x == y) << ", expected " << expected << endl;
    return result;
}

bool additionTest1() {
    Rational x = Rational();
    Rational expected = Rational(2);
    cout << "Rational() = " << x << endl;
    cout << "Rational(1) = " << Rational(1) << endl;
    cout << "Rational(1,1) = " << Rational(1,1) << endl;
    cout << "x + x = " << (x+x) << endl;
    cout << "Rational(2) = " << expected << endl;
    return x + x == expected;
}
bool constructorTests() {

    bool allTestsPass = true;
    cout << "\nconstructor tests:" << endl;
    allTestsPass = allTestsPass && constructorTest1();
    allTestsPass = allTestsPass && constructorTest2();
    allTestsPass = allTestsPass && constructorTest3();
    return allTestsPass;
}

bool operatorEqualTests() {    

    bool allTestsPass = true;
    cout << "\nEquality tests:" << endl;
    allTestsPass = allTestsPass && equalityTest1();
    allTestsPass = allTestsPass && equalityTest2();
    allTestsPass = allTestsPass && equalityTest3();
    return allTestsPass;
}
bool operatorAddTests() {
    bool allTestsPass = true;
    cout << "\nAddition tests:" << endl;
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