curl localhost:8080 | grep Welcome
curl -X POST localhost:8080 | grep Welcome

curl localhost:8080/first/ | grep Not Allowed
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
