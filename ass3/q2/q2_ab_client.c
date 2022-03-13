#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#define MAX 500
#define PORT 8080
#define SA struct sockaddr
int no_of_bytes = 0, f = 0;

void sig_handler(int signum)
{
    FILE *fp2 = fopen("points2.txt", "a");

    fprintf(fp2, "%d %f\n", no_of_bytes, 0.01);

    fclose(fp2);

    no_of_bytes = 0;
    return;
}

void chat(int sockfd)
{

    char buff[MAX];

    int n = 0, i;
    struct itimerval it_val;
    signal(SIGALRM, sig_handler);
    it_val.it_value.tv_sec = 0;
    it_val.it_value.tv_usec = 10000;
    it_val.it_interval.tv_sec = 0;
    it_val.it_interval.tv_usec = 10000;

    while (1)
    {

        bzero(buff, MAX);
        printf("Input : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        write(sockfd, buff, sizeof(buff));
        if ((strncmp(buff, "Bye", 3)) == 0)
        {
            break;
        }

        else if (strncmp("GivemeyourVideo", buff, 15) == 0)
        {
            FILE *fp = fopen("filerecieved.txt", "wb");
            if (fp == NULL)
            {
                printf("File opern error");
                return;
            }
            bzero(buff, MAX);
            no_of_bytes = 0;
            setitimer(ITIMER_REAL, &it_val, NULL);
            while (1)
            {
                n = read(sockfd, buff, 500);
                no_of_bytes += n;

                if (strncmp("success", buff, 7) == 0)
                {
                    printf("File Transfer Success\n");
                    break;
                }
                fflush(stdout);
                fwrite(buff, n, 1, fp);
                bzero(buff, MAX);
            }
            setitimer(ITIMER_REAL, NULL, NULL);
            fclose(fp);
        }
        else
        {
            bzero(buff, MAX);
            read(sockfd, buff, sizeof(buff));
            printf("Server : %s", buff);
        }
    }
}

int main()
{
    int sockfd;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    // function for chat
    chat(sockfd);

    // close the socket
    close(sockfd);
}
