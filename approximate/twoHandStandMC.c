#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef char card;

// Function declarations
int value(int num_cards, card hand[]);
void printCards(int numCards, card cards[]);
void populateShoe(int numDecks, card Shoe[]);
void swap(card* a, card* b);
void fisherYates(int numCards, card cards[]);
int playDealerHand(card dealer[22], card shoe[]);
void printResults(int losses[19], int wins[19], int pushes[19]);
void runSim(int numDecks, card Shoe[], int numWins[], int numPushes[], int numLosses[]);


// Essentially, for 4-16, how frequently does the dealer bust?
// for 17,18,19,20,21,22, how frequently do we beat the dealer?
int main() {
    int numDecks = 1;
    card Shoe[1*52];
    int numLosses[19] = {0};
    int numWins[19] = {0};
    int numPushes[19] = {0};

    runSim(numDecks, Shoe, numWins, numPushes, numLosses);
    printResults(numLosses, numWins, numPushes);

	return 0;
} 	
// frozen functions (guaranteed bug free)
int value(int num_cards, card hand[]) {
	int total = 0;
	int num_aces = 0;
	for (int i =0; i < num_cards; i++) {
		switch (hand[i]) {
            case 'A':
                num_aces += 1;
                total += 1;
                break;
            case 'K':
                total += 10;
                break;
            case 'Q':
                total += 10;
                break;
            case 'J':
                total += 10;
                break;
            case 'T':
                total += 10;
                break;
            default:
                total += hand[i] - '0';
        }
	}
	if ((total == 11) && (num_aces == 1) && (num_cards == 2)) {
		return 22;
	} else if (total > 21) {
		return -1;
	} else if (num_aces == 0) {
		return total;
	} else {
		return total > 11 ? total : total + 10;
	}	
}
void printCards(int numCards, card cards[]){
    for (int i = 0; i< numCards; i++){
        printf("%c", cards[i]);
    }
    printf("\n");
}
void populateShoe(int numDecks, card Shoe[]) {
    int num_repeats = 4*numDecks;
    char string_repr[13] = {'A','2','3','4','5','6','7','8','9','T','J','Q','K'};
    for (int rank = 0; rank < 13; rank++) {
        for (int rep = 0; rep < num_repeats; rep++) {
            Shoe[rank*num_repeats + rep] = string_repr[rank];
        }
    }
}
void swap(card* a, card* b) {
    card tmp = *a;
    *a = *b;
    *b = tmp;
}
// Uses the Fisher-Yates shuffle.
void fisherYates(int numCards, card cards[]) {
    card tmp;
    int j;
    for (int i =0; i< numCards - 1; i++) {
        j = i + rand()%(numCards - i);
        swap(&cards[i], &cards[j]);
    }
}
int playDealerHand(card dealer[22], card shoe[]){
    int dealerScore = 0;
    for (int numCards = 1; numCards < 22; numCards++) {
        dealer[numCards-1] = shoe[numCards-1];
        dealerScore = value(numCards, dealer);
        if ((dealerScore < 0) || (dealerScore > 16)) {
            return dealerScore;
        }
    }
}
void printResults(int losses[19], int wins[19], int pushes[19]){
    double mean;
    double stdev;
    int n;
    printf("%4s|%8s|%8s|%8s|%14s|%12s\n",
        "Hand","Losses","Pushes","Wins","Expectation","Uncertainty");
    for (int i = 0; i < 19; i++) {
        n = (wins[i] + losses[i] + pushes[i]);
        if (i == 18){
            mean = (1.5*wins[i] - losses[i])/n;
            stdev = sqrt((1.5 - mean)*(1.5 - mean)*wins[i] +mean*mean*pushes[i] +(1+mean)*(1+mean)*losses[i])/n;

        } else if (i == 17) {
            continue;
        } else {
            mean = (1.0*wins[i] - losses[i])/n;
            stdev = sqrt(((1.0 - mean)*(1.0 - mean)*wins[i] +mean*mean*pushes[i] +(1+mean)*(1+mean)*losses[i]))/n;
        }
        printf("%3d | %6d | %6d | %6d | %12lf | +-%9lf\n", i + 4, losses[i], pushes[i], wins[i], mean, stdev);
    }
}


// developing functions (potentially BUGGY)


/*
Every ordered pair of two cards is equally likely to end up in our hand. We should impose
This constraint on the simulation.
*/
void runSim(int numDecks, card Shoe[], int wins[], int pushes[], int losses[]){
    card player[2], dealer[22];
    int playerScore, dealerScore, outcome;
    card tmp;
    card ranks[13] = {'A','2','3','4','5','6','7','8','9','T','J','Q','K'};
    int repetitions;
    printf("We're using a %d-deck shoe. All plays come straight off the top.\n", numDecks);
    printf("Enter the number of hands you wish to simulate, divided by ~169:");
    scanf("%d", &repetitions);

    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 13;j++){
            player[0] = ranks[i];
            player[1] = ranks[j];
            populateShoe(numDecks, Shoe);
            playerScore = value(2,player);
            // take these cards out of circulation, shuffle the rest.
            swap(&Shoe[4*numDecks*i], &Shoe[52*numDecks - 1]);
            swap(&Shoe[4*numDecks*j + 1], &Shoe[52*numDecks - 2]);

            for (int k = 0; k <repetitions; k++) {

                fisherYates(52*numDecks - 2, Shoe);        
                dealerScore = playDealerHand(dealer, Shoe);
                outcome = -(dealerScore > playerScore) + (playerScore > dealerScore);
                switch (outcome) {
                    case -1: 
                        losses[playerScore - 4] += 1; 
                        break;
                    case 0:
                        pushes[playerScore - 4] += 1;
                        break;
                    case 1:
                        wins[playerScore - 4] += 1;
                        break;
                }
            }
        }
    }
}
