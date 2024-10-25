#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

// Define the size of the buffer to store the packet
#define PACKET_SIZE 65536

// IP header structure
struct ipheader {
    unsigned char      iph_ihl:4, iph_ver:4;
    unsigned char      iph_tos;
    unsigned short int iph_len;
    unsigned short int iph_ident;
    unsigned short int iph_flag:3, iph_offset:13;
    unsigned char      iph_ttl;
    unsigned char      iph_protocol;
    unsigned short int iph_chksum;
    unsigned int       iph_sourceip;
    unsigned int       iph_destip;
};

// Function to print the IP header details
void print_ip_header(struct ipheader *iph) {
    std::cout << "-------------------- IP Header --------------------" << std::endl;
    std::cout << "Version: " << (unsigned int)iph->iph_ver << std::endl;
    std::cout << "Header Length: " << (unsigned int)iph->iph_ihl * 4 << " bytes" << std::endl;
    std::cout << "Type of Service: " << (unsigned int)iph->iph_tos << std::endl;
    std::cout << "Total Length: " << ntohs(iph->iph_len) << std::endl;
    std::cout << "Identification: " << ntohs(iph->iph_ident) << std::endl;
    std::cout << "TTL: " << (unsigned int)iph->iph_ttl << std::endl;
    std::cout << "Protocol: " << (unsigned int)iph->iph_protocol << std::endl;
    std::cout << "Checksum: " << ntohs(iph->iph_chksum) << std::endl;
    
    struct in_addr src_addr, dest_addr;
    src_addr.s_addr = iph->iph_sourceip;
    dest_addr.s_addr = iph->iph_destip;

    std::cout << "Source IP: " << inet_ntoa(src_addr) << std::endl;
    std::cout << "Destination IP: " << inet_ntoa(dest_addr) << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;
}

int main() {
    // Create a raw socket to capture incoming packets
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock < 0) {
        std::cerr << "Socket creation failed!" << std::endl;
        return 1;
    }

    // Buffer to hold the incoming packet
    char buffer[PACKET_SIZE];
    struct sockaddr_in sender_addr;
    socklen_t sender_len = sizeof(sender_addr);

    // Receive packets in an infinite loop
    while (true) {
        // Receive a raw packet
        ssize_t packet_len = recvfrom(sock, buffer, PACKET_SIZE, 0, (struct sockaddr *)&sender_addr, &sender_len);
        if (packet_len < 0) {
            std::cerr << "Failed to receive packet!" << std::endl;
            close(sock);
            return 1;
        }

        // Cast the received buffer to the IP header structure
        struct ipheader *iph = (struct ipheader *)buffer;

        // Print the IP header details
        print_ip_header(iph);
    }

    close(sock);
    return 0;
}
