#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

void fetch_man_page(const char *command)
{
    const char *host = "man.he.net";
    const int port = 80;
    char request[1024];
    char buffer[4096];
    struct hostent *server;
    struct sockaddr_in server_addr;
    int sockfd, bytes_received;

    snprintf(request, sizeof(request), "GET /?topic=%s&section=all HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/5.0\r\n\r\n", command, host);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error opening socket");
        exit(1);
    }

    server = gethostbyname(host);
    if (server == NULL)
    {
        fprintf(stderr, "Error: no such host\n");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    server_addr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error connecting");
        exit(1);
    }

    if (send(sockfd, request, strlen(request), 0) < 0)
    {
        perror("Error sending request");
        exit(1);
    }

    int total_bytes_received = 0;
    memset(buffer, 0, sizeof(buffer));

    while ((bytes_received = recv(sockfd, buffer + total_bytes_received, sizeof(buffer) - total_bytes_received - 1, 0)) > 0)
    {
        total_bytes_received += bytes_received;
        if (total_bytes_received >= sizeof(buffer) - 1)
        {
            break;
        }
    }
    buffer[total_bytes_received] = '\0';

    int flag = 0;

    for (int i = 0; i < total_bytes_received; i++)
    {
        if (buffer[i] == '<')
        {
            flag = 1;
        }
        else if (buffer[i] == '>')
        {
            flag = 0;
            continue;
        }

        if (!flag)
        {
            putchar(buffer[i]);
        }
    }

    close(sockfd);
}
