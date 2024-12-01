#!/bin/bash
docker build -t blackjack .
docker run blackjack &
sleep 5
id=$(docker ps -q)
echo $id
docker exec -it "$id" bash
docker stop $(docker ps -a -q)
docker rm $(docker ps -a -q)