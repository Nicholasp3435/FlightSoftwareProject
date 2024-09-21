CC = gcc
CFLAGS = -Wall -pedantic

all: encode.out decode.out

encode.out: encode.o encoding_functions.o
	$(CC) $(CFLAGS) -o encode.out encode.o encoding_functions.o -lm

decode.out: decode.o decoding_functions.o
	$(CC) $(CFLAGS) -o decode.out decode.o decoding_functions.o -lm

encoding_functions.o: encoding_functions.c
	$(CC) $(CFLAGS) -c encoding_functions.c

decoding_functions.o: decoding_functions.c
	$(CC) $(CFLAGS) -c decoding_functions.c

encode.o: encode.c
	$(CC) $(CFLAGS) -c encode.c

decode.o: decode.c
	$(CC) $(CFLAGS) -c decode.c

test: test.out
	./test.out

test.out: test.o encoding_functions.o decoding_functions.o
	$(CC) $(CFLAGS) -o test.out test.o encoding_functions.o decoding_functions.o -lm

test.o: test.c
	$(CC) $(CFLAGS) -c test.c


clean:
	rm -f *.out *.o