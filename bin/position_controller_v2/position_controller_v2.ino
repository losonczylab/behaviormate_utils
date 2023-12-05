#include "netcomm.h"
#include "op_new.h"

const unsigned char pinRSel = 9; //select pin for the multiplexer
const unsigned char pinRead = 8; //latch pin to enable the latch on the counters, active low
const unsigned char pinCntRst = 3; //reset pin for the counters, active high

const unsigned char pinLRIN = 4; //lap reset input pin
const unsigned char pinLRCLR = 5; //lap reset clear pin for clearing the flip flop

// if lap reset fails, prevent arduino from getting stuck sending duplicate lap
// reset messages
static bool lapLock;

static unsigned int prevPos = 0;
static unsigned int curPos = 0;
static int diffPos = 0;
static unsigned char *pos8BitArr = (unsigned char *)(&curPos);
static unsigned char readByte = 0;

static unsigned long tMillis;
static unsigned long pMillis;
static long diffT = 0;

//data pin mapping:
//(L): low byte, (H): high byte
//pin       counter bit
//14  (A0): D01L
//15  (A1): D23L
//16  (A2): D54L
//17  (A3): D76L
//18  (A4): D01H
//19  (A5): D23H
//6   (D6): D54H
//7   (D7): D76H

//bit mapping
//pinRSel   LOW  HIGH
//bit 0:    D0L  D1L
//bit 1:    D2L  D3L
//bit 2:    D5L  D4L
//bit 3:    D7L  D6L
//bit 4:    D0H  D1H
//bit 5:    D2H  D3H
//bit 6:    D5H  D4H
//bit 7:    D7H  D6H

const char* positionMessage = "{\"position\":{\"dy\":%d, \"dt\":%ld},\
 \"millis\":%lu}";
const char* lapMessage = "{\"lap_reset\":{\"type\":\"IR 2 pin\"},\
 \"millis\":%lu}";
static char messageBuffer[255];

static char ncMem[sizeof(NetComm)];
NetComm *nc;

void callback(uint16_t dest_port, uint8_t src_ip[IP_LEN], uint16_t src_port,
              uint8_t *src_mac, const char *data, uint16_t len)
{
    nc->message_callback(dest_port, src_ip, src_port, src_mac, data, len);
}

void setup()
{
  Serial.begin(115200);
  pMillis = millis();
  nc=new (ncMem) NetComm();
  nc->set_callback(&callback, 5005);

  //A0-A5 as digital input pins
  DDRC = 0;
  //D6-D7 as digital input pins
  DDRD = DDRD & B00111111;


  //lap reset pins
  pinMode(pinLRCLR, INPUT);
  pinMode(pinLRIN, INPUT);
  lapLock = false;

  //control pins
  pinMode(pinRSel, OUTPUT);
  pinMode(pinRead, OUTPUT);
  pinMode(pinCntRst, OUTPUT);

  digitalWrite(pinRSel, LOW);

  //reset counters
  digitalWrite(pinCntRst, LOW);
  delayMicroseconds(10);
  digitalWrite(pinCntRst, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinCntRst, LOW);

  //disable read latch
  digitalWrite(pinRead, HIGH);
}

void loop()
{
  nc->update();
  for(int i=0; i<1800; i++)
  {
    if(digitalRead(pinLRIN)==LOW)
    {
      lapLock = false;
    }
    else if ((!lapLock) && (digitalRead(pinLRIN)==HIGH))
    {
      tMillis = millis();
      pinMode(pinLRCLR, OUTPUT);
      digitalWrite(pinLRCLR, LOW);
      delayMicroseconds(1);
      pinMode(pinLRCLR, INPUT);
      sprintf(messageBuffer, lapMessage, tMillis);
//       Serial.println(messageBuffer);
      nc->sendMessage(messageBuffer);
      lapLock = true;
    }
  }

  digitalWrite(pinRead, LOW);
  tMillis = millis();
  //read bits 0L, 2L, 5L, 7L, 0H, 2H, 5H, 7H
  digitalWrite(pinRSel, LOW);
  delayMicroseconds(1);
  readByte = B11000000 & PIND;
  readByte = readByte | (B00111111 & PINC);
  digitalWrite(pinRSel, HIGH);
//  pos8BitArr[0]=readByte;
  pos8BitArr[0] =                  ((readByte & B00000001)>0)*B00000001;
  pos8BitArr[0] = pos8BitArr[0] | (((readByte & B00000010)>0)*B00000100);
  pos8BitArr[0] = pos8BitArr[0] | (((readByte & B00000100)>0)*B00100000);
  pos8BitArr[0] = pos8BitArr[0] | (((readByte & B00001000)>0)*B10000000);
  pos8BitArr[1] =                  ((readByte & B00010000)>0)*B00000001;
  pos8BitArr[1] = pos8BitArr[1] | (((readByte & B00100000)>0)*B00000100);
  pos8BitArr[1] = pos8BitArr[1] | (((readByte & B01000000)>0)*B00100000);
  pos8BitArr[1] = pos8BitArr[1] | (((readByte & B10000000)>0)*B10000000);
  
  //read bits 1L, 3L, 4L, 6L, 1H, 3H, 4H, 6H
  readByte = B11000000 & PIND;
  readByte = readByte | (B00111111 & PINC);
  digitalWrite(pinRSel, LOW);
//  pos8BitArr[0]=readByte;
  pos8BitArr[0] = pos8BitArr[0] | (((readByte & B00000001)>0)*B00000010);
  pos8BitArr[0] = pos8BitArr[0] | (((readByte & B00000010)>0)*B00001000);
  pos8BitArr[0] = pos8BitArr[0] | (((readByte & B00000100)>0)*B00010000);
  pos8BitArr[0] = pos8BitArr[0] | (((readByte & B00001000)>0)*B01000000);
  pos8BitArr[1] = pos8BitArr[1] | (((readByte & B00010000)>0)*B00000010);
  pos8BitArr[1] = pos8BitArr[1] | (((readByte & B00100000)>0)*B00001000);
  pos8BitArr[1] = pos8BitArr[1] | (((readByte & B01000000)>0)*B00010000);
  pos8BitArr[1] = pos8BitArr[1] | (((readByte & B10000000)>0)*B01000000);

  //end read
  digitalWrite(pinRead, HIGH);

  diffPos = curPos-prevPos;
  diffT = tMillis-pMillis;
  pMillis = tMillis;

  if (diffPos != 0)
  {
    sprintf(messageBuffer, positionMessage, diffPos, diffT, tMillis);
//     Serial.println(messageBuffer);
    nc->sendMessage(messageBuffer);

    prevPos = curPos;
  }
}
