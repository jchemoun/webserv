SHELL				=	/bin/zsh
# Executable name
NAME1				=	webserv
# Compilation mode
WALL				=   yes
WEXTRA              =   yes
WERROR              =   yes
FSANITIZE           =   yes
DEBUG               =   yes
WSH                 =		yes
O2                  =   no
PTHREAD							=		no
CC                  :=  c++ -std=c++98
GEN                 :=  "Generation in mode"
ifeq ($(WALL), yes)
   CC              :=  $(CC) -Wall
   GEN             :=  $(GEN) all
endif
ifeq ($(WEXTRA), yes)
   CC              :=  $(CC) -Wextra
   GEN             :=  $(GEN) extra
endif
ifeq ($(WERROR), yes)
   CC              :=  $(CC) -Werror
   GEN             :=  $(GEN) error
endif
ifeq ($(FSANITIZE), yes)
   CC              :=  $(CC) -fsanitize=address
   GEN             :=  $(GEN) sanitize
endif
ifeq ($(DEBUG), yes)
   CC              :=  $(CC) -g3
   GEN             :=  $(GEN) debug
endif
ifeq ($(WSH),yes)
	CC             := $(CC)  -Wshadow -Wno-shadow
	GEN            := $(GEN) shadow
endif
ifeq ($(O2),yes)
   CC              :=  $(CC) -O2 -flto -march=native
   GEN             :=  $(GEN) O2
endif
ifeq ($(PTHREAD),yes)
   CC              :=  $(CC) -pthread
   GEN             :=  $(GEN) pthread
endif
ifeq ($(GEN), "Generation in mode")
   GEN             :=  $(GEN) no flags
endif

# Name

SRC_NAME			=	main.cpp			\
						Webserv.cpp			\
						Config.cpp			\
						Client.cpp			\

INCLUDE_NAME		=	Webserv.hpp			\
						Config.hpp			\
						Client.hpp			\

# Path
SRC_PATH			=	./src/
OBJ_PATH			=	./obj/
INCLUDE_PATH	=	./include/
# Name + Path
SRC					=	$(addprefix $(SRC_PATH),		$(SRC_NAME))
OBJ					=	$(patsubst  $(SRC_PATH)%.cpp,		$(OBJ_PATH)%.o, $(SRC))
INCLUDE			=	$(addprefix $(INCLUDE_PATH),	$(INCLUDE_NAME))

# Text format
_DEF                =   $'\033[0m
_END                =   $'\033[0m
_GRAS               =   $'\033[1m
_SOUL               =   $'\033[4m
_CLIG               =   $'\033[5m
_SURL               =   $'\033[7m
# Colors
_BLACK              =   $'\033[30m
_RED                =   $'\033[31m
_GREEN              =   $'\033[32m
_YELLOW             =   $'\033[33m
_BLUE               =   $'\033[34m
_PURPLE             =   $'\033[35m
_CYAN               =   $'\033[36m
_GREY               =   $'\033[37m
# Background
_IBLACK             =   $'\033[40m
_IRED               =   $'\033[41m
_IGREEN             =   $'\033[42m
_IYELLOW            =   $'\033[43m
_IBLUE              =   $'\033[44m
_IPURPLE            =   $'\033[45m
_ICYAN              =   $'\033[46m
_IGREY              =   $'\033[47m

.PHONY: all clean fclean re

all: $(NAME1)

$(NAME1): $(OBJ) $(INCLUDE)
	@echo "\n$(NAME1) : $(GEN)"
	@echo "\n$(_WHITE)====================================================$(_END)"
	@echo "$(_YELLOW)      COMPILING $(NAME1)$(_END)"
	@echo "$(_WHITE)====================================================$(_END)"
	@$(CC) -o $(NAME1) -I $(INCLUDE_PATH) $(OBJ)
	@echo "\n$(_WHITE)$(_BOLD)$@\t$(_END)$(_GREEN)[OK]\n$(_END)"

$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp $(INCLUDE)
	@mkdir -p $(OBJ_PATH)
	@$(CC) -I $(INCLUDE_PATH) -c $< -o $@
	@echo "$(_END)$(_GREEN)[OK]\t$(_UNDER)$(_YELLOW)\t"	\
		"COMPILE :$(_END)$(_BOLD)$(_WHITE)\t$<"

clean:
	@rm -rf $(OBJ_PATH) $(OBJ_LIB_PATH)
	@echo "$(_YELLOW)Remove :\t$(_RED)" $(OBJ_PATH) \\n \\t\\t $(OBJ_LIB_PATH)"$(_END)"

fclean: clean
	@rm -f $(NAME1)
	@echo "$(_YELLOW)Remove :\t$(_RED)" $(NAME1) \\n\\t\\t

re: fclean all
