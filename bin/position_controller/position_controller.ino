#include "netcomm.h"
#include "op_new.h"

static unsigned char pinS1 = 9;
static unsigned char pinRR = 8;
static unsigned char pinCR = 5;

static unsigned char pinLRIN = 4;
static unsigned char pinLRCLR = 3;

// if lap reset fails, prevent arduino from getting stuck sending duplicate lap
// reset messages
bool lap_lock;

int prevPos = 0;
int curPos = 0;
int diffPos = 0;
unsigned char *pos8BitArr = (unsigned char *)(&curPos);

unsigned long tMillis;
unsigned long pMillis;
long diffT = 0;

const char* position_message = "{\"position\":{\"dy\":%d, \"dt\":%ld},\
 \"millis\":%lu}";
const char* lap_message = "{\"lap_reset\":{\"type\":\"IR 2 pin\"},\
 \"millis\":%lu}";
char message_buffer[255];

static char ncMem[sizeof(NetComm)];
NetComm* nc;
//TODO: Is there a better solution?
void callback(uint16_t dest_port, uint8_t src_ip[IP_LEN], uint16_t src_port,
              uint8_t *src_mac, const char *data, uint16_t len)
{
    nc->message_callback(dest_port, src_ip, src_port, src_mac, data, len);
}

void setup()
{
  Serial.begin(115200);
  pMillis = millis();
  nc = new (ncMem) NetComm();
  nc->set_callback(&callback, 5005);

  //A0-A5 as digital input pins, for reading the data from
  //HCTL2017 bit 0-5 (D0-D5)
  DDRC = 0;
  //D6-D7 as digital input pins for reading bit 6-7 (D6, D7)
  DDRD = DDRD & B00111111;


  //lap reset pins
  pinMode(pinLRCLR, INPUT);
  pinMode(pinLRIN, INPUT);
  lap_lock = false;

  //control pins
  pinMode(pinS1, OUTPUT);
  pinMode(pinRR, OUTPUT);
  pinMode(pinCR, OUTPUT);

  //reset EC (CR)
  digitalWrite(pinCR, LOW);
  delayMicroseconds(10);
  digitalWrite(pinCR, HIGH);

  //disable read latch
  digitalWrite(pinRR, HIGH);
}

void loop()
{
  nc->update();
  for(int i=0; i<1800; i++)
  {
    if(digitalRead(pinLRIN)==LOW)
    {
      lap_lock=false;
    }
    else if ((!lap_lock) && (digitalRead(pinLRIN)==HIGH))
    {
      tMillis=millis();
      pinMode(pinLRCLR, OUTPUT);
      digitalWrite(pinLRCLR, LOW);
      delayMicroseconds(1);
      pinMode(pinLRCLR, INPUT);
  
      sprintf(message_buffer, lap_message, tMillis);
//      Serial.println(message_buffer);
      nc->sendMessage(message_buffer);
      lap_lock = true;
    }
  }

  digitalWrite(pinRR, LOW);
  tMillis=millis();
  //read MSB
  digitalWrite(pinS1, LOW);
  delayMicroseconds(2);
  pos8BitArr[1] = B11000000 & PIND;
  pos8BitArr[1] = pos8BitArr[1] | B00111111 & PINC;

  //read LSB
  digitalWrite(pinS1, HIGH);
  delayMicroseconds(2);
  pos8BitArr[0] = B11000000 & PIND;
  pos8BitArr[0] = pos8BitArr[0] | B00111111 & PINC;

  //end read
  digitalWrite(pinRR, HIGH);
  delayMicroseconds(2);

  diffPos = curPos-prevPos;
  diffT = tMillis-pMillis;
  pMillis = tMillis;

  if (diffPos != 0)
  {
    sprintf(message_buffer, position_message, diffPos, diffT, tMillis);
//    Serial.println(message_buffer);
    nc->sendMessage(message_buffer);

    prevPos = curPos;
  }
}
