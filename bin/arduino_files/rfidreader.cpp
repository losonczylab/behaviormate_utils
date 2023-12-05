#include "rfidreader.h"

#define NECESSARY_STEPS 20


RfidReader::RfidReader(JsonObject& params, Communicator* communicator) : Sensor()
{
    communicator->sendMessage("creating rfid reader");
    if (!params.containsKey("pin"))
    {
        debug("rfid reader requires pin (reader reset pin)");
        return;
    }
    this->pin = params["pin"].as<int>();
    this->report_pin = -1;
    if (params.containsKey("report_pin"))
    {
        this->report_pin = params["report_pin"].as<int>();
        pinMode(this->report_pin, OUTPUT); 
        digitalWrite(this->report_pin, LOW);
    }

    pinMode(this->pin, OUTPUT);
    digitalWrite(this->pin, HIGH);
    digitalWrite(this->pin, LOW);
    digitalWrite(this->pin, HIGH);

    lastUpdate = millis();
    this->communicator = communicator;
    this->prevTag[0] = 0;
    this->currentTag[0] = 0;
    this->read_index = 0;
    this->reading = false;

    JsonObject& create_subjson = this->jsonBuffer.createObject();
    create_subjson["pin"] = pin;
    create_subjson["action"] = "created";
    JsonObject& create_json = this->jsonBuffer.createObject();
    create_json["tag_reader"] = create_subjson;
    communicator->sendMessage(create_json);
}

bool RfidReader::detect()
{
  // Delay is needed for RFID chip to reset
  unsigned long currentTime = millis();
  if (currentTime - lastUpdate > 150)
  {
      this->state = false;
      if (!Serial1.available()) 
      {
          this->prevTag[0] = 0;
      } 

      for (int i = 0; Serial1.available() && (i < 20); i++)
      {
          if (this->report_pin != -1)
          {
              digitalWrite(this->report_pin, HIGH);
          }
          char newChar = (char)Serial1.read();

          if (newChar == 3)
          {
              this->currentTag[read_index%20] = 0;
              if (strcmp(this->currentTag, this->prevTag) != 0)
              {
                  sprintf(this->prevTag, "%s", this->currentTag);
                  StaticJsonBuffer<500> buffer;
                  JsonObject& json = buffer.createObject();
                  JsonObject& subjson = buffer.createObject();
                  subjson["id"] = this->pin;
                  subjson["tag"] = this->currentTag;;
                  subjson["action"] = "read";
                  json["tag_reader"] = subjson;

                  communicator->sendMessage(json);
                  this->state = true;

                  digitalWrite(this->pin, LOW);
                  digitalWrite(this->pin, HIGH);
                  lastUpdate = currentTime;
              }

              this->read_index = 0;
              this->reading = false;
              break;
          }
          else if (newChar == 2)
          {
              this->reading = true;
              read_index = 0;
          }
          else if ((this->reading) && (newChar != '\n') && (newChar != '\r'))
          {
              this->currentTag[read_index%20] = newChar;
              read_index++;
          }
      }

  }

  return true;
}
