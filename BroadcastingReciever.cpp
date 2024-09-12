#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sockfd;
    struct sockaddr_in broadcastAddr;
    char buffer[1024];
    int broadcastPort = 8080;
    socklen_t addrLen = sizeof(broadcastAddr);
    int reuse = 1;  // Variable to enable SO_REUSEADDR

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed\n";
        return -1;
    } 

    // Enable SO_REUSEADDR
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0) {
        std::cerr << "Error setting SO_REUSEADDR\n";
        close(sockfd);
        return -1;
    }

    // Set up the broadcast address
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_addr.s_addr = INADDR_ANY;  // Listen on all interfaces
    broadcastAddr.sin_port = htons(broadcastPort);

    // Bind the socket to the broadcast address and port
    if (bind(sockfd, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr)) < 0) {
        std::cerr << "Binding failed\n";
        close(sockfd);
        return -1;
    }

    std::cout << "Waiting for broadcast messages...\n";

    // Receive broadcast messages
    while (true) {
        int recvLen = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&broadcastAddr, &addrLen);
        if (recvLen < 0) {
            std::cerr << "Receiving failed\n";
            break;
        }

        // Null-terminate the received data
        buffer[recvLen] = '\0';

        std::cout << "Received broadcast message: " << buffer << std::endl;
    }

    close(sockfd);
    return 0;
}
