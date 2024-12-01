class Shoe {
    private:
        const static unsigned s_ranks {10};
        const static unsigned s_suits {4};

        // The ith element counts the number of cards of a given rank
        // m_cards[0] is the number of cards in the deck, by fiat
        unsigned m_cards[s_ranks + 1] {};

    public:
        Shoe(){
            Shoe(1);
        }

        Shoe(unsigned numDecks){
            m_cards[0] = 0;
            for (int i = 1; i < s_ranks; ++i){
                m_cards[i] = numDecks*s_suits;
                m_cards[0] += m_cards[i];
            }
            m_cards[s_ranks] = 4*s_suits*numDecks;
            m_cards[0] += m_cards[s_ranks];
        }

        Shoe(unsigned cards[s_ranks + 1]){
            for (int i = 0; i < s_ranks + 1; ++i){
                m_cards[i] = cards[i];
            }
        }

        unsigned numCards() {
            return m_cards[0];
        }

};