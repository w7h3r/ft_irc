# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: alermi <alermi@student.42kocaeli.com.tr>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/03/23 16:47:46 by alermi            #+#    #+#              #
#    Updated: 2026/09/18 15:58:05 by alermi           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ============================================================
#                       COLOR CODES
# ============================================================
GREEN        = \033[1;32m
YELLOW       = \033[1;33m
RED          = \033[1;31m
CYAN         = \033[1;36m
RESET        = \033[0m

# ============================================================
#                       PROJECT CONFIG
# ============================================================
NAME        = ircserv
CXX         = c++
CXXFLAGS    = -Wall -Wextra -Werror -g -std=c++98 -I$(INC_DIR)
RM          = rm -rf

# ============================================================
#                       DEFAULT ARGUMENTS
# ============================================================
PORT        ?= 6667
PASS        ?= 1234

# ============================================================
#                       DIRECTORIES
# ============================================================
SRC_DIR     = src
INC_DIR     = inc
OBJ_DIR     = obj

# ============================================================
#                       SOURCE & HEADER FILES
# ============================================================
HEADERS     = $(INC_DIR)/Server/Server.hpp        	\
              $(INC_DIR)/Client/Client.hpp        	\
              $(INC_DIR)/Channel/Channel.hpp    	\
              $(INC_DIR)/Commands.hpp            	\
              $(INC_DIR)/utils.hpp

SRC         = $(SRC_DIR)/main.cpp                  	\
              $(SRC_DIR)/Server.cpp                 \
              $(SRC_DIR)/Client.cpp                 \
              $(SRC_DIR)/Channel.cpp                \
              $(SRC_DIR)/Commands.cpp               \
              $(SRC_DIR)/commands/cmdInvite.cpp     \
              $(SRC_DIR)/commands/cmdKick.cpp       \
              $(SRC_DIR)/commands/cmdJoin.cpp       \
              $(SRC_DIR)/commands/cmdPrivMsg.cpp    \
              $(SRC_DIR)/commands/cmdTopic.cpp      \
			  $(SRC_DIR)/commands/cmdMode.cpp		\
              $(SRC_DIR)/commands/cmdQuit.cpp		\
			  $(SRC_DIR)/utils.cpp

OBJS        = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))

# ============================================================
#                    DEFAULT TARGET (make)
# ============================================================
all: $(NAME)

# ============================================================
#                    BUILD EXECUTABLE
# ============================================================
$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)✔ Derleme tamamlandi: $(NAME)$(RESET)"

# ============================================================
#                OBJECT FILE COMPILATION RULE
# ============================================================
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	@echo "$(CYAN)Compiling: $<...$(RESET)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# ============================================================
#                        CLEANING
# ============================================================
clean:
	@$(RM) $(OBJ_DIR)
	@echo "$(YELLOW)󰃢 Objeler ve $(OBJ_DIR) dizini temizlendi.$(RESET)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(RED)󰃢 Executable silindi: $(NAME)$(RESET)"

re: fclean all

# ============================================================
#                        RUN COMMANDS
# ============================================================
run: all
	@echo "$(GREEN)>>> Running program on port $(PORT) with password '$(PASS)'...$(RESET)\n"
	@./$(NAME) $(PORT) $(PASS)

runv: all
	@echo "$(YELLOW)>>> Running program with valgrind on port $(PORT)...$(RESET)\n"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes ./$(NAME) $(PORT) $(PASS)

# ============================================================
#                        HELP COMMAND
# ============================================================
help:
	@echo ""
	@echo "$(CYAN)====================  MAKE HELP  ====================$(RESET)"
	@echo " $(GREEN)make$(RESET)         → Programi derle"
	@echo " $(YELLOW)make clean$(RESET)   → Objeleri sil"
	@echo " $(RED)make fclean$(RESET)  → Objeleri ve executable'i sil"
	@echo " $(CYAN)make re$(RESET)      → Bastan derle"
	@echo " $(GREEN)make run$(RESET)     → Standart calistir (Port: 6667, Pass: 1234)"
	@echo " $(YELLOW)make runv$(RESET)    → Valgrind ile calistir"
	@echo ""

.PHONY: all clean fclean re help run runv
