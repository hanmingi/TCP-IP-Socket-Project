#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 9999

void connect_Server(char* ipAddress);

//print error func
void error_handling(char *message);

int main(int argc, char **argv){

    if (argc != 2){
        printf("Usage : %s <IP>\n", argv[0]);
        exit(1);
    }

    connect_Server(argv[1]);

    printf("connect Server!!!\n");

}

void connect_Server(char* ipAddress){

    int clnt_sock;

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
