#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 67  // DHCP standard port

std::vector<std::string> IP_POOL = {
    "192.168.1.10", "192.168.1.11", "192.168.1.12", "192.168.1.13", "192.168.1.14"
};
std::map<int, std::string> ALLOCATED_IPS; // Mapping client (port) -> IP

void handle_dhcp_request(int server_socket, struct sockaddr_in &client_addr) {
    socklen_t client_len = sizeof(client_addr);
    int client_id = ntohs(client_addr.sin_port); // Using port as a client ID
    
    if (ALLOCATED_IPS.find(client_id) != ALLOCATED_IPS.end()) {
        // Client already has an IP assigned
        std::string ip = ALLOCATED_IPS[client_id];
        std::cout << "Reallocating IP " << ip << " to client " << client_id << std::endl;
        sendto(server_socket, ip.c_str(), ip.length(), 0, (struct sockaddr *)&client_addr, client_len);
    } else {
        // Assign a new IP if available
        if (!IP_POOL.empty()) {
            std::string ip = IP_POOL.back();
            IP_POOL.pop_back();  // Remove the assigned IP from the pool
            ALLOCATED_IPS[client_id] = ip;
            std::cout << "Assigned IP " << ip << " to client " << client_id << std::endl;
            sendto(server_socket, ip.c_str(), ip.length(), 0, (struct sockaddr *)&client_addr, client_len);
        } else {
            std::string msg = "No IP available";
            sendto(server_socket, msg.c_str(), msg.length(), 0, (struct sockaddr *)&client_addr, client_len);
        }
    }
}

int main() {
    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[1024];

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    // Bind the socket to DHCP port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        close(server_socket);
        return -1;
    }

    std::cout << "DHCP Server running on port " << PORT << std::endl;

    while (true) {
        socklen_t len = sizeof(client_addr);
        int n = recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &len);
        buffer[n] = '\0';
        std::cout << "Received request from client" << std::endl;

        handle_dhcp_request(server_socket, client_addr);
    }

    close(server_socket);
    return 0;
}
