#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

typedef struct fruits
{
    char name[10];
    int count;
} fruit;

void chat(int sockfd, struct sockaddr_in servaddr)
{
    fruit f[5];
    char buff[MAX];
    int n = 0, i, len = sizeof(servaddr);

    while (1)
    {
        bzero(buff, MAX);
        printf("Input : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;

        sendto(sockfd, buff, sizeof(buff), 0, (SA *)&servaddr, len);
        if (strncmp("SendInventory", buff, strlen("SendInventory")) == 0)
        {
            bzero(f, sizeof(fruit) * 5);
            recvfrom(sockfd, f, sizeof(f), 0, (SA *)&servaddr, &len);
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
            recvfrom(sockfd, buff, sizeof(buff), 0, (SA *)&servaddr, &len);
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
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        printf("Error in socket creation\n");
        return -1;
    }
    else
    {
        printf("Socket created\n");
    }

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    chat(sockfd, servaddr);
}
