curl localhost:8080
curl localhost:8080/first/
curl localhost:8080/second/
curl localhost:8080/site1/
curl localhost:8080/first | sed 's/nginx[^<]*/webserv\/0.1/g' # Not found
curl localhost:8080/second | sed 's/nginx[^<]*/webserv\/0.1/g' # Moved Permanently
# curl -i localhost:8080/second | grep Location
curl -L localhost:8080/second
curl localhost:8080/first/hello.txt
curl localhost:8080/second/hello.txt
