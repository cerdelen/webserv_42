SRC			=	main.cpp \
				src/config.cpp \
				src/misc.cpp \
				src/server.cpp \
				src/serverFillRequest.cpp \
				src/serverHandlers.cpp \
				src/serverLoop.cpp \
				src/serverValidations.cpp\
				src/empty.cpp \
				src/cgi.cpp
			
EXEC_NAME	=	exec

PINK    := \033[0;35m
PURPLE    := \033[0;34m
RESET    := \033[0m

# CFLAGS = -Wall -Wextra -Werror -std=c++98
OBJ  := $(SRC:.cpp =.o)

# %.o: %.c 
# 	@$(CC) $(CFLAGS) -c $< -o $@

all: $(EXEC_NAME)


$(EXEC_NAME): $(OBJ)
	c++ $(CFLAGS) $(OBJ) -o $(EXEC_NAME) -D DEBUG=0

fclean:
	rm -rf $(EXEC_NAME)

re: fclean all

run: all
	./exec config_files/test.conf 

run_debug:
	c++ $(SRC) -o $(EXEC_NAME) -D DEBUG=1
	./exec config_files/test.conf

git:
	git add .gitignore
	git add .
	@read -p "Enter the commit message: " halp; \
	git commit -m "$$halp"
	git push
	@echo "$(PURPLE)All addeded and commiteded and pusheded!$(RESET)"