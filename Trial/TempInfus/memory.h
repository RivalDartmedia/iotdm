#ifndef memory_h
#define memory_h

#include <ArduinoJson.h>
#include <LittleFS.h>
#include "FS.h"

#define FORMAT_LITTLEFS_IF_FAILED true

class InfusConfig:{
    private:
    String tokenID, template;
    const char *configDir = "/config.txt";

    public:
    InfusConfig();

    bool load(fs::FS &fs){
        // Open file for reading
        File file = fs.open(configDir);

        // Allocate a temporary JsonDocument
        // Don't forget to change the capacity to match your requirements.
        // Use arduinojson.org/v6/assistant to compute the capacity.
        StaticJsonDocument<384> doc;

        DeserializationError error = deserializeJson(doc, file);
        if (error)
        {
            Serial.print("Gagal load");
            Serial.println(error.c_str());
            return 0;
        }

        tokenID = (const char *)doc["tokenID"];
        template = (const char *)doc["template"];
        file.close();
        return 1;
    }

    // Write the configuration to a file
    bool write_config(fs::FS &fs)
    {
        // Delete existing file, otherwise the configuration is appended to the file
        fs.remove(configDir);

        // Open file for writing
        File file = fs.open(configDir, FILE_WRITE);
        if (!file)
        {
            Serial.println(F("Failed to create file"));
            return 0;
        }

        // Allocate a temporary JsonDocument
        // Don't forget to change the capacity to match your requirements.
        // Use arduinojson.org/assistant to compute the capacity.
        StaticJsonDocument<256> doc;

        doc["tokenID"] = config.tokenID;
        doc["template"] = config.template;

        serializeJson(doc, file);

        // Serialize JSON to file
        if (serializeJson(doc, file) == 0)
        {
            Serial.println(F("Failed to write to file"));
            return 0;
        }

        Serial.println("Config Saved");
        // Close the file
        file.close();
        return 1;
    }
}

#endif // !1