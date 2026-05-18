# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: alermi <alermi@student.42kocaeli.com.tr>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/03/23 16:47:46 by alermi            #+#    #+#              #
#    Updated: 2026/05/18 11:09:58 by alermi           ###   ########.fr        #
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
CXXFLAGS    = -Wall -Wextra -Werror -g -std=c++98
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
# Proje ilerledikçe kendi oluşturduğun header'ları buraya ekle
HEADERS     = $(INC_DIR)/Server.hpp \
              $(INC_DIR)/Client.hpp \
              $(INC_DIR)/Channel.hpp

# Proje ilerledikçe kendi oluşturduğun cpp'leri buraya ekle
SRC         = $(SRC_DIR)/main.cpp \
              $(SRC_DIR)/Server.cpp \
              $(SRC_DIR)/Client.cpp \
              $(SRC_DIR)/Channel.cpp

OBJS        = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))

# ============================================================
#                    DEFAULT TARGET (make)
# ============================================================
all: $(NAME)

# ============================================================
#                    BUILD EXECUTABLE
# ============================================================
$(NAME): $(OBJ_DIR) $(OBJS)
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)✔ Derleme tamamlandı: $(NAME)$(RESET)"

# ============================================================
#                OBJECT FILE COMPILATION RULE
# ============================================================
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@echo "$(CYAN)Compiling: $<...$(RESET)"
	@$(CXX) $(CXXFLAGS) -I $(INC_DIR) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

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
	@echo " $(GREEN)make$(RESET)         → Programı derle"
	@echo " $(YELLOW)make clean$(RESET)   → Objeleri sil"
	@echo " $(RED)make fclean$(RESET)  → Objeleri ve executable'ı sil"
	@echo " $(CYAN)make re$(RESET)      → Baştan derle"
	@echo " $(GREEN)make run$(RESET)     → Standart çalıştır (Port: 6667, Pass: 1234)"
	@echo " $(YELLOW)make runv$(RESET)    → Valgrind ile çalıştır (File descriptor takibi aktif)"
	@echo ""
	@echo " $(CYAN)💡 İpucu: Özel port ve şifre ile başlatmak için:$(RESET)"
	@echo "    make run PORT=8080 PASS=secret"
	@echo "$(CYAN)====================================================$(RESET)"
	@echo ""

.PHONY: all clean fclean re help run runv
