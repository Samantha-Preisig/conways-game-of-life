CC = gcc
CFLAGS = -Wpedantic -std=gnu99

all: gol_serial gol_data gol_task

gol_serial: gol_serial.o
	$(CC) $(CFLAGS) -o gol_serial gol_serial.o

gol_serial.o: gol_serial.c
	$(CC) $(CFLAGS) -c gol_serial.c

gol_data: gol_data.o
	$(CC) $(CFLAGS) -o gol_data gol_data.o -lpthread

gol_data.o: gol_data.c
	$(CC) $(CFLAGS) -c gol_data.c

gol_task: gol_task.o
	$(CC) $(CFLAGS) -o gol_task gol_task.o -lpthread

gol_task.o: gol_task.c
	$(CC) $(CFLAGS) -c gol_task.c

clean:
	rm -i *.o gol_serial gol_data gol_task
