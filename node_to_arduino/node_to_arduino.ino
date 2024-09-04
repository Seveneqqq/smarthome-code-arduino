#include <Servo.h>
#include <ArduinoJson.h>

int lightPin = 5;
int lightPin2 = 4;
Servo serwomechanizm;  
unsigned long previousMillis = 0;  
const long interval = 1000;  

void setup() {
  pinMode(lightPin, OUTPUT);
  pinMode(lightPin2, OUTPUT);
  serwomechanizm.attach(9);
  Serial.begin(9600);
}

void loop() {
  // Sprawdzenie, czy są dostępne dane
  if (Serial.available() > 0) {
    String receivedString = "";
    
    while (Serial.available() > 0) {
      receivedString += char(Serial.read());
    }
    
    // Wydruk odbieranej wiadomości
    //Serial.println("Received: " + receivedString);
  
    // Sprawdzenie, czy wiadomość jest w formacie JSON
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
      
  }
  //serwo();
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
  
  // Tworzenie tablicy JSON dla urządzeń
  JsonArray devices = doc.createNestedArray("devices");

  // Dodanie pierwszego urządzenia do tablicy
  JsonObject device1 = devices.createNestedObject();
  device1["id"] = 1;
  device1["name"] = "LED1";
  device1["status"] = "active";

  // Dodanie drugiego urządzenia do tablicy
  JsonObject device2 = devices.createNestedObject();
  device2["id"] = 2;
  device2["name"] = "LED2";
  device2["status"] = "not-active";
  
  // Dodanie trzeciego urządzenia do tablicy (np. serwomechanizm)
  JsonObject device3 = devices.createNestedObject();
  device3["id"] = 3;
  device3["name"] = "Servo";
  device3["status"] = "active";

  // Serializacja dokumentu JSON do Stringa
  String jsonOutput;
  serializeJson(doc, jsonOutput);

  // Wysłanie JSON przez Serial
  Serial.println(jsonOutput);
}
