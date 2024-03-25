#include "SerialManager.h"
#include "Globals.h"
#include "DisplayManager.h"
#include "PeripheryManager.h"
#include "PowerManager.h"

// The getter for the instantiated singleton instance
SerialManager_ &SerialManager_::getInstance() {
    static SerialManager_ instance;
    return instance;
}

// Initialize the global shared instance
SerialManager_ &SerialManager = SerialManager.getInstance();

const int bufferSize = 256; // Adjust the buffer size as needed
char buffer[256];   // Character array to store incoming data

/**
 * Example:
 * 
 * 
{
    "cmd": "custom",
    "data": {
        "name": "itx",
        "text": "Monica 2",
        "duration": 100
    }
}
*/
void SerialManager_::tick() {
    static StaticJsonDocument<256> json_doc;
      if (Serial.available() > 0) {
        const auto deser_err = deserializeJson(json_doc, Serial);
        if (!deser_err) {
            JsonObject obj = json_doc.as<JsonObject>();
            String cmd = obj[String("cmd")];

            String dataString;
            serializeJson(obj["data"], dataString);
            const char* data = dataString.c_str();

            if (cmd == "notify") {
                DisplayManager.generateNotification(0, data);
                return;
            }

            if (cmd == "nextapp") {
                DisplayManager.nextApp();
                return;
            }

            if (cmd == "notify/dismiss") {
                DisplayManager.dismissNotify();
                return;
            }

            if (cmd == "apps") {
                DisplayManager.updateAppVector(data);
                return;
            }

            if (cmd == "switch") {
                DisplayManager.switchToApp(data);
                return;
            }

            if (cmd == "settings") {
                DisplayManager.setNewSettings(data);
                return;
            }

            if (cmd == "r2d2") {
                PeripheryManager.r2d2(data);
                return;
            }

            if (cmd == "previousapp") {
                DisplayManager.previousApp();
                return;
            }

            if (cmd == "rtttl") {
                PeripheryManager.playRTTTLString(data);
                return;
            }

            if (cmd == "power") {
                StaticJsonDocument<128> doc;
                DeserializationError error = deserializeJson(doc, data);
                if (error) {
                    if (DEBUG_MODE)
                        DEBUG_PRINTLN(F("Failed to parse json"));
                    return;
                }
                if (doc.containsKey("power")) {
                    DisplayManager.setPower(doc["power"].as<bool>());
                }
                return;
            }

            if (cmd == "sleep") {
                StaticJsonDocument<128> doc;
                DeserializationError error = deserializeJson(doc, data);
                if (error) {
                    if (DEBUG_MODE)
                        DEBUG_PRINTLN(F("Failed to parse json"));
                    return;
                }
                if (doc.containsKey("sleep")) {
                    DisplayManager.setPower(false);
                    PowerManager.sleep(doc["sleep"].as<uint64_t>());
                }
                return;
            }

            if (cmd == "indicator1") {
                DisplayManager.indicatorParser(1, data);
                return;
            }

            if (cmd == "indicator2") {
                DisplayManager.indicatorParser(2, data);
                return;
            }

            if (cmd == "indicator3") {
                DisplayManager.indicatorParser(3, data);
                return;
            }

            if (cmd == "moodlight") {
                DisplayManager.moodlight(data);
                return;
            }

            if (cmd == "reboot") {
                if (DEBUG_MODE)
                    DEBUG_PRINTLN("REBOOT COMMAND RECEIVED")
                delay(1000);
                ESP.restart();
                return;
            }

            if (cmd == "sound") {
                PeripheryManager.parseSound(data);
                return;
            }

            if (cmd.startsWith("custom")) {
                DisplayManager.parseCustomPage(obj["data"]["name"], data, false);
                return;
            }
        }
    }
}

void SerialManager_::sendButton(byte btn, bool state){
    static bool btn0State, btn1State, btn2State;
    String payload;
    switch (btn) {
    case 0:
        if (btn0State != state) {
            btn0State = state;
            payload = "button=left&state=" + String(state);
        }
        break;
    case 1:
        if (btn1State != state) {
            btn1State = state;
            payload = "button=middle&state=" + String(state);
        }
        break;
    case 2:
        if (btn2State != state) {
            btn2State = state;
            payload = "button=right&state=" + String(state);
        }
        break;
    default:
        return;
    }

    if (!payload.isEmpty()) {
        Serial.println("BTN payload: " + payload);
    }
}
