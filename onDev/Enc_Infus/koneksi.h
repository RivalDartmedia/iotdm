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

    public:
    void get(){
        //Prioritas koneksi wifi
        if(checkwifi()){
            //Connect lewat wifi
        }
        else if (checksim()) 
        {
            //Connect lewat SIM jika tersedia
        }
    }
    
    bool checkwifi(){
        if(WiFi.status() != WL_CONNECTED){
            return 0;
        }
        // Check koneksi
        // Return 0 jika tidak bisa koneksi
        return 1;
    }

    bool checksim(){
        return 0;
    }


};

#endif // !1