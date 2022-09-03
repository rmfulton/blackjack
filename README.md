# blackjack
Blackjack is a popular casino card game. Rules and terminology can be found [here](https://bicyclecards.com/how-to-play/blackjack/). This repo contains a series of analyses of the game blackjack. In reverse chronological order, there is:

#### monteCarloBS.c
runs a Monte Carlo simumlation heads-up play between a basic strategy player (as defined on [BJA](https://www.blackjackapprenticeship.com/blackjack-strategy-charts/)) and a dealer. Variables in main() function specify the # of decks, the shoe penetration, and rule variations. Outputs the simulated expectation per dollar bet at the overall game, as well as at every given true count from -15 to +15. (this can be changed in the main function)
Compile and run at command line with

`gcc monteCarloBS.c -lm -O2 -Wall; ./a.out`
or equivalent with your OS.

#### threeHandStandMC.c
computes statistics on the average payoff associated with standing on a hand of 2 cards total to a value of V (4 <= V <= 22), against a dealer upCard of U (U=2,3,4,...,9,T,A). cards are dealt at the beginning of an n-deck shoe in a Monte Carlo simulation.
Compile and run at command line with

`gcc threeHandStandMC.c -lm -Wall; ./a.out`
or equivalent for your OS.

#### twoHandStandMC.c
computes statistics on the average payoff associated with standing on a hand of 2 cards totaling to a value of 4,5,6,..., 19,20, and 22. -1 is excluded because busted hands always lose, and 21 is excluded because the only way to get there is with a natural, which is actually valued at 22. Cards are dealt at the beginning of an n-deck shoe in a Monte Carlo simulation.
Compile and run at command line with
`gcc twoHandStandMC.c -lm -Wall; ./a.out`
or equivalent for your OS.

#### handValue.c
computes the value of a multiset of cards. busted totals are assigned the value -1, Naturals are assigned the value of 22, and soft totals are given their largest value that doesn't exceed 21.
Compile and run at command line with

`gcc handValue.c; ./a.out`
or equivalent for your OS.

#### crappyGame.c
is an exceedingly bad version of blackjack. You're only allowed to hit or stand, and nothing else. 
Compile and run at command line with

`gcc crappyGame.c; ./a.out`
or equivalent for your OS.

# TODO:

#### monteCarloH17.c
#### monteCarloS17.c
#### probabilities.c
