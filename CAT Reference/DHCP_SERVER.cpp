#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

class DHCPServer {
private:
    unordered_map<string, string> ipPool;
    unordered_map<string, string> assignedIPs;
    
public:
    DHCPServer() {
        // Initialize a simple pool of IP addresses
        ipPool["192.168.1.2"] = "free";
        ipPool["192.168.1.3"] = "free";
        ipPool["192.168.1.4"] = "free";
    }

    string assignIP(string macAddress) {
        if (assignedIPs.find(macAddress) != assignedIPs.end()) {
            return assignedIPs[macAddress];  // Client already has an IP assigned
        }
        
        for (auto& ip : ipPool) {
            if (ip.second == "free") {
                ip.second = "assigned";
                assignedIPs[macAddress] = ip.first;
                cout << "Assigned IP: " << ip.first << " to MAC: " << macAddress << endl;
                return ip.first;
            }
        }
        
        cout << "No IP available!" << endl;
        return "";
    }

    void releaseIP(string macAddress) {
        if (assignedIPs.find(macAddress) != assignedIPs.end()) {
            string ip = assignedIPs[macAddress];
            assignedIPs.erase(macAddress);
            ipPool[ip] = "free";
            cout << "Released IP: " << ip << " from MAC: " << macAddress << endl;
        }
    }
};

int main() {
    DHCPServer server;
    
    // Simulate DHCP requests from clients
    server.assignIP("00:0a:95:9d:68:16");
    server.assignIP("00:0a:95:9d:68:17");
    server.assignIP("00:0a:95:9d:68:18");
    
    // Simulate releasing an IP
    server.releaseIP("00:0a:95:9d:68:17");
    
    // Simulate another request
    server.assignIP("00:0a:95:9d:68:19");
    
    return 0;
}
