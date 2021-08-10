CC=gcc

install:
	$(CC) ercat.c -o /usr/bin/ercat
clean:
	rm /usr/bin/ercat
