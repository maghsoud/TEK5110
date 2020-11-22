/* This code inspired by following instruction:
 *  https://circuits4you.com/2019/02/08/esp8266-nodemcu-https-secured-post-request/
 *  https://github.com/esp8266/Arduino/issues/5975
 *  https://stackoverflow.com/questions/4980912/username-and-password-in-https-url
 *  https://learn.sparkfun.com/tutorials/piezo-vibration-sensor-hookup-guide/all
 */

#include <ESP8266WiFi.h>
//#include <WiFiClientSecure.h> 
//#include <ESP8266WebServer.h>
//#include <ESP8266HTTPClient.h>
//#include <base64.h> //used for base64 encoding of HTTP authentication
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker

 
// WiFi
// Make sure to update this for your own WiFi network!
const char* ssid = "Your SSID";
const char* wifi_password = "Your Password";

// MQTT
// Make sure to update this for your own MQTT Broker!
const char* mqtt_server = "192.168.1.7";
const char* mqtt_topic1 = "temperature";
const char* mqtt_topic2 = "humidity";
// The client id identifies the ESP8266 device. Think of it a bit like a hostname (Or just a name, like Greg).
const char* clientID = "ESP01";
 


#define DHTPIN            2         // Pin which is connected to the DHT sensor.
// Uncomment the type of sensor in use:
#define DHTTYPE           DHT11     // DHT 11 
//#define DHTTYPE           DHT22     // DHT 22 (AM2302)
//#define DHTTYPE           DHT21     // DHT 21 (AM2301)
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
int status = WL_IDLE_STATUS;
float myTemperature = 0, myHumidity = 0; 
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
  

  dht.begin();
   Serial.println("DHTxx Unified Sensor Example");
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  /*Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");  
  Serial.println("------------------------------------");
  // Print humidity sensor details. */
  dht.humidity().getSensor(&sensor);
 /* Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");  
  Serial.println("------------------------------------");
  // Set delay between sensor readings based on sensor details.
  */
  delayMS = sensor.min_delay / 1000;
}
 
//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {
  // Read data from DHT
// Delay between measurements.
  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    // Update temperature and humidity
    myTemperature = (float)event.temperature;
    Serial.print("Temperature: ");
    Serial.print(myTemperature);
    Serial.println(" C");
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
     myHumidity = (float)event.relative_humidity;
    Serial.print("Humidity: ");
    Serial.print(myHumidity);
    Serial.println("%");
  }
  char temperature_str[20];
  sprintf(temperature_str, "%d", (int)myTemperature);
  char humidity_str[20];
  sprintf(humidity_str, "%d", (int)myHumidity);
  
  if (client.publish(mqtt_topic1, temperature_str)) {
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(mqtt_topic1, temperature_str);
  }

  if (client.publish(mqtt_topic2, humidity_str)) {
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(mqtt_topic2, humidity_str);
  }  
    
  delay(2000);  //Publish Data at every 2 seconds
}
