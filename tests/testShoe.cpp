#include "../exact/Shoe/Shoe.h"
#include <iostream>

using namespace std;

bool constructorTest() {
    for (int numDecks = 1; numDecks < 9; ++numDecks){
        Shoe s = Shoe(numDecks);
        if (s.cards[0] != 52*numDecks){
            std::cout << "constructor FAILED on total # of cards " << s.cards[0] << " numDecks: " << numDecks << std::endl;
            return false;
        }
        for (int rank = 1; rank < Shoe::num_ranks; ++rank){
            if (s.cards[rank] != Shoe::num_suits * numDecks ) {
                std::cout << "constructor FAILED on rank " << rank << std::endl;
                return false;
            }
        }
        if (s.cards[Shoe::num_ranks] != 4*Shoe::num_suits*numDecks){
            std::cout << "constructor FAILED on rank " << Shoe::num_ranks << std::endl;
            return false;
        }
    }
    std::cout << "constructor test passed." << std::endl;
    return true;
}

int main() {
    bool allTestsPass = true;
    allTestsPass = allTestsPass && constructorTest();

    cout << (allTestsPass ? "All" : "Not all") << " tests pass" << endl;
    return allTestsPass ? 0 : 1;
}