#include <ESP8266WiFi.h>
#include <Ultrasonic.h>
#include <WebSocketClient.h>

// network stuff
const char* ssid     = "tjliphone";
const char* password = "abcdefgh";
char host[] = "172.20.10.1"; // iPhone personal hotspot gateway
char path[] = "/";
const int httpPort = 8000;
WebSocketClient webSocketClient;
WiFiClient client;

// sonar stuff
#define MAX_CM 300
int range = MAX_CM * 58; // 200 cm * 58 usec
#define SONAR_NUM 2
unsigned int cm[SONAR_NUM];         // Where the ping distances are stored.
Ultrasonic sonar[SONAR_NUM] = {
  Ultrasonic(D2, D1, range),
  Ultrasonic(D8, D7, range)
};

// misc stuff
#define LEDPIN D0
int noop = 0;

void ledFail(int x) {
  digitalWrite(LEDPIN, LOW);
  delay(1000 - (x * 100));
  for (uint8_t i=0; i<x; i++) {
    digitalWrite(LEDPIN, HIGH);
    delay(50);
    digitalWrite(LEDPIN, LOW);
    delay(50);
  }
}

void setup() {
  Serial.begin(115200); 

  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, HIGH);

  Serial.println();
  Serial.print("joining SSID " + String(*ssid));
    
  WiFi.begin(ssid, password);

  digitalWrite(LEDPIN, LOW);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to the websocket server
  if (client.connect(host, httpPort)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
    while(1) {
      ledFail(1);
    }
  }

  // Handshake with the server
  webSocketClient.path = path;
  webSocketClient.host = host;
  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
    webSocketClient.sendData("RANGE:" + String(MAX_CM));
  } else {
    Serial.println("Handshake failed.");
    while(1) {
      ledFail(2);
    }  
  }
}

void loop()
{
   String data;

  if (client.connected()) {

    digitalWrite(LEDPIN, HIGH);
    
    webSocketClient.getData(data);
    if (data.length() > 0) {
      Serial.print("Received data: ");
      Serial.println(data); // this is unlikely
    }

    // capture the value of sensor in cm, send it along
    bool found_something_interesting = false;
    for (uint8_t i = 0; i < SONAR_NUM; i++) {
      cm[i] = sonar[i].Ranging(CM);
      if (cm[i] != MAX_CM)
        found_something_interesting = true;
    }
    
    if (found_something_interesting || (noop > 30)) {
      digitalWrite(LEDPIN, LOW);
      data = String(millis());
      for (uint8_t i = 0; i < SONAR_NUM; i++) {
        data = data + "/" + String(cm[i]);
      }
      webSocketClient.sendData(data);
      noop = 0;
      digitalWrite(LEDPIN, HIGH);
    }
    else {
      noop = noop + 1;
    }
  } 
  else {
    Serial.println("Client disconnected.");
    while (1) {
      ledFail(3);
    }
  }
}
