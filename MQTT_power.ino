

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker
 
/* Set these to your desired credentials. */
const char* ssid = "TEK5110";  //ENTER YOUR WIFI SETTINGS
const char* wifi_password = "UioTEK5110";

// MQTT
// Make sure to update this for your own MQTT Broker!
const char* mqtt_server = "192.168.1.7";
const char* mqtt_topic = "powerConsumption";
// The client id identifies the ESP8266 device. Think of it a bit like a hostname (Or just a name, like Greg).
const char* clientID = "ESP02";



Adafruit_ADS1115 ads(0x48);
float Voltage = 0.0, Current = 0, power=0;
//=======================================================================
//                    Power on setup
//=======================================================================
// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient); // 1883 is the listener port for the Broker

void setup() {
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //Only Station No AP, This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, wifi_password);     //Connect to your WiFi router
  Serial.println("");
 
  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  if (client.connect(clientID)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }

  ads.setGain(GAIN_ONE);
  ads.begin();   // start analog-to-digital convertor
}
 
//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {
  int16_t adc0, adc1;

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  Voltage = ((adc0 * 4.096)/32767)*5;
  Current = (adc1 * 4.096) / 32767;
  Serial.print("AIN0: "); 
  Serial.print(adc0);
  Serial.print("\tVoltage: ");
  Serial.println(Voltage, 7); 
  
  Serial.print("AIN1: "); 
  Serial.print(adc1);
  Serial.print("\tCurrent: ");
  Serial.println(Current, 7); 
  
  
  power = Voltage * Current;
  char power_str[20];
  sprintf(power_str, "%f", power);
  
  if (client.publish(mqtt_topic, power_str)) {
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(mqtt_topic, power_str);
  }

    
  delay(2000);  //POST Data at every 2 seconds
}
