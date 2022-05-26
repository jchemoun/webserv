curl localhost
curl localhost/missing_page.html | sed 's/nginx[^<]*/webserv\/1.0/g'

# ================================= Big send ================================= #

# very small image
curl localhost/42_logo.png -o tmp.png
[ "$(<html/42_logo.png md5sum)" = "$(<tmp.png md5sum)" ] && echo true || echo files differ
rm tmp.png

if [! html/galaxy.jpg ]; then
  curl -o html/galaxy.jpg "https://upload.wikimedia.org/wikipedia/commons/c/c3/NGC_4414_(NASA-med).jpg"
fi

# 9MB image
curl localhost/galaxy.jpg -o tmp.jpg
[ "$(<html/galaxy.jpg md5sum)" = "$(<tmp.jpg md5sum)" ] && echo true || echo files differ
rm tmp.jpg

# ============================= Chunked encoding ============================= #

curl -X GET -H "Transfer-Encoding: chunked" -d "hello" localhost
# POST / HTTP/1.1
# Host: localhost:8080
# User-Agent: curl/7.81.0
# Accept: */*
# Transfer-Encoding: chunked
# Content-Type: application/x-www-form-urlencoded
# 
# 5
# hello
# 0
# 
# ✋

curl -X GET -H "Transfer-Encoding: chunked" -d @README.md localhost
