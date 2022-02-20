#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#define MAX 80
#define name_len 10
#define MAX_CLIENTS 100
#define PORT 8080
#define SA struct sockaddr

volatile sig_atomic_t flag = 0;
int sockfd;
char name[name_len];

void overwrite_stdout()
{
    printf("\r>> ");
    fflush(stdout);
}

void exit_chat(int sig)
{
    write(sockfd, "exit", 4);
    flag = 1;
}

void send_message()
{
    char buffer[MAX];
    bzero(buffer, MAX);

    while (1)
    {
        overwrite_stdout();
        scanf("%[^\n]%*c", buffer);

        if (strncmp(buffer, "exit", 4) == 0)
        {
            break;
        }
        else
            write(sockfd, buffer, MAX);

        bzero(buffer, MAX);
    }
    exit_chat(2);
}

void recv_message()
{
    char buffer[MAX];
    int n;
    bzero(buffer, MAX);
    while (1)
    {
        if (n = read(sockfd, buffer, MAX) > 0)
        {
            printf("%s", buffer);
            overwrite_stdout();
        }
        else
        {
            break;
        }
        bzero(buffer, MAX);
    }
}

void chatroom()
{
    write(sockfd, name, name_len);
    printf("***Joined the Chat Room***\n");

    pthread_t send_thread;
    if (pthread_create(&send_thread, NULL, (void *)send_message, NULL) != 0)
    {
        printf("Error creating thread\n");
        exit(1);
    }

    pthread_t recv_thread;
    if (pthread_create(&recv_thread, NULL, (void *)recv_message, NULL) != 0)
    {
        printf("Error creating thread\n");
        exit(1);
    }

    while (1)
    {
        if (flag)
        {
            printf("***Left the Chat Room***\n");
            break;
        }
    }

    return;
}

int main()
{
    signal(SIGINT, exit_chat);
    printf("Enter your name: ");
    scanf("%[^\n]%*c", name);

    struct sockaddr_in servaddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed\n");
        exit(1);
    }

    chatroom();
    close(sockfd);
}
