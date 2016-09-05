CC = gcc
CFLAGS = -Wall -lm -std=c99 -pedantic
OPT =

main: ./src/main.c ./src/bmp.c ./src/kmeans.c

	$(CC) $(CFLAGS) -o main ./src/main.c $(OPT)
