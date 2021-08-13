#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<signal.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>

#define MAX_CONNECTIONS 128

char *root;
int server_fd, clients[MAX_CONNECTIONS];

void server_start(int PORT) {
	struct sockaddr_in sa;

	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket()");
		exit(EXIT_FAILURE);
	}

	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(PORT);
	
	memset(sa.sin_zero, '\0', sizeof(sa.sin_zero));

	if(bind(server_fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
		perror("bind()");
		exit(EXIT_FAILURE);
	}

	if(listen(server_fd, 10000) < 0) {
		perror("listen()");
		exit(EXIT_FAILURE);
	}
}

void response(int n) {
	char buffer[15000], path[15000], data[1024], *req[3];
	int received, fd, bytes_read;

	memset((void*)buffer, (int)'\0', 15000);
	received = recv(clients[n], buffer, 15000, 0);

	if(received < 0)
		fprintf(stderr, "\nrecv() error.\n\n");
	else if(received == 0)
		fprintf(stderr, "\nClient dissconnected unexpetedly.\n\n");
	else {
		printf("%s", buffer);
		fflush(stdout);
		req[0] = strtok(buffer, " \t\n");

		if(strncmp(req[0], "GET\0", 4) == 0) {
			req[1] = strtok(NULL, " \t");
			req[2] = strtok(NULL, " \t\n");

			if(strncmp(req[2], "HTTP/1.1", 8) != 0) {
				char error400[] = "HTTP/1.1 403 Forbidden\nContent=Type: text/html\nServer: Ercat/1.0\nContent-Length: 1021\n\n<html><head><meta charset='UTF-8'><meta http-equiv='X-UA-Compatible' content='IE=edge'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Ercat | 404</title></head><body><style>*{margin:0;padding:0}body{background:#000}.error-404{margin-top:100px;position:absolute;text-align:center;top:25%;left:25%;right:25%;bottom:25%;color:#fff}.ercat{font-size:50px;margin-bottom:30px}.text-404{margin-bottom:50px;font-weight:700;font-size:37px}.button{border:1px solid #90f;margin-top:25px;padding:17px;font-size:20px;border-radius:17px;text-decoration:none;background:#90f;color:#fff}.button:hover{background:#93f;border-color:#90f;transition:all .25s;color:#fff}@media(max-width:370px){.ercat{font-size:35px}.text-404{font-size:20px}}@media(max-width:256px){.ercat{font-size:25px}.text-404{font-size:15px}}</style><div class='error-404'><h1 class='ercat'>Ercat/1.0</h1><h1 class='text-404'>400<span style='color: #90f;'>:</span> Bad Request</h1><a class='button' href='/'>Home</a></div></body></html>";
				write(clients[n], error400, strlen(error400));
			}
			else {
				if(strncmp(req[1], "/\0", 2) == 0)
					req[1] = "/index.html";

				strcpy(path, root);
				strcpy(&path[strlen(root)], req[1]);

				if((fd = open(path, O_RDONLY)) != -1) {
					send(clients[n], "HTTP/1.1 200 OK\nServer: Ercat/1.0\n\n", 35, 0);

					while((bytes_read = read(fd, data, 1024)) > 0)
						write(clients[n], data, bytes_read);
				}
				else if((fd = !access(path, F_OK)) == 0) {
					if(errno == ENOENT) {
						char error404[] = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nServer: Ercat/1.0\nContent-Length: 1139\n\n<html><head><meta charset='UTF-8'><meta http-equiv='X-UA-Compatible' content='IE=edge'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Ercat | 404</title></head><body><style>*{margin:0;padding:0}body{background:#000}.error-404{margin-top:100px;position:absolute;text-align:center;top:25%;left:25%;right:25%;bottom:25%;color:#fff}.ercat{font-size:50px;margin-bottom:30px}.text-404{margin-bottom:50px;font-weight:700;font-size:37px}.button{border:1px solid #90f;margin-top:25px;padding:17px;font-size:20px;border-radius:17px;text-decoration:none;background:#90f;color:#fff}.button:hover{background:#93f;border-color:#90f;transition:all .25s;color:#fff}@media(max-width:370px){.ercat{font-size:35px}.text-404{font-size:20px}}@media(max-width:256px){.ercat{font-size:25px}.text-404{font-size:15px}}</style><div class='error-404'><h1 class='ercat'>Ercat/1.0</h1><h1 class='text-404'>404<span style='color: #90f;'>:</span> Page <span style='color: #90f;'>Not</span> Found</h1><a class='button' href='/'>Home</a></div></body></html>";
						write(clients[n], error404, strlen(error404));
					}
				}
				else if((fd = access(path, R_OK)) == -1) {
					if(errno == EACCES) {
						char error403[1109] = "HTTP/1.1 403 Forbidden\nContent=Type: text/html\nServer: Ercat/1.0\nContent-Length: 1019\n\n<html><head><meta charset='UTF-8'><meta http-equiv='X-UA-Compatible' content='IE=edge'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Ercat | 404</title></head><body><style>*{margin:0;padding:0}body{background:#000}.error-404{margin-top:100px;position:absolute;text-align:center;top:25%;left:25%;right:25%;bottom:25%;color:#fff}.ercat{font-size:50px;margin-bottom:30px}.text-404{margin-bottom:50px;font-weight:700;font-size:37px}.button{border:1px solid #90f;margin-top:25px;padding:17px;font-size:20px;border-radius:17px;text-decoration:none;background:#90f;color:#fff}.button:hover{background:#93f;border-color:#90f;transition:all .25s;color:#fff}@media(max-width:370px){.ercat{font-size:35px}.text-404{font-size:20px}}@media(max-width:256px){.ercat{font-size:25px}.text-404{font-size:15px}}</style><div class='error-404'><h1 class='ercat'>Ercat/1.0</h1><h1 class='text-404'>403<span style='color: #90f;'>:</span> Forbidden</h1><a class='button' href='/'>Home</a></div></body></html>";
         	 write(clients[n], error403, strlen(error403));
        	}
				}
			}
		}
	}

	shutdown(clients[n], SHUT_RDWR);
	close(clients[n]);
	clients[n] = -1;
}

void ercat() {
	FILE *fp = fopen("/etc/ercat/ercat.txt", "r");
	char c = fgetc(fp);

	puts("");

	while(c != EOF) {
		printf("%c", c);
		c = fgetc(fp);
	}

	fclose(fp);
}

int main(int argc, char *argv[]) {
	if(argv[1] == NULL || strncasecmp(argv[1], "-h", 2) == 0 || strncasecmp(argv[1], "--help", 6) == 0 || argc == 1 || argv[2] == NULL) {
		ercat();
		printf("\n>> Usage: ercat [-p port]\n>> For Example: ercat -p 8000\n\n");

		return -1;
	}

	int PORT;
	int opt = getopt(argc, argv, "p:");

	switch(opt) {
		case 'p':
			PORT = atoi(optarg);
			break;
		default:
			printf("\n\n");
			return 1;
	}

	if(PORT > 65535 || PORT < 1) {
		printf("\nPlease enter the correct port!\n\n");

		return -1;
	}

	struct sockaddr_in clientaddr;
	socklen_t addrlen;
	root = getenv("PWD");
	int slot = 0;
	int i;

	for(i = 0; i < MAX_CONNECTIONS; i++)
		clients[i] = -1;

	server_start(PORT);

	ercat();
	printf("\n>> Starting a HTTP Server on port %d...", PORT);
	while(1) {
		printf("\n-----------------------------------------\n\n");

		addrlen = sizeof(clientaddr);
		clients[slot] = accept(server_fd, (struct sockaddr *)&clientaddr, (socklen_t *)&addrlen);

		if(clients[slot] < 0) {
			perror("accept()");
			exit(EXIT_FAILURE);
		}
		else {
			if(fork() == 0) {
				response(slot);
				exit(EXIT_FAILURE);
			}
		}

		while(clients[slot] != -1)
			slot = (slot + 1) % MAX_CONNECTIONS;
	}

	return 0;
}
