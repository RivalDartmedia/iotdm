
void send_alert_tele()
{
    Serial.println("Alert terkirim");
    char message[50];
    sprintf(message, "ALERT!, INFUS HABIS/MAMPET. Pasien %s, Kamar %s, Bed %s", config.nama_pasien, config.kode_kamar, config.kode_bed);
    bot.sendMessage(CHAT_ID, message, "");

    return_monitor();
}

void printconfig()
{
    Serial.println();
    Serial.println(config.seri_infus);
    Serial.println(config.nama_pasien);
    Serial.println(config.kode_kamar);
    Serial.println(config.kode_bed);
    Serial.println(config.koneksi);
    Serial.println(config.wifi_ssid);
    Serial.println(config.wifi_pass);
    Serial.println(config.sim_ssid);
    Serial.println(config.sim_pass);
    Serial.println();
}

void checkavailnetwork()
{
    avail_wifi = "SSID: <select name = wifi_ssid>";

    int n = WiFi.scanNetworks();
    // Serial.println("scan done");
    if (n == 0)
    {
        avail_wifi += "<option value=null>No Network Available</option><br>";
    }
    else
    {
        for (int i = 0; i < n; ++i)
        {
            avail_wifi += "<option value=" + WiFi.SSID(i) + ">" + WiFi.SSID(i) + "</option><br>";
        }
    }
    avail_wifi += "</select><br>";
}

String processor(const String &var)
{

    // Serial.println(var);

    if (var == "saved_nama_pasien")
    {
        return String(config.nama_pasien);
    }
    else if (var == "saved_kode_kamar")
    {
        return String(config.kode_kamar);
    }
    else if (var == "saved_kode_bed")
    {
        return String(config.kode_bed);
    }
    else if (var == "saved_koneksi")
    {
        return String(config.koneksi);
    }
    else if (var == "saved_ssid")
    {
        if (config.koneksi == "SIM")
        {
            return String(config.sim_ssid);
        }
        else
        {
            return String(config.wifi_ssid);
        }
    }
    else if (var == "saved_pass")
    {
        if (config.koneksi == "SIM")
        {
            return String(config.sim_pass);
        }
        else
        {
            return String(config.wifi_pass);
        }
    }
    else if (var == "wifi_selection")
    {
        return avail_wifi;
    }
    return String();
}

class CaptiveRequestHandler : public AsyncWebHandler
{
public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request)
    {
        request->addInterestingHeader("Setting Infus");
        return true;
    }

    void handleRequest(AsyncWebServerRequest *request)
    {
        request->send(LittleFS, "/start.htm", String(), false, processor);
    }
};

void setupServer()
{
    server.on(
        "/", 
        HTTP_GET, 
        [](AsyncWebServerRequest *request)
        {
            request->send(LittleFS, "/start.htm", String(), false, processor);
            Serial.println("Client Connected"); 
        }
    );

    server.on(
        "/exit",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            char message[100];
            sprintf(message, "Pasien %s. Kamar %s. Tempat tidur %s telah terdaftar.", config.nama_pasien, config.kode_kamar, config.kode_bed);
            request->send(200, "text/plain", message);
            // server.end();
            printconfig();
            Serial.println("Saving configuration...");
            write_config(LittleFS, config_dir, config);
            delay(5000);
            // Memulai koneksi
            state = connecting;
            Serial.println("Server closing");
        });

    server.on(
        "/edit_patt",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            request->send(LittleFS, "/edit_patt.htm", String(), false, processor);
        });

    server.on(
        "/save_patt",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            String inputMessage;
            char message[100];
            if (request->hasParam("nama_pasien"))
            {
                inputMessage = request->getParam("nama_pasien")->value();
                Serial.println(inputMessage.c_str());
                free(config.nama_pasien);
                config.nama_pasien = (char *)malloc(sizeof(char) * inputMessage.length());
                strcpy(config.nama_pasien, inputMessage.c_str());
            }

            if (request->hasParam("kode_kamar"))
            {
                inputMessage = request->getParam("kode_kamar")->value();
                Serial.println(inputMessage.c_str());
                free(config.kode_kamar);
                config.kode_kamar = (char *)malloc(sizeof(char) * inputMessage.length());
                strcpy(config.kode_kamar, inputMessage.c_str());
            }

            if (request->hasParam("kode_bed"))
            {
                inputMessage = request->getParam("kode_bed")->value();
                Serial.println(inputMessage.c_str());
                free(config.kode_bed);
                config.kode_bed = (char *)malloc(sizeof(char) * inputMessage.length());
                strcpy(config.kode_bed, inputMessage.c_str());
            }

            if (request->hasParam("mode_koneksi"))
            {
                inputMessage = request->getParam("mode_koneksi")->value();
                Serial.println(inputMessage.c_str());
                free(config.koneksi);
                config.koneksi = (char *)malloc(sizeof(char) * inputMessage.length());
                strcpy(config.koneksi, inputMessage.c_str());
            }

            if (strcmp(config.koneksi, "SIM"))
            {
                request->send(LittleFS, "/start.htm", String(), false, processor);
            }
            else
            {
                // Mode WiFi
                checkavailnetwork();
                request->send(LittleFS, "/edit_wifi.htm", String(), false, processor);
            }
        });

    server.on(
        "/save_wifi",
        HTTP_GET,
        [](AsyncWebServerRequest *request)
        {
            String inputMessage;

            if (request->hasParam("wifi_ssid"))
            {
                inputMessage = request->getParam("wifi_ssid")->value();
                free(config.wifi_ssid);
                config.wifi_ssid = (char *)malloc(sizeof(char) * inputMessage.length());
                strcpy(config.wifi_ssid, inputMessage.c_str());
                Serial.println(config.wifi_ssid);
            }
            if (request->hasParam("wifi_pass"))
            {
                inputMessage = request->getParam("wifi_pass")->value();
                free(config.wifi_pass);
                config.wifi_pass = (char *)malloc(sizeof(char) * inputMessage.length());
                strcpy(config.wifi_pass, inputMessage.c_str());
                Serial.println(config.wifi_pass);
            }
            // Mulai koneksi
            request->send(LittleFS, "/start.htm", String(), false, processor);
        }
    );
}
