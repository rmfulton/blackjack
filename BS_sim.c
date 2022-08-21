#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

/*
WHAT ARE THE FUNKY RULES THAT ARISE WHEN YOU SPLIT ACES?
- after splitting aces, you cannot surrender, double, or hit resulting hands,
- but you may (if RSA) resplit them
- tens dealt to split aces do not count as blackjack, and count as 21.


TODO:
    IMPLEMENT AND TEST:
        printResults
    TEST:
        pushh maybe?
*/

typedef char card;

struct cardList {
    card data;
    struct cardList* next;
};
typedef struct cardList clist;

struct hand {
    clist *cards;
    int canSurrender;
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
void debug(int N, int numDecks, int cutCard, int h17, int DAS, int RSA, int LS, int canDoubleOn[23], int q, int *tcFreq, double *totalReturn, double *totalSquares);
//////////////////////////
// tested functionality //
//////////////////////////
void printResults(int q, int *tcFreq, double *totalReturn, double *totalSquares);
void simulate(int N, int numDecks, int cutCard, int h17, int DAS, int RSA, int LS, int canDoubleOn[23], int q, int *tcFreq, double *totalReturn, double *totalSquares);
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
void pushh(hand *h, hlist **L);
void freehlist(hlist *L);
void printPlayer(hlist *L);

void test();
/*
What specifies the Game?
    - player betting strategy (here flatbet)
    - player # of hands (here 1)

    - player permitted playing strategy (here Basic Strategy according to blackjack apprenticeship)
        - can you resplit aces? (standard yes)
        - can you double after splitting (standard yes)
        - which hands are you allowed to double down on (standard all 2 card hands)
        - are you allowed to surrender hands? (standard yes)
    
    - Game parameters
        - number of decks in use (standard is 6 decks)
        - penetration before shuffle (standard is 1.5 decks)
    
    - Dealer playing strategy
        - does dealer hit soft 17? (standard yes)

    What do we want to know from the simulation?
    We want to know the result of every round
    as well as the true count (= (RC*52) integer divided by number of cards remaining)
*/
int main(){
    int rsa = 1, das = 1, numDecks = 2, cutCard =26, h17 = 1, LS = 1;
    int canDoubleOn[23] = {0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0};
    
    int N; // the # of shoes to play through
    char newline;

    int q = 31; // number of counts I will track, from -q/2 to +q/2
    int *tcFreq = calloc(q, sizeof(int));
    double *totalReturn = calloc(q, sizeof(double)); //use this to calculate expectations
    double *totalSquares = calloc(q,sizeof(long double)); //use this to calculate variance

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
    // test();

    return 0;
}
// results functions
void printResults(int q, int *tcFreq, double *totalReturn, double *totalSquares){ //incomplete + wrong header
    int N =0;
    double S = 0;
    double sq = 0;

    double mu;
    double var;
    for (int i = 0; i < q;i++){
        N += tcFreq[i];
        S += totalReturn[i];
        sq += totalSquares[i];
    }
    mu = S/N;
    var = sq - mu*mu;
    printf("%10d bets were made overall, with aggreg. exp. %8.5lf +- %.5lf\n\n", N,mu, 2*sqrt(var)/N);
    mu = totalReturn[0]/tcFreq[0];
    var = totalSquares[0] - mu*mu;
    printf("%10d bets were made <= true %3d, with avg exp. %8.5lf +- %.5lf\n", tcFreq[0], -q/2, mu, 2*sqrt(var)/tcFreq[0]);
    for (int i = 1; i < q-1; i++){
        mu = totalReturn[i]/tcFreq[i];
        var = totalSquares[i] - mu*mu;
        printf("%10d bets were made at true %3d, with avg exp. %8.5lf +- %.5lf\n", tcFreq[i], i - q/2, mu, 2*sqrt(var)/tcFreq[i]);
    }
    mu = totalReturn[q-1]/tcFreq[q-1];
    var = totalSquares[q-1] - mu*mu;
    printf("%10d bets were made >= true %3d, with avg exp. %8.5lf +- %.5lf\n", tcFreq[q-1],q/2, mu, 2*sqrt(var)/tcFreq[q-1]);
}

// gameplay functions

void simulate(int N, int numDecks, int cutCard, int h17, int DAS, int RSA, int LS, int canDoubleOn[23],
              int q, int *tcFreq, double *totalReturn, double *totalSquares){ //incomplete + wrong header
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

            //TODO: 
            //implement netPayoff

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
void debug(int N, int numDecks, int cutCard, int h17, int DAS, int RSA, int LS, int canDoubleOn[23],
              int q, int *tcFreq, double *totalReturn, double *totalSquares){ //incomplete + wrong header
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

            //TODO: 
            //implement netPayoff

            // deal two cards to player and dealer.
            initialDeal(&player,&s,&d,&rc,&ss);
            printf("After initial deal,");
            printState(player,d,rc,ss);

            playerTurn(&player, &s,d, &rc,&ss, DAS,RSA, LS,canDoubleOn);
            printf("After playerTurn,");
            printState(player,d,rc,ss);

            dealerTurn(player,&s,&d,&rc,&ss, h17); // need to know if player busted, or made blackjack.
            payoff = netPayoff(player, d);
            printf("After dealerTurn,");
            printState(player,d,rc,ss);
            printf("Payoff was %.1lf\n", payoff);


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
// returns a shuffled shoe containing an integer # of decks.
clist* makeShoe(int numDecks, pcg32_random_t *rng){ //tested
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

void fisherYates(int numCards, card cards[], pcg32_random_t *rng) { //tested
    int j;
    for (int i =0; i< numCards - 1; i++) {
        j = i + pcg32_random_r(rng)%(numCards - i);
        swap(&cards[i], &cards[j]);
    }
}
void swap(card* a, card* b) { //tested
    card tmp = *a;
    *a = *b;
    *b = tmp;
}
/*
A Permuted Congruential PRNG.
Thanks for inventing these Prof. O'Neill!
I owe you one!
*/
uint32_t pcg32_random_r(pcg32_random_t* rng)
{
    uint64_t oldstate = rng->state;
    // Advance internal state
    rng->state = oldstate * 6364136223846793005ULL + (rng->inc|1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

int removalScore(card c){ //tested
    if (('2' <= c) && (c <= '6')){
        return 1;
    } else if (('7' <= c) && ( c <= '9')){
        return 0;
    } else {
        return -1;
    }
}

void initialDeal(hlist **pplayer,clist **ps, clist **pd,int *prc, int* pss){//tested
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
    (*pplayer)->data->canSplit = ((*pplayer)->data->cards->data == (*pplayer)->data->cards->next->data);
    (*pplayer)->data->isBJ = (faceValue((*pplayer)->data->cards) == 22);

}

void playerTurn(hlist** player, clist **s, clist *d, int *rc, int *ss, int DAS, int RSA, int LS, int canDoubleOn[23]){ //tested
    hlist *hp, *tmp;
    hand *h;

    int numspl = 0; // tracks the number of times we've split a hand. max 3 allowed
    // insurance? -> NO, never.(remember, this is basic strategy.)
    // check if either player has blackjack
    if ((faceValue(d) == 22) || ((*player)->data->isBJ)){
        return;
    }

    // while there are unplayed hands
    hp = *player;
    while (hp != NULL){
        h = hp->data;

        // split Fork Pt. 2
        if (numCards(h) == 1){
            deal(s, &(h->cards));
            *rc += removalScore(h->cards->data);
            (*ss)--;

            h->canDouble = (DAS &&(numspl<3));

            //rules are much more restrictive when splitting Aces
            if (h->cards->next->data == 'A'){
                h->canHit = 0;
                h->canSurrender = 0;
                h->canDouble = 0;
                h->canSplit = RSA;
            }
        }


        // play through the current hand
        // Surrender Fork
        if (shouldSurrender(h,d->data, *rc, *ss, LS)){

            h->surrendered = 1;
            hp = hp->next;
        } 
        //Split Fork Pt 1.
        else if ( shouldSplit(h, d->data, *rc, *ss,numspl, DAS) ){ 
            //bookkeeping
            numspl++;
            // adding a newhand to the list
            tmp = hp->next;
            hp->next = makehlist(makeemptyhand());
            hp->next->next = tmp;
            deal(&(hp->data->cards), &(hp->next->data->cards)); // give one of the cards to the next hand.
        }
        // Double Down Fork
        else if ( shouldDouble(h, d->data, *rc, *ss, canDoubleOn)) {
            deal(s,&(h->cards));
            *rc += removalScore(h->cards->data);
            (*ss)--;
            h->doubledDown = 1;

            hp = hp->next;
        }
        // Hit Fork
        else if (shouldHit(h, d->data, *rc, *ss)){
            deal(s,&(h->cards));
            *rc += removalScore(h->cards->data);
            (*ss)--;
        } 
        // Stand Fork
        else {
            hp = hp->next;
        }
        // move the hand pointer forward after standing, doubling down, or surrendering - busting will result in standing.
    }
}

int faceValue(clist* cards) { //tested
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

int shouldSurrender(hand *h, card upCard, int rc, int ss, int LS){ //tested
    if ((h->canSurrender == 0) || (isSoft(h->cards)) || (numCards(h) != 2) || !LS) {
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

int shouldSplit(hand *h, card upCard, int rc, int ss,int numspl, int das){ //tested
    // some (possibly redundant) safeguards
    if ((!h->canSplit) || (numCards(h) != 2) || (numspl >= 3)){
        return 0;
    }
    if (h->cards->data != h->cards->next->data){
        return 0;
    }
    // now, we know it is theoretically possbile, but perhaps not advisable, to split
    switch (h->cards->data){
        case 'A':
            return 1;
        case '2':
            return (das && ('2' <= upCard) && (upCard <= '3')) || ( ('4' <= upCard) && (upCard <= '7'));
        case '3':
            return (das && ('2' <= upCard) && (upCard <= '3')) || (('4' <= upCard) && (upCard <= '7'));
        case '4':
            return (das && ('5' <= upCard) && (upCard <= '6'));
        case '6':
            return (das &&  upCard == '2') || (('3' <= upCard ) &&(upCard <= '6'));
        case '7':
            return (upCard <= '7');
        case '8':
            return 1;
        case '9':
            return (upCard != '7') &&(upCard <= '9');
        default:
            return 0;
    }
}

int shouldDouble(hand *h, card upCard, int rc, int ss, int canDoubleOn[23]){ //tested
    if ((!h->canDouble) || (faceValue(h->cards) > 21) || ( !canDoubleOn[faceValue(h->cards)] ) || (numCards(h)!= 2)){
        return 0;
    }

    if (isSoft(h->cards)){
        switch (faceValue(h->cards)){
            case 19:
                return upCard == '6';
            case 18:
                return (upCard <= '6');
            case 17:
                return ('3' <= upCard) && (upCard <= '6');
            case 16:
                return ('4' <= upCard) && (upCard <= '6');
            case 15:
                return ('4' <= upCard) && (upCard <= '6');
            case 14:
                return ('5' <= upCard) && (upCard <= '6');
            case 13:
                return ('5' <= upCard) && (upCard <= '6');
            default:
                return 0;
        }
    } else {
        switch(faceValue(h->cards)){
            case 11:
                return 1;
            case 10:
                return (upCard <= '9');
            case 9:
                return ('3' <= upCard) && (upCard <= '6');
            default:
                return 0;
        }
    }
}

int shouldHit(hand*h, card upCard, int rc, int ss){ //tested
    if ((!h->canHit) || (faceValue(h->cards) > 20) || (faceValue(h->cards) < 0)) {
        return 0;
    }

    if (isSoft(h->cards)){
        return (faceValue(h->cards) < 18) || ( (faceValue(h->cards) == 18) && ( upCard > '8' ));
    } else {
        if (faceValue(h->cards) < 12){
            return 1;
        } else if ((faceValue(h->cards) < 17) && ('7' <= upCard)) {
            return 1;
        } else if (faceValue(h->cards) == 12) {
            return ((upCard == '2') || (upCard == '3'));
        }
        return 0;
    }
}

int isSoft(clist *L){ //tested
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

void dealerTurn(hlist *player, clist **s,clist **d, int *rc, int *ss,int h17){ //tested
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

double netPayoff(hlist *player, clist *d){ //tested
    int dealerScore = faceValue(d);
    int playerScore;
    hand *h;
    double payout = 0.0;
    for(hlist *p = player; p != NULL; p=p->next){
        h = p->data;
        playerScore = faceValue(h->cards);

        if (h->surrendered){
            payout -= 0.5;
        } else if (h->isBJ){
            payout += 1.5*(dealerScore != 22);
        } else if (playerScore == -1){
            payout -= 1 + h->doubledDown;
        } else {
            payout += (1+h->doubledDown)*((playerScore > dealerScore) - (playerScore < dealerScore));
        }
    }
    return payout;
}
// struct methods

clist* makeclist(card c){ //tested
    clist* L = malloc(sizeof(clist));
    L->data = c;
    L->next = NULL;
    return L;
}
void freeclist(clist* L){ //tested
    clist* tmp;
    while(L != NULL){
        tmp = L;
        L = L->next;
        free(tmp);
    }
}
void pushc(card data, clist **L){ //tested
    clist* top = makeclist(data);
    top -> next = *L;
    *L = top;
}
card popc(clist** L){ //tested
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
clist* makeclistFromArr(card *arr, int size){ //tested
    clist *L = NULL, *tmp;
    for (int i = size-1; i > -1; i--){
        tmp = makeclist(arr[i]);
        tmp->next = L;
        L = tmp;
    }
    return L;
}
void printCards(clist *cards){ //tested
    while (cards != NULL){
        printf("%c",cards->data);
        cards = cards->next;
    }
}
void deal(clist **shoe, clist **cards){ //tested
    clist *tmp = *cards;
    clist *tmp2 = (*shoe)->next;
    *cards = *shoe;
    (*cards)->next = tmp;
    *shoe = tmp2;

}

hand* makeemptyhand(){ //tested
    hand *h = malloc(sizeof(hand));
    h->cards = NULL;
    h->canSurrender = 1;
    h->canSplit = 1;
    h->canDouble = 1;
    h->canHit = 1;
    h->surrendered = 0;
    h->isBJ = 0;
    h->doubledDown = 0;

    return h;
}

int numCards(hand* h){ //tested
    int n = 0;
    for (clist *p = h->cards; p != NULL; p = p->next, n++);
    return n;
}
void freehand(hand *h){ //tested
    freeclist(h->cards);
    free(h);
}

hlist* makehlist(hand *h){ //tested
    hlist* L = malloc(sizeof(hlist));
    L -> data = h;
    L -> next = NULL;
    return L;
}

void pushh(hand *h, hlist **L){ //UNTESTED
    hlist* top = makehlist(h);
    top -> next = *L;
    *L = top;
}

void freehlist(hlist *L){ //tested
    hlist *tmp;
    while (L != NULL){
        tmp = L;
        L = L->next;
        freehand(tmp->data);
        free(tmp);
    }
}
void printPlayer(hlist *L){ //tested
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
