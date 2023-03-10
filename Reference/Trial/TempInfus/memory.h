#ifndef memory_h
#define memory_h

#include <ArduinoJson.h>
#include <LittleFS.h>
#include "FS.h"

#define FORMAT_LITTLEFS_IF_FAILED true

enum param { tokenID_p, temp_p, koneksi_p, wifi_ssid_p, wifi_pass_p};

class InfusConfig:
{
private:
    String tokenID, temp_IoT, koneksi, wifi_ssid, wifi_pass;
    const char *configDir = "/config.txt";
    // Allocate a temporary JsonDocument
    // Don't forget to change the capacity to match your requirements.
    // Use arduinojson.org/v6/assistant to compute the capacity.
    StaticJsonDocument<384> doc;

public:
    InfusConfig(){};

    bool init()
    {
        if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED))
        {
            Serial.println(F("LITTLEFS Mount Failed"));
            return 0;
        }
        return 1;
    }

    bool load()
    {
        // Open file for reading
        File file = LittleFS.open(configDir);

        DeserializationError error = deserializeJson(doc, file);
        if (error)
        {
            Serial.print("Gagal load");
            Serial.println(error.c_str());
            return 0;
        }

        tokenID = (const char *)doc["tokenID"];
        temp_IoT = (const char *)doc["temp_IoT"];
        koneksi = (const char *)doc["koneksi"];
        wifi_ssid = (const char *)doc["wifi_ssid"];
        wifi_pass = (const char *)doc["wifi_pass"];
        file.close();
        return 1;
    }

    void edit(enum param, String val_edit){
        switch (param){
            case tokenID_p:{
                tokenID = val_edit;
                break;
            }
            case temp_p:{
                temp_IoT = val_edit;
                break;
            }
            case koneksi_p:{
                koneksi = val_edit;
                break;
            }
            case wifi_ssid_p:{
                wifi_ssid = val_edit;
                break;
            }
            case wifi_pass_p:{
                wifi_pass = val_edit;
                break;
            }
        }
    }

    String get(enum param){
        switch (param){
            case tokenID_p: return tokenID;
            case temp_p: return temp_IoT;
            case koneksi_p: return koneksi;
            case wifi_ssid_p: return wifi_ssid;
            case wifi_pass_p: return wifi_pass;
        }
    }
    
    void print(){
        Serial.printf("Token:%s\nTemplate:%s\nKoneksi:%s\nSSID:%s\nPass:%s\n", tokenID, template, koneksi, wifi_ssid, wifi_pass);
    }

    // Write the configuration to a file
    bool write_config()
    {
        // Delete existing file, otherwise the configuration is appended to the file
        LittleFS.remove(configDir);

        // Open file for writing
        File file = LittleFS.open(configDir, FILE_WRITE);
        if (!file)
        {
            Serial.println(F("Failed to create file"));
            return 0;
        }

        doc["tokenID"] = config.tokenID;
        doc["temp_IoT"] = config.temp_IoT;
        doc["koneksi"] = config.koneksi;
        doc["wifi_ssid"] = config.wifi_ssid;
        doc["wifi_pass"] = config.wifi_pass;

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