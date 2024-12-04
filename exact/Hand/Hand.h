// player.h
#ifndef HAND
#define HAND

class Hand {
    public:
        int minValue;
        bool isSoft;
        bool canDouble;
        bool canHit;
        unsigned numTimesSplit;
        bool canSurrender;

        Hand();
        Hand(int card1, int card2);
        bool operator==(Hand const &other);
        bool operator!=(Hand const &other);
        // std::string toString(); // used for hashing, maybe
};

#endif