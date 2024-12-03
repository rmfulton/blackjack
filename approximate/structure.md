## Simulating blackjack using RNGs

- there is an overarching flow to blackjack

# the flow:
- initially, the dealer shuffles 6 decks together into a shoe and places the cutCard at the 26th card from the back
- While the cutcard has not come to the forefront of the shoe,
    - players place initial bets
    - the dealer pops 2 cards from the front for each of the players and 2 for himself.
    - All players get to see the cards dealt from the deck, except for the dealer's downcard, which is hidden
    - the players play their hands one after another, with each player finishing before the next begins.
    - the dealer plays his hand
    - the dealer calculates and provides payouts to the players
    - the dealer clears the players' hands and puts their cards into the discard tray

# the players' strategy
- in order to determine their play, players need the following information
- the point total of their hand
- whether they can split
- whether they can double down
- whether they can surrender
- whether they can take insurance
- whether their hand is soft
- the running count
- the number of cards left in the shoe
- the rank of the dealer's upCard

# the dealer's play
- 