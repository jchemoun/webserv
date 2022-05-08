curl localhost:8080
curl localhost:8080/
# existing folder/ with autoindex
curl localhost:8080/folder/ | sed 's/\s\+/ /g' | sed 's/[0-9]\+:[0-9]\+/hour:min/' | sed 's/ $//'
# empty subfolder/
curl localhost:8080/folder/subfolder/| sed 's/nginx[^<]*/webserv\/0.1/g' | sed 's///g'
# TODO: existing folder with autoindex (return 301)
# curl localhost:8080/folder | sed 's/\s\+/ /g' | sed 's/[0-9]\+:[0-9]\+/hour:min/' | sed 's/ $//'

# telnet
printf "\e[33mShould just timeout (no empty line sent):\e[0m "
curl --max-time 0.5 telnet://localhost:8080 <<EOF
GET / HTTP/1.1
EOF
echo
# ---
# printf "\e[32mShould return index before timeout:\e[0m "
# curl --max-time 0.5 telnet://localhost:8080 2>/dev/null <<EOF | grep -v : # not interested in the header for now
# GET / HTTP/1.1
# Host: localhost:8080
# User-Agent: Monkey D. Luffy
# Accept: */*
#
# EOF
# echo
# ---
# printf "\e[32mShould return index, then folder's autoindex before timeout:\e[0m "
# curl --max-time 0.5 telnet://localhost:8080 2>/dev/null <<EOF | grep -v : # not interested in the header for now
# GET / HTTP/1.1
# Host: localhost:8080
# User-Agent: Monkey D. Luffy
# Accept: */*
#
# GET /folder/ HTTP/1.1
# Host: localhost:8080
# User-Agent: Monkey D. Luffy
# Accept: */*
#
# EOF
# echo
# # ---
# printf "\e[32mSame \e[0m(Should return index, then folder's autoindex before timeout): "
# curl --max-time 0.5 telnet://localhost:8080 2>/dev/null <<EOF | grep -v : # not interested in the header for now
# GET / HTTP/1.1
# Host: localhost:8080
# User-Agent: Monkey D. Luffy
# Accept: */*
# 
# 
# 
# GET /folder/ HTTP/1.1
# Host: localhost:8080
# User-Agent: Monkey D. Luffy
# Accept: */*
# 
# EOF
# # ---
# printf "\e[32mShould return bad request:\e[0m "
# curl --max-time 0.5 telnet://localhost:8080 2>/dev/null <<EOF | grep -v : # not interested in the header for now
# GET / HTTP/1.1
# Host: localhost:8080
# User-Agent: Monkey D. Luffy
# Accept: */*
# GET /folder/ HTTP/1.1
# Host: localhost:8080
# User-Agent: Monkey D. Luffy
# Accept: */*
# 
# EOF
# ---
# TODO: 404 if missing mandatory headers
# printf "\e[32mShould return 404 before timeout:\e[0m "
# curl --max-time 0.5 telnet://localhost:8080 <<EOF
# GET / HTTP/1.1
#
# EOF
