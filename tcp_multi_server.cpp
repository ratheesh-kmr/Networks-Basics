#include <iostream>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void* handle_client(void* client_socket) {
    int client_fd = *(int*)client_socket;
    char buffer[BUFFER_SIZE];

    while (true) {
        bzero(buffer, BUFFER_SIZE);
        int n = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (n <= 0) {
            std::cout << "Client disconnected or error occurred." << std::endl;
            break;
        }
        std::cout << "Message received: " << buffer << std::endl;

        n = write(client_fd, buffer, strlen(buffer));
        if (n < 0) {
            error("ERROR writing to socket");
        }

        if (strcmp(buffer, "exit") == 0) {
            break;
        }
    }

    close(client_fd);
    pthread_exit(NULL);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        error("ERROR opening socket");
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        error("ERROR on binding");
    }

    listen(server_fd, 5);
    client_len = sizeof(client_addr);

    while (true) {
        client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_len);
        if (client_fd < 0) {
            error("ERROR on accept");
        }

        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handle_client, (void*)&client_fd) != 0) {
            error("ERROR creating thread");
        }
        pthread_detach(client_thread);
    }

   
    close(server_fd);
    return 0;
}
