#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // Create raw socket to capture incoming packets
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock < 0) {
        std::cerr << "Socket creation failed!" << std::endl;
        return 1;
    }

    char buffer[4096];
    struct sockaddr_in sender_addr;
    socklen_t sender_len = sizeof(sender_addr);

    // Receive packets in an infinite loop
    while (true) {
        // Receive a raw packet
        ssize_t packet_len = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&sender_addr, &sender_len);
        if (packet_len < 0) {
            std::cerr << "Failed to receive packet!" << std::endl;
            close(sock);
            return 1;
        }

        std::cout << "Packet received from: " << inet_ntoa(sender_addr.sin_addr) << std::endl;
        std::cout << "Packet data: " << std::string(buffer, packet_len) << std::endl;
    }

    close(sock);
    return 0;
}
