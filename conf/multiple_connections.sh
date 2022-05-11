# Five client simultaneously
cat \
  <(curl localhost:8080) \
  <(curl localhost:8080/folder/) \
  <(curl localhost:8080/folder/subfolder) \
  <(curl localhost:8080/hello_world.html) \
  <(curl localhost:8080/not_found) \
  | sed 's/nginx[^<]*/webserv\/0.1/g' | sed 's/\s\+/ /g' | sed 's/[0-9]\+:[0-9]\+/hour:min/' | sed 's/ $//'

# Same client but multiple packets in a raw
{ sleep 0.5;
  echo GET / HTTP/1.1          ; sleep 0.1 ; echo Host: localhost:8080 ; sleep 0.1 ; echo ; sleep 0.1 ;
  echo GET /folder/ HTTP/1.1   ; sleep 0.1 ; echo Host: localhost:8080 ; sleep 0.1 ; echo ; sleep 0.1 ;
  echo GET /not_found HTTP/1.1 ; sleep 0.1 ; echo Host: localhost:8080 ; sleep 0.1 ; echo ; sleep 0.1 ;
  echo GET / HTTP/1.1          ; sleep 0.1 ; echo Host: localhost:8080 ; sleep 0.1 ; echo ; sleep 0.1 ;
} | telnet localhost 8080 |
  sed 's/nginx[^<]*/webserv\/0.1/g' |
  sed 's/\s\+/ /g' |
  sed 's/[0-9]\+:[0-9]\+/hour:min/' |
  sed 's/ $//' |
  sed '/^\S\+:.*$/d' |            # remove headers
  sed '/^[0-9]*$/d' | sed '/^$/d' # remove chuck encoding lines


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
