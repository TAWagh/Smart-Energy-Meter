
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "****";
const char* password = "*****";
const char* mqtt_server = "*****";  
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
char msg2[50];


#define imp D7

int count=0;
int threshold=5;
int flag=0;
float percentage=0;
String state;


void setup() {
  pinMode(2, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '0') {
     Serial.println("LOW");
    digitalWrite(2, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } 

 if ((char)payload[0] == '1') {
    Serial.println("HIGH");
    digitalWrite(2, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("event", "hello world");
      // ... and resubscribe
      client.subscribe("event");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop(){

if(digitalRead(imp)==HIGH){
     count++;

     
          if(count<threshold){
            flag=0;//within limits
            state= "ALERT";
       }
      if(count>=threshold){
            flag=1;//ALERT
            state= "within Limis";
       }

  }

     Serial.print( "count");
     Serial.println(count);    
     Serial.print( "thresold ");
     Serial.println(threshold);     
     Serial.print( "percentage");
     percentage=float((count/threshold)*100);
     Serial.println(percentage);
     Serial.print( "flag");
     Serial.println(flag);

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
   
    snprintf (msg, 75, " %ld Wh", count);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("event1", msg);

    snprintf (msg2, 75, "%ld Wh", threshold);
    Serial.print("Publish message: ");
    Serial.println(msg2);
    client.publish("event2", msg2);

  //event 3
    if (flag==0){
    client.publish("event3","within limits",13);
      }
    if (flag==1){
      client.publish("event3", "ALERT",5);
      }
//evenr 4
    char PString[8];
    dtostrf(percentage, 1, 2, PString);
    client.publish("event4", PString);

//Event 5
if (count==threshold){
      client.publish("event5", "ALERT",5);
      }


  }
}
