FLAGS = -Wall -Werror -fsanitize=address -std=c11

all: third

third: third.c
	gcc $(FLAGS) third.c -o third
	
clean:
	rm -f third
