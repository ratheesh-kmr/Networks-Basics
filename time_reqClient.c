#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFSIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    socklen_t server_len = sizeof(server_addr);
    char buffer[BUFSIZE];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        error("socket creation failed");
    }

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Convert server address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        error("inet_pton failed");
    }

    // Send a message to the server
    const char *message = "TIME";
    if (sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&server_addr, server_len) < 0) {
        error("sendto failed");
    }

    printf("Sent request to server\n");

    // Receive the time string from the server
    if (recvfrom(sockfd, buffer, BUFSIZE, 0, (struct sockaddr *)&server_addr, &server_len) < 0) {
        error("recvfrom failed");
    }

    printf("Received time from server: %s\n", buffer);

    close(sockfd);
    return 0;
}
