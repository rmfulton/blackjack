#include "utils.h"
#include <iostream>

using namespace std;


bool test1(){
    int x = 132;
    int y = 1;
    int expected = 1;
    if (gcd(x,y) == expected) {
        cout << "test1 Pass: gcd("<<x<<", "<<y<<") = " << gcd(x,y) << ", expected " << expected << endl;
        return true;
    } else {
        cout << "test1 ERROR: gcd("<<x<<", "<<y<<") = " << gcd(x,y) << ", expected " << expected << endl;
        return false;
    }
}

bool test2() {
    int x = 0;
    int y = 40;
    int expected = 40;
    if (gcd(x,y) == expected) {
        cout << "test2 Pass: gcd("<<x<<", "<<y<<") = " << gcd(x,y) << ", expected " << expected << endl;
        return true;
    } else {
        cout << "test2 ERROR: gcd("<<x<<", "<<y<<") = " << gcd(x,y) << ", expected " << expected << endl;
        return false;
    }
}


bool test3() {
    int x = 12;
    int y = 18;
    int expected = 6;
    if (gcd(x,y) == expected) {
        cout << "test3 Pass: gcd("<<x<<", "<<y<<") = " << gcd(x,y) << ", expected " << expected << endl;
        return true;
    } else {
        cout << "test3 ERROR: gcd("<<x<<", "<<y<<") = " << gcd(x,y) << ", expected " << expected << endl;
        return false;
    }
}

bool test4() {
    int x = -12;
    int y = 18;
    int expected = 6;
    if (gcd(x,y) == expected) {
        cout << "test4 Pass: gcd("<<x<<", "<<y<<") = " << gcd(x,y) << ", expected " << expected << endl;
        return true;
    } else {
        cout << "test4 ERROR: gcd("<<x<<", "<<y<<") = " << gcd(x,y) << ", expected " << expected << endl;
        return false;
    }
}


int main() {
    bool allTestsPass = true;
    allTestsPass = allTestsPass && test1();
    allTestsPass = allTestsPass && test2();
    allTestsPass = allTestsPass && test3();
    allTestsPass = allTestsPass && test4();
    cout << (allTestsPass ? "All" : "Not all") << " tests pass" << endl;
    return 0;
}