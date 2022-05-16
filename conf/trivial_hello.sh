# ============================ Auto-index testing ============================ #

curl localhost:8080
curl localhost:8080/
# existing folder/ with autoindex
curl localhost:8080/folder/ | sed 's/\s\+/ /g' | sed 's/[0-9]\+:[0-9]\+/hour:min/' | sed 's/ $//'
# empty subfolder/
curl localhost:8080/folder/subfolder/| sed 's/nginx[^<]*/webserv\/0.1/g' | sed 's/\r//g'
# TODO: existing folder with autoindex (return 301)
# curl localhost:8080/folder | sed 's/\s\+/ /g' | sed 's/[0-9]\+:[0-9]\+/hour:min/' | sed 's/ $//'

# ========================= Parsing Request testing ========================== #

# telnet (NOTE: this non-interactive telnet will end lines by '\n' instead of '\r\n')
printf "\e[33mShould just timeout (no empty line sent):\e[0m "
curl --max-time 0.5 telnet://localhost:8080 <<EOF
GET / HTTP/1.1
EOF
echo
# ---
printf "\e[32mShould return index before timeout:\e[0m\n"
curl --max-time 0.5 telnet://localhost:8080 2>/dev/null <<EOF | grep -v : # not interested in the header for now
GET / HTTP/1.1
Host: localhost:8080
User-Agent: Monkey D. Luffy
Accept: */*

EOF
echo
# ---
printf "\e[32mShould return index before timeout (multiple spaces in uri line, none or many in header):\e[0m\n"
curl --max-time 0.5 telnet://localhost:8080 2>/dev/null <<EOF | grep -v : # not interested in the header for now
GET    /    HTTP/1.1
Host:localhost:8080
User-Agent:    Monkey D. Luffy
Empty-field:
Accept:    */*

EOF
echo
# ---
printf "\e[32mShould return bad request:\e[0m\n"
curl --max-time 0.5 telnet://localhost:8080 2>/dev/null <<EOF | sed 's/nginx[^<]*/webserv\/0.1/g' | sed 's/\r//g' | grep -v : 
GET / HTTP/1.1
Host: localhost:8080
User-Agent: Monkey D. Luffy
Accept: */*
GET /folder/ HTTP/1.1
Host: localhost:8080
User-Agent: Monkey D. Luffy
Accept: */*

EOF
echo
# ---
printf "\e[32mShould return OK (body with right size):\e[0m\n"
curl --max-time 0.5 telnet://localhost:8080 2>/dev/null <<EOF | grep -v :
GET / HTTP/1.1
Host: localhost:8080
User-Agent: Monkey D. Luffy
Content-Length: 12
Accept: */*

summer_body
EOF
# there are 12 characters because the heredoc adds a final \n
echo
printf "\e[32mShould return HTTP Version not supported (error 505):\e[0m\n"
curl --max-time 0.5 telnet://localhost:8080 2>/dev/null <<EOF | sed 's/nginx[^<]*/webserv\/0.1/g' | sed 's/\r//g' | grep -v : 
GET / HTTP/3.0
Host: localhost:8080
User-Agent: Monkey D. Luffy
Accept: */*

EOF
echo
# ---
# printf "\e[32mShould return Bad request (Content-Length too short):\e[0m\n"
# curl --max-time 0.5 telnet://localhost:8080 2>/dev/null <<EOF
# GET / HTTP/1.1
# Host: localhost:8080
# User-Agent: Monkey D. Luffy
# Content-Length: 10
# Accept: */*
#
# summer_body
# EOF
# echo
# # ---
# printf "\e[32mShould return OK (Content-Length too long):\e[0m\n"
# curl --max-time 0.5 telnet://localhost:8080 2>/dev/null <<EOF
# GET /folder/ HTTP/1.1
# Host: localhost:8080
# User-Agent: Monkey D. Luffy
# Content-Length: 4242
# Accept: */*
#
# summer_body
# EOF
# ---
# TODO: 404 if missing mandatory headers
# printf "\e[32mShould return 404 before timeout:\e[0m "
# curl --max-time 0.5 telnet://localhost:8080 <<EOF
# GET / HTTP/1.1
#
# EOF
# TODO: try space line instead of empty line
