#! /usr/bin/env zsh

# server three_listen
curl localhost:8080
curl localhost:8000
curl localhost:8080
curl localhost:8888
curl localhost:8888

# no port
curl localhost:9000

# server site_1
curl localhost:8081

# server site_2
curl localhost:8282
curl localhost:8082
