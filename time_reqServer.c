#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFSIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFSIZE];
    time_t rawtime;
    struct tm *timeinfo;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        error("socket creation failed");
    }

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the address and port
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("bind failed");
    }

    printf("UDP Time Server is running on port %d\n", PORT);

    while (1) {
        memset(buffer, 0, BUFSIZE);

        // Receive a message from the client
        if (recvfrom(sockfd, buffer, BUFSIZE, 0, (struct sockaddr *)&client_addr, &client_len) < 0) {
            error("recvfrom failed");
        }

        printf("Received request from client\n");

        // Get the current time
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        // Convert the time to a string
        strftime(buffer, BUFSIZE, "%Y-%m-%d %H:%M:%S", timeinfo);

        // Send the time string back to the client
        if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, client_len) < 0) {
            error("sendto failed");
        }

        printf("Sent current time to client: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
