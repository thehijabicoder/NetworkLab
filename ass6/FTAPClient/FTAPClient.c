// File Transfer Application Program CLIENT

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
#define MAX 80
#define PORT 4035
#define SA struct sockaddr

/*
Return Codes:
200 OK Connection is set up
300 Correct Username; Need password
301 Incorrect Username
305 User Authenticated with password
310 Incorrect password
505 Command not supported
*/
int returncode;
char command[MAX];
char msg[MAX];
int n = 0;

void login(int sockfd)
{
    while (1)
    {
        bzero(command, MAX);
        printf("\n$~: ");

        fgets(command, MAX, stdin);
        command[strlen(command) - 1] = '\0';

        write(sockfd, command, MAX);

        bzero(&returncode, sizeof(returncode));
        read(sockfd, &returncode, sizeof(returncode));

        printf("\n~%d~\n", returncode);

        switch (returncode)
        {
        case 300:
            printf("Correct Username; Need password\n");
            break;
        case 301:
            printf("Incorrect Username\n");
            break;
        case 305:
            bzero(command, MAX);
            read(sockfd, command, MAX);
            printf("%s\n", command);
            return;
        case 310:
            printf("Incorrect password\n");
            break;
        case 505:
            printf("Please Login using `USERN` and `PASSWD` to Access the Server\n");
            break;
        default:
            printf("Unknown return code\n");
            break;
        }
    }
}

void store_file(int sockfd, char command[])
{
    char buffer[MAX];
    char filename[MAX];
    int n;

    strcpy(filename, command + 10);

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("\nFile not found\n");
        return;
    }

    write(sockfd, command, MAX);

    bzero(buffer, MAX);
    printf("\n File Uploading...\n");

    clock_t t = clock();

    while (n = fread(buffer, 1, MAX, fp) > 0)
    {
        write(sockfd, buffer, n);
        bzero(buffer, MAX);
    }

    if (feof(fp))
    {
        printf("\nFile Uploaded Successfully\n");
    }
    if (ferror(fp))
    {
        printf("\nError in File Upload\n");
    }
    fclose(fp);

    write(sockfd, "END OF FILE", 11);

    t = clock() - t;

    printf("Time elapsed: %f seconds\n", ((float)t) / CLOCKS_PER_SEC);

    bzero(msg, MAX);
    read(sockfd, msg, MAX);
    printf("%s\n", msg);
}

void get_file(int sockfd, char command[])
{
    bzero(msg, MAX);
    read(sockfd, msg, MAX);

    if (strcmp(msg, "File Doesn't Exist") == 0)
    {
        printf("%s\n", msg);
        return;
    }
    else
    {
        char filename[MAX];
        strcpy(filename, command + 8);

        FILE *fp = fopen(filename, "wb");
        if (fp == NULL)
        {
            printf("\nFile Creation Failed\n");
            exit(0);
        }

        printf("\n File Downloading...\n");
        clock_t t = clock();

        while (strcmp(msg, "END OF FILE") != 0)
        {
            fwrite(msg, 1, MAX, fp);
            bzero(msg, MAX);
            read(sockfd, msg, MAX);
        }
        fclose(fp);

        t = clock() - t;

        printf("\nFile Downloaded Successfully\n");
        printf("Time elapsed: %f seconds\n", ((float)t) / CLOCKS_PER_SEC);
    }
}

void access_server(int sockfd)
{
    while (1)
    {
        bzero(command, MAX);
        printf("\n$~: ");
        fgets(command, MAX, stdin);
        command[strlen(command) - 1] = '\0';

        if (strncmp(command, "StoreFile", 9) == 0)
        {
            store_file(sockfd, command);
            continue;
        }

        write(sockfd, command, MAX);

        if (strncmp(command, "ListDir", 7) == 0)
        {
            printf("\n");
            while (1)
            {
                bzero(msg, MAX);
                read(sockfd, msg, MAX);
                if (strncmp(msg, "END OF DIRECTORY LISTING", 24) == 0)
                {
                    break;
                }
                printf("%s\n", msg);
            }
            printf("\n");
        }
        else if (strncmp(command, "GetFile", 7) == 0)
        {
            get_file(sockfd, command);
        }
        else
        {
            bzero(msg, MAX);
            read(sockfd, msg, MAX);
            printf("\n%s\n", msg);

            if (strncmp(msg, "Bye", 3) == 0)
            {
                return;
            }
        }
    }
}

void connect_server()
{
    int sockfd;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed...\n");
        exit(0);
    }

    read(sockfd, &returncode, sizeof(returncode));
    printf("\n~%d~\n", returncode);

    login(sockfd);
    access_server(sockfd);

    close(sockfd);
}

int main()
{

    while (1)
    {
        n = 0;
        bzero(command, MAX);
        printf("\n$~: ");
        while ((command[n++] = getchar()) != '\n')
            ;

        if (strcmp(command, "START\n") == 0)
        {
            connect_server();
        }
        else if (strcmp(command, "QUIT\n") == 0)
        {
            exit(0);
        }
        else
        {
            printf("\nInvalid Command\nUse `START` to connect to the server.\nUse `QUIT` to exit\n");
        }
    }
}