#include "cap1188.h"

Cap1188::Cap1188(JsonObject& params,
                 Communicator* aCommunicator)
{
    communicator = aCommunicator;

    if (!params.containsKey("pin"))
    {
        this->error("CAP1188 requires power pin to be specified");
        this->connected = false;
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

    if (!params.containsKey("reset_pin"))
    {
        this->error("CAP1188 requires reset pin to be specified");
        this->connected = false;
        return;
    }
    this->reset_pin = params["reset_pin"].as<int>();

    pinMode(this->pin, INPUT);
    //digitalWrite(this->pin, HIGH);
    //delay(10);

    pinMode(this->reset_pin, OUTPUT);

    digitalWrite(this->reset_pin, LOW);
    delay(10);
    digitalWrite(this->reset_pin, HIGH);
    delay(10);
    digitalWrite(this->reset_pin, LOW);
    delay(100);

    this->sensor_status = -1;
    if (params.containsKey("sensor"))
    {
        this->sensor = params["sensor"].as<int>();
    }
    else
    {
        this->sensor = -1;
    }
    initMessages();

    //this->cap = Adafruit_CAP1188(5);
    Wire.begin();
    this->i2caddr = CAP1188_I2CADDR;
    this->readRegister(CAP1188_PRODID);

    // Useful debugging info
    //Serial.print("Product ID: 0x");
    //Serial.println(readRegister(CAP1188_PRODID), HEX);
    //Serial.print("Manuf. ID: 0x");
    //Serial.println(readRegister(CAP1188_MANUID), HEX);
    //Serial.print("Revision: 0x");
    //Serial.println(readRegister(CAP1188_REV), HEX);


    if ( (readRegister(CAP1188_PRODID) != 0x50) ||
         (readRegister(CAP1188_MANUID) != 0x5D) ||
         (readRegister(CAP1188_REV) != 0x83))
    {
        this->connected = false;
        this->error("failed to find CAP1188 touch sensor");
    }
    else
    {
        this->connected = true;

        // Set multiple touches to off
        this->writeRegister(CAP1188_MTBLK, 0x80);
        this->writeRegister(0x44, 0x0);

        // Set "speed up" setting for standby and normal mode
        this->writeRegister(CAP1188_STANDBYCFG, 0x20);
        this->writeRegister(0x24, 0x20);

        if (!params.containsKey("lights") || !params["lights"])
        {
            // 0x72 default 0x00  — Sets LED links to off
            this->writeRegister(CAP1188_LEDLINK, 0x00);
        }
        else
        {
            this->writeRegister(CAP1188_LEDLINK, 0xFF);
        }

        int sensitivity = 7;
        if (params.containsKey("sensitivity"))
        {
            sensitivity = 7 - params["sensitivity"].as<int>();

            if (sensitivity < 0)
            {
                sensitivity = 0;
                this->error(
                    "sensitivity for CAP1188 must be an interger value from 0 to 7");
            }
            else if (sensitivity > 7)
            {
                sensitivity = 7;
                this->error(
                    "sensitivity for CAP1188 must be an interger value from 0 to 7");
            }

        }

        uint8_t reg = this->readRegister( 0x1F ) & 0x0F;
        this->writeRegister( 0x1F, reg | sensitivity*16);

        // Turn off interrupt repeat on button hold
        this->writeRegister(0x28, 0x00);
    }

    this->state = false;
    this->inLick = false;

    // clear sensor status to reset interrupt status
    while((this->connected) && (readRegister(CAP1188_SENINPUTSTATUS) != 0))
    {
        writeRegister(CAP1188_MAIN, readRegister(CAP1188_MAIN) & ~CAP1188_MAIN_INT);
    }
}

void Cap1188::initMessages()
{
    JsonObject& start_json = this->jsonBuffer.createObject();
    JsonObject& start_subjson = this->jsonBuffer.createObject();
    start_subjson["pin"] = pin;
    start_subjson["sensor"] = this->sensor;
    start_subjson["action"] = "start";
    this->start_json["lick"] = start_subjson;

    JsonObject& stop_json = this->jsonBuffer.createObject();
    JsonObject& stop_subjson = this->jsonBuffer.createObject();
    stop_subjson["pin"] = pin;
    stop_subjson["sensor"] = this->sensor;
    stop_subjson["action"] = "stop";
    this->stop_json["lick"] = stop_subjson;

    JsonObject& create_subjson = this->jsonBuffer.createObject();
    create_subjson["pin"] = pin;
    create_subjson["sensor"] = this->sensor;
    create_subjson["action"] = "created";
    JsonObject& create_json = this->jsonBuffer.createObject();
    create_json["lick"] = create_subjson;
    this->communicator->sendMessage(create_json);
}

int Cap1188::getSensorStatus()
{
    return this->sensor_status;
}


bool Cap1188::detect()
{
    if (!this->connected)
    {
        this->state = false;
        return false;
    }

    bool irq = digitalRead(this->pin);
    if (irq)
    {
        uint8_t touched = readRegister(CAP1188_SENINPUTSTATUS);
        if (touched != 0)
        {
            writeRegister(CAP1188_MAIN, readRegister(CAP1188_MAIN) & ~CAP1188_MAIN_INT);
            if ((!this->inLick) &&
                ((this->sensor == -1) || (touched & (1 << this->sensor))))
            {
                if (this->sensor == -1)
                {
                    for (int i = 0; i < 8; i++)
                    {
                        if (bitRead(touched, i))
                        {
                            this->start_json["lick"]["sensor"] = i;
                            this->stop_json["lick"]["sensor"] = i;
                            this->sensor_status = i;
                            break;
                        }
                    }
                }
                else
                {
                    this->sensor_status = this->sensor;
                }
                if (this->report_pin != -1)
                {
                    digitalWrite(this->report_pin, HIGH);
                }
                this->communicator->sendMessage(this->start_json);
                this->inLick = true;
                this->state = true;
                return true;
            }
            else
            {
                if (this->inLick)
                {
                    this->inLick = false;
                    this->sensor_status = -1;
                    if (this->report_pin != -1)
                    {
                        digitalWrite(this->report_pin, LOW);
                    }
                    this->communicator->sendMessage(this->stop_json);
                }
            }
        }
    }
    this->state = false;
    return false;
}

uint8_t Cap1188::readRegister(uint8_t reg)
{
    Wire.beginTransmission((uint8_t)this->i2caddr);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)this->i2caddr, (uint8_t)1);
    return Wire.read();
}

void Cap1188::writeRegister(uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(this->i2caddr);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}
