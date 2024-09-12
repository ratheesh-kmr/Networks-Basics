#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sockfd;
    struct sockaddr_in broadcastAddr;
    int broadcastPermission = 1;
    const char *broadcastMessage = "This is a broadcast message";

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed\n";
        return -1;
    }

    // Allow socket to send broadcast messages
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastPermission, sizeof(broadcastPermission)) < 0) {
        std::cerr << "Error in setting broadcast permission\n";
        return -1;
    }

    // Set up the broadcast address
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_addr.s_addr = inet_addr("255.255.255.255");  // Broadcast address
    broadcastAddr.sin_port = htons(8080);  // Port to broadcast on

    // Send broadcast message
    if (sendto(sockfd, broadcastMessage, strlen(broadcastMessage), 0, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr)) < 0) {
        std::cerr << "Broadcast message failed\n";
        return -1;
    }

    std::cout << "Broadcast message sent successfully\n";

    close(sockfd);
    return 0;
}
