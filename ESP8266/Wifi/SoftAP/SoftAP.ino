// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>


const char* ssid     = "E-glove";
const char* password = "123456789";

const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0
uint8_t LEDpin = 2; 
float gtoN=9.81/1000, inter=-25.161, slope=0.3418 ,invslope= 1/slope;

// current temperature & humidity, updated in loop()
float ft = 0.0;
float fi = 0.0;
float fm = 0.0;
float fa = 0.0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 20 milliseconds
const long interval = 20;  

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    body {
      min-width: 310px;
     max-width: 800px;
      height: 400px;
      margin: 0 auto;
    }
    h2 {
      font-family: Arial;
      font-size: 2.5rem;
      text-align: center;
    }
      .cardt{
    max-width: 350px;
     min-height: 250px;
     background: #02b875;
     padding: 30px;
     box-sizing: border-box;
     color: #FFF;
     margin:20px;
     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
}
           .cardi{
    max-width: 350px;
     min-height: 250px;
     background: #02b875;
     padding: 30px;
     box-sizing: border-box;
     color: #FFF;
     margin:20px;
     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
}
           .cardm{
    max-width: 350px;
     min-height: 250px;
     background: #02b875;
     padding: 30px;
     box-sizing: border-box;
     color: #FFF;
     margin:20px;
     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
}
           .carda{
    max-width: 350px;
     min-height: 250px;
     background: #02b875;
     padding: 30px;
     box-sizing: border-box;
     color: #FFF;
     margin:20px;
     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
}

  </style>
</head>
<body>
<h2>E-glove interface</h2>
   <div class="cardt">
  <h3>Thumb force:        </h3>
  <h1><span id="Thumb">%THUMBFORCE%</span> N</h1>
  </div>
      <div class="cardi" >
  <h3>Index force:        </h3>
          <h1><span id="Index">%INDEXFORCE%</span> N</h1>
  </div>
      <div class="cardm">
  <h3>Middle finger force:</h3>
          <h1><span id="Middle">%MIDDLEFORCE%</span> N</h1>
  </div>
      <div class="carda">
  <h3>Arch force:         </h3>
          <h1><span id="Arch">%ARCHFORCE%</span> N</h1>
    </div>
  
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 20) {
      document.getElementById("Thumb").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "/thumb", true);
  xhttp.send();
}, 20 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 20) {
        document.getElementById("Index").innerHTML =
      this.responseText;
     
    }
  };
  xhttp.open("GET", "/index", true);
  xhttp.send();
}, 20 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 20) {
        document.getElementById("Middle").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "/middle", true);
  xhttp.send();
}, 20 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 20) {
      document.getElementById("Arch").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "/arch", true);
  xhttp.send();
}, 20 ) ;

</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "THUMBFORCE"){
    return String(ft);
  }
  else if(var == "INDEXFORCE"){
    return String(fi);
  }
  else if(var == "MIDDLEFORCE"){
    return String(fm);
  }
  else if(var == "ARCHFORCE"){
    return String(fa);
  }
  return String();
}

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
  
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(20, "text/html", index_html, processor);
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
