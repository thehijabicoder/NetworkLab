//  File Transfer Application Program SERVER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#define MAX 80
#define PORT 4035
#define SA struct sockaddr

/*
Return Codes:B
200 OK Connection is set up
300 Correct Username; Need password
301 Incorrect Username
305 User Authenticated with password
310 Incorrect password
505 Command not supported
*/

int returncodes[6] = {200, 300, 301, 305, 310, 505};
char command[MAX];
char username[MAX];
char msg[MAX];

int get_password_index(char line[])
{
    int i = 0;
    while (line[i] != ',')
    {
        i++;
    }
    i++;
    return i;
}

int login(int connfd)
{
    char fileline[MAX];
    char password[MAX];

    int flag = 0;

    while (1)
    {
        bzero(command, MAX);
        read(connfd, command, MAX);

        if (strncmp(command, "USERN", 5) == 0)
        {
            FILE *fp = fopen("../logincred.txt", "r");
            if (fp == NULL)
            {
                printf("\nFile not found\n");
                exit(0);
            }

            while (fgets(fileline, MAX, fp) != NULL)
            {
                int i = get_password_index(fileline);
                bzero(username, MAX);
                strncpy(username, fileline, i - 1);
                if (strcmp(username, command + 6) == 0)
                {

                    write(connfd, &returncodes[1], sizeof(returncodes[1]));
                    bzero(password, MAX);
                    strcpy(password, fileline + i);
                    password[strlen(password) - 1] = '\0';
                    flag = 1;
                    break;
                }
            }
            fclose(fp);

            if (!flag)
            {
                write(connfd, &returncodes[2], sizeof(returncodes[2]));
            }
        }
        else if (strncmp(command, "PASSWD", 6) == 0 && flag == 1)
        {
            if (strcmp(password, command + 7) == 0)
            {
                char msg[MAX];
                write(connfd, &returncodes[3], sizeof(returncodes[3]));
                sprintf(msg, "Welcome, %s!", username);
                write(connfd, msg, sizeof(msg));
                return 1;
            }
            else
            {
                write(connfd, &returncodes[4], sizeof(returncodes[4]));
            }
        }
        else if (strcmp(command, "QUIT") == 0)
        {

            return 0;
        }
        else
        {
            write(connfd, &returncodes[5], sizeof(returncodes[5]));
        }
    }
}

void create_file(char filename[], int connfd)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        fp = fopen(filename, "w");
        if (fp == NULL)
        {
            printf("\nFile Creation Failed\n");
            exit(0);
        }
        else
        {
            bzero(msg, MAX);
            sprintf(msg, "File Creation Successful");
            write(connfd, msg, sizeof(msg));
        }
        fclose(fp);
    }
    else
    {
        fclose(fp);
        bzero(msg, MAX);
        sprintf(msg, "File Already Exists");
        write(connfd, msg, sizeof(msg));
    }
}

void list_dir(int connfd)
{
    DIR *dir;
    struct dirent *direntry;
    if ((dir = opendir(".")) != NULL)
    {
        while ((direntry = readdir(dir)) != NULL)
        {
            if (strcmp(direntry->d_name, ".") != 0 && strcmp(direntry->d_name, "..") != 0)
            {
                bzero(msg, MAX);
                sprintf(msg, "%s", direntry->d_name);
                write(connfd, msg, sizeof(msg));
            }
        }
        bzero(msg, MAX);
        sprintf(msg, "END OF DIRECTORY LISTING");
        write(connfd, msg, sizeof(msg));
        closedir(dir);
    }
    else
    {
        perror("");
        exit(0);
    }
}

void store_file(char filename[], int connfd)
{
    char buffer[MAX];
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        printf("\nFile Creation Failed\n");
        exit(0);
    }
    else
    {
        bzero(buffer, MAX);
        read(connfd, buffer, MAX);
        while (strcmp(buffer, "END OF FILE") != 0)
        {
            fwrite(buffer, 1, MAX, fp);
            bzero(buffer, MAX);
            read(connfd, buffer, MAX);
        }
        fclose(fp);

        bzero(msg, MAX);
        sprintf(msg, "File Saved Successfully");
        write(connfd, msg, sizeof(msg));
    }
}

void get_file(char filename[], int connfd)
{
    char buffer[MAX];
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        bzero(msg, MAX);
        sprintf(msg, "File Doesn't Exist");
        write(connfd, msg, sizeof(msg));
        return;
    }
    else
    {
        int n;
        bzero(buffer, MAX);
        while (n = fread(buffer, 1, MAX, fp) > 0)
        {
            write(connfd, buffer, MAX);
            bzero(buffer, MAX);
        }
        if (!feof(fp) || ferror(fp))
        {
            perror("");
            exit(0);
        }
        fclose(fp);

        bzero(msg, MAX);
        sprintf(msg, "END OF FILE");
        write(connfd, msg, MAX);
    }
}

void serve(int connfd)
{

    char filename[MAX];

    while (1)
    {
        bzero(command, MAX);
        read(connfd, command, MAX);

        if (strncmp(command, "CreateFile", 10) == 0)
        {
            bzero(filename, MAX);
            strcpy(filename, command + 11);
            create_file(filename, connfd);
        }
        else if (strcmp(command, "ListDir") == 0)
        {
            list_dir(connfd);
        }
        else if (strncmp(command, "StoreFile", 9) == 0)
        {
            bzero(filename, MAX);
            strcpy(filename, command + 10);
            store_file(filename, connfd);
        }
        else if (strncmp(command, "GetFile", 7) == 0)
        {
            bzero(filename, MAX);
            strcpy(filename, command + 8);
            get_file(filename, connfd);
        }
        else if (strcmp(command, "QUIT") == 0)
        {
            bzero(msg, MAX);
            sprintf(msg, "Bye %s!", username);
            write(connfd, msg, sizeof(msg));
            exit(0);
        }
        else
        {
            bzero(msg, MAX);
            sprintf(msg, "Invalid Command");
            write(connfd, msg, sizeof(msg));
        }
    }
}

void accept_connection(int sockfd)
{
    struct sockaddr_in cliAddr;
    int len = sizeof(cliAddr), connfd;

    while (1)
    {
        connfd = accept(sockfd, (SA *)&cliAddr, &len);
        if (connfd < 0)
        {
            printf("\nConnection with a client failed\n");
            exit(1);
        }

        write(connfd, &returncodes[0], sizeof(returncodes[0]));

        if (fork() == 0)
        {
            close(sockfd);
            if (login(connfd))
                serve(connfd);
            exit(0);
        }
    }
}

int main()
{

    int sockfd;
    struct sockaddr_in serverAddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("[-]Error in connection.\n");
        exit(1);
    }

    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (SA *)&serverAddr, sizeof(serverAddr)) != 0)
    {
        printf("[-]Error in binding.\n");
        exit(1);
    }

    if (listen(sockfd, 10) != 0)
    {
        printf("[-]Error in Listening.\n");
    }

    accept_connection(sockfd);

    return 0;
}