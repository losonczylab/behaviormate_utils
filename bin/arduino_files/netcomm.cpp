
NetComm::NetComm()
{
    byte mymac[] = { ARDUINO_MAC };
    byte myip[] = { ARDUINO_IP };

    if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
    {
        Serial.println( "Failed to access Ethernet controller");
    }

    ether.staticSetup(myip);

    ether.printIp("IP: ", ether.myip);
    this->d_port = UDP_PORT;
    this->src_port = -1;
    this->send_port = UDP_PORT;

    byte d_ip_[] = { BEHAVIOR_PC_IP };
    memcpy(this->d_ip, d_ip_, sizeof(d_ip_));
    byte d_mac_[] = { BEHAVIOR_PC_MAC };
    memcpy(this->d_mac, d_mac_, sizeof(d_mac_));
}

void NetComm::set_callback(void* cb, int port)
{
    ether.udpServerListenOnPort(cb, port);
}

void NetComm::message_callback(uint16_t dest_port, uint8_t src_ip[IP_LEN],
                               uint16_t src_port, uint8_t *src_mac,
                               const char *data, uint16_t len)
{
    if (src_port != this->src_port)
    {
        memcpy(this->d_mac, src_mac, MAC_LEN*sizeof(uint8_t));
        memcpy(this->d_ip, src_ip, IP_LEN*sizeof(uint8_t));
        this->src_port = src_port;
    }

    this->sendMessage("ack");
}

void NetComm::sendMessage(char* textToSend)
{
    ether.sendUdp(textToSend, strlen(textToSend), this->send_port, this->d_ip,
                  this->d_port, this->d_mac );
}

void NetComm::update()
{
    ether.packetLoop(ether.packetReceive());
}
