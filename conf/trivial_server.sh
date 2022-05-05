curl localhost
curl localhost/missing_page.html | sed 's/nginx[^<]*/webserv\/0.1/g' | sed 's///g'
