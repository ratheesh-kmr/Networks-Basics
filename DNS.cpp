#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

void resolveHostname(const char* hostname) {
    struct addrinfo hints, *res;
    struct sockaddr_in *ipv4;
    char ipstr[INET_ADDRSTRLEN];

    hints.ai_family = AF_INET; // AF_INET means IPv4, AF_INET6 means IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    // Resolve the hostname
    int status = getaddrinfo(hostname, NULL, &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
        return;
    }

    // Extract the IP address
    ipv4 = (struct sockaddr_in *)res->ai_addr;
    inet_ntop(res->ai_family, &(ipv4->sin_addr), ipstr, sizeof ipstr);

    std::cout << "IP address for " << hostname << " is: " << ipstr << std::endl;

    freeaddrinfo(res); // free the linked list
}

int main() {
    const char* hostname = "www.example.com"; // Replace with the hostname you want to resolve
    resolveHostname(hostname);
    return 0;
}

