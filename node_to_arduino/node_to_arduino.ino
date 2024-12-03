#include <Servo.h>
#include <ArduinoJson.h>

#define echoPin 2
#define trigPin 3

long duration;
int distance;
int lightPin = 5;
int lightPin2 = 4;
Servo serwomechanizm;

unsigned long previousMillis = 0;
const long interval = 1000;
bool isStarted = false;

void setup() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(lightPin, OUTPUT);
    pinMode(lightPin2, OUTPUT);
    serwomechanizm.attach(9);
    Serial.begin(9600);
}

void loop() {
    if(isStarted == true){
        delay(300);
        startApp();
    }

    if (Serial.available() > 0) {
        String receivedString = "";
        
        while (Serial.available() > 0) {
            receivedString += char(Serial.read());
        }

        StaticJsonDocument<200> doc;
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
            else if(deviceName == "Servo") {
                if(state == 1){
                    serwomechanizm.write(brightness); // Using brightness as angle for servo
                } else {
                    serwomechanizm.write(0);
                }
            }
        }
    }
}

// Rest of the functions remain the same
void startApp(){
    digitalWrite(trigPin, LOW);
    delayMicroseconds(300);
    digitalWrite(trigPin,HIGH);
    delayMicroseconds(300);
    digitalWrite(trigPin,LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.0344 / 2;
    Serial.println("{ \"distance\": " + String(distance) + " }");
}

void returnIsConnected(){
    StaticJsonDocument<200> doc;
    doc["connected"] = true;
    
    String jsonOutput;
    serializeJson(doc, jsonOutput);
    Serial.println(jsonOutput);
}

void serwo() {
    digitalWrite(lightPin, HIGH);
    Serial.println(serwomechanizm.read());
    serwomechanizm.write(0);
    delay(1000);
    Serial.println(serwomechanizm.read());
    serwomechanizm.write(90);
    Serial.println(serwomechanizm.read());
    delay(1000);
    serwomechanizm.write(0);
}

void sendDeviceList() {
    StaticJsonDocument<200> doc;
    
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
    device3["name"] = "Servo";
    device3["status"] = "active";

    String jsonOutput;
    serializeJson(doc, jsonOutput);
    Serial.println(jsonOutput);
}