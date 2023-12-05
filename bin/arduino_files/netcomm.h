#ifndef h_netcomm
#define h_netcomm

#include <EtherCard.h>
#define MAC_LEN 6

static byte Ethernet::buffer[700];

class NetComm
{
public:
    NetComm();
    void sendMessage(char* textToSend);
    void update();
    void message_callback(uint16_t dest_port, uint8_t src_ip[IP_LEN],
                  uint16_t src_port, uint8_t *src_mac, const char *data,
                  uint16_t len);
    void set_callback(void* callback, int port);

private:
    int d_port;
    int src_port;
    int send_port;
    byte d_ip[IP_LEN];
    byte d_mac[MAC_LEN];
};
#endif
