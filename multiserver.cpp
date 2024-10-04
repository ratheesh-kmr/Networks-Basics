#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

#define MULTICAST_GROUP "239.0.0.1"  // Multicast address
#define MULTICAST_PORT 12345         // Multicast port
#define MESSAGE "Hello, Multicast!"

int main() {
    int sockfd;
    struct sockaddr_in multicast_addr;
    
    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Failed to create socket.\n";
        return -1;
    }

    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_port = htons(MULTICAST_PORT);
    multicast_addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);

    // Send multicast message
    if (sendto(sockfd, MESSAGE, strlen(MESSAGE), 0, 
               (struct sockaddr*)&multicast_addr, sizeof(multicast_addr)) < 0) {
        std::cerr << "Failed to send message.\n";
        close(sockfd);
        return -1;
    }

    std::cout << "Multicast message sent.\n";
    close(sockfd);
    return 0;
}
