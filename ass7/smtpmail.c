#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <time.h>
// #include <dirent.h>
#define MAX 100
#define SA struct sockaddr

/*
Return Codes:
301 Incorrect Username
305 User Authenticated with password
310 Incorrect password
*/
int returncodes[] = {301, 305, 310};
char username[MAX], password[MAX];

int get_password_index(char line[])
{
    int i = 0;
    while (line[i] != ' ')
    {
        i++;
    }
    i++;
    return i;
}

int login(int connfd)
{
    read(connfd, username, MAX);
    read(connfd, password, MAX);

    char fileline[MAX];
    char fileuser[MAX], filepass[MAX];

    FILE *fp = fopen("./userlogincred.txt", "r");
    if (fp == NULL)
    {
        printf("\nFile not found\n");
        exit(0);
    }

    while (fgets(fileline, MAX, fp) != NULL)
    {
        int i = get_password_index(fileline);
        bzero(fileuser, MAX);
        strncpy(fileuser, fileline, i - 1);
        if (strcmp(username, fileuser) == 0)
        {
            bzero(filepass, MAX);
            strcpy(filepass, fileline + i);
            filepass[strlen(filepass) - 1] = '\0';
            if (strcmp(password, filepass) == 0)
            {
                write(connfd, &returncodes[1], sizeof(returncodes[1]));
                fclose(fp);
                return 1;
            }
            else
            {
                write(connfd, &returncodes[2], sizeof(returncodes[2]));
                fclose(fp);
                return 0;
            }
        }
    }
    fclose(fp);
    write(connfd, &returncodes[0], sizeof(returncodes[0]));
    return 0;
}

int get_username_len(char line[])
{
    int i = 0;
    while (line[i] != '@')
    {
        i++;
    }
    return i;
}

void serve(int connfd)
{
    char filename[1000], filecontent[80], from[80];

    bzero(from, 80);
    read(connfd, from, 80);

    bzero(filecontent, 80);
    read(connfd, filecontent, 80);

    int i = get_username_len(filecontent);
    char uname[80];
    strncpy(uname, filecontent + 4, i - 4);
    uname[i] = '\0';

    bzero(filename, 1000);
    sprintf(filename, "./%s/mymailbox", uname);
    FILE *fp = fopen(filename, "a");
    if (fp == NULL)
    {
        printf("\nFile not found\n");
        exit(0);
    }

    fprintf(fp, "%s", from);
    fprintf(fp, "%s", filecontent);

    bzero(filecontent, 80);
    read(connfd, filecontent, 80);
    i = 0;
    while (strcmp(filecontent, ".\n") != 0)
    {
        fprintf(fp, "%s", filecontent);

        if (++i == 1)
        {
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            char recievedtime[30];
            bzero(recievedtime, 30);
            strftime(recievedtime, 80, "%a %d %b %Y %H:%M", t);
            bzero(filecontent, 80);
            sprintf(filecontent, "Received: %s\n", recievedtime);
            fprintf(fp, "%s", filecontent);
        }
        bzero(filecontent, 80);
        read(connfd, filecontent, 80);
    }
    fprintf(fp, "%s", filecontent);
    fclose(fp);
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

        if (fork() == 0)
        {
            close(sockfd);
            if (login(connfd))
                serve(connfd);
            close(connfd);
            exit(0);
        }
    }
}

int main(int argc, char **argv)
{

    int sockfd, my_port;
    struct sockaddr_in serverAddr;

    my_port = atoi(argv[1]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("[-]Error in connection.\n");
        exit(1);
    }

    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(my_port);
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