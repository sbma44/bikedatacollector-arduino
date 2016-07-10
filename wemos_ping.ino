#include <ESP8266WiFi.h>
#include <Ultrasonic.h>

const char* ssid     = "Q Continuum";
const char* password = "section31";
const char* host = "192.168.0.101";
const int httpPort = 9999;

int range = 200 * 58; // 200 cm * 58 usec
Ultrasonic ultrasonic(D6,D5,range); // (Trig PIN,Echo PIN,delay in usec)

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
}

void loop()
{
  WiFiClient client;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    delay(1000);
    return;
  }
  Serial.println("connected");

  while(true) {
    client.print(ultrasonic.Ranging(CM));
//    delay(100);
  }
}
