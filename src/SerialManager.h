#ifndef SerialManager_h
#define SerialManager_h

#include <Arduino.h>
#include <ArduinoJson.h>

class SerialManager_
{
private:
    SerialManager_() = default;

public:
    static SerialManager_ &getInstance();
    void setup();
    void tick();
    void loadSettings();
    void sendButton(byte btn, bool state);
};

extern SerialManager_ &SerialManager;
 
#endif
