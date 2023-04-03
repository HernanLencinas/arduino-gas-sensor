#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define LED_RED_PIN 16
#define LED_GREEN_PIN 5 
#define LED_BLUE_PIN 4

const char* ssid = "";
const char* password = "";
const char* mqtt_server = "";
const char* mqttUser = "";
const char* mqttPassword = "";
const String mqttTopic = "/testing";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
String clientId = "esp8266-";
String macaddr;
String publishTopic;

const int analogInPin = A0;
int sensorValue = 0;

void setup_wifi() {
    delay(100);
    Serial.println("");
    Serial.print("Connecting to ");
    Serial.print(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  
} 

void reconnect() {
  while (!client.connected()) 
  {
    Serial.println("Attempting MQTT connection...");
    macaddr = String(WiFi.macAddress());
    macaddr.replace(":","");
    macaddr.toLowerCase();
    clientId += macaddr;
    publishTopic = macaddr;
    publishTopic += mqttTopic;
    
    if (client.connect("esp8622", mqttUser, mqttPassword)) {
      Serial.print("MQTT connected [");
      Serial.print(clientId);
      Serial.println("]");
      client.subscribe("iot");
    } else {
      Serial.print("MQTT failed, rc=");
      Serial.print(client.state());
      Serial.println("MQTT try again in 5 seconds");
      delay(5000);
    }
  }
} 

void RGB_color(int LED_RED_VALUE, int LED_GREEN_VALUE, int LED_BLUE_VALUE)
 {
  analogWrite(LED_RED_PIN, LED_RED_VALUE);
  analogWrite(LED_GREEN_PIN, LED_GREEN_VALUE);
  analogWrite(LED_BLUE_PIN, LED_BLUE_VALUE);
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(LED_RED_PIN,OUTPUT);
  pinMode(LED_GREEN_PIN,OUTPUT);
  pinMode(LED_BLUE_PIN,OUTPUT);
  
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  
  if (now - lastMsg > 1000) {
     lastMsg = now;
     sensorValue = analogRead(analogInPin);

     switch (sensorValue) {
        case 0 ... 340:
          RGB_color(0, 255, 0); 
          break;
        case 341 ... 360:
          RGB_color(255, 255, 0); 
          break;
        case 361 ... 1024:
          RGB_color(255, 0, 0); 
          break;
        default: 
          RGB_color(0, 0, 255); 
          break;
    }

     if (sensorValue > 120) {
       Serial.println(sensorValue);
       client.publish( publishTopic.c_str(), "Normal" );
     }

     if (sensorValue > 500) {
       Serial.println(sensorValue);
       client.publish( publishTopic.c_str(), "Warning" );
     }
  
     /* client.publish( publishTopic.c_str(), "Running" ); */
     Serial.println(sensorValue);   
    
  }

  /*
  RGB_color(255, 0, 0); // Red
  RGB_color(0, 255, 0); // Green
  RGB_color(0, 0, 255); // Blue
  RGB_color(255, 255, 125); // Raspberry
  RGB_color(0, 255, 255); // Cyan
  RGB_color(255, 0, 255); // Magenta
  RGB_color(255, 255, 0); // Yellow
  RGB_color(255, 255, 255); // White 
  */
  
}
