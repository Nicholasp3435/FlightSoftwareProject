CC = gcc
CFLAGS = -Wall -pedantic

all: encode.out decode.out

encode.out: encode.c encoding_functions.o common_functions.o
	$(CC) $(CFLAGS) -o encode.out encode.c encoding_functions.o common_functions.o -lm

decode.out: decode.c decoding_functions.o common_functions.o
	$(CC) $(CFLAGS) -o decode.out decode.c decoding_functions.o common_functions.o -lm

encoding_functions.o: encoding_functions.c encoding_functions.h common_functions.h
	$(CC) $(CFLAGS) -c encoding_functions.c

decoding_functions.o: decoding_functions.c decoding_functions.h common_functions.h
	$(CC) $(CFLAGS) -c decoding_functions.c

common_functions.o: common_functions.c common_functions.h
	$(CC) $(CFLAGS) -c common_functions.c

test: test.out
	./test.out

test.out: test.o encoding_functions.o decoding_functions.o common_functions.o
	$(CC) $(CFLAGS) -o test.out test.o encoding_functions.o decoding_functions.o common_functions.o -lm

test.o: test.c
	$(CC) $(CFLAGS) -c test.c

clean:
	rm -f *.out *.o