CC = gcc
CFLAGS = -Wpedantic -std=gnu99

serial_cgol: serial_cgol.o
	$(CC) $(CFLAGS) -o serial_cgol serial_cgol.o

serial_cgol.o: serial_cgol.c
	$(CC) $(CFLAGS) -c serial_cgol.c

clean:
    rm -i *.o serial_cgol
