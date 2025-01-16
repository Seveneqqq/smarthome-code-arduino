#include <ArduinoJson.h>
#include "DHT.h"
#include <Servo.h>

#define SERIAL_TX_BUFFER_SIZE 256
#define SERIAL_RX_BUFFER_SIZE 256

#define LIGHT_GARDEN 0 
#define LIGHT_HALL 1
#define LIGHT_OFFICE 2
#define LIGHT_BEDROOM 3
#define LIGHT_KITCHEN 4
#define LIGHT_LIVING_ROOM 5
#define LIGHT_GARAGE 6
#define DHT11_PIN 7
#define FRONT_GATE_1 9 
#define FRONT_GATE_2 10
#define GARAGE_GATE 11
#define POMPA_CIEPLA 12
#define POMPA_CIEPLA_WYL 13



Servo frontGate1;
Servo frontGate2;
Servo garageGate;
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
    
    //---------------------------------------------------------------------------------------------
    pinMode(LIGHT_GARDEN, OUTPUT);
    pinMode(LIGHT_HALL, OUTPUT);
    pinMode(LIGHT_OFFICE, OUTPUT);
    pinMode(LIGHT_BEDROOM, OUTPUT);
    pinMode(LIGHT_KITCHEN, OUTPUT);
    pinMode(LIGHT_LIVING_ROOM, OUTPUT);
    pinMode(LIGHT_GARAGE, OUTPUT);
    pinMode(POMPA_CIEPLA, OUTPUT);
    pinMode(POMPA_CIEPLA_WYL, OUTPUT);
    //---------------------------------------------------------------------------------------------
    digitalWrite(POMPA_CIEPLA_WYL, HIGH); 

    dht.setup(DHT11_PIN);
    Serial.begin(9600);
    frontGate1.attach(FRONT_GATE_1);
    frontGate2.attach(FRONT_GATE_2);
    garageGate.attach(GARAGE_GATE);
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

    while (Serial.available() > 0) {
        char inChar = (char)Serial.read();
        
        if (bufferIndex < sizeof(jsonBuffer) - 1) {
            jsonBuffer[bufferIndex] = inChar;
            bufferIndex++;

            if (inChar == '\n' || inChar == '\r') {
                jsonBuffer[bufferIndex - 1] = '\0';
                
                char* start = strchr(jsonBuffer, '{');
                char* end = strrchr(jsonBuffer, '}');
                
                if (start != NULL && end != NULL && end > start) {
                    processJsonCommand(start);
                }
                

                bufferIndex = 0;
                memset(jsonBuffer, 0, sizeof(jsonBuffer));
            }
        } else {
            bufferIndex = 0;
            memset(jsonBuffer, 0, sizeof(jsonBuffer));
            sendErrorResponse("Buffer overflow", "");
        }
    }
}

void processJsonCommand(const char* jsonString) {
    StaticJsonDocument<200> debugDoc;
    debugDoc["type"] = "debug";
    debugDoc["received"] = jsonString;
    String debugOutput;
    serializeJson(debugDoc, debugOutput);
    Serial.println(debugOutput);

    StaticJsonDocument<800> doc;
    DeserializationError error = deserializeJson(doc, jsonString);

    if (error) {
        char errorMsg[100];
        snprintf(errorMsg, sizeof(errorMsg), "Deserializacja JSON nie powiodła się: %s", error.c_str());
        sendErrorResponse(errorMsg, "");
        return;
    }

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
    

    if(strcmp(deviceName, "LIGHT_GARDEN") == 0) {
            int pwmValue = map(brightness, 0, 100, 0, 255);
            if(state == 1) {
                analogWrite(lightPin, pwmValue);
            } else {
                analogWrite(lightPin, 0);
            }
        }
        else if(strcmp(deviceName, "LIGHT_GARAGE") == 0) {
            int pwmValue = map(brightness, 0, 100, 0, 255);
            if(state == 1) {
                analogWrite(lightPin, pwmValue);
            } else {
                analogWrite(lightPin, 0);
            }
        }
        else if(strcmp(deviceName, "LIGHT_LIVING_ROOM") == 0) {
            int pwmValue = map(brightness, 0, 100, 0, 255);
            if(state == 1) {
                analogWrite(lightPin, pwmValue);
            } else {
                analogWrite(lightPin, 0);
            }
        }
        else if(strcmp(deviceName, "LIGHT_KITCHEN") == 0) {
            int pwmValue = map(brightness, 0, 100, 0, 255);
            if(state == 1) {
                analogWrite(lightPin, pwmValue);
            } else {
                analogWrite(lightPin, 0);
            }
        }
        else if(strcmp(deviceName, "LIGHT_BEDROOM") == 0) {
            int pwmValue = map(brightness, 0, 100, 0, 255);
            if(state == 1) {
                analogWrite(lightPin, pwmValue);
            } else {
                analogWrite(lightPin, 0);
            }
        }
        else if(strcmp(deviceName, "LIGHT_OFFICE") == 0) {
            int pwmValue = map(brightness, 0, 100, 0, 255);
            if(state == 1) {
                analogWrite(lightPin, pwmValue);
            } else {
                analogWrite(lightPin, 0);
            }
        }
        else if(strcmp(deviceName, "LIGHT_HALL") == 0) {
            int pwmValue = map(brightness, 0, 100, 0, 255);
            if(state == 1) {
                analogWrite(lightPin, pwmValue);
            } else {
                analogWrite(lightPin, 0);
            }
        }

    else if(strcmp(deviceName, "HEAT_PUMP") == 0) {
    if(state == 1) {
        digitalWrite(POMPA_CIEPLA, HIGH);
        digitalWrite(POMPA_CIEPLA_WYL, LOW);    
    } else {
        digitalWrite(POMPA_CIEPLA, LOW);
        digitalWrite(POMPA_CIEPLA_WYL, HIGH);   
    }
}
    else if(strcmp(deviceName, "FRONT_GATE") == 0) {
        if(state == 0){
          frontGate1.write(162);
          frontGate2.write(43);
          delay(500);
        }
        else{
        frontGate1.write(62);
        frontGate2.write(140);
        delay(500);
        }
    }
    else if(strcmp(deviceName, "GARAGE_GATE") == 0) {
        if(state == 0){
          garageGate.write(10);
          delay(500);
        }
        else{
        garageGate.write(160);
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
        int targetTemp = device["actions"]["temperature"] | 24;

        if(strcmp(deviceName, "LIGHT_GARDEN") == 0) {
            int pwmValue = map(brightness, 0, 100, 0, 255);
            if(state == 1) {
                analogWrite(lightPin, pwmValue);
            } else {
                analogWrite(lightPin, 0);
            }
        }
        else if(strcmp(deviceName, "LIGHT_GARAGE") == 0) {
            int pwmValue = map(brightness, 0, 100, 0, 255);
            if(state == 1) {
                analogWrite(lightPin, pwmValue);
            } else {
                analogWrite(lightPin, 0);
            }
        }
        else if(strcmp(deviceName, "LIGHT_LIVING_ROOM") == 0) {
            int pwmValue = map(brightness, 0, 100, 0, 255);
            if(state == 1) {
                analogWrite(lightPin, pwmValue);
            } else {
                analogWrite(lightPin, 0);
            }
        }
        else if(strcmp(deviceName, "LIGHT_KITCHEN") == 0) {
            int pwmValue = map(brightness, 0, 100, 0, 255);
            if(state == 1) {
                analogWrite(lightPin, pwmValue);
            } else {
                analogWrite(lightPin, 0);
            }
        }
        else if(strcmp(deviceName, "LIGHT_BEDROOM") == 0) {
            int pwmValue = map(brightness, 0, 100, 0, 255);
            if(state == 1) {
                analogWrite(lightPin, pwmValue);
            } else {
                analogWrite(lightPin, 0);
            }
        }
        else if(strcmp(deviceName, "LIGHT_OFFICE") == 0) {
            int pwmValue = map(brightness, 0, 100, 0, 255);
            if(state == 1) {
                analogWrite(lightPin, pwmValue);
            } else {
                analogWrite(lightPin, 0);
            }
        }
        else if(strcmp(deviceName, "LIGHT_HALL") == 0) {
            int pwmValue = map(brightness, 0, 100, 0, 255);
            if(state == 1) {
                analogWrite(lightPin, pwmValue);
            } else {
                analogWrite(lightPin, 0);
            }
        }
        
        else if(strcmp(deviceName, "HEAT_PUMP") == 0) {
            if(state == 1) {  
                  float currentTemp = dht.getTemperature();  
                  if(currentTemp < targetTemp && state == 1) {
                      digitalWrite(POMPA_CIEPLA, HIGH);
                      digitalWrite(POMPA_CIEPLA_WYL, LOW);    
                  } else {
                      digitalWrite(POMPA_CIEPLA, LOW);
                      digitalWrite(POMPA_CIEPLA_WYL, HIGH);   
                  }
            } else {
                  digitalWrite(POMPA_CIEPLA_WYL, state == 1 ? LOW : HIGH);
                  digitalWrite(POMPA_CIEPLA, LOW);
            }
        }
        else if(strcmp(deviceName, "FRONT_GATE") == 0) {
          if(state == 0){
          frontGate1.write(162);
          frontGate2.write(43);
          delay(500);
          }
          else{
          frontGate1.write(62);
          frontGate2.write(140);
          delay(500);
          }
        }
        else if(strcmp(deviceName, "GARAGE_GATE") == 0) {
        if(state == 0){
          garageGate.write(10);
          delay(500);
        }
        else{
        garageGate.write(160);
        delay(500);
        }
    }
    }
}

void sendSensorData() {
    StaticJsonDocument<200> doc;
  
    doc["type"] = "sensor_data";
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
    device1["name"] = "LIGHT_GARDEN";
    device1["status"] = "active";

    JsonObject device2 = devices.createNestedObject();
    device2["id"] = 2;
    device2["name"] = "LIGHT_GARAGE";
    device2["status"] = "active";

    JsonObject device3 = devices.createNestedObject();
    device3["id"] = 3;
    device3["name"] = "FRONT_GATE";
    device3["status"] = "active";

    JsonObject device4 = devices.createNestedObject();
    device4["id"] = 4;
    device4["name"] = "GARAGE_GATE";
    device4["status"] = "active";

    JsonObject device5 = devices.createNestedObject();
    device5["id"] = 5;
    device5["name"] = "HEAT_PUMP";
    device5["status"] = "active";

    JsonObject device6 = devices.createNestedObject();
    device6["id"] = 6;
    device6["name"] = "temperature and humidity sensor";
    device6["status"] = "active";

    JsonObject device7 = devices.createNestedObject();
    device7["id"] = 7;
    device7["name"] = "LIGHT_LIVING_ROOM";
    device7["status"] = "active";
    
    JsonObject device8 = devices.createNestedObject();
    device8["id"] = 8;
    device8["name"] = "LIGHT_KITCHEN";
    device8["status"] = "active";

    JsonObject device9 = devices.createNestedObject();
    device9["id"] = 9;
    device9["name"] = "LIGHT_BEDROOM";
    device9["status"] = "active";

    JsonObject device10 = devices.createNestedObject();
    device10["id"] = 10;
    device10["name"] = "LIGHT_OFFICE";
    device10["status"] = "active";

    JsonObject device11 = devices.createNestedObject();
    device11["id"] = 11;
    device11["name"] = "LIGHT_HALL";
    device11["status"] = "active";

    String jsonOutput;
    serializeJson(doc, jsonOutput);
    Serial.println(jsonOutput);
}