#ifndef h_irport
#define h_irport

class IRLickPort : public Sensor {
public:
    IRLickPort(JsonObject& parmas, Communicator* aCommunicator);
    bool detect();

private:
    void initMessages();

    int report_pin;
    unsigned long prev_time;
    unsigned long prev_lick;
    int lick_threshold;
    bool in_lick;
    int value;
    int debug_mode;
};

#endif
