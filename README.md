# blackjack
Blackjack is a popular casino card game. Rules and terminology can be found [here](https://bicyclecards.com/how-to-play/blackjack/). This repo contains a series of more and more sophisticated simulations of the game blackjack. In chronological order of creation, there is:

#### crappyGame.c
is an exceedingly bad version of blackjack. You're only allowed to hit or stand, and nothing else.

#### handValue.c
computes the value of a multiset of cards. busted totals are assigned the value -1, Naturals are assigned the value of 22, and soft totals are given their largest value that doesn't exceed 21.

#### twoHandStand.c
computes statistics on the average payoff associated with standing on a hand of 2 cards totaling to a value of 4,5,6,..., 19,20, and 22. -1 is excluded because busted hands always lose, and 21 is excluded because the only way to get there is with a natural, which is actually valued at 22. Cards are dealt at the beginning of an n-deck shoe.

#### threeHandStand.c
computes statistics on the average payoff associated with standing on a hand of 2 cards total to a value of V (4 <= V <= 22), against a dealer upCard of U (U=2,3,4,...,9,T,A). cards are dealt at the beginning of an n-deck shoe.
