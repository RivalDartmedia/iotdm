// Ref : https://github.com/me-no-dev/ESPAsyncWebServer/blob/master/examples/CaptivePortal/CaptivePortal.ino

#include <ArduinoJson.h>
#include <LittleFS.h>

#include <DNSServer.h>
#ifdef ESP32
#include "FS.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include "ESPAsyncWebServer.h"

#include <UniversalTelegramBot.h>

#define FORMAT_LITTLEFS_IF_FAILED true

DNSServer dnsServer;
AsyncWebServer server(80);

// Our configuration structure.
struct Config
{
    char *seri_infus, *nama_pasien, *kode_kamar, *kode_bed,
        *koneksi, *wifi_ssid, *wifi_pass, *sim_ssid, *sim_pass;
};

bool isWiFi;
bool patient_info_received = false;

bool isSetting;
const char *config_dir = "/config.txt";
Config config, config_buff; // <- global configuration object
String avail_wifi;

// Bentuk json config
//{
//  "seri_infus": " A5102",
//  "nama_pasien": "default test pertama kedua",
//  "kode_kamar": "AK421",
//  "kode_bed": "4",
//  "koneksi": "SIM",
//  "wifi_ssid": "Dart_WiFi",
//  "wifi_pass": "WiFi_Pass",
//  "sim_ssid": "SIM_ID",
//  "sim_pass": "SIM_PASS"
//}

// GPIO Setting
const int set_pin = 23;  // GPIO pin setting
const int sens_pin = 21; // Pin Sensor

// State
const int loadconfig = 10;
const int setting = 11;
const int connecting = 12;
const int monitoring = 13;
const int error_state = 14;
const int waiting_conn = 15;
int state = loadconfig;

// State Monitor
// delay dengan millis agar tidak terblock
#define INTERVAL_READING 5000                    // 5000ms : Interval membaca tetesan
unsigned long time_1, time_drop, time_drop_prev; // Variabel pewaktu
int tpm;                                         // Tetes per menit

#define BOTtoken "6025777872:AAGOiaT-UVZCXnW7Ur9KS2xmuLIXgirmRE4"
#define CHAT_ID "-894765173"
#ifdef ESP8266
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

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

void setup()
{
    // Initialize serial port
    Serial.begin(115200);
    while (!Serial)
        continue;
    Serial.println(F("Starting"));
    delay(1000);

    // Pertama diaktifkan : Load n Show Config
    // Init LittleFS Lib
    if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED))
    {
        Serial.println(F("LITTLEFS Mount Failed"));
        return;
    }

    // Load Default config
    Serial.println(F("Load config"));
    load_config(LittleFS, config_dir, config);
    Serial.println(F("Config Loaded"));

    // Debug informasi yang ada
    printconfig();

    Serial.println(F("Melakukan pengaturan ?"));
    // Tunggu trigger pengaturan
    // Apakah perlu pengaturan?
    pinMode(set_pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(set_pin), sett_v, FALLING);

    isSetting = 0;
    const int del_itvl = 100;  // ms
    const int del_time = 5000; // ms
    const int del_limit = del_time / del_itvl;
    int cnt = 0;

    // while (!isSetting && cnt < del_limit)
    // {
    //   cnt++;
    //   delay(del_itvl);
    // }

    // if (isSetting)
    // {
    //   Serial.println(F("Masuk mode pengaturan"));
    //   state = setting;
    // }
    // else
    // {
    //   // Jika tidak, masuk ke state connecting
    //   Serial.println(F("Mulai koneksi"));
    //   state = connecting;
    // }

    // // Create configuration file
    // Serial.println(F("Saving configuration..."));
    // write_config(LittleFS, config_dir, config);

    // Mulai monitoring
    pinMode(sens_pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(sens_pin), sens_v, RISING);

    // Demo state setting
    state = setting;
}

void loop()
{
    // Finite State Machine
    switch (state)
    {
    case monitoring:
    {
        Serial.println("Monitoring");
        // Melakukan monitoring per INTERVAL_READING
        if (millis() >= time_1 + INTERVAL_READING)
        {
            time_1 += INTERVAL_READING;
            sensor_read();
        }
        break;
    }

    case setting:
    {
        // Melakukan pengaturan
        Serial.println("Mode Pengaturan");
        // display.clearDisplay();
        // display.setCursor(0, 0);
        // display.println("Patient");
        // display.println("Input");
        // display.display();
        WiFi.mode(WIFI_OFF);
        WiFi.mode(WIFI_AP_STA);
        int rand_pass = random(10000000, 999999999);

        // Untuk debug
        rand_pass = 12345678;
        Serial.println(F("SSID : "));
        Serial.println(config.seri_infus);
        Serial.println(F("PASS : "));
        Serial.println(rand_pass);

        WiFi.softAP(config.seri_infus, String(rand_pass));
        Serial.print("AP IP address: ");
        Serial.println(WiFi.softAPIP());
        Serial.println("Setting up Async WebServer");
        setupServer();
        Serial.println("Starting DNS Server");

        dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
        dnsServer.start(53, "*", WiFi.softAPIP());

        server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); // only when requested from AP
        // more handlers...
        server.begin();
        state = waiting_conn;
        delay(2000);
        break;
    }
    case waiting_conn:
    {
        // waiting request
        dnsServer.processNextRequest();
        delay(200);
        break;
    }
    case connecting:
    {
        server.end();
        // Melakukan pengaturan konektifitas
        Serial.println("Mencoba koneksi");

        printconfig();
#ifdef ESP8266
        configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
        client.setTrustAnchors(&cert);    // Add root certificate for api.telegram.org
#endif

        WiFi.mode(WIFI_OFF);
        WiFi.mode(WIFI_STA);

        delay(500);
        Serial.println(strcmp(config.koneksi, "WiFi"));
        Serial.println(config.koneksi);
        if (strcmp(config.koneksi, "WiFi"))
        {
            Serial.println("WiFi");
            Serial.println(config.wifi_ssid);
            Serial.println(config.wifi_pass);
            WiFi.begin(config.wifi_ssid, config.wifi_pass);
        }
        else
        {
            Serial.println("SIM");
            Serial.println(config.sim_ssid);
            Serial.println(config.sim_pass);
            WiFi.begin(config.sim_ssid, config.sim_pass);
        }

#ifdef ESP32
        client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
#endif

        Serial.print("Connecting to WiFi ..");

        int cnt = 0, cnt_lim = 10;
        while (WiFi.status() != WL_CONNECTED && cnt < cnt_lim)
        {
            cnt++;
            Serial.print('.');
            delay(1000);
        }

        if (cnt < cnt_lim)
        {
            Serial.println(WiFi.localIP());
            Serial.println("SETUP BERHASIL");
            return_monitor();
        }
        else
        {
            Serial.println("Koneksi gagal, ke pengaturan");
            Serial.println();
            state = setting;
        }

        break;
    }
    case error_state:
    {
        Serial.println("Error");
        break;
    }

    default:
    {
        // Diluar state, something wrong?
        state = error_state;
        break;
    }
    }
    delay(200);
}

void ICACHE_RAM_ATTR sens_v()
{
    time_drop = millis() - time_drop_prev;
    time_drop_prev = millis();
    tpm = time_drop * 0.06;
}

void ICACHE_RAM_ATTR sett_v()
{
    isSetting = 1;
}

void return_monitor()
{
    // Selesai pengaturan, kembali ke monitoring
    state = monitoring;
    time_1 = millis() + INTERVAL_READING;
    time_drop_prev = millis();
}

void sensor_read()
{
    // delay dengan millis agar tidak terblock
    // setelah delay, cek jumlah tetesan per Interval Read
    if (tpm == 0)
    {
        Serial.println("INFUSION LOW");

        send_alert_tele();
    }
    else
    {
        Serial.println("INFUSION RUNNING");
    }
}