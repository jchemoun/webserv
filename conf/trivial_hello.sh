curl localhost:8080
curl localhost:8080/
# existing folder with autoindex
curl localhost:8080/folder/ | sed 's/\s\+/ /g' | sed 's/[0-9]\+:[0-9]\+/hour:min/' | sed 's/ $//' | sed 's///g'
# empty subfolder
curl localhost:8080/folder/subfolder/| sed 's/nginx[^<]*/webserv\/0.1/g' | sed 's///g'
