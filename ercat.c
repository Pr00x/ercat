#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>

int main(int argc, char *argv[]) {
	if(argv[1] == NULL || strncasecmp(argv[1], "-h", 2) == 0 || strncasecmp(argv[1], "--help", 6) == 0 || argc == 1 || argv[2] == NULL) {
		printf("\n>> Usage: ercat [-p port]\n>> For Example: ercat -p 8000\n\n");
		
		return 0;
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
		
		return 0;
	}
	
	struct sockaddr_in sa;
	int server_fd, new_socket;
	long valread;
	int sa_len = sizeof(sa);
	char buffer[15000] = {0};
	
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("In socket");		
		exit(EXIT_FAILURE);
	}

	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	sa.sin_port = htons(PORT);


	if(bind(server_fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
		perror("In bind");
		exit(EXIT_FAILURE);
	}

	if(listen(server_fd, 10) < 0) {
		perror("In listen");
		exit(EXIT_FAILURE);		
	}

	FILE *fp;
	fp = fopen("index.html", "r");
  int count = 0;
	char txt[1024];
  int i = 0;
  char ch;

  if(!fp) {
    printf("\n>> Cannot open the file!\n\n");
    return 0;
  }

	puts("");

	FILE *fe = fopen("ercat.txt", "r");
	
	char ce = fgetc(fe); 
	while(ce != EOF) {
		printf("%c", ce);
		ce = fgetc(fe);
	}

	fclose(fe);

	printf("\n>> Starting a HTTP Server on port %d...", PORT);

	for(ch = getc(fp); ch != EOF; ch = getc(fp))
		count++;

	fclose(fp);

  fp = fopen("index.html", "r");

	ch = 0;
  ch = fgetc(fp);
  while(i < count) {
    txt[i] = ch;
    i++;
    ch = fgetc(fp);
  }

  fclose(fp);

	char res[15000] = "HTTP/1.1 200 OK\nContent-Type: text/html\nServer: Ercat/1.0\nContent-Length: ";
	char content_lenght[16];
	char new_line[3] = "\n\n";
	sprintf(content_lenght, "%d", count);

  txt[i] = 0;
	strcat(res, content_lenght);
	strcat(res, new_line);
	strcat(res, txt);

	while(1) {
		printf("\n-----------------------------------------\n\n");
		
		if((new_socket = accept(server_fd, (struct sockaddr *)&sa, (socklen_t *)&sa_len)) < 0) {
			perror("In accept");
			exit(EXIT_FAILURE);
		}
	
		valread = read(new_socket, buffer, 15000);
  	printf("%s\n", buffer);
  	write(new_socket, res, strlen(res));
  	close(new_socket);
		fflush(stdout);
	}

	return 0;
}
