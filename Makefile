NAME := Program

SRC_DIR := srcs/
OBJ_DIR := .obj/
HEADERS_DIR := include/

SRC := $(SRC_DIR)main.cpp \
	   $(SRC_DIR)portScanner.cpp \

OBJ := $(SRC:$(SRC_DIR)%.cpp=$(OBJ_DIR)%.o)
DEPS := $(OBJ:.o=.d)

CC := c++
CFLAGS := -Wall -Wextra -Werror -std=c++11 -pthread
CPPFLAGS := -MMD -MP
INCLUDES := -I$(HEADERS_DIR)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@
	@echo "\033[32mCompilation complete: $@\033[0m"

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)
	@echo "\033[33mCleaned object files\033[0m"

fclean: clean
	@rm -f $(NAME)
	@echo "\033[33mRemoved executable\033[0m"

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
