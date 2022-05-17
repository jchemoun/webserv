curl localhost
curl localhost/missing_page.html | sed 's/nginx[^<]*/webserv\/0.1/g'

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
