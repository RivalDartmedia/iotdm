#ifndef koneksi_h
#define koneksi_h

#include "mem_set.h"

#include <DNSServer.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include <AsyncTCP.h>
#include <WiFiClientSecure.h>

DNSServer dnsServer;
AsyncWebServer server(80);

WiFiClientSecure client;


class ConnectionManager:{
    private:
    bool iswifiavail;

    public:

    bool checkwifi(){
        
    }
    
};

#endif // !1