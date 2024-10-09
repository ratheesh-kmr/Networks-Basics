#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

#define MULTICAST_GROUP "239.0.0.1"  // Multicast address
#define MULTICAST_PORT 12346         // Multicast port
#define BUFFER_SIZE 256

int main() {
    int sockfd;
    struct sockaddr_in multicast_addr, server_addr;
    struct ip_mreq multicast_request;
    char buffer[BUFFER_SIZE];

    // Create a UDP socket for receiving multicast messages
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Failed to create socket.\n";
        return -1;
    }

    // Set up the multicast address
    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_port = htons(MULTICAST_PORT);
    multicast_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket to the multicast port
    if (bind(sockfd, (struct sockaddr*)&multicast_addr, sizeof(multicast_addr)) < 0) {
        std::cerr << "Failed to bind socket.\n";
        close(sockfd);
        return -1;
    }

    // Join the multicast group
    multicast_request.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    multicast_request.imr_interface.s_addr = htonl(INADDR_ANY);

    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&multicast_request, sizeof(multicast_request)) < 0) {
        std::cerr << "Failed to join multicast group.\n";
        close(sockfd);
        return -1;
    }

    std::cout << "Joined the multicast group, waiting for quiz questions...\n";

    while (true) {
        // Receive the question from the server
        memset(buffer, 0, BUFFER_SIZE);
        int len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
        if (len < 0) {
            std::cerr << "Error receiving question.\n";
            break;
        }

        buffer[len] = '\0';
        std::cout << "Received: " << buffer << "\n";

        // Simulate answering the question
        std::string answer;
        std::cout << "Enter your answer (e.g., 4): ";
        std::cin >> answer;

        // Send the answer back to the server
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(MULTICAST_PORT);
        server_addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);

        if (sendto(sockfd, answer.c_str(), answer.length(), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << "Failed to send answer.\n";
        }

        std::cout << "Answer sent: " << answer << "\n";
    }

    // Leave the multicast group and close the socket
    setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, (void*)&multicast_request, sizeof(multicast_request));
    close(sockfd);
    return 0;
}
