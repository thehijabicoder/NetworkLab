#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

void chat(int sockfd)
{
    struct sockaddr_in newAddr;
    int len = sizeof(newAddr), newSocket;

    char buffer[MAX];
    int childpid, n = 0;
    while (1)
    {
        n++;
        newSocket = accept(sockfd, (SA *)&newAddr, &len);
        if (newSocket < 0)
        {
            printf("\nConnection with a client failed\n");
            exit(1);
        }

        printf("\nConnected with Client%d\n\n", n);

        if ((childpid = fork()) == 0)
        {
            close(sockfd);

            while (1)
            {
                bzero(buffer, MAX);
                read(newSocket, buffer, MAX);

                int i = 0, n1 = 0, n2 = 0;
                char op = 0;

                if (strncmp(buffer, "\0", 1) == 0)
                {
                    printf("\nClient%d disconnected\n", n);
                    exit(0);
                }

                while (buffer[i] != '+' && buffer[i] != '-' && buffer[i] != '*' && buffer[i] != '/' && buffer[i] != '\0')
                {
                    n1 = n1 * 10 + (buffer[i] - '0');
                    i++;
                }

                op = buffer[i];

                i++;
                n2 = 0;
                while (buffer[i] != '\n' && buffer[i] != '\0')
                {
                    n2 = n2 * 10 + (buffer[i] - '0');
                    i++;
                }

                bzero(buffer, MAX);

                printf("\n[Client%d]: %d %c %d\n", n, n1, op, n2);

                if (op == '+')
                {
                    sprintf(buffer, "%d", n1 + n2);
                }
                else if (op == '-')
                {
                    sprintf(buffer, "%d", n1 - n2);
                }
                else if (op == '*')
                {
                    sprintf(buffer, "%d", n1 * n2);
                }
                else if (op == '/' && n2 != 0)
                {
                    sprintf(buffer, "%d", n1 / n2);
                }
                else
                {
                    strcpy(buffer, "\0");
                }
                printf("[Server]: %s\n", buffer);
                write(newSocket, buffer, MAX);
                bzero(buffer, sizeof(buffer));
            }
        }
    }

    close(newSocket);
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
    printf("[+]Server Socket is created.\n");

    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (SA *)&serverAddr, sizeof(serverAddr)) != 0)
    {
        printf("[-]Error in binding.\n");
        exit(1);
    }
    printf("[+]Bind to port %d\n", 8080);

    if (listen(sockfd, 10) == 0)
    {
        printf("[+]Listening....\n");
    }
    else
    {
        printf("[-]Error in binding.\n");
    }

    chat(sockfd);

    return 0;
}