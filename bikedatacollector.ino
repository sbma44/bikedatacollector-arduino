#include <ESP8266WiFi.h>
#include <Ultrasonic.h>
#include <WebSocketClient.h>

const char* ssid     = "Q Continuum";
const char* password = "section31";
char host[] = "192.168.0.101";
//char host[] = "echo.websocket.org";
char path[] = "/";
const int httpPort = 8000;

int range = 200 * 58; // 200 cm * 58 usec
Ultrasonic ultrasonic(D6, D5, range); // (Trig PIN,Echo PIN,delay in usec)
WebSocketClient webSocketClient;
WiFiClient client;

void setup() {
  Serial.begin(115200); 

  Serial.println();
  Serial.print("joining SSID " + String(*ssid));
    
  WiFi.begin(ssid, password);

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
      // Hang on failure
    }
  }

  // Handshake with the server
  webSocketClient.path = path;
  webSocketClient.host = host;
  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
  } else {
    Serial.println("Handshake failed.");
    while(1) {
      // Hang on failure
    }  
  }
}

void loop()
{
   String data;

  if (client.connected()) {
    
    webSocketClient.getData(data);
    if (data.length() > 0) {
      Serial.print("Received data: ");
      Serial.println(data);
    }
    
    // capture the value of sensor in cm, send it along
    data = String(ultrasonic.Ranging(CM));
    
    webSocketClient.sendData(data);
    
  } else {
    Serial.println("Client disconnected.");
    while (1) {
      // Hang on disconnect.
    }
  }
  
  // wait to fully let the client disconnect
  delay(1000);
}
