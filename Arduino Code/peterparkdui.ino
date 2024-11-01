#include <Wire.h>
#include <Servo.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo myservo;

int Slot = 4;           // Total number of parking Slots

int IR1 = 2;
int IR2 = 3;
int flag1 = 0;
int flag2 = 0;

// WiFi settings
const char* ssid = "TOTfttx_98/140_2.4G";
const char* password = "0813630897";

// MQTT settings
const char* mqtt_server = "mqtt-dashboard.com";
const char* mqtt_topic = "peter/parkdui";
const int mqtt_port = 8884;                    

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  lcd.init(); // Initialize the lcd
  lcd.backlight(); // Open the backlight

  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  
  myservo.attach(4);
  myservo.write(100);

  lcd.setCursor(0, 0);
  lcd.print("     ARDUINO    ");
  lcd.setCursor(0, 1);
  lcd.print(" PARKING SYSTEM ");
  delay(2000);
  lcd.clear();  

  // Connect to WiFi
  connectWiFi();

  // Set up MQTT client
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (digitalRead(IR1) == LOW && flag1 == 0) {
    if (Slot > 0) {
      flag1 = 1;
      if (flag2 == 0) {
        myservo.write(0);
        Slot = Slot - 1;
        publishStatus(); // Publish status
        publishMessage("IN");
      }
    } else {
      lcd.setCursor(0, 0);
      lcd.print("    SORRY :(    ");  
      lcd.setCursor(0, 1);
      lcd.print("  Parking Full  "); 
      delay(3000);
      lcd.clear(); 
    }
  }

  if (digitalRead(IR2) == LOW && flag2 == 0) {
    flag2 = 1;
    if (flag1 == 0) {
      myservo.write(0);
      Slot = Slot + 1;
      publishStatus(); // Publish status
      publishMessage("OUT");
    }
  }

  if (flag1 == 1 && flag2 == 1) {
    delay(1000);
    myservo.write(100);
    flag1 = 0;
    flag2 = 0;
  }

  lcd.setCursor(0, 0);
  lcd.print("    WELCOME!    ");
  lcd.setCursor(0, 1);
  lcd.print("Slot Left: ");
  lcd.print(Slot);
}

void connectWiFi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ParkingSystemClient")) { // Add username and password if required
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state()); // Print reason for failure
      delay(2000);
    }
  }
}

void publishStatus() {
  String statusMessage = "Slots Left: " + String(Slot);
  client.publish(mqtt_topic, statusMessage.c_str());
}

void publishMessage(const char* message) {
  client.publish(mqtt_topic, message); // Publish the message to the MQTT topic
}