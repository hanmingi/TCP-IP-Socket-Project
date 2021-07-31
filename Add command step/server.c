#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>

#define TRUE 1
#define BUFSIZE 1024

void open_Server();

//print error func
void error_handling(char *message);

int clnt_sock;

int main(int argc, char **argv){

    char buf[BUFSIZE];
    char command[10];
    int filehandle;
    int size, change;
    char filename[BUFSIZE];

    struct stat obj;

    open_Server();
    printf("Connected Client success!\n");

    while(TRUE){
        recv(clnt_sock, buf, sizeof(buf) + 1, 0);
        sscanf(buf, "%s", command);
        printf("Recv command > %s\n", command);

        // quit command
        if(!strcmp(command, "quit")){
            int status = 1;
            send(clnt_sock, &status, sizeof(int), 0);
            exit(0);
        }

        // ls command
        else if (!strcmp(command, "ls")) {
            system("ls | grep -v temp_ls.txt > temp_ls.txt");
            stat("temp_ls.txt", &obj);
            size = obj.st_size;
            send(clnt_sock, &size, sizeof(int), 0);
            filehandle = open("temp_ls.txt", O_RDONLY);
            sendfile(clnt_sock, filehandle, NULL, size);
            system("rm temp_ls.txt");
        }

        // pwd command
        else if (!strcmp(command, "pwd")) {
            system("pwd > temp_pwd.txt");
            stat("temp_pwd.txt", &obj);
            size = obj.st_size;
            send(clnt_sock, &size, sizeof(int), 0);
            filehandle = open("temp_pwd.txt", 0644);
            sendfile(clnt_sock, filehandle, NULL, size);
            system("rm temp_pwd.txt");
        }

        // cd command
        else if (!strcmp(command, "cd")) {
            if (chdir(buf + 3) == 0)
                change = 1;
            else
                change = 0;
            send(clnt_sock, &change, sizeof(int), 0);
        }

        //mkdir command
        else if (!strcmp(command, "mkdir")){
            sscanf(buf, "%s%s", filename, filename);
            char *mkdir = malloc(sizeof(char) *BUFSIZE);
            strcpy(mkdir, "mkdir ");
            strcat(mkdir, filename);
            system(mkdir);
            free(mkdir);
        }

        //rmdir command
        else if (!strcmp(command, "rm")) {
            sscanf(buf, "%s%s", filename, filename);
            char *rm = malloc(sizeof(char) * BUFSIZE);
            strcpy(rm, "rm -rf ");
            strcat(rm, filename);
            system(rm);
            free(rm);
        }
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
