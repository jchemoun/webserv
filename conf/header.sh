#! /usr/bin/env zsh

supported_headers="^HTTP|Content-Length|Content-Type|Connection"

printf "\e[33mUri is empty\e[0m\n"
curl -i localhost:8080 | grep -E "$supported_headers" | sort

check_header() {
  delay = "0.02"
  for uri in $@; do
    printf "\e[33mUri: %s\e[0m\n" "$uri"
    # curl -i localhost:8080$uri |
    { echo GET $uri HTTP/1.1 ; sleep $delay ; echo Host: localhost:8080 ; sleep $delay ; echo ; sleep 0.05 ; } |
      telnet localhost 8080 |
      grep -E "$supported_headers" |
      sort
  done
}

check_header / /42_logo.png /index.html '/?query=trololo' '/42_logo.png?a=b'

# For error and autoindex, Content-Length will differ
supported_headers="^HTTP|Content-Type|Connection"
printf "\n\e[34mAutoindex and error page: Content-Length may differ\e[0m\n"
check_header /folder/ /not_found.png /folder
