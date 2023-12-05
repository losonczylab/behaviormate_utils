#ifndef h_cap1188
#define h_cap1188

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>

#define CAP1188_I2CADDR 0x29
#define CAP1188_PRODID 0xFD
#define CAP1188_MANUID 0xFE
#define CAP1188_REV 0xFF
#define CAP1188_MTBLK 0x2A
#define CAP1188_LEDLINK 0x72
#define CAP1188_STANDBYCFG 0x41
#define CAP1188_SENINPUTSTATUS 0x3
#define CAP1188_MAIN 0x00
#define CAP1188_MAIN_INT 0x01


class Cap1188 : public Sensor
{
public:
    Cap1188(JsonObject& params, Communicator* aCommunicator);
    bool detect();
    int getSensorStatus();

private:
    void initMessages();
    uint8_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint8_t value);

    int sensor;
    int report_pin;
    int sensor_status;
    int reset_pin;
    uint8_t i2caddr;
    bool connected;
    bool inLick;
};

#endif
