#include <Servo.h>
#include <ArduinoJson.h>

int lightPin = 5;
int lightPin2 = 4;
Servo serwomechanizm;  
unsigned long previousMillis = 0;  // Zmienna do przechowywania poprzedniego czasu
const long interval = 1000;  // Interwał czasu w milisekundach (1 sekunda)

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

    // Sprawdzenie wartości "device"
    String instruction = doc["instruction"].as<String>();
      if (instruction == "check-connection") {
        // Zapal wszystkie diody LED na 1 sekundę
        digitalWrite(lightPin, HIGH);
        digitalWrite(lightPin2, HIGH);
        delay(1000);
        digitalWrite(lightPin, LOW);
        digitalWrite(lightPin2, LOW);
        returnIsConnected();
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
