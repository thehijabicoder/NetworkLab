#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

typedef struct fruits
{
    char name[10];
    int count;
} fruit;

void chat(int sockfd)
{
    fruit f[5];

    char buff[MAX];

    int n = 0, i;

    while (1)
    {

        bzero(buff, MAX);
        printf("Input : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;

        write(sockfd, buff, sizeof(buff));
        if (strncmp("SendInventory", buff, strlen("SendInventory")) == 0)
        {
            bzero(f, sizeof(fruit) * 5);
            read(sockfd, f, sizeof(f));
            // f = (fruit *)buff;
            printf("Server: \n");
            for (i = 0; i < 5; i++)
            {
                printf("\t%s %d\n", f[i].name, f[i].count);
            }
        }
        else
        {
            bzero(buff, MAX);
            read(sockfd, buff, sizeof(buff));
            printf("Server : %s", buff);
            if ((strncmp(buff, "exit", 4)) == 0)
            {
                printf("Client Exit...\n");
                break;
            }
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
