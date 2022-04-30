#! /usr/bin/env bash

# ================================== Usage =================================== #

if tty &>/dev/null; then
printf "\n\e[34mHello, first you need to make sure that you mounted your sources in docker:\n\
     \e[33m\$\e[32m docker\e[0m run -it -p 8080:80 \e[32m-v \$(pwd):${MOUNTING_POINT}\e[0m webserv\n\n" >&2

printf "\e[34mTo run nginx with a specific config file, do:\n\
     \e[33m\$\e[32m sudo cp\e[0m -R html www /usr/share/nginx/\n\
     \e[33m\$\e[32m sudo cp\e[0m conf/my_confile.conf /etc/nginx/sites-enabled/\n\
     \e[33m\$\e[32m sudo nginx\e[0m\n\n" >&2
printf "\e[34mYou can then signal to nginx with:\n\
     \e[33m\$\e[32m sudo nginx\e[0m -s <reload|quit>\n\n" >&2
fi

# ============================== Util functions ============================== #

# set -x
wait_server_up() {
  # while ! nc -z localhost 80; do
    sleep 0.1
  # done
}
run_on_nginx() {
  out="failed_tests/nginx_test"; mkdir -p failed_tests; rm -f $out ${out}_stderr
  if [ ! -f conf/$1.conf ]; then printf "\e[1;31mconf/$1.conf file not found\e[0m\n"; exit 1; fi
  sudo cp conf/$1.conf /etc/nginx/sites-enabled/
  sudo nginx; wait_server_up
  bash conf/$1.sh > $out 2> ${out}_stderr
  sudo nginx -s quit
}
run_on_webserv() {
  out="failed_tests/webserv_test"; mkdir -p failed_tests; rm -f $out ${out}_stderr
  if [ ! -f conf/$1.conf ]; then printf "\e[1;31mconf/$1.conf file not found\e[0m\n"; exit 1; fi
  sudo cp conf/$1.conf /etc/nginx/sites-enabled/
  sudo nginx; wait_server_up
  bash conf/$1.sh > $out 2> ${out}_stderr
  echo "⚠️  webserv not implemented yet!" >> $out
  sudo nginx -s quit
}

# ================================== Rules =================================== #

if [ $# -eq 0 ]; then
  exec /usr/bin/env zsh

elif [ "$1" = "make" ]; then
  exec $@

elif [ "$1" = "compiledb" ]; then
  exec compiledb make

elif [ "$1" = "webserv" ] && [ $# -eq 2]; then
  make
  ./webserv $2

elif [ "$1" = "nginx" ]; then
  sudo rm /etc/nginx/sites-enabled/default
  sudo cp -R html www /usr/share/nginx/ # nginx's prefix is /usr/share/nginx and the default root within it is html
  if [ -n "$2" ]; then
    sudo cp conf/$2.conf /etc/nginx/sites-enabled/
  fi
  sudo nginx
  exec /usr/bin/env zsh

elif [ "$1" = "test" ] && [ "$2" = "nginx" ] && [ $# -eq 3 ]; then
  sudo rm /etc/nginx/sites-enabled/default
  sudo cp -R html www /usr/share/nginx/
  run_on_nginx $3

elif [ "$1" = "test" ] && [ "$2" = "webserv" ] && [ $# -eq 3 ]; then
  make --silent
  sudo rm /etc/nginx/sites-enabled/default # remove when webserv ok
  sudo cp -R html www /usr/share/nginx/    # idem
  run_on_webserv $3

else
  printf "\e[1;31mUnknown instruction:\e[0m $1 $2 $3...\n"
fi
