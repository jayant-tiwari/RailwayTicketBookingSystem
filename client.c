#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define port_no 8000
#define RESPONSE_BYTES 512
#define REQUEST_BYTES 512

char* receiveMsgFromServer(int sock_fd) {
    int numPacketsToReceive = 0;
    int n = read(sock_fd, &numPacketsToReceive, sizeof(int));
    if(n <= 0) {
        shutdown(sock_fd, SHUT_WR);
        return NULL;
    }
    char *str = (char*)malloc(numPacketsToReceive*RESPONSE_BYTES);
    memset(str, 0, numPacketsToReceive*RESPONSE_BYTES);
    char *str_p = str;
    int i;
    for(i = 0; i < numPacketsToReceive; ++i) {
        int n = read(sock_fd, str, RESPONSE_BYTES);
        str = str+RESPONSE_BYTES;
    }
    return str_p;
}

void sendMsgToServer(int sock_fd, char *str) {
    int numPacketsToSend = (strlen(str)-1)/REQUEST_BYTES + 1;
    int n = write(sock_fd, &numPacketsToSend, sizeof(int));
    char *msgToSend = (char*)malloc(numPacketsToSend*REQUEST_BYTES);
    strcpy(msgToSend, str);
    int i;
    for(i = 0; i < numPacketsToSend; ++i) {
        int n = write(sock_fd, msgToSend, REQUEST_BYTES);
        msgToSend += REQUEST_BYTES;
    }
}

/*
int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    send(sock , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");
    valread = read( sock , buffer, 1024);
    printf("%s\n",buffer );
    return 0;
}*/
int main(int argc,char **argv)
{
	int sock_fd;
	struct sockaddr_in serv_addr;
	char *msgFromServer;
    char msgToServer[256];


	sock_fd=socket(AF_INET, SOCK_STREAM, 0);
	//port_no = atoi("8080");

	memset(&serv_addr, 0, sizeof(serv_addr));  //setting sockaddr_in serv_addr
	serv_addr.sin_family = AF_INET;         //setting DOMAIN
    serv_addr.sin_port = htons(port_no);     //setting port number
    inet_aton(argv[1], &serv_addr.sin_addr);

    connect(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    printf("Connection Established.\n");
    // printf("Asking server what to do(by receiving input from server).\n");

    while(1) {
        msgFromServer = receiveMsgFromServer(sock_fd);
        if(msgFromServer == NULL)
            break;
        if(strncmp(msgFromServer, "unauth", 6) == 0) {
            printf("Unautherized User.\n");
            shutdown(sock_fd, SHUT_WR);
            break;
        }
        printf("%s\n",msgFromServer);
        free(msgFromServer);
        
        memset(msgToServer, 0, sizeof(msgToServer));
        scanf("%s", msgToServer);
        sendMsgToServer(sock_fd, msgToServer);
        if(strncmp(msgToServer, "exit", 4) == 0) {
            shutdown(sock_fd, SHUT_WR);
            break;
        }
    }

    while(1) {
        msgFromServer = receiveMsgFromServer(sock_fd);
        if(msgFromServer == NULL)
            break;
        printf("%s\n",msgFromServer);
        free(msgFromServer);
    }
    printf("Write end closed by the server.\n");
    shutdown(sock_fd, SHUT_RD);
    printf("Connection closed gracefully.\n");
    return 0;


}
