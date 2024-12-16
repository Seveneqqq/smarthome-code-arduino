#include <ArduinoJson.h>
#include "DHT.h"

#define SERIAL_TX_BUFFER_SIZE 256
#define SERIAL_RX_BUFFER_SIZE 256
#define echoPin 2
#define trigPin 3
#define DHT11_PIN 7

DHT dht;

long duration;
int distance;
int lightPin = 5;
int lightPin2 = 4;


unsigned long previousMillis = 0;
const long interval = 1000;
bool isStarted = false;

void setup() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(lightPin, OUTPUT);
    pinMode(lightPin2, OUTPUT);
    dht.setup(DHT11_PIN);
    Serial.begin(9600);
}

void loop() {
 
    delay(1000); //Odczekanie wymaganego czasu

    if(isStarted == true){
        delay(300);
        startApp();
    }

    if (Serial.available() > 0) {

      delay(500); //bez tego delaya wariuje program 

        String receivedString = "";
        
        while (Serial.available() > 0) {
            receivedString += char(Serial.read());
        }

        StaticJsonDocument<800> doc;
        DeserializationError error = deserializeJson(doc, receivedString);

        if (error) {
            Serial.println("JSON deserialization failed");
            return;
        }

        String instruction = doc["instruction"].as<String>();
        
        if (instruction == "check-connection") {
            digitalWrite(lightPin, HIGH);
            digitalWrite(lightPin2, HIGH);
            delay(1000);
            digitalWrite(lightPin, LOW);
            digitalWrite(lightPin2, LOW);
            returnIsConnected();
        }
        if (instruction == "scenario") {
          Serial.println("Wywolywanie");
          handleScenario(doc);
        }
        if(instruction == "send-devices-list"){
            sendDeviceList();
        }
        
        if(instruction == "start-app"){
            startApp();
            isStarted = true;
        }
        
        if(instruction == "device-control"){
            String deviceName = doc["device"].as<String>();
            int state = doc["actions"]["state"].as<int>();
            int brightness = doc["actions"]["brightness"].as<int>();

            if(deviceName == "LED1") {
                if(state == 1){
                    digitalWrite(lightPin, HIGH);
                } else {
                    digitalWrite(lightPin, LOW);
                }
            }
            else if(deviceName == "LED2") {
                if(state == 1){
                    digitalWrite(lightPin2, HIGH);
                } else {
                    digitalWrite(lightPin2, LOW);
                }
            }
        }
    }
}

void handleScenario(StaticJsonDocument<800>& doc) {

  Serial.println("Wywolywanie 2");
    bool scenarioState = doc["state"];
    JsonArray devices = doc["devices"];

    for (JsonObject device : devices) {
        String deviceName = device["name"].as<String>();
        JsonObject actions = device["actions"];
        int state = actions["state"];
        int brightness = actions["brightness"];

        if (deviceName == "LED1") {
            if (state == 1) {
                digitalWrite(lightPin, HIGH);
            } else {
                digitalWrite(lightPin, LOW);
            }
        }
        else if (deviceName == "LED2") {
            if (state == 1) {
                digitalWrite(lightPin2, HIGH);
            } else {
                digitalWrite(lightPin2, LOW);
            }
        }
    }
}

void startApp(){

    digitalWrite(trigPin, LOW);
    delayMicroseconds(300);
    digitalWrite(trigPin,HIGH);
    delayMicroseconds(300);
    digitalWrite(trigPin,LOW);

    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.0344 / 2;

    int humidity = dht.getHumidity();
    int temperature = dht.getTemperature();

    Serial.println("{ \"distance\": " + String(distance) + 
                  ", \"humidity\": " + String(humidity) + 
                  ", \"temperature\": " + String(temperature) + " }");
}

void returnIsConnected(){
    StaticJsonDocument<800> doc;
    doc["connected"] = true;
    
    String jsonOutput;
    serializeJson(doc, jsonOutput);
    Serial.println(jsonOutput);
}

void sendDeviceList() {
    StaticJsonDocument<800> doc; //rozmiar dokumentow, przy 200 wysyla 2 
    
    JsonArray devices = doc.createNestedArray("devices");

    JsonObject device1 = devices.createNestedObject();
    device1["id"] = 1;
    device1["name"] = "LED1";
    device1["status"] = "active";

    JsonObject device2 = devices.createNestedObject();
    device2["id"] = 2;
    device2["name"] = "LED2";
    device2["status"] = "not-active";
    
    JsonObject device3 = devices.createNestedObject();
    device3["id"] = 3;
    device3["name"] = "LED3";
    device3["status"] = "active";

    JsonObject device4 = devices.createNestedObject();
    device4["id"] = 4;
    device4["name"] = "LED4";
    device4["status"] = "not-active";
   

    String jsonOutput;
    serializeJson(doc, jsonOutput);
    Serial.println(jsonOutput);
}



