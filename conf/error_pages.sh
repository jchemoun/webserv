curl -L localhost:8080/fdsgfds # 404 Not Found
curl -i localhost:8080/fdsgfds | grep Content-Type # text/html
curl -L localhost:8080/folder/ # 403 Forbidden
{ sleep 0.1; echo "ABC / HTTP/1.1"; sleep 0.1; echo; sleep 0.1 } | telnet localhost 8080 | grep -v : # Bad request

curl -i localhost:8081 -d "hellow" | grep -v : # 413 Request Entity Too Large

# Note: in nginx, if error_page does not start with a '/', it will do a redirection
# curl localhost:8080/fdsgfds # 302 Found
# curl -L localhost:8080/fdsgfds # 404 Not Found
