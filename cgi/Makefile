all: env_cgi

env_cgi: env_cgi.cpp
	c++ -std=c++98 -Wall -Werror -Wextra $^ -o $@
	cp $@ ../html/cgi/

fclean:
	rm -f env_cgi
