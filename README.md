# blackjack
Blackjack is a popular casino card game. Rules and terminology can be found [here](https://bicyclecards.com/how-to-play/blackjack/). This repo contains a series of more and more sophisticated simulations of the game blackjack. In chronological order of creation, there is:

#### crappyGame.c
is an exceedingly bad version of blackjack. You're only allowed to hit or stand, and nothing else. 
Compile and run at command line with

`gcc crappyGame.c; ./a.out`
or equivalent for your OS.

#### handValue.c
computes the value of a multiset of cards. busted totals are assigned the value -1, Naturals are assigned the value of 22, and soft totals are given their largest value that doesn't exceed 21.
Compile and run at command line with

`gcc handValue.c; ./a.out`
or equivalent for your OS.

#### twoHandStand.c
computes statistics on the average payoff associated with standing on a hand of 2 cards totaling to a value of 4,5,6,..., 19,20, and 22. -1 is excluded because busted hands always lose, and 21 is excluded because the only way to get there is with a natural, which is actually valued at 22. Cards are dealt at the beginning of an n-deck shoe.
Compile and run at command line with
`gcc twoHandStand.c -lm; ./a.out`
or equivalent for your OS.

#### threeHandStand.c
computes statistics on the average payoff associated with standing on a hand of 2 cards total to a value of V (4 <= V <= 22), against a dealer upCard of U (U=2,3,4,...,9,T,A). cards are dealt at the beginning of an n-deck shoe.
Compile and run at command line with

`gcc threeHandStand.c -lm; ./a.out`
or equivalent for your OS.

#### BS_flatbet.c
simulates heads-up play between a basic strategy player (as defined on [BJA](https://www.blackjackapprenticeship.com/blackjack-strategy-charts/)) and a dealer. Variables in main() function specify the # of decks, the shoe penetration, and rule variations. Outputs the simulated expectation per dollar bet at the overall game, as well as at every given true count from -15 to +15. (this can be changed in the main function)
Compile and run at command line with

`gcc BS_flatbet.c -lm; ./a.out`
or equivalent with your OS.

#### H17_flatbet.c
Simulates heads up play between a player using H17 deviations (as defined on [BJA](https://www.blackjackapprenticeship.com/wp-content/uploads/2019/07/BJA_H17.pdf?utm_source=getresponse&utm_medium=email&utm_campaign=bja_deviations_charts&utm_content=Here+are+your+Blackjack+Deviations+Charts)) and a dealer. Variables in main() function specify the # of decks, the shoe penetration, and rule variations. Outputs the simulated expectation per dollar bet at the overall game, as well as at every given true count from -15 to +15. (this can be changed in the main function)
Compile and run at command line with

`gcc BS_flatbet.c -lm; ./a.out`
or equivalent with your OS.

#### S17_flatbet.c
Simulates heads up play between a player using H17 deviations (as defined on [BJA]( https://www.blackjackapprenticeship.com/wp-content/uploads/2019/07/BJA_S17.pdf?utm_source=getresponse&utm_medium=email&utm_campaign=bja_deviations_charts&utm_content=Here+are+your+Blackjack+Deviations+Charts)) and a dealer. Variables in main() function specify the # of decks, the shoe penetration, and rule variations. Outputs the simulated expectation per dollar bet at the overall game, as well as at every given true count from -15 to +15. (this can be changed in the main function)
Compile and run at command line with

`gcc BS_flatbet.c -lm; ./a.out`
or equivalent with your OS.
