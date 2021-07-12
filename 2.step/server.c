#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define BUFSIZE 1024

void open_Server();

//print error func
void error_handling(char *message);

int clnt_sock;

int main(int argc, char **argv){

    char buf[BUFSIZE];
    char command[10];

    open_Server();
    printf("connect success!\n");

    //Recv command line
    while(TRUE){
        recv(clnt_sock, buf, sizeof(buf), 0);
        sscanf(buf, "%s", command);
        printf("Recv Command > %s\n", command);
    }
}

void open_Server(){

    int serv_sock;

    int clnt_addr_size;
    int PORT = 9999;

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1){
        // socket error
        error_handling("socket() error");
    }

    // include netinet/in.h
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if(bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
        error_handling("bind() error");
    }

    if(listen(serv_sock, 5) == -1){
        error_handling("listen() error");
    }

    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);

    if(clnt_sock == -1){
        error_handling("accept() error");
    }
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
