// shoe.h
#ifndef SHOE
#define SHOE

#include "Rational.h"
class Shoe {
    private:
        static const unsigned num_ranks = 10;
        int cards[num_ranks + 1];
        int numCards;
    public:
        static const unsigned num_ranks = 10;
        Rational deal_probability(int i); // should this be card ?
        Shoe shoe_after_deal(int i);
        bool operator==(Rational const &other);
        bool operator!=(Rational const &other);
        std::string toString(); // used for hashing, maybe
};

#endif