NAME = module_spy
NAME_LIB = libft.a
CC = gcc
CFLAGS = -Wall -Wextra -Werror
SRC = client.c \
	  daemon.c \
	  main.c
OBJECTS = $(patsubst %.c, %.o, $(SRC))
SRC_LIB = $(filter-out main.c,$(SRC))
OBJECTS_LIB = $(patsubst %.c, %.o, $(SRC_LIB))
HEADER = ./include/
 
all: $(NAME)

$(NAME_LIB):
	ar rc $(NAME_LIB) $(OBJECTS_LIB)
	ranlib $(NAME_LIB)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c  $< -I $(HEADER)

$(NAME): $(OBJECTS) $(NAME_LIB)
	$(CC) $(CFLAGS) main.o -L. -lft -I $(HEADER) -o $@

clean:
	@/bin/rm -rf $(OBJECTS)

fclean: clean
	@/bin/rm -rf $(NAME) $(NAME_LIB)

re: fclean all
