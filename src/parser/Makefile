NAME = parser

#  ================================ Compiler ================================  #

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98
CXXFLAGS += -g
CXXFLAGS += -fsanitize=address

#  ================================ Sources =================================  #

HEADERS = Token.hpp Lexer.hpp Parser.hpp Config.hpp
SRC = Token.cpp Lexer.cpp Parser.cpp Config.cpp ../utils/file.cpp ../utils/utils.cpp ../utils/http_response_codes.cpp

#  ================================= Rules ==================================  #

all: $(NAME)

$(NAME): main.cpp $(HEADERS) $(SRC)
	$(CXX) $(CXXFLAGS) main.cpp $(SRC) -o $@

clean:

fclean: clean
	rm -f $(NAME)

re: fclean all
