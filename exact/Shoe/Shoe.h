// shoe.h
#ifndef SHOE
#define SHOE

#include "../Rational/Rational.h"
class Shoe {
    public:
        static const unsigned num_ranks = 10;
        static const unsigned num_suits = 4;
        // cards[0] will store the sum
        int cards[num_ranks + 1];

        Shoe();
        Shoe(int numDecks);
        Rational deal_probability(int i); // should this be card ?
        Shoe shoe_after_deal(int i);
        bool operator==(Rational const &other);
        bool operator!=(Rational const &other);
        std::string toString(); // used for hashing, maybe
};

#endif