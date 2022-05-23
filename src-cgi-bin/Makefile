all: env_cpp.cgi

env_cpp.cgi: env_cgi.cpp
	c++ -std=c++98 -Wall -Werror -Wextra $^ -o $@
	cp $@ ../html/cgi-bin/

fclean:
	rm -f env_cpp.cgi
