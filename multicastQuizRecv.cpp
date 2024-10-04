#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

#define MULTICAST_GROUP "239.0.0.1"
#define MULTICAST_PORT 12345
#define BUFFER_SIZE 256

int main() {
    int sockfd;
    struct sockaddr_in local_addr, server_addr;
    struct ip_mreq multicast_request;
    char buffer[BUFFER_SIZE];
    
    // Create UDP socket for receiving multicast messages
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Failed to create socket.\n";
        return -1;
    }

    // Bind to local address
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(MULTICAST_PORT);
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(sockfd, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        std::cerr << "Failed to bind socket.\n";
        return -1;
    }

    // Join multicast group
    multicast_request.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    multicast_request.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&multicast_request, sizeof(multicast_request)) < 0) {
        std::cerr << "Failed to join multicast group.\n";
        return -1;
    }

    while (true) {
        // Receive questions
        int len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, 0);
        if (len < 0) {
            std::cerr << "Failed to receive message.\n";
            break;
        }
        buffer[len] = '\0';
        std::cout << "Received: " << buffer << "\n";

        // Send answer
        std::string answer;
        std::cout << "Enter your answer: ";
        std::cin >> answer;

        int udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (udp_sock < 0) {
            std::cerr << "Failed to create UDP socket.\n";
            break;
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(MULTICAST_PORT);
        server_addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);

        sendto(udp_sock, answer.c_str(), answer.size(), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
        close(udp_sock);
    }

    close(sockfd);
    return 0;
}
