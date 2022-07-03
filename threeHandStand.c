#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

typedef char card;

// Function declarations
int value(int num_cards, card hand[]);
void printCards(int numCards, card cards[]);
void populateShoe(int numDecks, card Shoe[]);
void swap(card* a, card* b);
void fisherYates(int numCards, card cards[]);
int playDealerHand(card dealer[22], card shoe[]);
void printResults(int losses[19][10], int wins[19][10], int pushes[19][10]);
void shuffleAndPlay(int numDecks, card Shoe[], int playerScore, int losses[19][10], int pushes[19][10], int wins[19][10], int k);
void runSim(int numDecks, card Shoe[], int numWins[19][10], int numPushes[19][10], int numLosses[19][10]);
void printSpecific(int losses[19][10], int wins[19][10], int pushes[19][10], int col);


// Essentially, for 4-16, how frequently does the dealer bust?
// for 17,18,19,20,21,22, how frequently do we beat the dealer?
int main() {
    int numDecks = 1;
    card Shoe[1*52];
    int numLosses[19][10] = {0};
    int numWins[19][10] = {0};
    int numPushes[19][10] = {0};
    char rank[10] = {'2','3','4','5','6','7','8','9','T','A'};

    runSim(numDecks, Shoe, numWins, numPushes, numLosses);

    // get expectation and stdev for hand total x upCard
    printResults(numLosses, numWins, numPushes);


    // Alternatively, get more data, for a single column
    for (int col =0; col < 10; col++) {
        printf("\nFull Data for upCard %c\n",rank[col]);
        printSpecific(numLosses, numWins, numPushes, col);
    }
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
    char string_repr[13] = {'2','3','4','5','6','7','8','9','T','J','Q','K','A'};
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

// developing functions (potentially BUGGY)

void printResults(int losses[19][10], int wins[19][10], int pushes[19][10]){
    double mean;
    double stdev;
    int n;
    printf("%4s|%12s|%12s|%12s|%12s|%12s|%12s|%12s|%12s|%12s|%12s|\n",
        "Hand","2","3","4","5","6", "7", "8", "9", "T","A");
    for (int i = 0; i < 19; i++) {
        if (i == 17) {
            continue;
        }
        printf("%3d | ", i+4);
        for (int j = 0; j < 10; j++) {
            n = (wins[i][j] + losses[i][j] + pushes[i][j]);
            if (i == 18) {
                mean = (1.5*wins[i][j] - losses[i][j])/n;
                stdev = sqrt((1.5 - mean)*(1.5 - mean)*wins[i][j] +mean*mean*pushes[i][j] +(1+mean)*(1+mean)*losses[i][j])/n;
            } else {
                mean = (1.0*wins[i][j] - losses[i][j])/n;
                stdev = sqrt(((1.0 - mean)*(1.0 - mean)*wins[i][j] +mean*mean*pushes[i][j] +(1+mean)*(1+mean)*losses[i][j]))/n;
            }
            printf("%+4.2f+-%2.2lf| ",  mean, stdev);
        }
        printf("\n");
    }
}
void shuffleAndPlay(int numDecks, card Shoe[], int playerScore, int losses[19][10], int pushes[19][10], int wins[19][10], int k){
    card dealer[22];
    int dealerScore, outcome;
    //always keep dealer upCard at the front!
    swap(&Shoe[0], &Shoe[52*numDecks-3]);
    fisherYates(52*numDecks-3, Shoe);
    swap(&Shoe[0], &Shoe[52*numDecks-3]);

    dealerScore = playDealerHand(dealer, Shoe);
    outcome = (playerScore > dealerScore) - (dealerScore > playerScore);
    switch (outcome) {
        case -1:
            losses[playerScore - 4][(k<8)?k:(k<12)?8:9] += 1;
            break;
        case 0:
            pushes[playerScore - 4][(k<8)?k:(k<12)?8:9] += 1;
            break;
        case 1:
            wins[playerScore-4][(k<8)?k:(k<12)?8:9] += 1;
            break;
    }
}
/*
Every ordered pair of two cards is equally likely to end up in our hand. We should impose
This constraint on the simulation.
*/
void runSim(int numDecks, card Shoe[], int wins[19][10], int pushes[19][10], int losses[19][10]){
    card player[2];
    int playerScore;
    card ranks[13] = {'2','3','4','5','6','7','8','9','T','J','Q','K','A'};
    int repetitions;
    printf("We're using a %d-deck shoe. All plays come straight off the top.\n", numDecks);
    printf("Enter the number of hands you wish to simulate, divided by ~2197:");
    scanf("%d", &repetitions);

    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 13;j++){
            for (int k = 0; k < 13; k++) {
                player[0] = ranks[i];
                player[1] = ranks[j];
                
                populateShoe(numDecks,Shoe);

                playerScore = value(2,player);
                // take the three special cards out of circulation.
                swap(&Shoe[4*numDecks*(i+1)-1], &Shoe[52*numDecks - 1]);
                swap(&Shoe[4*numDecks*(j+1)-2], &Shoe[52*numDecks - 2]);
                swap(&Shoe[4*numDecks*(k+1) -3], &Shoe[0]);
                for (int l =0; l < repetitions; l++) {
                    shuffleAndPlay(numDecks,Shoe,playerScore,losses,pushes,wins,k);
                }
            }
        }
    }
}

void printSpecific(int losses[19][10], int wins[19][10], int pushes[19][10], int col){
    double mean;
    double stdev;
    int n;
    printf("%4s|%8s|%8s|%8s|%14s|%12s\n",
        "Hand","Losses","Pushes","Wins","Expectation","Uncertainty");
    for (int i = 0; i < 19; i++) {
        n = (wins[i][col] + losses[i][col] + pushes[i][col]);
        if (i == 18){
            mean = (1.5*wins[i][col] - losses[i][col])/n;
            stdev = sqrt((1.5 - mean)*(1.5 - mean)*wins[i][col] +mean*mean*pushes[i][col] +(1+mean)*(1+mean)*losses[i][col])/n;

        } else if (i == 17) {
            continue;
        } else {
            mean = (1.0*wins[i][col] - losses[i][col])/n;
            stdev = sqrt(((1.0 - mean)*(1.0 - mean)*wins[i][col] +mean*mean*pushes[i][col] +(1+mean)*(1+mean)*losses[i][col]))/n;
        }
        printf("%3d | %6d | %6d | %6d | %12lf | +-%9lf\n", i + 4, losses[i][col], pushes[i][col], wins[i][col], mean, stdev);
    }
}