#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

typedef char card;

struct cardList {
    card data;
    struct cardList* next;
};
typedef struct cardList clist;

struct hand {
    clist *cards;

    int canSplit;
    int canDouble;
    int canHit;

    int surrendered;
    int isBJ;
    int doubledDown;
};
typedef struct hand hand;

struct handList {
    hand *data;
    struct handList* next;
};
typedef struct handList hlist;
typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;

// function declarations
void printState(hlist *player, clist *dealer, int rc, int ss);
void printResults(int q, long int *tcFreq, long double *totalReturn, long double *totalSquares);
void simulate(int N, int numDecks, int cutCard, int h17, int DAS, int RSA, int LS, int canDoubleOn[23], int q, long int *tcFreq, long double *totalReturn, long double *totalSquares);
double netPayoff(hlist *player, clist *d);
void initialDeal(hlist **pplayer,clist **ps, clist **pd,int *prc, int* pss);
clist* makeShoe(int numDecks, pcg32_random_t *rng);
void fisherYates(int numCards, card cards[], pcg32_random_t *rng);
void swap(card* a, card* b);
uint32_t pcg32_random_r(pcg32_random_t* rng);
int removalScore(card c);
int faceValue(clist* cards);
void playerTurn(hlist** player, clist **s, clist *d, int *rc, int *ss, int DAS, int RSA, int sur, int canDoubleOn[23]);
int shouldSurrender(hand *h, card upCard, int rc, int ss, int LS);
int shouldSplit(hand *h, card upCard, int rc, int ss,int numspl, int das);
int shouldDouble(hand *h, card upCard, int rc, int ss, int canDoubleOn[23]);
int shouldHit(hand*h, card upCard, int rc, int ss);
int isSoft(clist *L);
void dealerTurn(hlist *player, clist **s,clist **d, int *rc, int *ss,int h17);
//struct methods
clist* makeclist(card c);
void freeclist(clist* L);
void pushc(card data, clist **L);
card popc(clist** L);
clist* makeclistFromArr(card *arr, int size);
void printCards(clist *cards);
void deal(clist **shoe, clist **cards);
hand* makeemptyhand();
int numCards(hand* h);
void freehand(hand *h);
hlist* makehlist(hand *h);
void freehlist(hlist *L);
void printPlayer(hlist *L);

int main(){
    int rsa = 1, das = 1, numDecks = 2, cutCard = 26, h17 = 1, LS = 1;
    int canDoubleOn[23] = {0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0};
    
    int N; // the # of shoes to play through
    char newline;

    int q = 31; // number of counts I will track, from -q/2 to +q/2
    long int *tcFreq = calloc(q, sizeof(long int));
    long double *totalReturn = calloc(q, sizeof(long double)); //use this to calculate expectations
    long double *totalSquares = calloc(q,sizeof(long double)); //use this to calculate variance

    printf("How many shoes would you like to play through? ");
    if (scanf("%d", &N) == 0){
        return 1;
    }
    if (scanf("%c", &newline) != 1){
        return 1;
    }

    simulate(N, numDecks, cutCard, h17, das, rsa, LS, canDoubleOn, q, tcFreq, totalReturn,totalSquares);
    printResults(q, tcFreq, totalReturn, totalSquares);

    free(tcFreq);
    free(totalReturn);
    free(totalSquares);

    return 0;
}
/*
printResults should print the following for each true count:
    - The # of rounds played
    - The mean payoff
    - 95% confidence interval, as measured by 2*(inter sample deviation)/N
*/
void printResults(int q, long int *tcFreq, long double *totalReturn, long double *totalSquares){
    long int N =0;
    long double S = 0;
    long double sq = 0;

    double mu;
    double var;
    for (int i = 0; i < q;i++){
        N += tcFreq[i];
        S += totalReturn[i];
        sq += totalSquares[i];
    }
    mu = S/N;
    var = sq - mu*mu;
    printf("%11ld bets were made overall, with aggreg. exp. %9.6lf +- %.6lf\n\n", N,mu, 2*sqrt(var)/N);
    mu = totalReturn[0]/tcFreq[0];
    var = totalSquares[0] - mu*mu;
    printf("%11ld bets were made <= true %3d, with avg exp. %9.6lf +- %.6lf\n", tcFreq[0], -q/2, mu, 2*sqrt(var)/tcFreq[0]);
    for (int i = 1; i < q-1; i++){
        mu = totalReturn[i]/tcFreq[i];
        var = totalSquares[i] - mu*mu;
        printf("%11ld bets were made at true %3d, with avg exp. %9.6lf +- %.6lf\n", tcFreq[i], i - q/2, mu, 2*sqrt(var)/tcFreq[i]);
    }
    mu = totalReturn[q-1]/tcFreq[q-1];
    var = totalSquares[q-1] - mu*mu;
    printf("%11ld bets were made >= true %3d, with avg exp. %9.6lf +- %.6lf\n", tcFreq[q-1],q/2, mu, 2*sqrt(var)/tcFreq[q-1]);
}
///////////////////////////////
///// gameplay functions //////
///////////////////////////////

/*
simulate should play through N numDecks-deck shoes according to Basic 
Strategy, with gameplay proceding according to the general rules of Player
may take insurance (but never does), then may choose whether to surrender,
then may choose whether to split, then may choose whether to double down,
then may choose whether to hit, otherwise stands as default. Split hands 
can generally always be surrendered, resplit 3 times up to 4 hands total, 
doubled down upon, and hit until they bust. Ace-Ten pairs arising after a
split are considered 21, not a natural, and split Aces are dealt one card
each and then must stand, except when RSA is allowed (see parameters below)
and they split. After all players' hands are settled, the dealer hits until
exceeding a faceValue of 17 (see h17 parameter below) or busting. If a player's
hand busts, they lose the associated wager, even if the dealer busts as well. 
If a player's hand's faceValue is equal to that of the dealer, the player
"pushes", and recoups their original wager. If a player's hand exceeds that of 
the dealer's, then they recoup twice their original wager, and if the player's
hand is also a natural, then they recoup a total of 2.5x their original wager.

following parameters:

    - cutCard:      when the shoe size is this many cards,
                    the dealer shuffles before the next hand.

    - h17:          When the dealer is dealt a soft 17, this
                    dictates whether the dealer hits

    - DAS:          After a player splits their hand, this 
                    dictates whether they can double down on
                    the two resulting hands. Except for Aces.

    - RSA:          When a player splits AA, and are dealt
                    AA and AX, this dictates whether they 
                    may resplit the hand AA. In general,
                    you may split 3 times up to 4 hands.

    - LS:           AFTER the dealer has peeked for blackjack
                    (hence late), but before any other actions
                    save insurance, a player may elect to 
                    relinquish 1/2 of their bet and reclaim
                    the remainder, ending the hand.

    - canDoubleOn:  Some Blackjack games only permit the action
                    of doubling down when the faceValue of the
                    Player's hand takes certain values. This array
                    encodes the permissibility of doubling down
                    at every possible value.


*/
void simulate(int N, int numDecks, int cutCard, int h17, int DAS, int RSA, int LS, int canDoubleOn[23],
              int q, long int *tcFreq, long double *totalReturn, long double *totalSquares){ 
    clist *s, *d = NULL;
    hlist *player;
    int ss, rc; //shoe size, runningcount
    int tc, index;
    double payoff;
    pcg32_random_t rng = (pcg32_random_t) {42u, 54u};

    for (int i = 0; i < N; i++){
        s = makeShoe(numDecks, &rng);
        ss = numDecks*52;
        rc = 0;
        while(ss > cutCard){


            // record the true count, so that -1 < tc < 1 records as 0
            // and tc < -q/2 records as -q/2 and tc > q/2 records as q/2
            if (rc < 0){
                tc = -((-rc*52)/ss);
                index = (tc < -(q/2)) ? 0: q/2 + tc;
            } else {
                tc = (rc*52)/ss;
                index = (tc > q/2) ? q-1: q/2 + tc;
            }
            tcFreq[index]++;


            // deal two cards to player and dealer.
            initialDeal(&player,&s,&d,&rc,&ss);

            playerTurn(&player, &s,d, &rc,&ss, DAS,RSA, LS,canDoubleOn);

            dealerTurn(player,&s,&d,&rc,&ss, h17); // need to know if player busted, or made blackjack.
            payoff = netPayoff(player, d);


            totalReturn[index] += payoff;
            totalSquares[index] += payoff*payoff;
            
            freehlist(player);
            freeclist(d);
            d = NULL;
        }

        freeclist(s);
        ss = -1;
    }

}

/*
makeShoe should populate and return a stack of cards with numDecks
decks shuffled together.
*/
clist* makeShoe(int numDecks, pcg32_random_t *rng){
    char suits[13] = {'A','2','3','4','5','6','7','8','9','T','J','Q','K'};
    char *s = malloc(numDecks*52*sizeof(char));
    clist* L;
    for (int rep = 0; rep < numDecks*4; rep++){
        for (int i = 0; i < 13; i++){
            s[13*rep + i] = suits[i];
        }
    }
    fisherYates(numDecks*52, s, rng);
    L = makeclistFromArr(s,52*numDecks);
    free(s);
    return L;
}
/* The Fisher-Yates algorithm for shuffling an array */
void fisherYates(int numCards, card cards[], pcg32_random_t *rng) {
    int j;
    for (int i =0; i< numCards - 1; i++) {
        j = i + pcg32_random_r(rng)%(numCards - i);
        swap(&cards[i], &cards[j]);
    }
}
void swap(card* a, card* b) {
    card tmp = *a;
    *a = *b;
    *b = tmp;
}
/*
A Permuted Congruential PRNG.
Thanks for inventing these Prof. O'Neill!
*/
uint32_t pcg32_random_r(pcg32_random_t* rng){
    uint64_t oldstate = rng->state;
    // Advance internal state
    rng->state = oldstate * 6364136223846793005ULL + (rng->inc|1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}
/*
2, 3, 4, 5, and 6 count as +1 each,
T, J, Q, K, and A count as -1 each,
7, 8, and 9 count as 0 each.
*/
int removalScore(card c){
    return (c <'7') - (c > '9');
}
/*
initialDeal should give the dealer and the player two cards each,
alternating. All cards except the hole card should affect the rc
and the shoe size. If the player has a blackjack, we should 
recognize it now.
*/
void initialDeal(hlist **pplayer,clist **ps, clist **pd,int *prc, int* pss){
    *pplayer = makehlist(makeemptyhand());

    for (int i = 0;i < 2; i++){
        //deal card to dealer
        deal(ps, pd);
    
        // deal card to player & record removalScore
        deal(ps, &((*pplayer)->data->cards));
        *prc += removalScore((*pplayer)->data->cards->data);
        (*pss)--;
    }
    //record removalScore of dealer upCard
    *prc += removalScore((*pd)->data);
    (*pss)--;

    //customize attributes of hand
    (*pplayer)->data->isBJ = faceValue((*pplayer)->data->cards) == 22;

}
/*
playerTurn should play according to the following order of operations
    - Insurance: In BS, we ignore this option
    - Check for BJ: If either player or dealer has a BJ, then return
    - offer surrender: If shouldSurrender, mark hand as surrendered and return.
    - While the player has unplayed hands:
        - get the next hand
        - finish a split if applicable (only 1 card in the hand)
        - If shouldSplit, add a hand and give it one of the cards,else
        - If shouldDouble, take a card, mark hand as 
          doubled down, and mark hand as played. else 
        - If shouldHit, take a card. else
        - mark hand as played.
Whenever a card is dealt, the running count and shoe size should be
adjusted accordingly.
*/
void playerTurn(hlist** player, clist **s, clist *d, int *rc, int *ss, int DAS, int RSA, int LS, int canDoubleOn[23]){
    hlist *tmp;
    hand *h;

    int numspl = 0; // tracks the number of times we've split a hand. max 3 allowed

    // insurance? -> NO, never.(remember, this is basic strategy.)

    // check if either player has blackjack
    if ((faceValue(d) == 22) || ((*player)->data->isBJ)){
        return;
    }
    // offer surrender
    if (shouldSurrender((*player)->data,d->data, *rc, *ss, LS)){
        (*player)->data->surrendered = 1;
        return;
    }

    // while there are unplayed hands
    for (hlist* hp = *player; hp != NULL;){
        h = hp->data;

        // split Fork Pt. 2
        if (numCards(h) == 1){
            deal(s, &(h->cards));
            *rc += removalScore(h->cards->data);
            (*ss)--;

            h->canDouble = DAS;

            //rules are much more restrictive when splitting Aces
            if (h->cards->next->data == 'A'){
                h->canHit = 0;
                h->canDouble = 0;
                h->canSplit = RSA;
            }
        }

        // play through the current hand
 
        //Split Fork Pt 1.
        else if ( shouldSplit(h, d->data, *rc, *ss,numspl, DAS) ){ 
            //bookkeeping
            numspl++;
            // adding a newhand to the list
            tmp = hp->next;
            hp->next = makehlist(makeemptyhand());
            hp->next->next = tmp;
            // give one of the cards to the next hand.
            deal(&(hp->data->cards), &(hp->next->data->cards));
        }

        else if ( shouldDouble(h, d->data, *rc, *ss, canDoubleOn)) {
            deal(s,&(h->cards));
            *rc += removalScore(h->cards->data);
            (*ss)--;
            h->doubledDown = 1;

            hp = hp->next;
        }

        else if (shouldHit(h, d->data, *rc, *ss)){
            deal(s,&(h->cards));
            *rc += removalScore(h->cards->data);
            (*ss)--;
        } 

        else {
            hp = hp->next;
        }
    }
}

int faceValue(clist* cards) {
	int total = 0;
	int num_aces = 0;
    int num_cards = 0;
    card c;
	for (clist* p = cards; p != NULL; p=p->next, num_cards++) {
        c = p->data;
		switch (c) {
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
                total += c - '0';
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
/*
In BS, you should surrender if LS is offered and
    - you have a hard 15 against a ten valued card OR
    - you have a hard 16 against a 9,T,J,Q,K, or A
*/
int shouldSurrender(hand *h, card upCard, int rc, int ss, int LS){
    if ( isSoft(h->cards) || !LS) {
        return 0;
    }
    switch (faceValue(h->cards)){
        case 15:
            return (upCard == 'T') || (upCard == 'J') || (upCard == 'Q') || (upCard == 'K');
        case 16:
            return (upCard == '9') || (upCard == 'T') || (upCard == 'J') || (upCard == 'Q') || (upCard == 'K') || (upCard == 'A');
        default:
            return 0;
    }
}
/*
In BS, you MAY split if you have exactly two cards of the same value
                    AND you have split fewer than 3 times prior
                    AND you have not split aces under NRSA rules.
       you SHOULD split Aces
                        Eights
                        Nines except against 7s Tens and Aces
                        7s up to 7
                        6s up to 6 (against 2 only if DAS allowed)
                        4s against 5&6 if DAS allowed
                        3s up to 7 (against 2&3 only if DAS allowed)
                        2s up to 7 (against 2&3 only if DAS allowed)
       you SHOULD NOT split Tens
                        and 5s

*/
int shouldSplit(hand *h, card upCard, int rc, int ss,int numspl, int das){
    clist *tmp;

    // safeguards
    if ( numCards(h) != 2 || numspl >= 3 || !h->canSplit){
        return 0;
    }
    // we test for value equality because you can split AK, for example.
    tmp = h->cards->next;
    h->cards->next = NULL;
    if (faceValue(h->cards) != faceValue(tmp)){
        h->cards->next = tmp;
        return 0;
    }
    h->cards->next = tmp;

    // now, we know it is theoretically possbile, but perhaps not advisable, to split
    switch (h->cards->data){
        case 'A':
            return 1;
        case '2':
            return (das && upCard <= '3') || ( '4' <= upCard && upCard <= '7');
        case '3':
            return (das && upCard <= '3') || ( '4' <= upCard && upCard <= '7');
        case '4':
            return (das && '5' <= upCard && upCard <= '6');
        case '6':
            return (das &&  upCard == '2') || ('3' <= upCard  && upCard <= '6');
        case '7':
            return (upCard <= '7');
        case '8':
            return 1;
        case '9':
            return (upCard != '7') && (upCard <= '9');
        default:
            return 0;
    }
}
/*
In BS, you MAY double down if you have exactly two cards
                          AND you have not split under NDAS rules
                          AND you have not split Aces
                          AND the rules permit doubling on your total
       you SHOULD double down
            - if you have a hard total of 
                - 11
                - 10 and the dealer's upcard <= '9'
                - 9 and the dealer's upCard is between 3 and 6, inclusive
            - OR if you have a soft total of 
                - 13 or 14 and the dealer's upCard is 5 or 6
                - 15 or 16 and the dealer's upCard is 4,5,or 6
                - 17 and the dealer's upCard is 3,4,5, or 6
                - 18 and the dealer's upCard is 2,3,4,5,or6
                - 19 and the dealer's upcard is 6.
*/
int shouldDouble(hand *h, card upCard, int rc, int ss, int canDoubleOn[23]){
    int v = faceValue(h->cards);
    if ( !h->canDouble || !canDoubleOn[v] || numCards(h)!= 2){
        return 0;
    }

    if (isSoft(h->cards)){
        switch (v){
            case 19:
                return upCard == '6';
            case 18:
                return upCard <= '6';
            case 17:
                return '3' <= upCard && upCard <= '6';
            case 16:
                return '4' <= upCard && upCard <= '6';
            case 15:
                return '4' <= upCard && upCard <= '6';
            case 14:
                return '5' <= upCard && upCard <= '6';
            case 13:
                return '5' <= upCard && upCard <= '6';
            default:
                return 0;
        }
    } else {
        switch(v){
            case 11:
                return 1;
            case 10:
                return upCard <= '9';
            case 9:
                return '3' <= upCard && upCard <= '6';
            default:
                return 0;
        }
    }
}

int shouldHit(hand*h, card upCard, int rc, int ss){ 
    int v = faceValue(h->cards);
    if ((!h->canHit) || (v > 20) || (v < 0)) {
        return 0;
    }

    if (isSoft(h->cards)){
        return (v < 18) || ( (v == 18) && ( upCard > '8' ));
    } else {
        if (v < 12){
            return 1;
        } else if ((v < 17) && ('7' <= upCard)) {
            return 1;
        } else if (v == 12) {
            return ((upCard == '2') || (upCard == '3'));
        }
        return 0;
    }
}

int isSoft(clist *L){
    int total = 0;
    int numAces = 0;
    card c;
    for (clist *p = L; p != NULL; p = p->next){
        c = p->data;
        if (c == 'A'){
            numAces++;
            total += 1;
        }
        else if ((c == 'T') || (c == 'J') || (c == 'Q') || (c == 'K')){
            total += 10;
        } 
        else {
            total += c - '0';
        }
    }
    return (numAces > 0) && (total < 11);
}

void dealerTurn(hlist *player, clist **s,clist **d, int *rc, int *ss,int h17){
    int allLost = 1;


    // reveal the hidden card FIRST
    *rc += removalScore((*d)->next->data);
    (*ss)--;

    if (player->data->isBJ){
        return; //dealer pushes or loses
    }


    for (hlist* p = player; p != NULL; p=p->next){
        if (faceValue(p->data->cards) > 0){
            if (p->data->surrendered == 0){
                allLost = 0;
                break;
            }
        }
    }
    if (allLost){
        return; // dealer automatically wins
    }

    // hit until the rules say stop
    while (0 < faceValue(*d) && faceValue(*d) < 17 + h17*isSoft(*d) ){
        deal(s, d);
        *rc += removalScore((*d)->data);
        (*ss)--;
    }
}

double netPayoff(hlist *player, clist *d){
    int dealerScore = faceValue(d);
    int playerScore;
    hand *h;
    double payout = 0.0;
    for(hlist *p = player; p != NULL; p=p->next){
        h = p->data;
        playerScore = faceValue(h->cards);
        if (playerScore == 22 && !h->isBJ){
            playerScore = 21;
        }

        if (h->surrendered){
            payout += -0.5;
        } else if (h->isBJ){
            payout += 1.5*(dealerScore != 22);
        } else if (playerScore == -1){
            payout += -1 - h->doubledDown;
        } else {
            payout += (1+h->doubledDown)*( (playerScore > dealerScore) - (playerScore < dealerScore) );
        }
    }
    return payout;
}
// struct methods

clist* makeclist(card c){
    clist* L = malloc(sizeof(clist));
    L->data = c;
    L->next = NULL;
    return L;
}
void freeclist(clist* L){
    clist* tmp;
    while(L != NULL){
        tmp = L;
        L = L->next;
        free(tmp);
    }
}
void pushc(card data, clist **L){
    clist* top = makeclist(data);
    top -> next = *L;
    *L = top;
}
card popc(clist** L){
    card c;
    clist *tmp;
    if (*L == NULL){
        return '0';
    }
    tmp = *L;
    c = tmp->data;
    *L = (*L)->next;
    free(tmp);
    return c;  
}
clist* makeclistFromArr(card *arr, int size){
    clist *L = NULL, *tmp;
    for (int i = size-1; i > -1; i--){
        tmp = makeclist(arr[i]);
        tmp->next = L;
        L = tmp;
    }
    return L;
}
void printCards(clist *cards){
    while (cards != NULL){
        printf("%c",cards->data);
        cards = cards->next;
    }
}
void deal(clist **shoe, clist **cards){
    clist *tmp = *cards;
    clist *tmp2 = (*shoe)->next;
    *cards = *shoe;
    (*cards)->next = tmp;
    *shoe = tmp2;

}

hand* makeemptyhand(){
    hand *h = malloc(sizeof(hand));
    h->cards = NULL;
    // action flags
    h->canSplit = 1;
    h->canDouble = 1;
    h->canHit = 1;
    // payoff flags
    h->surrendered = 0;
    h->isBJ = 0;
    h->doubledDown = 0;

    return h;
}

int numCards(hand* h){
    int n = 0;
    for (clist *p = h->cards; p != NULL; p = p->next, n++);
    return n;
}
void freehand(hand *h){
    freeclist(h->cards);
    free(h);
}

hlist* makehlist(hand *h){
    hlist* L = malloc(sizeof(hlist));
    L -> data = h;
    L -> next = NULL;
    return L;
}

void freehlist(hlist *L){
    hlist *tmp;
    while (L != NULL){
        tmp = L;
        L = L->next;
        freehand(tmp->data);
        free(tmp);
    }
}
void printPlayer(hlist *L){
    while (L != NULL){
        printCards(L->data->cards);
        printf(", ");
        L=L->next;
    }
}
void printState(hlist *player, clist *d, int rc, int ss){
    char input;
    printf("player has ");
    printPlayer(player);
    printf(" and dealer has ");
    printCards(d);
    printf("\nRC is %d and shoe size is %d\n",rc,ss);
    printf("Press Enter to continue:");
    if (scanf("%c", &input) != 1){
        printf("Ahhhhh, we have a problem with carriage return");
    }
    
}
