#include <stdio.h>
#include <stdlib.h>
char generateCard(void) {
	int card = rand() % 13 + 1;
	char character;
	if (card >= 10) {
		return 'T';
	}
	else if (card == 1){
		return 'A';
	}
	else {
		return '0' + card;
	}
} 


void playRound(void) {
	int action;
	char pCard1, pCard2, dCard1, dCard2;
	pCard1 = generateCard();
	pCard2 = generateCard();
	dCard1 = generateCard();
	dCard2 =generateCard();
	printf("You're cards are %c and %c against dealer's %c.\n",pCard1,pCard2, dCard1);
	printf("Would you like to hit(1) or stand(0)?");
	scanf("%d", &action);
	if (action == 1) {
		printf("Ended up as %c%c%c versus %c%c.", pCard1, pCard2, generateCard(), dCard1, dCard2);
		printf(" Next cards in stream are %c%c%c\n", generateCard(),generateCard(),generateCard());
	}  else {
		printf("Ended up as %c%c versus %c%c.", pCard1, pCard2, dCard1,dCard2);
		printf(" Next cards in stream were %c%c\n",generateCard(), generateCard());
	}
}

void playGame(void){
	int numRounds;
	printf("How many hands would you like to play?\n");
	scanf("%d", &numRounds);
	for (int i = 0; i < numRounds; i++){
		playRound();
		printf("You finished round %d\n", i);
	}	
}
int main() {
	playGame();	
	return 0;
}
