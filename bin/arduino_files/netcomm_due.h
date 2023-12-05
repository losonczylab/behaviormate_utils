#ifndef h_netcomm_due
#define h_netcomm_due

#include <SPI.h>
//#include <Ethernet2.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
//#include <UIPEthernet.h>

#define MAC_LEN 6
#define IP_LEN 4

class NetComm
{
public:
    //NetComm(uint8_t* ip, uint8_t* mac, int addr=0);
    NetComm(int addr=0);
    void reset(int addr=0);
    void sendMessage(const char* textToSend);
    bool update(char* msg_buffer);
    void message_callback(int packetSize, char* msg_buffer);

private:
    int d_port;
    int src_port;
    int local_port;
    byte d_ip[IP_LEN];
    byte d_mac[MAC_LEN];
    EthernetUDP Udp;
    char packetBuffer[1024];
};
#endif
