#ifndef mem_set_h
#define mem_set_h

#include <ArduinoJson.h>
#include <LittleFS.h>
#include "FS.h"

#define FORMAT_LITTLEFS_IF_FAILED true

enum mem_par
{
    tokenID_p,
    temp_p,
    koneksi_p,
    wifi_ssid_p,
    wifi_pass_p
};

/**
 * @brief inisiasi LittleFS untuk akses memori ESP32
 *
 * @return true LittleFS berhasil Load
 * @return false LittleFS tidak berhasil Load
 */
bool init_fs()
{
    if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED))
    {
        Serial.println(F("LITTLEFS Mount Failed"));
        return 0;
    }
    Serial.println(F("LITTLEFS Mounted"));
    return 1;
}

class InfusConfig
{
private:
    String tokenID, temp_IoT, koneksi, wifi_ssid, wifi_pass;
    const char *configDir = "/config.txt";

public:
    /**
     * @brief Load configurasi tersimpan dalam ESP32
     *
     * @param fs
     * @return true Load Berhasil
     * @return false Load Gagal
     */
    bool load(fs::FS &fs)
    {
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
        temp_IoT = (const char *)doc["temp_IoT"];
        koneksi = (const char *)doc["koneksi"];
        wifi_ssid = (const char *)doc["wifi_ssid"];
        wifi_pass = (const char *)doc["wifi_pass"];
        file.close();
        return 1;
    }

    /**
     * @brief edit parameter config, simpan dengan save()
     *
     * @param which_par parameter yg diedit
     * @param val_edit string nilai teredit
     */
    void edit(mem_par which_par, String val_edit)
    {
        switch (which_par)
        {
        case tokenID_p:
        {
            tokenID = val_edit;
            break;
        }
        case temp_p:
        {
            temp_IoT = val_edit;
            break;
        }
        case koneksi_p:
        {
            koneksi = val_edit;
            break;
        }
        case wifi_ssid_p:
        {
            wifi_ssid = val_edit;
            break;
        }
        case wifi_pass_p:
        {
            wifi_pass = val_edit;
            break;
        }
        }
    }

    /**
     * @param which_par
     * @return String dari parameter yang diinginkan
     */
    String get(mem_par which_par)
    {
        switch (which_par)
        {
        case tokenID_p:
            return tokenID;
        case temp_p:
            return temp_IoT;
        case koneksi_p:
            return koneksi;
        case wifi_ssid_p:
            return wifi_ssid;
        case wifi_pass_p:
            return wifi_pass;
        }
        return "";
    }

    void print()
    {
        Serial.printf("\nToken:%s\nTemplate:%s\nKoneksi:%s\nSSID:%s\nPass:%s\n", tokenID.c_str(), temp_IoT.c_str(), koneksi.c_str(), wifi_ssid.c_str(), wifi_pass.c_str());
    }

    /**
     * @brief Save nilai config ke memori
     *
     * @param fs
     * @return true
     * @return false
     */
    bool save(fs::FS &fs)
    {
        // Delete existing file, otherwise the configuration is appended to the file
        fs.remove(configDir);
        // Allocate a temporary JsonDocument
        // Don't forget to change the capacity to match your requirements.
        // Use arduinojson.org/v6/assistant to compute the capacity.
        StaticJsonDocument<384> doc;

        // Open file for writing
        File file = fs.open(configDir, FILE_WRITE);
        if (!file)
        {
            Serial.println(F("Failed to create file"));
            return 0;
        }

        doc["tokenID"] = tokenID;
        doc["temp_IoT"] = temp_IoT;
        doc["koneksi"] = koneksi;
        doc["wifi_ssid"] = wifi_ssid;
        doc["wifi_pass"] = wifi_pass;

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
};

class LoadCellConfig
{
private:
    float scale_factor;
    const char *configDir = "/weigh.txt";
    bool isload;
public:
    bool load(fs::FS &fs)
    {
        // Open file for reading
        File file = fs.open(configDir);
        // Allocate a temporary JsonDocument
        // Don't forget to change the capacity to match your requirements.
        // Use arduinojson.org/v6/assistant to compute the capacity.
        StaticJsonDocument<48> doc;

        DeserializationError error = deserializeJson(doc, fs);

        if (error)
        {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
            return;
        }

        scale_factor = doc["callib_fact"];
        return 1;
    }

    void edit(float new_fact)
    {
        scale_factor = new_fact;
    }

    float get()
    {
        if (!isload){
            load();
            isload = true;
        }
        return scale_factor;
    }

    bool save(fs::FS &fs)
    {
        // Delete existing file, otherwise the configuration is appended to the file
        fs.remove(configDir);
        // Allocate a temporary JsonDocument
        // Don't forget to change the capacity to match your requirements.
        // Use arduinojson.org/v6/assistant to compute the capacity.
        StaticJsonDocument<16> doc;

        doc["callib_fact"] = scale_factor;

        serializeJson(doc, fs);

        Serial.println("Config Saved");
        // Close the file
        file.close();
        return 1;
    }
};

#endif // !1