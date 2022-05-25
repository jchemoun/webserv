curl localhost:8080 | grep Welcome
curl -X POST localhost:8080 | grep Welcome

curl localhost:8080/first/ | grep "Not Allowed"
curl -i -X PUT localhost:8080/first/new-file.txt -T README.md | grep 201
diff README.md www/site1/first/new-file.txt
curl -i -X PUT localhost:8080/first/new-file.txt -T README.md | grep 204
diff README.md www/site1/first/new-file.txt
curl -i -X DELETE localhost:8080/first/new-file.txt | grep 204
curl -i -X DELETE localhost:8080/first/new-file.txt | grep 404

curl localhost:8080/site1/first | grep 301
curl -L localhost:8080/site1/first | grep site1
curl localhost:8080/first/hello.txt | grep Not allowed
curl localhost:8080/site1/first/hello.txt | grep "hello first"

# Wildcard locations
curl localhost:8080/42_logo.png | grep "Not Allowed"
curl -X POST localhost:8080/42_logo.png -o /tmp/image.png; \
  diff <(sha256sum </tmp/image.png) <(sha256sum <html/42_logo.png);
rm /tmp/image.png
curl -X PUT localhost:8080/put_not_allowed.png -d"data" | grep "Not Allowed"

# rewrite prefix
curl localhost:8080/remove/this/prefix/site1/ | grep site1
curl localhost:8080/rewrite/to/site2/ | grep site2
