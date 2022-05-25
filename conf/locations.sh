curl localhost:8080
curl localhost:8080/first/
curl localhost:8080/second/
curl localhost:8080/site1/
curl localhost:8080/first | sed 's/nginx[^<]*/webserv\/1.0/g' # Not found
curl localhost:8080/second | sed 's/nginx[^<]*/webserv\/1.0/g' # Moved Permanently
# curl -i localhost:8080/second | grep Location
curl -L localhost:8080/second
curl localhost:8080/first/hello.txt
curl localhost:8080/second/hello.txt

# Redirections
curl localhost:8080/redir | sed 's/nginx[^<]*/webserv\/1.0/g'
curl -L localhost:8080/redir

# Max body size
#
#
curl -X GET localhost:8080 -d "123" # OK
curl -X GET localhost:8080 -d "1234" | sed 's/nginx[^<]*/webserv\/1.0/g'  # 413
curl -X GET localhost:8080/first/ -d "1" # OK site1
curl -X GET localhost:8080/first/ -d "12" | sed 's/nginx[^<]*/webserv\/1.0/g'  # 413
curl -X GET localhost:8080/site1/first/ -d "12"
curl -X GET localhost:8080/site1/first/ -d "123" | sed 's/nginx[^<]*/webserv\/1.0/g' 
curl -X GET localhost:8080/second/ -d "123"
curl -X GET localhost:8080/second/ -d "1234" | sed 's/nginx[^<]*/webserv\/1.0/g' 
curl -X GET localhost:8080/site1/ -d "123"
# We differ from nginx, our server client_max_body_sizes cannot be exceeded by the location's
# curl -X GET localhost:8080/site1/ -d "1234"
curl -X GET localhost:8080/site1/ -d "12345" | sed 's/nginx[^<]*/webserv\/1.0/g' 
