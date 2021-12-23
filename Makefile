all: cgol

cgol.o: cgol.c
	gcc -ansi -Wall -c cgol.c

flush.o: flush.c
	gcc -ansi -Wall -c flush.c

cgol: cgol.o flush.o
	gcc -ansi -Wall -o cgol cgol.o flush.o

clean:
	rm -i *.o cgol
