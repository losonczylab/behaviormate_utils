#include "netcomm.h"

const unsigned char pinN = 3;

volatile unsigned char count = 0;
volatile unsigned long frame_count = 0;
volatile unsigned long mT = 0;
volatile bool captured = false;
volatile unsigned long captured_frame = 0;

const char* message_template = "{\"count\":%lu, \"millis\":%lu}";
char message_buffer[255];

NetComm* nc;
void callback(uint16_t dest_port, uint8_t src_ip[IP_LEN], uint16_t src_port,
              uint8_t *src_mac, const char *data, uint16_t len)
{
    nc->message_callback(dest_port, src_ip, src_port, src_mac, data, len);
}

void capturePulse()
{
    count++;
    frame_count++;
    if (count >= 1)
    {
        captured = true;
        captured_frame = frame_count;
        count = 0;
        mT = millis();
    }
}

void setup()
{
    nc = new NetComm();
    nc->set_callback(&callback, UDP_PORT);
    attachInterrupt(digitalPinToInterrupt(pinN), capturePulse, RISING);
}

void loop() {
    if (captured)
    {
        captured=false;
        sprintf(message_buffer, message_template, frame_count, mT);
        nc->sendMessage(message_buffer);
    }
}
