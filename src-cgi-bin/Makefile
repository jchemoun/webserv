all: env_cpp.cgi

env_cpp.cgi: env_cgi.cpp
	@printf "\e[35m✓ Building $@...\e[0m\n";
	c++ -std=c++98 -Wall -Werror -Wextra $^ -o $@
	cp $@ ../html/cgi-bin/

clean:

fclean:
	@rm -f env_cpp.cgi ../html/cgi-bin/env_cpp.cgi

re: fclean all
