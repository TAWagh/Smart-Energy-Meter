#include <ESP8266WiFi.h>  //all wifi related functions (like access point)
#include <WiFiClient.h>   //send requests to web browser
#include <ESP8266WebServer.h>   //handles all HTTP protocols 

#define imp D6
#define testLED D7

int count=0;
float threshold=5;
int flag=0;
String state;


//SSID and Password of your WiFi router
const char* ssid = "********";
const char* password = "********";
 
ESP8266WebServer server(80); //create server object on port 80(default)
//to open web page with different port number you have to enter port number after IP address. Ex. For port number 81 you have to type  192.168.2.2:81  in browser.

// HTML webpage contents
const char MAIN_page[] PROGMEM = R"=====(
<HTML>
  <HEAD>
      <TITLE>Smart Energy Meter</TITLE>
  </HEAD>
<BODY>
  <CENTER>
      <B><h2>Smart Energy Meter<h2></B>
      <h3>Know Your Power</h3>
  </CENTER> 
  
<P>Power consumed:@@POWER@@ kW.</P> <br>
<P>Status:@@STATUS@@</P>

<form action="/action_page">
  Threshold Value: <input type="text" name="value"  value="Enter Here">
  <br><br>
  <input type="submit" value="Update">
</form> 
</BODY>
</HTML>
)=====";



void handleRoot() {// This routine is executed when you open its IP in browser
 String s = MAIN_page; //Read HTML contents
 s.replace("@@POWER@@",String(count));
 if (flag==0)
 s.replace("@@STATUS@@","WITHIN LIMIT");
 if (flag==1)
 s.replace("@@STATUS@@","ALERT");
 server.send(200, "text/html", s); //Send web page
}



void handleInput() { // This routine is executed when you press submit
 String th = server.arg("value"); 
 threshold=th.toFloat();
 Serial.print("Threshld Value:");
 Serial.println(threshold);
 
 String s = "<a href='/'> Go Back </a>";
 server.send(200, "text/html", s); //Send web page
}


void handleAlert() { 
String s = app; 
  if (flag==1){
 server.send(200, "text/html", "Alert"); 
}

}


void setup(void){

  pinMode(imp,INPUT); 
  
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");
 
  // Waiting for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address of NodeMCU
 
  server.on("/", handleRoot);  
  server.on("/action_page", handleInput);
  server.on("/alert", handleAlert);
  server.begin();  //Start server
  Serial.println("HTTP server started");
}



void loop(void){
  server.handleClient();//Handle client requests
  if(digitalRead(imp)==HIGH){
     digitalWrite(testLED, HIGH);
    count++;
    
    if(count<threshold){
       flag=0;//within limit
       }
    if(count>=threshold){
       flag=1;//ALERT
       }
       
    Serial.println(threshold); 
    Serial.println(count);
    Serial.println(flag);
  }
  digitalWrite(testLED, LOW);
}
