#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// DHCP protocol constants
#define DHCP_DISCOVER 1
#define BOOTREQUEST 1
#define BOOTP_BROADCAST 0x8000
#define DHCP_MAGIC_COOKIE 0x63825363

// DHCP message structure
struct dhcpMessage {
    uint8_t op;         // Message opcode/type
    uint8_t htype;      // Hardware address type
    uint8_t hlen;       // Hardware address length
    uint8_t hops;       // Number of relay agent hops from client
    uint32_t xid;       // Transaction ID
    uint16_t secs;      // Seconds since client started looking
    uint16_t flags;     // DHCP flags
    uint32_t ciaddr;    // Client IP address (if already in use)
    uint32_t yiaddr;    // Client IP address (offered by server)
    uint32_t siaddr;    // IP address of next server to use in bootstrap
    uint32_t giaddr;    // Relay agent IP address
    unsigned char chaddr[16];   // Client hardware address
    char sname[64];     // Optional server host name
    char file[128];     // Boot file name
    uint32_t magicCookie;
    uint8_t options[312];   // Optional parameters field
};

void sendDhcpDiscover(int sock, struct sockaddr_in* dest) {
    dhcpMessage discover;
    memset(&discover, 0, sizeof(discover));

    discover.op = BOOTREQUEST;
    discover.htype = 1;  // Ethernet
    discover.hlen = 6;   // MAC address length
    discover.hops = 0;
    discover.xid = htonl(0x3903F326); // Transaction ID
    discover.secs = 0;
    discover.flags = htons(BOOTREQUEST);
    discover.magicCookie = htonl(DHCP_MAGIC_COOKIE);

    // Set DHCP options (DHCP DISCOVER)
    discover.options[0] = 53;   // DHCP Message Type
    discover.options[1] = 1;    // Length of option data
    discover.options[2] = DHCP_DISCOVER;

    if (sendto(sock, &discover, sizeof(discover), 0, (struct sockaddr*)dest, sizeof(*dest)) < 0) {
        perror("Failed to send DHCP discover");
    } else {
        std::cout << "DHCP Discover message sent." << std::endl;
    }
}

int main() {
    int sock;
    struct sockaddr_in dest;
    
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        perror("Failed to create socket");
        return 1;
    }

    
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(67);  // DHCP server port
    dest.sin_addr.s_addr = inet_addr("255.255.255.255");

    // Enable broadcast
    int broadcastEnable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        perror("Failed to set socket to broadcast mode");
        close(sock);
        return 1;
    }

    
    sendDhcpDiscover(sock, &dest);

    close(sock);
    return 0;
}
