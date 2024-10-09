#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <map>

#define MULTICAST_GROUP "239.0.0.1"  // Multicast address
#define MULTICAST_PORT 12346        // Multicast port
#define MAX_TEAMS 5
#define TIME_TO_ANSWER 10            // Time in seconds
#define BUFFER_SIZE 256

struct Team {
    std::string name;
    int score;
};

int main() {
    int sockfd, udp_sock;
    struct sockaddr_in multicast_addr, udp_addr;
    char buffer[BUFFER_SIZE];
    
    // Initialize teams
    std::vector<Team> teams = {{"Team A", 0}, {"Team B", 0}, {"Team C", 0}, {"Team D", 0}, {"Team E", 0}};
    int current_team = 0;
    int rounds = 3;
    int marks_per_question = 10;

    // Create UDP socket for multicasting
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Failed to create multicast socket.\n";
        return -1;
    }

    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_port = htons(MULTICAST_PORT);
    multicast_addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);

    // UDP socket for receiving answers
    udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sock < 0) {
        std::cerr << "Failed to create UDP socket.\n";
        return -1;
    }

    memset(&udp_addr, 0, sizeof(udp_addr));
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_port = htons(MULTICAST_PORT);
    udp_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(udp_sock, (struct sockaddr*)&udp_addr, sizeof(udp_addr)) < 0) {
        std::cerr << "Failed to bind UDP socket.\n";
        return -1;
    }

    for (int round = 1; round <= rounds; ++round) {
        for (int q = 0; q < MAX_TEAMS; ++q) {
            std::string question = "Question for " + teams[current_team].name + ": What is 2 + 2?";
            sendto(sockfd, question.c_str(), question.size(), 0, (struct sockaddr*)&multicast_addr, sizeof(multicast_addr));
            std::cout << "Sent: " << question << "\n";

            // Wait for the team's answer (with a timeout)
            fd_set read_fds;
            struct timeval timeout;
            timeout.tv_sec = TIME_TO_ANSWER;
            timeout.tv_usec = 0;

            FD_ZERO(&read_fds);
            FD_SET(udp_sock, &read_fds);

            int activity = select(udp_sock + 1, &read_fds, NULL, NULL, &timeout);
            if (activity > 0 && FD_ISSET(udp_sock, &read_fds)) {
                // Receive the answer
                struct sockaddr_in from_addr;
                socklen_t addr_len = sizeof(from_addr);
                int len = recvfrom(udp_sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&from_addr, &addr_len);
                if (len > 0) {
                    buffer[len] = '\0';
                    std::cout << teams[current_team].name << " answered: " << buffer << "\n";

                    // Check if the answer is correct
                    if (strcmp(buffer, "4") == 0) {
                        teams[current_team].score += marks_per_question;
                        std::cout << teams[current_team].name << " answered correctly! Current score: " << teams[current_team].score << "\n";
                    } else {
                        std::cout << teams[current_team].name << " answered incorrectly.\n";
                    }
                }
            } else {
                std::cout << teams[current_team].name << " did not answer in time.\n";
            }

            current_team = (current_team + 1) % MAX_TEAMS;
        }
    }

    // Announce final scores
    std::cout << "\nQuiz Over! Final Scores:\n";
    for (const auto& team : teams) {
        std::cout << team.name << ": " << team.score << " points\n";
    }

    close(sockfd);
    close(udp_sock);
    return 0;
}
