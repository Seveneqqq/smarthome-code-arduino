#include <ArduinoJson.h>
#include "DHT.h"
#include <Servo.h>

#define SERIAL_TX_BUFFER_SIZE 256
#define SERIAL_RX_BUFFER_SIZE 256
#define echoPin 2
#define trigPin 3
#define DHT11_PIN 7

#define SERVO_PIN 9
#define SERVO_PIN2 10
#define SERVO_PIN3 11
Servo myservo;
Servo myservo2;
Servo myservo3;
DHT dht;

long duration;
int distance;
int lightPin = 5;
int lightPin2 = 4;

unsigned long previousMillis = 0;
const long interval = 1000;
bool isStarted = false;

// Bufor na dane JSON
char jsonBuffer[512];
int bufferIndex = 0;
bool jsonComplete = false;

void setup() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(lightPin, OUTPUT);
    pinMode(lightPin2, OUTPUT);
    dht.setup(DHT11_PIN);
    Serial.begin(9600);
    myservo.attach(SERVO_PIN);
    myservo2.attach(SERVO_PIN2);
    myservo3.attach(SERVO_PIN3);
    while (!Serial) continue;
}

void loop() {

    


    if(isStarted) {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;
            sendSensorData();
        }
    }

    // Odczytaj dostępne dane
    while (Serial.available() > 0) {
        char inChar = (char)Serial.read();
        
        // Dodaj znak do bufora
        if (bufferIndex < sizeof(jsonBuffer) - 1) {
            jsonBuffer[bufferIndex] = inChar;
            bufferIndex++;
            
            // Sprawdź, czy to koniec JSONa
            if (inChar == '\n' || inChar == '\r') {
                // Zakończ string
                jsonBuffer[bufferIndex - 1] = '\0';
                
                // Znajdź początek i koniec JSON
                char* start = strchr(jsonBuffer, '{');
                char* end = strrchr(jsonBuffer, '}');
                
                if (start != NULL && end != NULL && end > start) {
                    // Mamy kompletny JSON
                    processJsonCommand(start);
                }
                
                // Resetuj bufor
                bufferIndex = 0;
                memset(jsonBuffer, 0, sizeof(jsonBuffer));
            }
        } else {
            // Przepełnienie bufora - resetuj
            bufferIndex = 0;
            memset(jsonBuffer, 0, sizeof(jsonBuffer));
            sendErrorResponse("Buffer overflow", "");
        }
    }
}

void processJsonCommand(const char* jsonString) {
    // Debug - pokaż otrzymany JSON
    StaticJsonDocument<200> debugDoc;
    debugDoc["type"] = "debug";
    debugDoc["received"] = jsonString;
    String debugOutput;
    serializeJson(debugDoc, debugOutput);
    Serial.println(debugOutput);

    // Parsuj JSON
    StaticJsonDocument<800> doc;
    DeserializationError error = deserializeJson(doc, jsonString);

    if (error) {
        char errorMsg[100];
        snprintf(errorMsg, sizeof(errorMsg), "Deserializacja JSON nie powiodła się: %s", error.c_str());
        sendErrorResponse(errorMsg, "");
        return;
    }

    // Sprawdź czy mamy wymagane pola
    if (!doc.containsKey("instruction")) {
        sendErrorResponse("Brak pola 'instruction'", "");
        return;
    }

    const char* instruction = doc["instruction"] | "";
    const char* commandId = doc["id"] | "";
    
    if (strcmp(instruction, "check-connection") == 0) {
        digitalWrite(lightPin, HIGH);
        digitalWrite(lightPin2, HIGH);
        delay(1000);
        digitalWrite(lightPin, LOW);
        digitalWrite(lightPin2, LOW);
        sendSuccessResponse(commandId, "connection_checked");
    }
    else if (strcmp(instruction, "scenario") == 0) {
        handleScenario(doc);
        sendSuccessResponse(commandId, "scenario_executed");
    }
    else if(strcmp(instruction, "send-devices-list") == 0) {
        sendDeviceList();
        sendSuccessResponse(commandId, "devices_list_sent");
    }
    else if(strcmp(instruction, "start-app") == 0) {
        isStarted = true;
        sendSuccessResponse(commandId, "app_started");
    }
    else if(strcmp(instruction, "device-control") == 0) {
        handleDeviceControl(doc);
        sendSuccessResponse(commandId, "device_controlled");
    }
    else {
        char errorMsg[100];
        snprintf(errorMsg, sizeof(errorMsg), "Nieznana instrukcja: %s", instruction);
        sendErrorResponse(errorMsg, commandId);
    }
}

void handleDeviceControl(const JsonDocument& doc) {
    const char* deviceName = doc["device"] | "";
    int state = doc["actions"]["state"] | 0;
    int brightness = doc["actions"]["brightness"] | 100;

    if(strcmp(deviceName, "LED1") == 0) {
        int pwmValue = map(brightness, 0, 100, 0, 255);
        if(state == 1) {
            analogWrite(lightPin, pwmValue);
        } else {
            analogWrite(lightPin, 0);
        }
    }
    else if(strcmp(deviceName, "LED2") == 0) {
        digitalWrite(lightPin2, state == 1 ? HIGH : LOW);
    }
    else if(strcmp(deviceName, "FRONT_GATE") == 0) {
        if(state == 0){
          myservo.write(162);
          myservo2.write(43);
          delay(500);
        }
        else{
        myservo.write(62);
        myservo2.write(140);
        delay(500);
        }
    }
    else if(strcmp(deviceName, "GARAGE_GATE") == 0) {
        if(state == 0){
          myservo3.write(10);
          delay(500);
        }
        else{
        myservo3.write(160);
        delay(500);
        }
    }

}

void handleScenario(const JsonDocument& doc) {
    bool scenarioState = doc["state"] | false;
    JsonArrayConst devices = doc["devices"].as<JsonArrayConst>();

    for (JsonVariantConst device : devices) {
        const char* deviceName = device["name"] | "";
        int state = device["actions"]["state"] | 0;
        int brightness = device["actions"]["brightness"] | 100;

        if(strcmp(deviceName, "LED1") == 0) {
            int pwmValue = map(brightness, 0, 100, 0, 255);
            if(state == 1) {
                analogWrite(lightPin, pwmValue);
            } else {
                analogWrite(lightPin, 0);
            }
        }
        else if(strcmp(deviceName, "LED2") == 0) {
            digitalWrite(lightPin2, state == 1 ? HIGH : LOW);
        }
        else if(strcmp(deviceName, "FRONT_GATE") == 0) {
          if(state == 0){
          myservo.write(162);
          myservo2.write(43);
          delay(500);
          }
          else{
          myservo.write(62);
          myservo2.write(140);
          delay(500);
          }
        }
        else if(strcmp(deviceName, "GARAGE_GATE") == 0) {
        if(state == 0){
          myservo3.write(10);
          delay(500);
        }
        else{
        myservo3.write(160);
        delay(500);
        }
    }
    }
}

void sendSensorData() {
    StaticJsonDocument<200> doc;
    
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.0344 / 2;

    doc["type"] = "sensor_data";
    doc["distance"] = distance;
    doc["humidity"] = dht.getHumidity();
    doc["temperature"] = dht.getTemperature();

    String jsonOutput;
    serializeJson(doc, jsonOutput);
    Serial.println(jsonOutput);
}

void sendSuccessResponse(const char* commandId, const char* details) {
    StaticJsonDocument<200> doc;
    doc["type"] = "command_response";
    doc["id"] = commandId;
    doc["status"] = "success";
    doc["details"] = details;
    
    String jsonOutput;
    serializeJson(doc, jsonOutput);
    Serial.println(jsonOutput);
}

void sendErrorResponse(const char* error, const char* commandId) {
    StaticJsonDocument<200> doc;
    doc["type"] = "command_response";
    doc["id"] = commandId;
    doc["status"] = "error";
    doc["error"] = error;
    
    String jsonOutput;
    serializeJson(doc, jsonOutput);
    Serial.println(jsonOutput);
}

void sendDeviceList() {
    StaticJsonDocument<800> doc;
    JsonArray devices = doc.createNestedArray("devices");

    JsonObject device1 = devices.createNestedObject();
    device1["id"] = 1;
    device1["name"] = "LED1";
    device1["status"] = "active";

    JsonObject device2 = devices.createNestedObject();
    device2["id"] = 2;
    device2["name"] = "LED2";
    device2["status"] = "active";

    JsonObject device3 = devices.createNestedObject();
    device3["id"] = 3;
    device3["name"] = "FRONT_GATE";
    device3["status"] = "active";

    JsonObject device4 = devices.createNestedObject();
    device4["id"] = 4;
    device4["name"] = "GARAGE_GATE";
    device4["status"] = "active";
    
    String jsonOutput;
    serializeJson(doc, jsonOutput);
    Serial.println(jsonOutput);
}