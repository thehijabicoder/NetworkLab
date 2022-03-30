#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#define SA struct sockaddr

/*
Return Codes:
301 Incorrect Username
305 User Authenticated with password
310 Incorrect password
*/
int returncode;
char username[100], password[100];

void read_credentials()
{
    bzero(username, 100);
    bzero(password, 100);
    printf("\nusername: ");
    scanf("%s%*c", username);
    printf("\npassword: ");
    scanf("%s%*c", password);
}

int check_mail_format(char string[], int x)
{
    for (int i = x + 1; i < strlen(string) - 2; i++)
    {
        if (string[i] == '@')
        {
            return 1;
        }
    }
    return 0;
}

int connect_server(int port)
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
    servaddr.sin_port = htons(port);

    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed...\n");
        exit(0);
    }
    return sockfd;
}

int login(int sockfd)
{
    write(sockfd, username, 100);
    write(sockfd, password, 100);
    read(sockfd, &returncode, sizeof(returncode));
    switch (returncode)
    {
    case 301:
        printf("\n Incorrect Username\n");
        return 0;
    case 305:
        return 1;
    case 310:
        printf("\n Incorrect Password\n");
        return 0;
    default:
        printf("\n Unknown return code\n");
        return 0;
    }
}

void manage_mail(int sockfd)
{
    char mail_info[500];
Loop1:
    bzero(mail_info, 500);
    read(sockfd, mail_info, 500);
    while (strncmp(mail_info, "\n Enter a serial number or 'q'\n", 31) != 0)
    {
        printf("\n%s", mail_info);
        write(sockfd, "1", 1);
        bzero(mail_info, 500);
        read(sockfd, mail_info, 500);
    }
    printf("\n%s", mail_info);
    write(sockfd, "1", 1);
    int n;
    read(sockfd, &n, sizeof(n));

    while (1)
    {
        printf("\n >> ");
        char ch = getchar();
        getchar();
        write(sockfd, &ch, sizeof(ch));
        if (ch - '0' > 0 && ch - '0' <= n)
        {
            char from[80], to[80], subject[80], received[80], content[80];

            bzero(from, 80);
            read(sockfd, from, 80);
            printf("\n%s", from);
            bzero(to, 80);
            read(sockfd, to, 80);
            printf("%s", to);
            bzero(subject, 80);
            read(sockfd, subject, 80);
            printf("%s", subject);
            bzero(received, 80);
            read(sockfd, received, 80);
            printf("%s", received);

            bzero(content, 80);
            read(sockfd, content, 80);
            while (strcmp(content, ".\n") != 0)
            {
                printf("%s", content);
                bzero(content, 80);
                read(sockfd, content, 80);
            }
            printf("%s", content);

            printf("\n >> ");
            char resp[8];
            char cmd = getchar();
            getchar();
            write(sockfd, &cmd, sizeof(cmd));
            if (cmd == 'q')
            {
                read(sockfd, resp, sizeof(resp));
                printf("\n %s\n", resp);
                return;
            }
            else
            {
                goto Loop1;
            }
        }
        else
        {
            char response[8];
            read(sockfd, response, 7);
            printf("\n %s\n", response);
            if (strcmp(response, "goodbye") == 0)
            {
                return;
            }
        }
    }
}

void send_mail(int sockfd)
{
    char from[80], to[80], subject[80], message[50][80];
    bzero(from, 80);
    bzero(to, 80);
    bzero(subject, 80);

    printf("\nsend-mail>>\n");

    fgets(from, 80, stdin);
    fgets(to, 80, stdin);
    fgets(subject, 80, stdin);

    int i = 0;
    bzero(message[i], 80);
    fgets(message[i], 80, stdin);
    while (strcmp(message[i], ".\n") != 0)
    {
        bzero(message[++i], 80);
        fgets(message[i], 80, stdin);
    }

    if (strncmp(from, "From: ", 6) == 0)
    {
        if (check_mail_format(from, 6))
        {
            if (strncmp(to, "To: ", 4) == 0)
            {
                if (check_mail_format(to, 4))
                {
                    if (strncmp(subject, "Subject: ", 9) == 0)
                    {
                        write(sockfd, from, 80);
                        write(sockfd, to, 80);

                        bzero(&returncode, sizeof(returncode));
                        read(sockfd, &returncode, sizeof(returncode));
                        if (returncode == 400)
                        {
                            printf("\n Recipient Not Found \n");
                            return;
                        }

                        write(sockfd, subject, 80);
                        int i = 0;
                        while (strcmp(message[i], ".\n") != 0)
                        {
                            write(sockfd, message[i], 80);
                            i++;
                        }
                        write(sockfd, message[i], 80);

                        printf("\n Mail sent successfully\n");
                    }
                    else
                    {
                        printf("\n Incorrect format\n missing 'Subject: '\n");
                        return;
                    }
                }
                else
                {
                    printf("\n Incorrect format\n missing 'To: X@Y'\n");
                    return;
                }
            }
            else
            {
                printf("\n Incorrect format\n missing 'To: '\n");
                return;
            }
        }
        else
        {
            printf("\n Incorrect format\n missing 'From: X@Y'\n");
            return;
        }
    }
    else
    {
        printf("\n Incorrect format\n missing 'From: '\n");
        return;
    }
}

int main(int argc, char **argv)
{
    int smtpserver = atoi(argv[1]), pop3server = atoi(argv[2]);
    char command[100];

    read_credentials();

    while (1)
    {
        bzero(command, 100);
        printf("\n$~: ");
        fgets(command, 100, stdin);
        command[strlen(command) - 1] = '\0';

        if (strcmp(command, "Manage Mail") == 0)
        {
            int popfd = connect_server(pop3server);
            if (login(popfd))
                manage_mail(popfd);
            else
                read_credentials();

            close(popfd);
        }
        else if (strcmp(command, "Send Mail") == 0)
        {
            int smtpfd = connect_server(smtpserver);
            if (login(smtpfd))
                send_mail(smtpfd);
            else
                read_credentials();
            close(smtpfd);
        }
        else if (strcmp(command, "Quit") == 0)
        {
            return 0;
        }
        else
        {
            printf("\n Invalid Command \n \nSupported Commands : \n Manage Mail\n Send Mail\n Quit\n");
        }
    }
}