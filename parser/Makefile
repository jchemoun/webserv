NAME = parser

#  ================================ Compiler ================================  #

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98
CXXFLAGS += -g
# CXXFLAGS += -fsanitize=address

#  ================================ Sources =================================  #

HEADERS = token.hpp lexer.hpp Parser.hpp Config.hpp
SRC = token.cpp lexer.cpp Parser.cpp Config.cpp

#  ================================= Rules ==================================  #

all: $(NAME)

$(NAME): main.cpp $(HEADERS) $(SRC)
	$(CXX) $(CXXFLAGS) main.cpp $(SRC) -o $@

clean:

fclean: clean
	rm -f $(NAME)

re: fclean all
