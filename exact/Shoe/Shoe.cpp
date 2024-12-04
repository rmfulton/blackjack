#include "Shoe.h"
#include <iostream>


Shoe::Shoe(int numDecks){
    int totalNumCards = 0;
    // there are num_suits cards of each rank in each deck
    for (int i = 1; i < num_ranks; ++i){
        cards[i] = num_suits*numDecks;
        totalNumCards += cards[i];
    }
    // except for rank 10 (T,J,Q,K)
    cards[num_ranks] = 4*num_suits*numDecks;
    totalNumCards += cards[num_ranks];
    cards[0] = totalNumCards;
    
}