#include <WiFi.h> //add the wifi library
#include <WebServer.h> //REST api library
#include <SD.h>
#include <ArduinoJson.h>
#include "secret.h"

#define wifi_signal 2
#define sd_signal 33

WebServer server(80);

//--------------------------Setup code -----------------------------------

bool testSD(){//SD01BOOL
    if(SD.begin(5)){
        delay(3000);
        digitalWrite(sd_signal, HIGH);// if connected light up led.
        return true;
    }
    else{
        digitalWrite(sd_signal, LOW);
        delay(3000);
        digitalWrite(wifi_signal, HIGH);//end wifi setup
        return false;
    }
}

bool testWiFi(){//WIFI01BOOL
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    if(WiFi.status() == WL_CONNECTED){
        return true;
    }else{
        return false;        
    }
}

//--------------------------Setup Endpoint-----------------------------------

String diagnostics() {
    String ip = "IP address: " + WiFi.localIP().toString() + "\n";
    String freeHeap = "Free heap space: " + String(ESP.getFreeHeap()) + " bytes\n";
    String minHeap = "Minimum free heap: " + String(ESP.getMinFreeHeap()) + " bytes\n";
    String freeSketchSize = "Free sketch space: " + String(ESP.getFreeSketchSpace()) + " bytes\n";
    String sketchSize = "Sketch size: " + String(ESP.getSketchSize()) + " bytes\n";

    String info = ip + freeHeap + minHeap + freeSketchSize + sketchSize;

    return info;
}//return information about resources in the esp32

String ret_dir(String body){ //return directory

    JsonDocument doc; //allows you to store and manipulate json file.

    DeserializationError error = deserializeJson(doc, body); //parse received strin to json

    const char* dir = doc["path"]; //extract data linked to path
    
    File root = SD.open(dir); //Allows you to have a handle in the directory to interact with it.
    File entry; //Initialised for each entry in root

    while((entry = root.openNextFile())){
        doc["files"].add(entry.name());
        entry.close();
    }

    root.close(); //free up ram space.

    String json;
    serializeJson(doc, json);

    return json;
}//check the current directory


//Upload


//--------------------------Init Server-----------------------------------

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
        delay(3000);
    }
    digitalWrite(wifi_signal, HIGH);//end wifi setup
    

    //--------------------------Endpoints-----------------------------------

    server.on("/diagnose", HTTP_GET, [](){
        server.send(200, "appplication/json", diagnostics());
    });

    server.on("/directory", HTTP_POST, []() { //return folder content
        String request_body = server.arg("plain");
        server.send(200, "application/json", ret_dir(request_body));
    });

    server.begin();
}

void loop(){
    server.handleClient();
}