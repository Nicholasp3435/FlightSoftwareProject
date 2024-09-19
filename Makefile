CC = gcc
CFLAGS = -Wall

all: encode.out decode.out

# hey btw future me: -lm means Link Math (L M!) and the stb libs needs it

encode.out: encode.c encoding_functions.o
	$(CC) $(CFLAGS) -o encode.out encode.c encoding_functions.c -lm

decode.out: decode.c decoding_functions.o
	$(CC) $(CFLAGS) -o decode.out decode.c decoding_functions.c -lm

clean:
	rm -f *.out *.o
