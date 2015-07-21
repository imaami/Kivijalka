all: test wsserver

test: test-path test-watcher test-img

test-buf: buf.o test-buf.o
	$(CC) $(CFLAGS) -std=gnu11 buf.o test-buf.o -o test-buf

test-path: path.o test-path.o
	$(CC) $(CFLAGS) -std=gnu11 path.o test-path.o -o test-path

test-watcher: path.o watcher.o test-watcher.o
	$(CC) $(CFLAGS) -std=gnu11 path.o watcher.o test-watcher.o -lpthread -o test-watcher

test-img: img.o test-img.o read-file.o
	$(CC) $(CFLAGS) -std=gnu11 img.o test-img.o read-file.o -o test-img

test-task: task.o test-task.o
	$(CC) $(CFLAGS) -std=gnu11 task.o test-task.o -lpthread -o test-task

test-file: buf.o file.o test-file.o
	$(CC) $(CFLAGS) -std=gnu11 buf.o file.o test-file.o -o test-file

test-display: display.o test-display.o
	$(CC) $(CFLAGS) -std=gnu11 display.o test-display.o -o test-display

test-banner: read-file.o img_data.o banner.o test-banner.o
	$(CC) $(CFLAGS) -std=gnu11 read-file.o img_data.o banner.o test-banner.o -lssl -lcrypto -o test-banner

test-banner_cache: read-file.o img_data.o banner.o banner_cache.o test-banner_cache.o
	$(CC) $(CFLAGS) -std=gnu11 read-file.o img_data.o banner.o banner_cache.o test-banner_cache.o -lssl -lcrypto -o test-banner_cache

test-sha1: file.o sha1.o test-sha1.o
	$(CC) $(CFLAGS) -std=gnu11 file.o sha1.o test-sha1.o -lssl -lcrypto -o test-sha1

test-geo2d: geo2d.o test-geo2d.o
	$(CC) $(CFLAGS) -std=gnu11 geo2d.o test-geo2d.o -o test-geo2d

global.o: global.c
	$(CC) $(CFLAGS) -std=gnu11 -c global.c -o global.o

path.o: path.c
	$(CC) $(CFLAGS) -std=gnu11 -c path.c -o path.o

watcher.o: watcher.c
	$(CC) $(CFLAGS) -std=gnu11 -c watcher.c -o watcher.o

img.o: img.c
	$(CC) $(CFLAGS) -std=gnu11 -c img.c -o img.o

img_file.o: img_file.c
	$(CC) $(CFLAGS) -std=gnu11 -c img_file.c -o img_file.o

img_data.o: img_data.c
	$(CC) $(CFLAGS) -std=gnu11 -c img_data.c -o img_data.o

task.o: task.c
	$(CC) $(CFLAGS) -std=gnu11 -c task.c -o task.o

file.o: file.c
	$(CC) $(CFLAGS) -std=gnu11 -c file.c -o file.o

buf.o: buf.c
	$(CC) $(CFLAGS) -std=gnu11 -c buf.c -o buf.o

display.o: display.c
	$(CC) $(CFLAGS) -std=gnu11 -c display.c -o display.o

banner.o: banner.c
	$(CC) $(CFLAGS) -std=gnu11 -c banner.c -o banner.o

banner_cache.o: banner_cache.c
	$(CC) $(CFLAGS) -std=gnu11 -c banner_cache.c -o banner_cache.o

sha1.o: sha1.c
	$(CC) $(CFLAGS) -std=gnu11 -c sha1.c -o sha1.o

geo2d.o: geo2d.c
	$(CC) $(CFLAGS) -std=gnu11 -c geo2d.c -o geo2d.o

read-file.o: read-file.c
	$(CC) $(CFLAGS) -std=gnu11 -c read-file.c -o read-file.o

test-path.o: test-path.c
	$(CC) $(CFLAGS) -std=gnu11 -c test-path.c -o test-path.o

test-watcher.o: test-watcher.c
	$(CC) $(CFLAGS) -std=gnu11 -c test-watcher.c -o test-watcher.o

test-img.o: test-img.c
	$(CC) $(CFLAGS) -std=gnu11 -c test-img.c -o test-img.o

test-task.o: test-task.c
	$(CC) $(CFLAGS) -std=gnu11 -c test-task.c -o test-task.o

test-file.o: test-file.c
	$(CC) $(CFLAGS) -std=gnu11 -c test-file.c -o test-file.o

test-buf.o: test-buf.c
	$(CC) $(CFLAGS) -std=gnu11 -c test-buf.c -o test-buf.o

test-display.o: test-display.c
	$(CC) $(CFLAGS) -std=gnu11 -c test-display.c -o test-display.o

test-banner.o: test-banner.c
	$(CC) $(CFLAGS) -std=gnu11 -c test-banner.c -o test-banner.o

test-banner_cache.o: test-banner_cache.c
	$(CC) $(CFLAGS) -std=gnu11 -c test-banner_cache.c -o test-banner_cache.o

test-sha1.o: test-sha1.c
	$(CC) $(CFLAGS) -std=gnu11 -c test-sha1.c -o test-sha1.o

test-geo2d.o: test-geo2d.c
	$(CC) $(CFLAGS) -std=gnu11 -c test-geo2d.c -o test-geo2d.o

.PHONY: wsserver
wsserver:
	+./build.sh

clean: clean-test clean-wsserver

clean-test: clean-test-buf clean-test-path clean-test-watcher clean-test-img clean-test-task clean-test-file clean-test-display clean-test-banner clean-test-banner_cache

clean-test-buf:
	rm -f test-buf test-buf.o buf.o

clean-test-path:
	rm -f test-path test-path.o path.o

clean-test-watcher:
	rm -f test-watcher test-watcher.o watcher.o

clean-test-img:
	rm -f test-img test-img.o img.o read-file.o

clean-test-task:
	rm -f test-task test-task.o task.o

clean-test-file:
	rm -f test-file test-file.o file.o buf.o

clean-test-display:
	rm -f test-display test-display.o display.o

clean-test-banner:
	rm -f test-banner test-banner.o banner.o img_data.o read-file.o

clean-test-banner_cache:
	rm -f test-banner_cache test-banner_cache.o banner_cache.o banner.o img_data.o read-file.o

clean-test-sha1:
	rm -f test-sha1 test-sha1.o sha1.o

clean-test-geo2d:
	rm -f test-geo2d test-geo2d.o geo2d.o

clean-wsserver:
	rm -f *.o
	rm -f wsserver moc_* Makefile.qt Makefile.qt.old
