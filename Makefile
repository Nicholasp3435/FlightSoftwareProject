CC = gcc
CFLAGS = -Wall

all: encode.out decode.out

# hey btw future me: -lm means Link Math (L M!) and the stb libs needs it

encode.out: encode.c
	$(CC) $(CFLAGS) -o encode.out encode.c -lm

decode.out: decode.c
	$(CC) $(CFLAGS) -o decode.out decode.c -lm

clean:
	rm -f encode.out decode.out
