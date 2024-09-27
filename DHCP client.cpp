#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define PORT 67  // DHCP standard port

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[1024];

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Send DHCP request
    const char *msg = "REQUEST_IP";
    sendto(client_socket, msg, strlen(msg), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));

    // Receive IP address from server
    socklen_t len = sizeof(server_addr);
    int n = recvfrom(client_socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&server_addr, &len);
    buffer[n] = '\0';
    std::cout << "Assigned IP from server: " << buffer << std::endl;

    close(client_socket);
    return 0;
}
