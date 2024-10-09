#include <iostream>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12346
#define MULTICAST_GROUP "239.0.0.1"  // Multicast IP address
#define TTL_VALUE 64  // Time-to-Live for multicast packets
#define BUFFER_SIZE 1024

using namespace std;

int main() {
    int sock;
    struct sockaddr_in multicastAddr;
    char buffer[BUFFER_SIZE];
    int rounds, marksPerQuestion;

    // Create a socket for UDP
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Zero out the multicast address struct
    memset(&multicastAddr, 0, sizeof(multicastAddr));
    multicastAddr.sin_family = AF_INET;
    multicastAddr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);  // Multicast address
    multicastAddr.sin_port = htons(PORT);  // Multicast port

    // Set the TTL for multicast messages
    unsigned char ttl = TTL_VALUE;
    if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
        perror("Setting TTL failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Ask for quiz details
    cout << "Enter the number of quiz rounds: ";
    cin >> rounds;
    cout << "Enter marks per question: ";
    cin >> marksPerQuestion;

    // Start sending questions to multicast group
    for (int round = 1; round <= rounds; round++) {
        string question = "Question " + to_string(round) + ": What is the answer to question " + to_string(round) + "?";
        cout << "Sending: " << question << endl;

        // Send the question to the multicast group
        if (sendto(sock, question.c_str(), question.length(), 0, (struct sockaddr*)&multicastAddr, sizeof(multicastAddr)) < 0) {
            perror("Failed to send question");
            close(sock);
            exit(EXIT_FAILURE);
        }

        // Simulate waiting for teams to answer (in a real scenario, wait for client responses here)
        cout << "Waiting for teams to answer...\n";
        sleep(10);  // Wait 5 seconds before the next round
    }

    cout << "Quiz completed. All questions sent.\n";
    close(sock);
    return 0;
}
