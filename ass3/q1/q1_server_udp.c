#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr
typedef struct fruits
{
    char name[10];
    int count;
} fruit;

void chat(int sockfd, struct sockaddr_in cliaddr)
{

    int len, n, i;
    len = sizeof(cliaddr);

    fruit f[5] = {{"apple", 10}, {"mango", 10}, {"banana", 10}, {"chikoo", 10}, {"papaya", 10}};
    char buff[MAX];
    char query1[] = "Enter the name of the fruit\n";
    char query2[] = "Enter the number of fruits\n";
    char msg1[] = "Not available\n";
    char msg2[] = "Invalid Input\n";
    char msg3[] = "Success\n";

    while (1)
    {
        bzero(buff, MAX);
        recvfrom(sockfd, buff, sizeof(buff), 0, (SA *)&cliaddr, &len);
        printf("From client: %s\t", buff);
        if (strncmp("Fruits", buff, 6) == 0)
        {
            printf("To client : %s\n", query1);
            sendto(sockfd, query1, sizeof(query1), 0, (SA *)&cliaddr, len);
            bzero(buff, MAX);
            recvfrom(sockfd, buff, sizeof(buff), 0, (SA *)&cliaddr, &len);
            printf("From client: %s\t", buff);
            for (i = 0; i < 5; i++)
            {
                if (strncmp(f[i].name, buff, strlen(f[i].name)) == 0)
                {
                    printf("To client : %s\n", query2);
                    sendto(sockfd, query2, sizeof(query2), 0, (SA *)&cliaddr, len);
                    bzero(buff, MAX);
                    recvfrom(sockfd, buff, sizeof(buff), 0, (SA *)&cliaddr, &len);
                    printf("From client: %s\t", buff);
                    if (atoi(buff) <= f[i].count)
                    {
                        f[i].count -= atoi(buff);
                        printf("To client : %s\n", msg3);
                        sendto(sockfd, msg3, sizeof(msg3), 0, (SA *)&cliaddr, len);
                    }
                    else
                    {
                        printf("To client : %s\n", msg1);
                        sendto(sockfd, msg1, sizeof(msg1), 0, (SA *)&cliaddr, len);
                    }
                    break;
                }
            }
            if (i >= 5)
            {
                printf("To client : %s\n", msg1);
                sendto(sockfd, msg1, sizeof(msg1), 0, (SA *)&cliaddr, len);
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
            sendto(sockfd, f, sizeof(f), 0, (SA *)&cliaddr, len);
        }
        else if (strncmp("exit", buff, 4) == 0)
        {

            sendto(sockfd, buff, sizeof(buff), 0, (SA *)&cliaddr, len);
            break;
        }
        else
        {
            printf("To client : %s\n", msg2);
            sendto(sockfd, msg2, sizeof(msg2), 0, (SA *)&cliaddr, len);
        }
    }
}

int main()
{
    int sockfd;

    struct sockaddr_in servaddr, cliaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket creation failed");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(0);
    }
    else
        printf("Bind successfully done..\n");

    printf("Server Listening...\n");

    chat(sockfd, cliaddr);
    close(sockfd);
}