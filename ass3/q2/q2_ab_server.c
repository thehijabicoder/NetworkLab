#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#define MAX 500
#define PORT 8080
#define SA struct sockaddr
int no_of_packets = 0, f = 0;

void sig_handler(int signum)
{
    FILE *fp2 = fopen("points1.txt", "a");

    fprintf(fp2, "%d %f\n", no_of_packets * 500, 0.01);

    fclose(fp2);

    no_of_packets = 0;
    return;
}

void chat(int connfd)
{

    char buff[MAX];
    char msg1[] = "Invalid Input\n";
    char msg2[] = "success\n";
    int i, n;
    struct itimerval it_val;
    signal(SIGALRM, sig_handler);
    it_val.it_value.tv_sec = 0;
    it_val.it_value.tv_usec = 10000;
    it_val.it_interval.tv_sec = 0;
    it_val.it_interval.tv_usec = 10000;
    while (1)
    {

        bzero(buff, MAX);
        read(connfd, buff, sizeof(buff));
        printf("From client: %s\t", buff);

        if (strncmp("Bye", buff, 3) == 0)
        {
            break;
        }
        else if (strncmp("GivemeyourVideo", buff, 15) == 0)
        {
            FILE *fp = fopen("/home/hanna/S6/network lab/ass3/sample.txt", "rb");
            if (fp == NULL)
            {
                printf("File opern error");
                return;
            }

            no_of_packets = 0;
            bzero(buff, MAX);
            setitimer(ITIMER_REAL, &it_val, NULL);
            while (n = fread(buff, 500, 1, fp) > 0)
            {

                no_of_packets += n;
                write(connfd, buff, 500);
                bzero(buff, MAX);
            }
            setitimer(ITIMER_REAL, NULL, NULL);

            if (feof(fp))
                printf("File Transfer Success\n");
            if (ferror(fp))
                printf("Error reading\n");

            fclose(fp);
            sleep(1);
            write(connfd, msg2, sizeof(msg2));
        }
        else
        {
            printf("To client : %s\n", buff);
            write(connfd, buff, sizeof(buff));
        }
    }
}

// Driver function
int main()
{
    int sockfd, connfd, len;
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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);

    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA *)&cli, &len);
    if (connfd < 0)
    {
        printf("server accept failed...\n");
        exit(0);
    }
    else
        printf("server accept the client...\n");

    // Function for chatting between client and server
    chat(connfd);

    // After chatting close the socket
    close(sockfd);
}
