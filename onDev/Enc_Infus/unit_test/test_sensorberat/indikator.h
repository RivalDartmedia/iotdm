#ifndef indikator_h
#define indikator_h

enum indi_state {
    off,
    blink_slow,
    blink_fast,
    on
};

class indicator{
    private:
    int pin_led, pin_buzz;
    indi_state state = off, state_p = off;
    bool pin_val;
    unsigned long time_call, time_delay, time_slow = 4000, time_fast = 2000;

    public:
    void init(int pin_led, int pin_buzz)
    {
        this->pin_led = pin_led;
        this->pin_buzz = pin_buzz;
        pinMode(pin_led, OUTPUT);
        pinMode(pin_buzz, OUTPUT);
        write(0);
    }

    void write(bool val){
        digitalWrite(pin_led, !val);
        digitalWrite(pin_buzz, !val);
    }

    void set_state(indi_state val){
        state = val;
    }

    void show(){
        //new state
        if (state_p != state) {
            state_p = state;
            time_call = millis();
            switch (state_p) {
                case off: {
                    write(0);
                    break;
                }
                case on: {
                    write(1);
                    break;
                }
                case blink_fast:{
                    write(0);
                    time_delay = time_fast;
                    break;
                }
                case blink_slow:{
                    write(0);
                    time_delay = time_slow;
                    break;
                }
                pin_val = 0;
            }
        }
        else {
            //Update jika lewat perioda
            if(millis() - time_call > time_delay){
                time_call = millis();
                pin_val = !pin_val;
                write(pin_val);
            }
        }
    }
};

#endif // !1