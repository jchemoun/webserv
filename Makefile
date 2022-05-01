NAME              = webserv

#  ========================== Compilation Options ===========================  #

SHELL              = /bin/zsh

# Compilation mode
WALL               = yes
WEXTRA             = yes
WERROR             = yes
FSANITIZE          = yes
DEBUG              = yes
WSH                = yes
O2                 = no
PTHREAD            = no
CC                 :=  c++ -std=c++98
GEN                :=  "Generation in mode"
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

#  ====================== Sources / Includes / Objects ======================  #

# Source Names
SRC_NAME = main.cpp \
					 $(addprefix server/, Webserv.cpp Config.cpp Client.cpp Request.cpp Response.cpp) \
					 $(addprefix parser/, Config.cpp Lexer.cpp Parser.cpp Token.cpp) \

# Path
SRC_PATH      =  ./src/
OBJ_PATH      =  ./obj/

# Name + Path
SRC          =  $(addprefix $(SRC_PATH),    $(SRC_NAME))
OBJ          =  $(patsubst  $(SRC_PATH)%.cpp,    $(OBJ_PATH)%.o, $(SRC))

# Includes
INCLUDE_FLAGS = -Iinclude -Isrc/server -Isrc/parser
# NOTE: include files for compilation dependancy checks only
INCLUDE_FILES = $(shell find -name "*.hpp")

#  =============================== Formatting ===============================  #

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

#  ================================= Rules ==================================  #

all: $(NAME)

$(NAME): $(OBJ)
	@echo "\n$(NAME) : $(GEN)"
	@echo "\n$(_WHITE)====================================================$(_END)"
	@echo "$(_YELLOW)      COMPILING $(NAME)$(_END)"
	@echo "$(_WHITE)====================================================$(_END)"
	@$(CC) -o $(NAME) $(INCLUDE_FLAGS) $(OBJ)
	@echo "\n$(_WHITE)$(_BOLD)$@\t$(_END)$(_GREEN)[OK]\n$(_END)"

$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp $(INCLUDE_FILES)
	@mkdir -p $(shell dirname $@)
	@$(CC) $(INCLUDE_FLAGS) -c $< -o $@
	@echo "$(_END)$(_GREEN)[OK]\t$(_UNDER)$(_YELLOW)\t"	\
		"COMPILE :$(_END)$(_BOLD)$(_WHITE)\t$<"

clean:
	@rm -rf $(OBJ_PATH)
	@echo "$(_YELLOW)Remove :\t$(_RED)" $(OBJ_PATH) \\n "$(_END)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(_YELLOW)Remove :\t$(_RED)" $(NAME) \\n\\t\\t"$(_END)"

re: fclean all

.PHONY: all clean fclean re
