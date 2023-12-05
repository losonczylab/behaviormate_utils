
//NetComm::NetComm(uint8_t* ip, uint8_t* mac, int addr)
NetComm::NetComm(int addr)
{
    byte mymac[MAC_LEN];
    byte myip[IP_LEN];
    if (addr == 0)
    {
        byte _mymac[] = { ARDUINO_MAC_0 };
        byte _myip[] = { ARDUINO_IP_0 };
        memcpy(mymac, _mymac, sizeof(mymac));
        memcpy(myip, _myip, sizeof(myip));
        this->d_port = PC_PORT_0;
        this->src_port = PC_PORT_0;
        this->local_port = ARDUINO_PORT_0;
    }
    else
    {
        byte _mymac[] = { ARDUINO_MAC_1 };
        byte _myip[] = { ARDUINO_IP_1 };
        memcpy(mymac, _mymac, sizeof(mymac));
        memcpy(myip, _myip, sizeof(myip));
        this->d_port = PC_PORT_1;
        this->src_port = PC_PORT_1;
        this->local_port = ARDUINO_PORT_1;
    }

    Ethernet.begin(mymac, myip);
    Udp.begin(this->local_port);

    Serial.print("IP: ");
    for (int i = 0; i < 4; i++)
    {
        Serial.print(myip[i]);
    }

    byte d_ip_[] = { BEHAVIOR_PC_IP };
    memcpy(this->d_ip, d_ip_, sizeof(d_ip_));
}


void NetComm::reset(int addr)
{
    byte mymac[MAC_LEN];
    byte myip[IP_LEN];
    if (addr == 0)
    {
        byte _mymac[] = { ARDUINO_MAC_0 };
        byte _myip[] = { ARDUINO_IP_0 };
        memcpy(mymac, _mymac, sizeof(mymac));
        memcpy(myip, _myip, sizeof(myip));
        this->d_port = PC_PORT_0;
        this->src_port = PC_PORT_0;
        this->local_port = ARDUINO_PORT_0;
    }
    else
    {
        byte _mymac[] = { ARDUINO_MAC_1 };
        byte _myip[] = { ARDUINO_IP_1 };
        memcpy(mymac, _mymac, sizeof(mymac));
        memcpy(myip, _myip, sizeof(myip));
        this->d_port = PC_PORT_1;
        this->src_port = PC_PORT_1;
        this->local_port = ARDUINO_PORT_1;
    }

    Ethernet.begin(mymac, myip);
    Udp.begin(this->local_port);

    Serial.print("IP: ");
    for (int i = 0; i < 4; i++)
    {
        Serial.print(myip[i]);
    }
}


void NetComm::message_callback(int packetSize, char* msg_buffer)
{
    //if (this->src_port != Udp.remotePort())
    //{
        //IPAddress remote = Udp.remoteIP();
        //for (int i = 0; i < IP_LEN; i++)
        //{
        //    this->d_ip[i] = remote[i];
        //}
        //this->d_port = Udp.remotePort();
        //this->src_port = Udp.remotePort();
        //this->sendMessage("starting");
    //}

    Udp.read(packetBuffer, 1024);
    //TODO: Why is this necessary?
    if (packetSize == 116 || packetSize == 100 || packetSize == 212)
    {
        memcpy(&packetBuffer[packetSize], ",", 1);
        packetSize++;
    }
    packetBuffer[packetSize] = '\0';
    strcpy(msg_buffer, packetBuffer);
}

void NetComm::sendMessage(const char* textToSend)
{
    Udp.beginPacket(this->d_ip, this->d_port);
    Udp.write(textToSend);
    Udp.endPacket();
}

bool NetComm::update(char* msg_buffer)
{
    int packetSize = Udp.parsePacket();
    if (packetSize) {
        this->message_callback(packetSize, msg_buffer);
        return true;
    }

    return false;
}
