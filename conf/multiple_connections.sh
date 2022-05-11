#! /usr/bin/env zsh

# ============================= Multiple clients ============================= #

cat \
  <(curl localhost:8080) \
  <(curl localhost:8080/folder/) \
  <(curl localhost:8080/folder/subfolder) \
  <(curl localhost:8080/hello_world.html) \
  <(curl localhost:8080/not_found) \
  | sed 's/nginx[^<]*/webserv\/0.1/g' | sed 's/\s\+/ /g' | sed 's/[0-9]\+:[0-9]\+/hour:min/' | sed 's/ $//'

# =================== Single client with multiple requests =================== #

multi_get() {
  delay="$1"
  shift
  { sleep 0.5;
    for uri in $@; do
      echo GET $uri HTTP/1.1 ; sleep $delay ; echo Host: localhost:8080 ; sleep $delay ; echo ; sleep 0.1 ;
    done;
  } | telnet localhost 8080 |
    sed 's/nginx[^<]*/webserv\/0.1/g' |
    sed 's/\s\+/ /g' |
    sed 's/[0-9]\+:[0-9]\+/hour:min/' |
    sed 's/ $//' |
    sed '/^\S\+:.*$/d' |            # remove headers
    sed '/^[0-9]*$/d' | sed '/^$/d' # remove chuck encoding lines
}

multi_get 0.02 / /foler/ /not_found /

# ================= Multiple clients with multiple requests ================== #

cat \
  <(multi_get 0.02 / /folder/ / /folder/ / /folder /not_found / /not_found /not_found /folder/) \
  <(multi_get 0.02 /not_found / /folder/ / /folder/ / /folder /not_found / /not_found /not_found /folder/) \
  <(multi_get 0.05 / /folder/ /subfolder/ /not_found / / /folder/) \
  <(multi_get 0.02 / /not_found / /folder/ / /folder/ / /folder /not_found / /not_found /not_found /folder/) \
  <(multi_get 0.07 /hello_world.html /folder / /) \
  <(multi_get 0.1 /fds / /folder/)

cat \
  <(multi_get 0.01 /fds / /folder/) \
  <(multi_get 0.01 /fds / /folder/) \
  <(multi_get 0.01 /fds / /folder/) \
  <(multi_get 0.01 /fds / /folder/) \
  <(multi_get 0.01 /fds / /folder/) \
  <(multi_get 0.01 /fds / /folder/) \
  <(multi_get 0.01 /fds / /folder/) \
  <(multi_get 0.01 /fds / /folder/) \
  <(multi_get 0.01 /fds / /folder/) \
  <(multi_get 0.01 /fds / /folder/) \
  <(multi_get 0.01 /fds / /folder/) \
  <(multi_get 0.01 /fds / /folder/) \
  <(multi_get 0.01 /fds / /folder/) \
  <(multi_get 0.01 /fds / /folder/) \
  <(multi_get 0.01 /fds / /folder/) \
  <(multi_get 0.01 /fds / /folder/) \
  <(multi_get 0.01 /fds / /folder/) \
  <(multi_get 0.01 /fds / /folder/) \
  <(multi_get 0.01 /fds / /folder/) \
  <(multi_get 0.01 /fds / /folder/) \
  <(multi_get 0.01 /fds / /folder/)

# create unknown epoll event (25) loop
# expect <<- EOF
#     spawn telnet localhost 8080
#     sleep 0.2
#     send "GET / HTTP\r"
#     sleep 0.2
#     send "\r"
#     expect -re "(.*)"
#     puts "\$expect_out(1, string)"
#     sleep 0.2
# EOF

# same error
# { sleep 1;echo GET / http; echo $'\e'; } | telnet localhost 8080
