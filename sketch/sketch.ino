#include <WiFi.h> //add the wifi library
#include <WebServer.h> //REST api library
#include <SD.h>
#include <ArduinoJson.h>
#include "secret.h"

#define wifi_signal 2
#define sd_signal 33

WebServer server(80);

    //--------------------------Support Code-----------------------------------
    
String diagnostics() {
    String ip = "IP address: " + WiFi.localIP().toString() + "\n";
    String freeHeap = "Free heap space: " + String(ESP.getFreeHeap()) + " bytes\n";
    String minHeap = "Minimum free heap: " + String(ESP.getMinFreeHeap()) + " bytes\n";
    String freeSketchSize = "Free sketch space: " + String(ESP.getFreeSketchSpace()) + " bytes\n";
    String sketchSize = "Sketch size: " + String(ESP.getSketchSize()) + " bytes\n";

    String info = ip + freeHeap + minHeap + freeSketchSize + sketchSize;

    return info;
}//return information about resources in the esp32


String root_folder(){

    JsonDocument doc;
    
    File root = SD.open("/");
    File entry;

    while((entry = root.openNextFile())){
        doc["files"].add(entry.name());
        entry.close();
    }

    root.close();

    String json;
    serializeJson(doc, json);

    return json;
}//check the current directory

bool testSD(){//SD01BOOL
    if(SD.begin(5)){
        delay(3000);
        digitalWrite(sd_signal, HIGH);// if connected light up led.
        return true;
    }
    else{
        digitalWrite(sd_signal, LOW);
        return false;
    }
}

void setup(){
    Serial.begin(9600); //temporary to check where it will connect

    //--------------------------Modules-----------------------------------

    //SD01SET
    pinMode(sd_signal, OUTPUT);
    bool init = false;
    while(init == false){
        init = testSD();
    }
    

    //WIFI01SET
    pinMode(wifi_signal, OUTPUT);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.println("Connecting...");
        delay(5000);
    }
    delay(3000);
    digitalWrite(wifi_signal, HIGH);//end wifi setup

    //--------------------------Endpoints-----------------------------------
    server.on("/diagnose", HTTP_GET, [](){
        server.send(200, "appplication/json", diagnostics());
    });

    server.on("/root", HTTP_GET, []() { //return folder content
        server.send(200, "application/json", root_folder());
    });

    server.begin();
}

void loop(){
    server.handleClient();
}