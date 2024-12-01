FROM gcc
WORKDIR /blackjack
COPY . /blackjack
RUN chmod 777 entrypoint.sh
ENTRYPOINT [./entrypoint.sh]