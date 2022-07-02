#include <stdio.h>

typedef char card;

int value(card hand[], int num_cards) {
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

int setPlayerHand(card playerHand[22]){
    int numCards;
	char newline;
    printf("Enter the number of cards in your hand: ");
	scanf("%d", &numCards);
	printf("Enter the cards: \n");
	scanf("%c", &newline);
	for (int i = 0; i < numCards; i++) {
		scanf("%c", &playerHand[i]);
	}
    return value(playerHand, numCards);
}

int main() {
	card playerHand[22];
    int playerScore = setPlayerHand(playerHand);
	printf("The value of your hand is %d!\n", playerScore);
	return 0;
} 	
