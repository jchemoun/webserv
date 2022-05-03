#! /usr/bin/env zsh

# ================================== Usage =================================== #

# printf "\n\e[34mHello, first you need to make sure that you mounted your sources in docker:\n\
#      \e[33m\$\e[32m docker\e[0m run -it -p 8080:80 \e[32m-v \$(pwd):${MOUNTING_POINT}\e[0m webserv\n\n" >&2

nginx_conf_usage() {
if tty &>/dev/null; then
printf "\n\e[34mTo run nginx with a specific config file, do:\n\
     \e[33m\$\e[32m sudo cp\e[0m -R html www /usr/share/nginx/\n\
     \e[33m\$\e[32m sudo cp\e[0m conf/my_confile.conf /etc/nginx/sites-enabled/\n\
     \e[33m\$\e[32m sudo nginx -s reload\e[0m\n\n" >&2
fi
}
nginx_signal_usage() {
if tty &>/dev/null; then
  printf "\n\e[34mYou can always signal to nginx (if running) with:\n\
     \e[33m\$\e[32m sudo nginx\e[0m -s <reload|quit>\n\n" >&2
fi
}

# ============================== Util functions ============================== #

# set -x
wait_server_up() {
  # while ! nc -z localhost 80; do
    sleep 0.1
  # done
}
run_on_nginx() {
  conf="$1"
  tests="${conf:r}.sh"
  out="failed_tests/nginx_test"; mkdir -p failed_tests; rm -f $out ${out}_stderr
  if [ ! -f "$conf" ]; then printf "\e[1;31m$conf file not found\e[0m\n"; exit 1; fi
  sudo cp "$conf" /etc/nginx/sites-enabled/
  sudo nginx; wait_server_up
  bash "$tests" > $out 2> ${out}_stderr
  sudo nginx -s quit
}
run_on_webserv() {
  conf="$1"
  tests="${conf:r}.sh"
  out="failed_tests/webserv_test"; mkdir -p failed_tests; rm -f $out ${out}_stderr
  if [ ! -f "$conf" ]; then printf "\e[1;31m$conf file not found\e[0m\n"; exit 1; fi
  ./webserv "$conf" &
  pid=$!
  wait_server_up
  bash "$tests" > $out 2> ${out}_stderr
  kill $pid
}

# ================================== Rules =================================== #

if [ $# -eq 0 ]; then
  exec /usr/bin/env zsh

elif [ "$1" = "make" ]; then
  exec $@

# elif [ "$1" = "compiledb" ]; then
#   exec compiledb make

elif [ "$1" = "webserv" ] ; then
  if [ -z "$2" ]; then
    printf "\e[33mWarning: you did not provide any configuration file.\e[0m\n"
  fi
  make
  printf "\e[32mLaunching ./webserv $2\e[0m\n"
  nohup ./webserv $2 &> ../webserv.log &
  exec /usr/bin/env zsh

elif [ "$1" = "nginx" ]; then
  echo
  sudo rm /etc/nginx/sites-enabled/default
  sudo cp -R html www /usr/share/nginx/ # nginx's prefix is /usr/share/nginx and the default root within it is html
  if [ -n "$2" ]; then
    sudo cp $2 /etc/nginx/sites-enabled/ && printf "\e[32m$2 if loaded.\e[0m\n" || printf "\e[31mCould not load $2.\e[0m\n"
  else
    printf "\e[33mNginx has no configuration yet loaded.\e[0m\n"
    nginx_conf_usage
  fi
  echo "Launching nginx..." && sudo nginx && printf "\e[32mnginx is running!\e[0m\n"
  nginx_signal_usage
  exec /usr/bin/env zsh

elif [ "$1" = "test" ] && [ "$2" = "nginx" ] && [ $# -eq 3 ]; then
  sudo rm /etc/nginx/sites-enabled/default
  sudo cp -R html www /usr/share/nginx/
  run_on_nginx $3

elif [ "$1" = "test" ] && [ "$2" = "webserv" ] && [ $# -eq 3 ]; then
  make --silent
  # sudo rm /etc/nginx/sites-enabled/default # remove when webserv ok
  # sudo cp -R html www /usr/share/nginx/    # idem
  run_on_webserv $3

else
  printf "\e[1;31mUnknown instruction:\e[0m $1 $2 $3...\n"
fi
