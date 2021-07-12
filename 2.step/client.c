#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 9999
#define TRUE 1
#define BUFSIZE 1024

void connect_Server(char* ipAddress);

//print error func
void error_handling(char *message);

int clnt_sock;

int main(int argc, char **argv){

    char menu[BUFSIZE];

    if (argc != 2){
        printf("Usage : %s <IP>\n", argv[0]);
        exit(1);
    }

    connect_Server(argv[1]);

    //system call clear
    system("clear");
    while (TRUE) {
        // command line
        printf("\033[1;32mCommand line > ");
        fgets(menu, BUFSIZE, stdin);
        fprintf(stderr, "\033[97m");
        send(clnt_sock, menu, sizeof(menu) + 1, 0);
    }
}

void connect_Server(char* ipAddress){


    struct sockaddr_in serv_addr;

    clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(clnt_sock == -1){
        error_handling("socket() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ipAddress);
    serv_addr.sin_port = htons(PORT);

    if (connect(clnt_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("connect() error");
    }
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
