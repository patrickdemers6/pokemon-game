
binary: terrain.o main.o travel.o util.o npc.o heap.o
	gcc terrain.o travel.o main.o util.o npc.o heap.o -o binary

terrain.o: terrain.c terrain.h travel.h util.h npc.h
	gcc -Wall -Werror -g -c terrain.c -o terrain.o

main.o: main.c
	gcc -Wall -Werror -g -c main.c -o main.o

travel.o: travel.c travel.h travel.h util.h
	gcc -Wall -Werror -g -c travel.c -o travel.o

util.o: util.c travel.h
	gcc -Wall -Werror -g -c util.c -o util.o

npc.o: npc.c npc.h
	gcc -Wall -Werror -g -c npc.c -o npc.o

heap.o: heap.c heap.h
	gcc -Wall -Werror -g -c heap.c -o heap.o

clean:
	rm -f binary *~ core *.o
