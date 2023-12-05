class LickPortPiezo : public Sensor {
public:
    LickPortPiezo(JsonObject& parmas, Communicator* aCommunicator);
    bool detect();

private:
    void initMessages();

    int report_pin;
    unsigned long prev_time;
    unsigned long prev_lick;
    int lick_duration;
    float lick_threshold;
    float min_interval;
    float prev_value;
    bool licked;
};
