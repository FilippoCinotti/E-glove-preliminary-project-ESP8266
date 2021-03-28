// Import required libraries
  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <Hash.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  
uint8_t LEDpin = 2; 
const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0
float gtoN=9.81/1000, inter=-25.161, slope=0.3418 ,invslope= 1/slope;
// Replace with your network credentials
const char* ssid = "Your network";
const char* password = "Your password";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readThumb() {
  // Read thumb force
  int t = analogRead(analogInPin);
  // Convert to force
  float ft = ((t*invslope)+inter)*(gtoN);
  int tmap=map(t,0,350,0,1023);
  analogWrite(LEDpin,tmap);
  Serial.println(ft);
  return String(ft);
}
String readIndex() {
  // Read thumb force
  int i = analogRead(analogInPin);
  // Convert to force
  float fi = ((i*invslope)+inter)*(gtoN);
  Serial.println(fi);
  return String(fi);
}
String readMiddle() {
  // Read thumb force
  int m = analogRead(analogInPin);
  // Convert to force
  float fm = ((m*invslope)+inter)*(gtoN);
  Serial.println(fm);
  return String(fm);
}
String readArch() {
  // Read thumb force
  int a = analogRead(analogInPin);
  // Convert to force
  float fa = ((a*invslope)+inter)*(gtoN);
  Serial.println(fa);
  return String(fa);
}


void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(LEDpin, OUTPUT);
 

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  server.on("/thumb", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(20, "text/plain", readThumb().c_str());
  });
  server.on("/index", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(20, "text/plain", readIndex().c_str());
  });
  server.on("/middle", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(20, "text/plain", readMiddle().c_str());
  });
  server.on("/arch", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(20, "text/plain", readArch().c_str());
  });

  // Start server
  server.begin();
}
 
void loop(){
  
}
