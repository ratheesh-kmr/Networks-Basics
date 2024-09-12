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
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        error("Invalid address/ Address not supported");
    }

    while (true) {
        std::cout << "Please enter the message: ";
        bzero(buffer, BUFFER_SIZE);
        std::cin.getline(buffer, BUFFER_SIZE);

        int n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &server_addr, sizeof(server_addr));
        if (n < 0) {
            error("ERROR on sendto");
        }

        bzero(buffer, BUFFER_SIZE);
        socklen_t server_len = sizeof(server_addr);
        n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &server_addr, &server_len);
        if (n < 0) {
            error("ERROR on recvfrom");
        }

        std::cout << "Echo from server: " << buffer << std::endl;

        if (strcmp(buffer, "exit") == 0) {
            break;
        }
    }

    close(sockfd);
    return 0;
}
