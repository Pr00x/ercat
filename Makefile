CC=gcc

install:
	$(CC) ercat.c -o /usr/bin/ercat
	mkdir /etc/ercat/
	cp ercat.txt /etc/ercat/
clean:
	rm /usr/bin/ercat
	rm -rf /etc/ercat/
