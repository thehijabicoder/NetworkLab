#include <stdio.h>
#include <netdb.h>
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

static _Atomic unsigned int client_count = 0;
static int uid = 0;

typedef struct client_info
{
    struct sockaddr_in addr;
    int sock;
    int uid;
    char name[name_len];
} client_t;

client_t *clients[MAX_CLIENTS];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void trim_string(char *str, int len)
{
    for (int i = 0; i < len; i++)
    {
        if (str[i] == '\n')
        {
            str[i] = '\0';
            break;
        }
    }
}

void add_client(client_t *client)
{
    pthread_mutex_lock(&mutex);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (!clients[i])
        {
            clients[i] = client;
            break;
        }
    }

    pthread_mutex_unlock(&mutex);
}

void remove_client(int uid)
{
    pthread_mutex_lock(&mutex);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] && clients[i]->uid == uid)
        {
            clients[i] = NULL;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
}

void send_msg(int uid, char *message)
{
    pthread_mutex_lock(&mutex);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] && clients[i]->uid != uid)
        {
            write(clients[i]->sock, message, strlen(message));
        }
    }
    pthread_mutex_unlock(&mutex);
}

void *handle_client(void *arg)
{
    client_t *client = (client_t *)arg;
    char buffer[MAX];
    char buffer2[MAX + MAX];
    char name[name_len];
    int flag = 0, n;
    client_count++;

    read(client->sock, name, name_len);
    trim_string(name, name_len);
    strcpy(client->name, name);

    sprintf(buffer, "%s has joined the chat\n", client->name);
    printf("%s", buffer);
    send_msg(client->uid, buffer);

    bzero(buffer, MAX);

    while (1)
    {
        if (flag)
            break;

        if (n = read(client->sock, buffer, MAX) >= 0)
        {
            trim_string(buffer, MAX);
            if (strncmp(buffer, "exit", 4) == 0 || n == 0)
            {
                sprintf(buffer, "%s has left the chat\n", client->name);
                printf("%s", buffer);
                send_msg(client->uid, buffer);
                flag = 1;
            }
            else if (strlen(buffer) > 0)
            {
                sprintf(buffer2, "[%s]: %s\n", client->name, buffer);
                printf("%s", buffer2);
                send_msg(client->uid, buffer2);
            }
        }
        bzero(buffer, MAX);
    }
    close(client->sock);
    remove_client(client->uid);
    free(client);
    client_count--;
    pthread_detach(pthread_self());
    return NULL;
}

// Function designed for chat between client and server.
void chatroom(int sockfd, pthread_t thread_id)
{
    printf("***Chat Room Opened***\n");
    struct sockaddr_in cli_addr;
    int len = sizeof(cli_addr), connfd;

    while (1)
    {
        connfd = accept(sockfd, (SA *)&cli_addr, &len);

        if (client_count + 1 == MAX_CLIENTS)
        {
            printf("Chatroom is full. Connection rejected for %s %d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
            close(connfd);
            continue;
        }

        client_t *client = (client_t *)malloc(sizeof(client_t));
        client->addr = cli_addr;
        client->sock = connfd;
        client->uid = uid++;

        add_client(client);
        pthread_create(&thread_id, NULL, &handle_client, (void *)client);

        sleep(1);
    }

    return;
}

// Driver function
int main()
{

    int sockfd, opt = 1;
    struct sockaddr_in servaddr;
    pthread_t thread_id;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed\n");
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Signals
    signal(SIGPIPE, SIG_IGN);

    if (setsockopt(sockfd, SOL_SOCKET, (SO_REUSEADDR | SO_REUSEPORT), (char *)&opt, sizeof(opt)) < 0)
    {
        printf("setsockopt error\n");
        exit(1);
    }
    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind failed\n");
        exit(1);
    }

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed\n");
        exit(0);
    }

    // Function for chatting between client and server
    chatroom(sockfd, thread_id);

    // After chatting close the socket
    close(sockfd);
}
