#! /usr/bin/env zsh

# server three_listen
curl -H "Host: site_one" localhost:8080
curl -H "Host: site_two" localhost:8080
curl -H "Host: three_listen" localhost:8080
curl localhost:8888
curl --resolve site_one:8080:127.0.0.1 http://site_one:8080
curl -H "Host: site_two" localhost:8000 # server name exists, port too, but not on that server -> port primes
curl -H "Host: not_found" localhost:8888 # ports prime
curl -H "Host: not_found" localhost:8080 # first server that matches port

# server site_1
curl -H "Host: site_one" localhost:8081
curl -H "Host: not_found" localhost:8081 # ports primes
curl localhost:8081

# server site_2
curl localhost:8282
curl localhost:8082

#multiple server names
curl -H "Host: hello" localhost:8080
curl --resolve world:8080:127.0.0.1 http://world:8080
curl --resolve server42:8080:127.0.0.1 http://server42:8080

#Check with query_string
curl -H "Host: site_one" 'localhost:8080?query=hello'
curl -H "Host: site_two" 'localhost:8080?query=world'
curl -H "Host: site_two" 'localhost:8080/site2.html?query=world'
