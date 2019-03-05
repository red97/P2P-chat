#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<pthread.h>

static int stpr;
void *reader(void *vargp)
{
	int *n_servfd = (int *)vargp;
	char buf[1024] = {0};
	while(stpr == 1)
	{
		recv(*n_servfd, buf, 1024, 0);
		printf("%s\n", buf);
		if(strcmp(buf, "Server>end") == 0)
			stpr = 0;
		bzero(buf, sizeof(buf));
	}
	return NULL;
}
void *writer(void *vargp)
{
	int *n_servfd = (int *)vargp;
	char buf[1024] = {0};
	while(stpr == 1)
	{
		scanf("%s", buf);
		char tmp[1024];
		strcpy(tmp,"Client>");
		strcat(tmp, buf);
		send(*n_servfd, tmp, 1024, 0);
		if(strcmp(tmp, "Client>end") == 0)
			stpr = 0;
		bzero(buf, sizeof(buf));
	}
	return NULL;
}

void *tmp_server(void *arg)
{

	int servfd, n_servfd, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	int PORT = *((int *)arg);
	//printf("Hello\n");
	printf("Port = %d\n",PORT);

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

	//when a client attempts a connetion accept function accepts it
	if((n_servfd = accept(servfd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
	{
		perror("Accept Failiure!");
		exit(0);
	}

	stpr = 1;
	pthread_t read_id, write_id;
	pthread_create(&read_id, NULL, reader, (void *)&n_servfd);
	pthread_create(&write_id, NULL, writer, (void *)&n_servfd);
	pthread_join(read_id, NULL);
	pthread_join(write_id, NULL);
	printf("connection ended\n");
	return NULL;

}
void *tmp_client(void *arg)
{

	struct sockaddr_in address;
	int clientfd = 0, valread;
	struct sockaddr_in serv_addr;
	char buf[1024] = {0};
	int PORT = *((int *)arg);

	//creation of socket
	if((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Socket creation error!");
		return NULL;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	//Convert IPv4/Ipv6 addresses from text to binary format
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
	{
		printf("Invalid Address!");
		return NULL;
	}

	if(connect(clientfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("Connection error!");
		return NULL;
	}

	stpr = 1;
	pthread_t read_id, write_id;
	pthread_create(&read_id, NULL, reader, (void *)&clientfd);
	pthread_create(&write_id, NULL, writer, (void *)&clientfd);
	pthread_join(read_id, NULL);
	pthread_join(write_id, NULL);
	printf("connection ended\n");
	return NULL;

}
int main(int argc, char const *argv[])
{

	struct sockaddr_in address;
	int clientfd = 0, valread;
	struct sockaddr_in serv_addr;
	char buf[1024] = {0};
	int PORT = atoi(argv[1]);

	//creation of socket
	if((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Socket creation error!");
		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	//Convert IPv4/Ipv6 addresses from text to binary format
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
	{
		printf("Invalid Address!");
		return -1;
	}

	if(connect(clientfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("Connection error!");
		return -1;
	}

	char port_ch[10];
	recv(clientfd, port_ch, 10, 0);
	int port = atoi(port_ch);
	printf("PortNo : %d\n", port);
	
	while(1)
	{

		printf("Choose the index:\n1.wait\n2.list\n3.connect\n4.exit\n5.clear screen\n");
		int inx;
		scanf("%d", &inx);
		printf("Choice = %d\n", inx);
		switch(inx)
		{

			case 1:
			send(clientfd, "0", 10, 0);
			pthread_t ser_fd;
			pthread_create(&ser_fd, NULL, tmp_server, (void *)&port);
			pthread_join(ser_fd, NULL);
			break;
			case 2:
			send(clientfd, "1", 10, 0);
			char buf1[1024];
			recv(clientfd, buf1, 1024, 0);
			printf("%s\n",buf1);
			bzero(buf1, sizeof(buf1));
			break;
			case 3:
			//printf("Hello1\n");
			send(clientfd, "2", 10, 0);
			//printf("Hello1\n");
			char buf2[1024];
			recv(clientfd, buf2, 1024, 0);
			printf("%s\n",buf2);
			bzero(buf2, sizeof(buf2));
			printf("Enter the port number to connect : ");
			int c_port;
			scanf("%d", &c_port);
			char tmp_port1[5];
			sprintf(tmp_port1, "%d", c_port);
			send(clientfd, tmp_port1, 5, 0);
			bzero(tmp_port1, sizeof(tmp_port1));
			recv(clientfd, tmp_port1, 1, 0);
			if(strcmp(tmp_port1, "0") == 0)
			{
				printf("Wrong port!\n");
				break;
			}
			pthread_t cli_fd;
			pthread_create(&cli_fd, NULL, tmp_client, (void *)&c_port);
			pthread_join(cli_fd, NULL);
			break;
			case 4:
			exit(0);
			case 5:
			printf("\033[H\033[J");
			printf("PortNo : %d\n", port);
			break;
			default:
			printf("Wrong choice!!\n");

		}

	}
	return 0;

}