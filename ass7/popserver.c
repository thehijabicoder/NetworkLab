#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
// #include <dirent.h>
#define SA struct sockaddr
#define MAX 100

/*
Return Codes:
301 Incorrect Username
305 User Authenticated with password
310 Incorrect password
*/
int returncodes[] = {301, 305, 310};
char username[MAX], password[MAX];

typedef struct email_info
{
    char from[80];
    char to[80];
    char subject[80];
    char received[80];
    char content[50][80];
    char mail_info[500];
} email;
email mymail[50];

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

int show_allmail(int connfd)
{
    char filename[1000], filecontent[80], temp;

    sprintf(filename, "./%c%s/mymailbox", tolower(username[0]), username + 1);
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("\nFile not found\n");
        exit(0);
    }

    bzero(filecontent, 80);
    int i = 0, j = 0;
    char from[80], recvtime[80], subject[80];
    while (fgets(filecontent, 80, fp) != NULL)
    {
        if (strncmp(filecontent, "From: ", 6) == 0)
        {
            bzero(mymail[i].from, 80);
            bzero(from, 80);
            strcpy(mymail[i].from, filecontent);
            strcpy(from, filecontent + 6);
            from[strlen(from) - 1] = '\0';
        }
        else if (strncmp(filecontent, "To: ", 4) == 0)
        {
            bzero(mymail[i].to, 80);
            strcpy(mymail[i].to, filecontent);
        }
        else if (strncmp(filecontent, "Subject: ", 9) == 0)
        {
            bzero(mymail[i].subject, 80);
            bzero(subject, 80);
            strcpy(mymail[i].subject, filecontent);
            strcpy(subject, filecontent + 9);
            subject[strlen(subject) - 1] = '\0';
        }
        else if (strncmp(filecontent, "Received: ", 10) == 0)
        {
            bzero(mymail[i].received, 80);
            bzero(recvtime, 80);
            strcpy(mymail[i].received, filecontent);
            strcpy(recvtime, filecontent + 10);
            recvtime[strlen(recvtime) - 1] = '\0';

            bzero(mymail[i].mail_info, 500);
            sprintf(mymail[i].mail_info, "%d.\t%s\t%s\t%s", i + 1, from, recvtime, subject);

            write(connfd, mymail[i].mail_info, 500);

            read(connfd, &temp, 1);
        }
        else
        {

            bzero(mymail[i].content[j], 80);
            strcpy(mymail[i].content[j], filecontent);
            if (strcmp(mymail[i].content[j], ".\n") == 0)
            {
                i++;
                j = -1;
            }
            j++;
        }
    }
    write(connfd, "\n Enter a serial number or 'q'\n", 31);
    read(connfd, &temp, 1);
    fclose(fp);
    return i;
}
void delete_mail(int x, int n)
{
    char filename[1000], filecontent[80];

    bzero(filename, 1000);
    sprintf(filename, "./%c%s/mymailbox", tolower(username[0]), username + 1);
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("\nFile not found\n");
        exit(0);
    }

    for (int i = 0; i < n; i++)
    {
        if (i != x)
        {
            fprintf(fp, "%s", mymail[i].from);
            fprintf(fp, "%s", mymail[i].to);
            fprintf(fp, "%s", mymail[i].subject);
            fprintf(fp, "%s", mymail[i].received);
            int j = 0;
            while (strcmp(mymail[i].content[j], ".\n") != 0)
            {
                fprintf(fp, "%s", mymail[i].content[j]);
                j++;
            }
            fprintf(fp, "%s", mymail[i].content[j]);
        }
    }
    fclose(fp);
}

void show_email(int connfd, int x, int n)
{
    x--;
    write(connfd, mymail[x].from, 80);
    write(connfd, mymail[x].to, 80);
    write(connfd, mymail[x].subject, 80);
    write(connfd, mymail[x].received, 80);
    int i = 0;
    while (strcmp(mymail[x].content[i], ".\n") != 0)
    {
        write(connfd, mymail[x].content[i], 80);
        i++;
    }
    write(connfd, mymail[x].content[i], 80);

    char ch;
    read(connfd, &ch, sizeof(ch));
    if (ch == 'd')
    {
        delete_mail(x, n);
        return;
    }
    else if (ch == 'q')
    {
        write(connfd, "goodbye", 7);
        close(connfd);
        exit(0);
    }
    else
    {
        return;
    }
}

void serve(int connfd)
{
    int n;
Loop1:
    n = show_allmail(connfd);
    write(connfd, &n, sizeof(n));
    while (1)
    {
        char ch;
        read(connfd, &ch, sizeof(ch));
        if (ch - '0' > 0 && ch - '0' <= n)
        {
            show_email(connfd, ch - '0', n);
            goto Loop1;
        }
        else if (ch == 'q')
        {
            write(connfd, "goodbye", 7);
            close(connfd);
            return;
        }
        else
        {
            write(connfd, "Invalid", 7);
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

        if (fork() == 0)
        {
            close(sockfd);
            if (login(connfd))
                serve(connfd);
            exit(0);
        }
    }
}

int main(int argc, char **argv)
{

    int sockfd, pop3_port;
    struct sockaddr_in serverAddr;

    pop3_port = atoi(argv[1]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("[-]Error in connection.\n");
        exit(1);
    }

    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(pop3_port);
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