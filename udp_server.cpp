#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        error("ERROR on binding");
    }

    client_len = sizeof(client_addr);
    while (true) {
        bzero(buffer, BUFFER_SIZE);
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &client_addr, &client_len);
        if (n < 0) {
            error("ERROR on recvfrom");
        }

        std::cout << "Message received: " << buffer << std::endl;

        n = sendto(sockfd, buffer, n, 0, (struct sockaddr *) &client_addr, client_len);
        if (n < 0) {
            error("ERROR on sendto");
        }

        if (strcmp(buffer, "exit") == 0) {
            break;
        }
    }

    close(sockfd);
    return 0;
}
