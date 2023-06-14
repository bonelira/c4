#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define LENGTH 2048
#define MAX_NAME_LENGTH 42

volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[MAX_NAME_LENGTH];

void str_overwrite_stdout()
{
    printf("%s", "> ");
    fflush(stdout);
}

void str_trim_lf(char *arr, int length)
{
    for (int i = 0; i < length; i++)
    {
        if (arr[i] == '\n')
        {
            arr[i] = '\0';
            break;
        }
    }
}

void catch_exit_by_ctrl_c(int sig)
{
    flag = 1;
}

void get_time(char *buffer, size_t bufferSize)
{
    time_t currentTime;
    struct tm *timeInfo;

    currentTime = time(NULL);

    timeInfo = localtime(&currentTime);

    strftime(buffer, bufferSize, "%Y-%m-%d %H:%M:%S", timeInfo);
}

void send_msg_handler()
{
    char message[LENGTH] = {};
    char buffer[LENGTH + MAX_NAME_LENGTH] = {};

    while (1)
    {
        str_overwrite_stdout();
        fgets(message, LENGTH, stdin);
        str_trim_lf(message, LENGTH);

        char timestamp[20];
        get_time(timestamp, sizeof(timestamp));
        sprintf(buffer, "[%s] %s: %s\n", timestamp, name, message);
        int bytesSent = send(sockfd, buffer, strlen(buffer), 0);
        if (bytesSent == -1)
        {
            printf("ERROR: Failed to send message\n");
            break;
        }

        if (strcmp(message, "exit") == 0)
        {
            flag = 1; // Define a flag para sair do loop principal
            break;
        }

        bzero(message, LENGTH);
        bzero(buffer, LENGTH + MAX_NAME_LENGTH);
    }

    catch_exit_by_ctrl_c(2);
}

void recv_msg_handler()
{
    char message[LENGTH] = {};
    while (1)
    {
        int receive = recv(sockfd, message, LENGTH, 0);
        if (receive > 0)
        {
            printf("%s", message);
            str_overwrite_stdout();
        }
        else if (receive == 0)
        {
            break;
        }
        else
        {
            // -1
        }
        memset(message, 0, sizeof(message));
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *ip = "127.0.0.1";
    int port = atoi(argv[1]);

    signal(SIGINT, catch_exit_by_ctrl_c);

    printf("Enter your name: ");
    fgets(name, MAX_NAME_LENGTH, stdin);
    str_trim_lf(name, strlen(name));

    if (strlen(name) > MAX_NAME_LENGTH || strlen(name) < 2)
    {
        printf("Enter a valid name. Your name must be between 3 and 42 characters\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;

    // config sockets
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    // conexão com o servidor
    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1)
    {
        printf("ERROR: not connected. Connect\n");
        return EXIT_FAILURE;
    }

    // Enviando o nome
    send(sockfd, name, MAX_NAME_LENGTH, 0);
    printf("***** Welcome to C4 *****\n");
    printf("You can start chatting now\n");

    pthread_t send_msg_thread;
    if (pthread_create(&send_msg_thread, NULL, (void *)send_msg_handler, NULL) != 0)
    {
        printf("ERROR: pthread\n");
        return EXIT_FAILURE;
    }

    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, (void *)recv_msg_handler, NULL) != 0)
    {
        printf("ERROR: pthread\n");
        return EXIT_FAILURE;
    }

    while (1)
    {
        if (flag)
        {
            printf("\nGoodbye, Popeye!\n");
            break;
        }
    }
    close(sockfd);

    return EXIT_SUCCESS;
}
