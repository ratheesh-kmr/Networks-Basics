#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

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

// Function to calculate checksum
unsigned short checksum(void *b, int len) {
    unsigned short *buf = (unsigned short *)b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2) {
        sum += *buf++;
    }

    if (len == 1) {
        sum += *(unsigned char*)buf;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

int main() {
    // Create a raw socket
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0) {
        std::cerr << "Socket creation failed!" << std::endl;
        return 1;
    }

    // Enable IP_HDRINCL option
    int one = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        std::cerr << "Error setting IP_HDRINCL option!" << std::endl;
        close(sock);
        return 1;
    }

    // Destination address
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr("192.168.1.2");  // Receiver's IP

    // Packet
    char packet[4096];
    memset(packet, 0, 4096);

    // Fill in IP header
    struct ipheader *iph = (struct ipheader *)packet;
    iph->iph_ver = 4;  // IPv4
    iph->iph_ihl = 5;  // IP header length
    iph->iph_tos = 0;
    iph->iph_len = sizeof(struct ipheader);
    iph->iph_ident = htons(54321);  // Identification
    iph->iph_ttl = 64;  // Time to live
    iph->iph_protocol = IPPROTO_TCP;  // Protocol
    iph->iph_sourceip = inet_addr("192.168.1.1");  // Sender's IP
    iph->iph_destip = inet_addr("192.168.1.2");    // Receiver's IP

    // Calculate checksum
    iph->iph_chksum = checksum((void *)packet, iph->iph_len);

    // Send packet
    if (sendto(sock, packet, iph->iph_len, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
        std::cerr << "Packet send failed!" << std::endl;
        close(sock);
        return 1;
    }

    std::cout << "Packet sent successfully!" << std::endl;

    close(sock);
    return 0;
}
