#include <iostream>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12345
#define MULTICAST_GROUP "239.0.0.1"  // Multicast IP address
#define BUFFER_SIZE 1024

using namespace std;

int main() {
    int sock;
    struct sockaddr_in serverAddr;
    struct ip_mreq multicastRequest;
    char buffer[BUFFER_SIZE];

    // Create a socket for UDP
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Zero out the server address struct
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);  // Receive from any address
    serverAddr.sin_port = htons(PORT);

    // Bind the socket to the multicast port
    if (bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Binding failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Specify the multicast group address and join it
    multicastRequest.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&multicastRequest, sizeof(multicastRequest)) < 0) {
        perror("Failed to join multicast group");
        close(sock);
        exit(EXIT_FAILURE);
    }

    cout << "Client has joined the multicast group, waiting for quiz questions...\n";

    while (true) {
        // Receive quiz question
        memset(buffer, 0, BUFFER_SIZE);
        int len = recvfrom(sock, buffer, BUFFER_SIZE, 0, NULL, 0);
        if (len < 0) {
            perror("Failed to receive question");
            break;
        }

        cout << "Received Question: " << buffer << endl;

        // Simulate answering the question
        cout << "Enter your answer (e.g., A/B/C/D): ";
        string answer;
        cin >> answer;

        // Send answer back to the server
        if (sendto(sock, answer.c_str(), answer.length(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            perror("Failed to send answer");
        }
    }

    // Leave multicast group and close the socket
    setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (void*)&multicastRequest, sizeof(multicastRequest));
    close(sock);
    return 0;
}
