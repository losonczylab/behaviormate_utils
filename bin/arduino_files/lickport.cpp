#include "lickport.h"

LickPortClass::LickPortClass(JsonObject& params, bool initialize,
         Communicator* aCommunicator) : Sensor()
{
    this->communicator = aCommunicator;
    communicator->sendMessage("in constructor");
    if (!params.containsKey("pin"))
    {
        debug("lick port requires pin (interrupt pin for mpr121)");
        return;
    }

    this->pin = params["pin"].as<int>();
    pinMode(this->pin, INPUT);
    digitalWrite(this->pin, HIGH); // enable pullup resistor

    this->report_pin = -1;
    if (params.containsKey("report_pin"))
    {
        this->report_pin = params["report_pin"].as<int>();
        pinMode(this->report_pin, OUTPUT);
        digitalWrite(this->report_pin, LOW);
    }

    if (params.containsKey("lickstart_threshold"))
    {
        this->start_thresh = params["lickstart_threshold"].as<int>();
    }
    else if (params.containsKey("lickstart_threashold"))
    {
        this->start_thresh = params["lickstart_threashold"].as<int>();
    }
    else
    {
        this->start_thresh = 19;
    }

    if (params.containsKey("lickstop_threshold"))
    {
        this->stop_thresh = params["lickstop_threshold"].as<int>();
    }
    else if (params.containsKey("lickstop_threashold"))
    {
        this->stop_thresh = params["lickstop_threashold"].as<int>();
    }
    else
    {
        this->stop_thresh = 8;
    }

    if (params.containsKey("sensor"))
    {
        this->sensor = params["sensor"].as<int>();
    }
    else
    {
        this->sensor = 0;
    }

    if (initialize)
    {
        Wire.begin();
        mpr121_setup();
    }
    initMessages();
    this->state = false;
    this->inLick = false;
}

void LickPortClass::initMessages()
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
    communicator->sendMessage(create_json);
}

bool LickPortClass::detect()
{
    if(!digitalRead(this->pin))
    {
        digitalWrite(this->pin, HIGH);
        Wire.requestFrom(0x5A, this->pin);
        byte LSB = Wire.read();
        byte MSB = Wire.read();

        // each bit represents a state of one touch detector
        uint16_t detector_states = (MSB << 8 | LSB);

        if ((!inLick) && (detector_states & (1 << this->sensor)))
        {
            this->state = true;
            this->inLick = true;
            communicator->sendMessage(this->start_json);
            if (this->report_pin != -1)
            {
                digitalWrite(this->report_pin, HIGH);
            }

            // return true to indicate a new lick was detected
            return true;
        }
        
        if ((inLick) && ((~detector_states) & (1 << this->sensor)))
        {
            this->inLick = false;
            communicator->sendMessage(this->stop_json);
            if (this->report_pin != -1)
            {
                digitalWrite(this->report_pin, LOW);
            }
        }
    }

    // return false if no licking action was detected or if a lick ended
    this->state = false;
    return false;
}


void LickPortClass::mpr121_setup()
{
    // Section A - Controls filtering when data is > baseline.
    set_register(0x5A, MHD_R, 0x01);
    set_register(0x5A, NHD_R, 0x01);
    set_register(0x5A, NCL_R, 0x00);
    set_register(0x5A, FDL_R, 0x00);

    // Section B - Controls filtering when data is < baseline.
    set_register(0x5A, MHD_F, 0x01);
    set_register(0x5A, NHD_F, 0x01);
    set_register(0x5A, NCL_F, 0xFF);
    set_register(0x5A, FDL_F, 0x02);

    // Section C - Sets touch and release thresholds for each electrode
    set_register(0x5A, ELE0_T, this->start_thresh);
    set_register(0x5A, ELE0_R, this->stop_thresh);

    set_register(0x5A, ELE1_T, this->start_thresh);
    set_register(0x5A, ELE1_R, this->stop_thresh);

    set_register(0x5A, ELE2_T, this->start_thresh);
    set_register(0x5A, ELE2_R, this->stop_thresh);

    set_register(0x5A, ELE3_T, this->start_thresh);
    set_register(0x5A, ELE3_R, this->stop_thresh);

    set_register(0x5A, ELE4_T, this->start_thresh);
    set_register(0x5A, ELE4_R, this->stop_thresh);

    set_register(0x5A, ELE5_T, this->start_thresh);
    set_register(0x5A, ELE5_R, this->stop_thresh);

    set_register(0x5A, ELE6_T, this->start_thresh);
    set_register(0x5A, ELE6_R, this->stop_thresh);

    set_register(0x5A, ELE7_T, this->start_thresh);
    set_register(0x5A, ELE7_R, this->stop_thresh);

    set_register(0x5A, ELE8_T, this->start_thresh);
    set_register(0x5A, ELE8_R, this->stop_thresh);

    set_register(0x5A, ELE9_T, this->start_thresh);
    set_register(0x5A, ELE9_R, this->stop_thresh);

    set_register(0x5A, ELE10_T, this->start_thresh);
    set_register(0x5A, ELE10_R, this->stop_thresh);

    set_register(0x5A, ELE11_T, this->start_thresh);
    set_register(0x5A, ELE11_R, this->stop_thresh);

    set_register(0x5A, DEBOUNCE, DEBOUNCE_VALUE);

    // Section D
    // Set the Filter Configuration
    // Set ESI2
    set_register(0x5A, FIL_CFG, 0x02);

    // Section E
    // Electrode Configuration
    // Set ELE_CFG to 0x00 to return to standby mode
    set_register(0x5A, ELE_CFG, 0x0C);  // Enables all 12 Electrodes


    // Section F
    // Enable Auto Config and auto Reconfig
    /*
    set_register(0x5A, ATO_CFG0, 0x0B);
    set_register(0x5A, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V
    set_register(0x5A, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
    set_register(0x5A, ATO_CFGT, 0xB5);  // Target = 0.9*USL = 0xB5 @3.3V
    */
}


void LickPortClass::set_register(int address, unsigned char r, unsigned char v)
{
    Wire.beginTransmission(address);
    Wire.write(r);
    Wire.write(v);
    Wire.endTransmission();
    delay(5); // NOTE: program crashes without this delay
}
