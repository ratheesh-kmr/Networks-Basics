#include <iostream>
#include <string>
using namespace std;

class DHCPClient {
private:
    string macAddress;
    string ipAddress;

public:
    DHCPClient(string mac) : macAddress(mac), ipAddress("") {}

    void requestIP() {
        cout << "Requesting IP for MAC: " << macAddress << endl;
        // In a real-world application, this would send a DHCPDISCOVER message to the server
    }

    void receiveIP(string ip) {
        ipAddress = ip;
        cout << "Received IP: " << ipAddress << " for MAC: " << macAddress << endl;
    }
    
    void releaseIP() {
        cout << "Releasing IP: " << ipAddress << " for MAC: " << macAddress << endl;
        ipAddress = "";
        // In a real-world application, this would send a DHCPRELEASE message to the server
    }
};

int main() {
    DHCPClient client1("00:0a:95:9d:68:16");
    DHCPClient client2("00:0a:95:9d:68:17");

    client1.requestIP();
    client2.requestIP();

    // The server part would assign IPs to clients, this is just a placeholder
    client1.receiveIP("192.168.1.2");
    client2.receiveIP("192.168.1.3");

    // Releasing IP
    client2.releaseIP();

    return 0;
}
