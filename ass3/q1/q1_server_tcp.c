#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

typedef struct fruits
{
    char name[10];
    int count;
} fruit;

// Function designed for chat between client and server.
void chat(int connfd)
{
    fruit f[5] = {{"apple", 10}, {"mango", 10}, {"banana", 10}, {"chikoo", 10}, {"papaya", 10}};
    char buff[MAX];
    char query1[] = "Enter the name of the fruit\n";
    char query2[] = "Enter the number of fruits\n";
    char msg1[] = "Not available\n";
    char msg2[] = "Invalid Input\n";
    char msg3[] = "Success\n";
    int i;

    while (1)
    {

        bzero(buff, MAX);
        read(connfd, buff, sizeof(buff));
        printf("From client: %s\t", buff);
        if (strncmp("Fruits", buff, 6) == 0)
        {
            printf("To client : %s\n", query1);
            write(connfd, query1, sizeof(query1));
            bzero(buff, MAX);
            read(connfd, buff, sizeof(buff));
            printf("From client: %s\t", buff);
            for (i = 0; i < 5; i++)
            {
                if (strncmp(f[i].name, buff, strlen(f[i].name)) == 0)
                {
                    printf("To client : %s\n", query2);
                    write(connfd, query2, sizeof(query2));
                    bzero(buff, MAX);
                    read(connfd, buff, sizeof(buff));
                    printf("From client: %s\t", buff);
                    if (atoi(buff) <= f[i].count)
                    {
                        f[i].count -= atoi(buff);
                        printf("To client : %s\n", msg3);
                        write(connfd, msg3, sizeof(msg3));
                    }
                    else
                    {
                        printf("To client : %s\n", msg1);
                        write(connfd, msg1, sizeof(msg1));
                    }
                    break;
                }
            }
            if (i >= 5)
            {
                printf("To client : %s\n", msg1);
                write(connfd, msg1, sizeof(msg1));
            }
        }
        else if (strncmp("SendInventory", buff, strlen("SendInventory")) == 0)
        {
            printf("To client : \n");
            for (i = 0; i < 5; i++)
            {
                printf("\t\t%s %d\n", f[i].name, f[i].count);
            }
            printf("\n");
            write(connfd, f, sizeof(f));
        }
        else if (strncmp("exit", buff, 4) == 0)
        {
            write(connfd, buff, sizeof(buff));
            printf("Server Exit...\n");
            break;
        }
        else
        {
            printf("To client : %s\n", msg2);
            write(connfd, msg2, sizeof(msg2));
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
