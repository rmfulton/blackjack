#include "Rational.h"
#include "utils.h"

class Rational {
    public:
        MyClass(){
            this.MyClass(1);
        }
        MyClass(int n){
            this.MyClass(n, 1);
        }
        MyClass(int x, int y){
            this->p = x;
            this->q = y;
            this->simplify();
        }
        simplify(){

        }

    private:
        int p;
        int q;
}