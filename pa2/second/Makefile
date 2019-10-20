FLAGS = -ggdb -Wall -Werror -fsanitize=address -std=c11

all: second

second: second.c
	gcc $(FLAGS) second.c -o second

clean:
	rm -f second
