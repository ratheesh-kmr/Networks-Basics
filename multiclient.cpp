#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

#define MULTICAST_GROUP "239.0.0.1"  // Multicast address
#define MULTICAST_PORT 12345         // Multicast port
#define BUFFER_SIZE 256

int main() {
    int sockfd;
    struct sockaddr_in local_addr;
    struct ip_mreq multicast_request;
    char buffer[BUFFER_SIZE];
    
    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Failed to create socket.\n";
        return -1;
    }

    // Allow multiple sockets to use the same port
    u_int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes)) < 0) {
        std::cerr << "Reusing address failed.\n";
        close(sockfd);
        return -1;
    }

    // Bind to any local interface
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(MULTICAST_PORT);
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Listen on any available network interface

    // Bind socket
    if (bind(sockfd, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        std::cerr << "Failed to bind socket. Error: " << strerror(errno) << "\n";
        close(sockfd);
        return -1;
    }

    // Join multicast group
    multicast_request.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    multicast_request.imr_interface.s_addr = htonl(INADDR_ANY);  // Use default network interface
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
                   (char*)&multicast_request, sizeof(multicast_request)) < 0) {
        std::cerr << "Failed to join multicast group. Error: " << strerror(errno) << "\n";
        close(sockfd);
        return -1;
    }

    // Receive multicast messages
    while (true) {
        int len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, 0);
        if (len < 0) {
            std::cerr << "Failed to receive message. Error: " << strerror(errno) << "\n";
            break;
        }
        buffer[len] = '\0';
        std::cout << "Received: " << buffer << "\n";
    }

    close(sockfd);
    return 0;
}
