#include <stdio.h>
#include <stdlib.h>
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

typedef struct intList {int data;struct intList *next;} list;
typedef struct fList {double data; struct fList *next;} flist;


// function declarations
void printState(hlist *player, clist *dealer, int rc, int ss);
void debug(int N, int numDecks, int cutCard, int h17, int DAS, int RSA, int LS, int canDoubleOn[23], flist **payoffs, list **tc);
//////////////////////////
// tested functionality //
//////////////////////////
void printResults(int q, int *tcFreq, double *totalReturn, double *totalSquares);
void simulate(int N, int numDecks, int cutCard, int h17, int DAS, int RSA, int LS, int canDoubleOn[23], int q, int *tcFreq, double *totalReturn, double *totalSquares);
double netPayoff(hlist *player, clist *d);
void initialDeal(hlist **pplayer,clist **ps, clist **pd,int *prc, int* pss);
clist* makeShoe(int numDecks);
void fisherYates(int numCards, card cards[]);
void swap(card* a, card* b);
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
list* makelist( int data);
void freelist( list* L);
void pushi(int data, list **L);
flist* makeflist(double data);
void freeflist(flist* L);
void pushf(double data, flist **L);
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
    srand(time(0));
    int rsa = 1, das = 1, numDecks = 2, cutCard =26, h17 = 1, LS = 1;
    int canDoubleOn[23] = {0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0};
    
    int N; // the # of shoes to play through
    char newline;

    int q = 31; // number of counts I will track, from -q/2 to +q/2
    int *tcFreq = calloc(q, sizeof(int));
    double *totalReturn = calloc(q, sizeof(double)); //use this to calculate expectations
    double *totalSquares = calloc(q,sizeof(long double)); //use this to calculate variance

    printf("How many shoes would you like to play through? ");
    scanf("%d", &N);
    scanf("%c", &newline);

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
    printf("%d bets were made, with avg expectation %.4lf +- %.4lf\n\n", N,mu, 2*sqrt(var)/N);
    mu = totalReturn[0]/tcFreq[0];
    var = totalSquares[0] - mu*mu;
    printf("%d bets were made at or below a true %d, with avg exp. %.4lf +- %.4lf\n", tcFreq[0], -q/2, mu, 2*sqrt(var)/tcFreq[0]);
    for (int i = 1; i < q-1; i++){
        mu = totalReturn[i]/tcFreq[i];
        var = totalSquares[i] - mu*mu;
        printf("%d bets were made at true %d, with avg exp. %.4lf +- %.4lf\n", tcFreq[i], i - q/2, mu, 2*sqrt(var)/tcFreq[i]);
    }
    mu = totalReturn[q-1]/tcFreq[q-1];
    var = totalSquares[q-1] - mu*mu;
    printf("%d bets were made at or above a true %d, with avg exp. %.4lf +- %.4lf\n", tcFreq[q-1],q/2, mu, 2*sqrt(var)/tcFreq[q-1]);
}

// gameplay functions

void simulate(int N, int numDecks, int cutCard, int h17, int DAS, int RSA, int LS, int canDoubleOn[23],
              int q, int *tcFreq, double *totalReturn, double *totalSquares){ //incomplete + wrong header
    clist *s, *d = NULL;
    hlist *player;
    int ss, rc; //shoe size, runningcount
    int tc, index;
    double payoff;
    //debug
    char input;
    for (int i = 0; i < N; i++){
        s = makeShoe(numDecks);
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
void debug(int N, int numDecks, int cutCard, int h17, int DAS, int RSA, int LS, int canDoubleOn[23], flist **payoffs, list **tc){ //tested
    clist *s, *d = NULL;
    hlist *player;
    int ss, rc; //shoe size, runningcount
    double payoff;
    //debug
    char input;
    for (int i = 0; i < N; i++){
        s = makeShoe(numDecks);
        ss = numDecks*52;
        rc = 0;
        while(ss > cutCard){


            // record the true count, so that -1 < tc < 0 records as 0
            if (rc < 0){
                pushi(-((-rc*52)/ss), tc);
            } else {
                pushi((rc*52)/ss, tc);
            }

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


            pushf(payoff, payoffs);
            
            freehlist(player);
            freeclist(d);
            d = NULL;
        }

        freeclist(s);
        ss = -1;
    }

}
// returns a shuffled shoe containing an integer # of decks.
clist* makeShoe(int numDecks){ //testetime(NULL)d
    char suits[13] = {'A','2','3','4','5','6','7','8','9','T','J','Q','K'};
    char *s = malloc(numDecks*52*sizeof(char));
    clist* L;
    for (int rep = 0; rep < numDecks*4; rep++){
        for (int i = 0; i < 13; i++){
            s[13*rep + i] = suits[i];
        }
    }
    fisherYates(numDecks*52, s);
    L = makeclistFromArr(s,52*numDecks);
    free(s);
    return L;
}

void fisherYates(int numCards, card cards[]) { //tested
    card tmp;
    int j;
    for (int i =0; i< numCards - 1; i++) {
        j = i + rand()%(numCards - i);
        swap(&cards[i], &cards[j]);
    }
}
void swap(card* a, card* b) { //tested
    card tmp = *a;
    *a = *b;
    *b = tmp;
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

list* makelist( int data){ //tested
    list *L = malloc(sizeof(list));
    L->data = data;
    L->next = NULL;
    return L;
}
void freelist( list* L){ //tested
    list* tmp;
    while (L != NULL){
        tmp = L;
        L = L->next;
        free(tmp);
    }
}
void pushi(int data, list **L) { //tested
    list* top = makelist(data);
    top -> next = *L;
    *L = top;
}
flist* makeflist(double data){ //tested
    flist *L = malloc(sizeof(flist));
    L-> data = data;
    L-> next = NULL;
    return L;
}
void freeflist(flist* L){ //tested
    flist* tmp;
    while (L != NULL){
        tmp = L;
        L = L->next;
        free(tmp);
    }
}
void pushf(double data, flist **L) { //tested
    flist* top = makeflist(data);
    top -> next = *L;
    *L = top;
}
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
    scanf("%c", &input);
    
}


void test(){
    list *L;
    flist *F;
    clist *cards;
    clist *cards2 = NULL;
    hand *h;
    card c;
    card ranks[13] = {'2','3','4','5','6','7','8','9','T','J','Q', 'K', 'A'};
    int canDoubleOn[23];
    for (int i = 0; i < 23; i++){
        canDoubleOn[i] = 1;
    }
    // // tests for makeList, pushi, freeList
    // printf("tests for list methods:\n");
    // L = makelist(1);
    // pushi(2, &L);
    // printf("first element should be 2: %d\n", L->data);
    // printf("second element should be 1: %d\n",L->next->data);
    // freelist(L);
    // printf("\n");

    // // tests for makeflist, pushf, freeflist
    // printf("tests for flist methods:\n");
    // F = makeflist(1.0);
    // pushf(3.0,&F);
    // printf("first element should be 3.0: %.1lf\n", F->data);
    // printf("second element should be 1.0: %.1lf\n", F->next->data);
    // freeflist(F);
    // printf("\n");

    // // tests for makeclist, freeclist, makeclistfromArr, makeShoe, printCards, (implicitly fisherYates and swap)
    // printf("test for clist methods and makeShoe\n");
    // cards = makeShoe(1);
    // printCards(cards);
    // freeclist(cards);
    // printf("\n\n");

    // // tests removalScore
    // printf("test for removalScore:\n");
    // printf("A->%d ", removalScore('A'));
    // for (int i = 0; i < 8; i++){
    //     printf("%c->%d ", '2'+i, removalScore('2'+i));
    // }
    // printf("T->%d ", removalScore('T'));
    // printf("J->%d ", removalScore('J'));
    // printf("Q->%d ", removalScore('Q'));
    // printf("K->%d ", removalScore('K'));
    // printf("\n\n");

    // //tests pushc, popc
    // printf("test for pushc, popc:\n");
    // cards = NULL;
    // printf("push order:\n");
    // for (char c = 'A'; c != 'Z'+1; c++){
    //     printf("%c", c);
    //     pushc(c, &cards);
    // }
    // printf("\npop order:\n");
    // while (cards != NULL){
    //     printf("%c", popc(&cards));
    // }
    // printf("\n\n");

    // // tests deal, faceValue
    // printf("test for deal, faceValue:\n");
    // cards = makeShoe(2);
    // for (int i = 0; i < 1; i++){
    //     while ((-1 < faceValue(cards2)) &&(faceValue(cards2)< 17)){
    //         deal(&cards,&cards2);
    //         printf("shoe is ");
    //         printCards(cards);
    //         printf("hand is ");
    //         printCards(cards2);
    //         printf("faceValue is %d\n", faceValue(cards2));
    //         printf("\n");
    //     }
    //     freeclist(cards2);
    //     cards2 = NULL;
    // }
    // freeclist(cards);

    // // tested initialDeal, makehlist, makeemptyhand, freehlist,freehand
    
    // //testing numCards
    // printf("Testing numCards method\n");
    // h = makeemptyhand();
    // cards = makeShoe(1);
    // h->cards = cards;
    // printf("there are %d cards in a shuffled deck.\n", numCards(h));
    // cards2 = NULL;
    // while (faceValue(cards2) > -1){
    //     deal(&cards,&cards2);
    // }
    // h->cards = cards2;
    // printf("there are %d cards in the hand ", numCards(h));
    // printCards(cards2);
    // h->cards = cards;
    // printf("and now there are %d remaining cards in the deck", numCards(h));
    // freeclist(cards);
    // freeclist(cards2);
    // h->cards = NULL;
    // freehand(h);
    // printf("\n\n");

    // // testing shouldSurrender
    // printf("Testing surrendering\n");
    // printf("Should surrender two card hard hands totaling 15 against Dealer TJQK, or 16 against dealer 9TJQKA\n");
    // h = makeemptyhand();
    // pushc('T', &(h->cards));
    // for (card pcard = '4'; pcard <= '7'; pcard++){
    //     pushc(pcard, &(h->cards));
    //     for (int i = 6; i < 13; i++){
    //         c = ranks[i];
    //         printf("Do we surrender ");
    //         printCards(h->cards);
    //         printf(" vs dealer %c: ", c);
    //         if (shouldSurrender(h,c,0,0)){
    //             printf("YES\n");
    //         } else {
    //             printf("NO\n");
    //         }

    //     }
    //     pcard = popc(&(h->cards));
    // }
    // printf("\n\n should not surrender for > 2 card hands \n");
    // c = popc(&(h->cards));
    // pushc('5', &(h->cards));
    // pushc('5', &(h->cards));
    // for (card pcard = '5'; pcard <= '6'; pcard++){
    //     pushc(pcard, &(h->cards));
    //     for (int i = 7; i < 13; i++){
    //         c = ranks[i];
    //         printf("Do we surrender ");
    //         printCards(h->cards);
    //         printf(" vs dealer %c: ", c);
    //         if (shouldSurrender(h,c,0,0)){
    //             printf("YES\n");
    //         } else {
    //             printf("NO\n");
    //         }

    //     }
    //     pcard = popc(&(h->cards));
    // }
    // freehand(h);
    // printf("should not surrender if hand has been labeled as not surrenderable\n");
    // h = makeemptyhand();
    // c = 'A';
    // h->canSurrender = 0;
    // pushc('9', &(h->cards));
    // pushc('7', &(h->cards));
    // printf("Should we stand on ");
    // printCards(h->cards);
    // printf(" vs. dealer %c  when adversarily labeled?: %d\n", c, shouldSurrender(h,c,0,0));
    
    // // Testing shouldSplit
    // printf("Testing shouldsplit\n");
    // h = makeemptyhand();
    // printf("First pass: Assume DAS\n");
    // for (int i = 0; i < 13; i++){
    //     pushc(ranks[i], &(h->cards));
    //     pushc(ranks[i], &(h->cards));
    //     for (int j = 0; j < 13; j++){
    //         c = ranks[j];
    //         printf("Should we split %c's vs %c?: ", ranks[i], ranks[j]);
    //         if (shouldSplit(h, c, 0,0,0,1)){ //hand, upCard, rc, ss, numspl, 
    //             printf("YES\n");
    //         } else {
    //             printf("NO\n");
    //         }
    //     }
    //     c = popc(&(h->cards));
    //     c = popc(&(h->cards));
    // }
    // printf("\nPass 2: Assume NDAS\n");
    // for (int i = 0; i < 13; i++){
    //     pushc(ranks[i], &(h->cards));
    //     pushc(ranks[i], &(h->cards));
    //     for (int j = 0; j < 13; j++){
    //         c = ranks[j];
    //         printf("Should we split %c's vs %c?: ", ranks[i], ranks[j]);
    //         if (shouldSplit(h, c, 0,0,0,0)){ //hand, upCard, rc, ss, numspl, das
    //             printf("YES\n");
    //         } else {
    //             printf("NO\n");
    //         }
    //     }
    //     c = popc(&(h->cards));
    //     c = popc(&(h->cards));
    // }
    // printf("\nPass 3: Special Cases\n");
    // pushc('A', &(h->cards));
    // c = '6';
    // printf("Can we split on 1 card: ");
    // if (shouldSplit(h,c,0,0,0,1)){
    //     printf("YES\n");
    // } else {
    //     printf("NO\n");
    // }
    // pushc('A', &(h->cards));
    // pushc('A',&(h->cards));
    // printf("Can we split on 3 cards: ");
    // if (shouldSplit(h,c,0,0,0,1)){
    //     printf("YES\n");
    // } else {
    //     printf("NO\n");
    // }
    // c = popc(&(h->cards));
    // printf("Can we split after having split 3 times prior: ");
    // if (shouldSplit(h,c,0,0,3,1)){
    //     printf("YES\n");
    // } else {
    //     printf("NO\n");
    // }
    // h->canSplit = 0;
    // printf("Can we split against a adversarial tag:");
    // if (shouldSplit(h,c,0,0,0,1)){
    //     printf("YES\n");
    // } else {
    //     printf("NO\n");
    // }
    // freehand(h);

    // // Testing isSoft -> not very thorough.
    // printf("Testing isSoft\n");
    // cards = makeclist('A');
    // pushc('5', &cards);
    // printf("Is the hand ");
    // printCards(cards);
    // printf(" soft? ");
    // if (isSoft(cards)){
    //     printf("YES\n");
    // } else {
    //     printf("NO\n");
    // }
    // pushc('3', &cards);
    //     printf("Is the hand ");
    // printCards(cards);
    // printf(" soft? ");
    // if (isSoft(cards)){
    //     printf("YES\n");
    // } else {
    //     printf("NO\n");
    // }
    // popc(&cards);
    // popc(&cards);
    // pushc('J', &cards);
    // printf("Is the hand ");
    // printCards(cards);
    // printf(" soft? ");
    // if (isSoft(cards)){
    //     printf("YES\n");
    // } else {
    //     printf("NO\n");
    // }
    // freeclist(cards);

    // // Test for shouldDouble
    // printf("Testing shouldDouble\n");
    // printf("Testing soft totals\n");
    // h = makeemptyhand();
    // pushc('A', &(h->cards));
    // for (int i = 0; i < 9; i++){
    //     pushc(ranks[i], &(h->cards));
    //     for (int j = 0; j < 13; j++){
    //         printf("should we double ");
    //         printCards(h->cards);
    //         printf(" versus %c: ", ranks[j]);
    //         if (shouldDouble(h,ranks[j], 0,0,canDoubleOn)){
    //             printf("YES\n");
    //         } else {
    //             printf("NO\n");
    //         }
    //     }
    //     popc(&(h->cards));
    // }
    // freehand(h);

    // printf("\nTesting hard totals\n");
    // h = makeemptyhand();
    // pushc('6', &(h->cards));
    // for (int i = 0; i < 5; i++){
    //     pushc(ranks[i], &(h->cards));
    //     for (int j = 0; j < 13; j++){
    //         printf("should we double ");
    //         printCards(h->cards);
    //         printf(" versus %c: ", ranks[j]);
    //         if (shouldDouble(h,ranks[j], 0,0,canDoubleOn)){
    //             printf("YES\n");
    //         } else {
    //             printf("NO\n");
    //         }
    //     }
    //     popc(&(h->cards));
    // }
    // freehand(h);

    // printf("\nTesting edge cases\n");
    // h = makeemptyhand();
    // pushc('T', &(h->cards));
    // pushc('T', &(h->cards));
    // pushc('2', &(h->cards));
    // for (int j = 0; j < 13; j++){
    //     printf("should we double ");
    //     printCards(h->cards);
    //     printf(" versus %c: ", ranks[j]);
    //     if (shouldDouble(h,ranks[j], 0,0,canDoubleOn)){
    //         printf("YES\n");
    //     } else {
    //         printf("NO\n");
    //     }
    // }
    // printf("\n");
    // freeclist(h->cards);
    // h->cards = NULL;

    // pushc('A', &(h->cards));
    // pushc('7', &(h->cards));
    // h->canDouble = 0;

    // printf("should we double ");
    // printCards(h->cards);
    // printf(" versus %c with adverse signalling: ", '6');
    // if (shouldDouble(h,'6', 0,0,canDoubleOn)){
    //     printf("YES\n");
    // } else {
    //     printf("NO\n");
    // }

    // canDoubleOn[18] = 0;
    // h->canDouble = 1;
    // printf("should we double ");
    // printCards(h->cards);
    // printf(" versus %c when illegal: ", '6');
    // if (shouldDouble(h,'6', 0,0,canDoubleOn)){
    //     printf("YES\n");
    // } else {
    //     printf("NO\n");
    // }
    // freehand(h);

    // // Test for shouldHit;
    // printf("Testing ShouldHit functionality\n");
    // // Hard Totals:
    // printf("Hard Totals\n");
    // h = makeemptyhand();
    // // 4 through 12
    // pushc('2', &(h->cards));
    // for (int i = 0; i < 9; i++){
    //     pushc(ranks[i], &(h->cards));
    //     for (int j = 0; j < 13; j++){
    //         c = ranks[j];
    //         printf("should we hit ");
    //         printCards(h->cards);
    //         printf(" versus %c: ", c);
    //         if (shouldHit(h,c,0,0)){
    //             printf("YES\n");
    //         } else {
    //             printf("NO\n");
    //         }
    //     }

    //     popc(&(h->cards));
    // }
    // //13 through 20
    // popc(&(h->cards));
    // pushc('T', &(h->cards));
    // for (int i = 1; i < 9; i++){
    //     pushc(ranks[i], &(h->cards));
    //     for (int j = 0; j < 13; j++){
    //         c = ranks[j];
    //         printf("should we hit ");
    //         printCards(h->cards);
    //         printf(" versus %c: ", c);
    //         if (shouldHit(h,c,0,0)){
    //             printf("YES\n");
    //         } else {
    //             printf("NO\n");
    //         }
    //     }

    //     popc(&(h->cards));
    // }
    // //21 and bust
    // popc(&(h->cards));
    // pushc('T', &(h->cards));
    // pushc('9', &(h->cards));
    // for (int i = 0; i < 2; i++){
    //     pushc(ranks[i], &(h->cards));
    //     printf("faceValue is %d\n", faceValue(h->cards));
    //     for (int j = 0; j < 13; j++){
    //         c = ranks[j];
    //         printf("should we hit ");
    //         printCards(h->cards);
    //         printf(" versus %c: ", c);
    //         if (shouldHit(h,c,0,0)){
    //             printf("YES\n");
    //         } else {
    //             printf("NO\n");
    //         }
    //     }

    //     popc(&(h->cards));
    // }
    // freehand(h);
    // h = NULL;

    // // Soft Totals
    // printf("\nSoft Totals\n");
    // h = makeemptyhand();
    // pushc('A', &(h->cards));
    // for (int i = 0; i < 13; i++){
    //     pushc(ranks[i], &(h->cards));
    //     printf("faceValue is %d\n", faceValue(h->cards));
    //     for (int j = 0; j < 13; j++){
    //         c = ranks[j];
    //         printf("should we hit ");
    //         printCards(h->cards);
    //         printf(" versus %c: ", c);
    //         if (shouldHit(h,c,0,0)){
    //             printf("YES\n");
    //         } else {
    //             printf("NO\n");
    //         }
    //     }
    //     popc(&(h->cards));
    // }


    // playerTurn was tested in an intermediate stage of simulate on a six deck shoe.
    // It made all the correct decisions.

    // dealerTurn was tested in an intermediate stage of simulate on a six deck shoe
    // It made all the correct decisions.

    //simulate as a whole was tested with print statements. on 6 2-deck shoes (cut at 1.5 decks)
    // It amde all the correct decisions, recordkeeping, and calculations.
}