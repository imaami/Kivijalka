all: test wsserver

test: test-path

test-path: path.o test-path.o
	$(CC) $(CFLAGS) -std=gnu11 path.o test-path.o -o test-path

path.o: path.c
	$(CC) $(CFLAGS) -std=gnu11 -c path.c -o path.o

test-path.o: test-path.c
	$(CC) $(CFLAGS) -std=gnu11 -c test-path.c -o test-path.o

wsserver:
	./build.sh

clean: clean-test clean-wsserver

clean-test: clean-test-path

clean-test-path:
	rm -f test-path *path.o

clean-wsserver:
	rm -f *.o
	rm -f wsserver moc_*
