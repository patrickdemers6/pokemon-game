
all: terrain

terrain: terrain.c
	gcc -Wall -Werror -g terrain.c -o terrain

clean:
	rm -f terrain *~ core
