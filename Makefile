SRC = main.cpp src/glad.cpp

OBJ = ${SRC:.cpp=.o}

INCLUDE = -I./include/ -I./external/glm-master/

NAME = game

CC = c++

CFLAGS = -Wall -Wextra -Werror -Wno-unused-function -g

RM = rm -f

%.o: %.cpp
	$(CC) -I./include/ $(CFLAGS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

clean:
		$(RM) -r $(OBJ)

fclean:	clean
		$(RM) $(NAME)

re:		fclean all

.PHONY: all clean fclean re
