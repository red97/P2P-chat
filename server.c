#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<pthread.h>

typedef struct data
{
	int sock_fd;
	int stpr;
	int port;
}data;

static int waiting[20];
void *thread(void *arg)
{
	data *tmp = (struct data *)arg;
	int sock_fd = (*tmp).sock_fd;
	int port_i = (*tmp).port;
	char port[5];
	sprintf(port, "%d", port_i);

	send(sock_fd, port, 10, 0);
	
	char inx[10];
	recv(sock_fd, inx, 10, 0);
	int ix = atoi(inx);
	char msg[1024];
	switch(ix)
	{

		case 0:
		waiting[port_i%20] = port_i;
		break;
		case 1:
		strcpy(msg, "List:-\n");
		for(int i=0;i<20;i++)
		{
			if(waiting[i] > 0)
			{
				char tmp[10];
				sprintf(tmp, "%d", waiting[i]);
				strcat(msg, tmp);
				strcat(msg, "\n");
			}	
		}
		send(sock_fd, msg, 1024, 0);
		bzero(msg, sizeof(msg));
		break;
		case 2:
		//printf("Hello1\n");
		strcpy(msg, "List:-\n");
		for(int i=0;i<20;i++)
		{
			if(waiting[i] > 0)
			{
				char tmp11[10];
				sprintf(tmp11, "%d", waiting[i]);
				strcat(msg, tmp11);
				strcat(msg, "\n");
			}	
		}
		send(sock_fd, msg, 1024, 0);
		bzero(msg, sizeof(msg));
		char tmp_port1[5];
		recv(sock_fd, tmp_port1, 5, 0);
		if(waiting[atoi(tmp_port1)%20] == 0)
			send(sock_fd, "0", 1, 0);
		else
		{
			waiting[atoi(tmp_port1)%20] = 0;
			send(sock_fd, "1", 1, 0);
		}

	}

	return NULL;
}
int main(int argc, char *argv[])
{

	int servfd, n_servfd, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	int PORT = atoi(argv[1]);

	//socket creation : socket(domain(IPv4/IPv6), type(TCP/UDP), protocol(value for Internet Protocol(IP)))
	servfd = socket(AF_INET, SOCK_STREAM, 0);
	if(servfd == 0)
	{
		perror("Socket Failed!");
		exit(0);
	}

	//Helps in reusing address and port (optional)
	if(setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
	{
		perror("setsockopt failed!");
		exit(0);
	}

	//After creation of socket, bind function binds the socket to address and port
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if(bind(servfd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("Binding Failiure!");
		exit(0);
	}

	//makes the server wait for new connections
	if(listen(servfd, 5) < 0)
	{
		perror("Listen Failiure!");
		exit(0);
	}

	int tmp_port = PORT;
	for(int i=0;i<20;i++)
		waiting[i] = 0;
	while(1) 
	{

		data nsoc;
		if((nsoc.sock_fd = accept(servfd, (struct sockaddr *)&address, (socklen_t *)&addrlen))<0)
		{
			printf("Acception error!\n");
			break;
		}
		nsoc.stpr = 1;
		nsoc.port = ++tmp_port;
		pthread_t thread_id;
		pthread_create(&thread_id, NULL, thread, (void *)&nsoc);

	}

}