
#include <WiFi.h>
#include <ESP8266HTTPClient.h>
#include <LoRaMESH.h>
#include <SoftwareSerial.h>
#include <stdint.h>
#include <stdbool.h>
#include <ESP8266WiFi.h>

/* This script:
    - Initializes the software serial interface on the pins 6 (RX) and 7 (TX).
    - Configures the GPIOs 5 and 6 of a remote device as analog inputs.
    - Sends requests for analog values periodically every 2 s.
    - Shows the analog values on the monitor.
*/

/* Local device ID */
uint16_t localId;
/* Remote device ID */
uint16_t remoteId;
/* Local device Unique ID */
uint32_t localUniqueId;
/* Local device Net */
uint16_t localNet;
/* Remote device ID to communicate with */
uint16_t ID = 1;

/* Payload buffer */
uint8_t bufferPayload[MAX_PAYLOAD_SIZE] = {0};
/* Values read on each analog inputs */
uint16_t AdcIn[2];

/* SoftwareSerial handles */
SoftwareSerial* hSerialCommands = NULL;

char * ssid = "LUMOLAB";
char * password = "MacaxeiracomGalinha";


void setup() {
  //Init the serial monitor
  Serial.begin(115200);
  delay(4000);   //Delay needed before calling the WiFi.begin

  /* Initialize SoftwareSerial */
  hSerialCommands = SerialCommandsInit(7, 6, 9600);

    /* Gets the local device ID */
  if(LocalRead(&localId, &localNet, &localUniqueId) != MESH_OK)
    Serial.print("Couldn't read local ID\n\n");
  else
  {
    Serial.print("Local ID: ");
    Serial.println(localId);
    Serial.print("Local NET: ");
    Serial.println(localNet);
    Serial.print("Local Unique ID: ");
    Serial.println(localUniqueId, HEX);
    Serial.print("\n");
  }

  delay(500);

    /* Configures the remote device's analog pins */
  if(GpioConfig(ID, GPIO5, ANALOG_IN, PULL_OFF) != MESH_OK)
    Serial.print("Error configuring GPIO5\n\n");

  delay(500);
  if(GpioConfig(ID, GPIO6, ANALOG_IN, PULL_OFF) != MESH_OK)
    Serial.print("Error configuring GPIO6\n\n");

  //Init the conection with the wifi
  WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
}
 
void loop() {
  delay(1000);
  if(GpioRead(ID, GPIO5, &AdcIn[0]) != MESH_OK)
    Serial.print("\nCommunication error\n\n");
  else
  {
    /* Sends to monitor */
    Serial.print("\nGPIO5: ");
    Serial.print(float(AdcIn[0])*8.0586e-4);
    Serial.print("V\n");
  }

  delay(1000);
  if(GpioRead(ID, GPIO6, &AdcIn[1]) != MESH_OK)
    Serial.print("\nCommunication error\n\n");
  else
  {
    /* Sends to monitor */
    Serial.print("\nGPIO6: ");
    Serial.print(float(AdcIn[1])*8.0586e-4);
    Serial.print("V\n");
  }

 if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
 
   HTTPClient http;   
 
   http.begin("http://jsonplaceholder.typicode.com/posts");  //Specify destination for HTTP request
   http.addHeader("Content-Type", "text/plain");             //Specify content-type header
 
   int httpResponseCode = http.POST("POSTING from ESP32");   //Send the actual POST request
 
   if(httpResponseCode>0){
 
    String response = http.getString();                       //Get the response to the request
 
    Serial.println(httpResponseCode);   //Print return code
    Serial.println(response);           //Print request answer
 
   }else{
 
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
 
   }
 
   http.end();  //Free resources
 
 }else{
 
    Serial.println("Error in WiFi connection");   
 
 }
 
  delay(10000);  //Send a request every 10 seconds
 
}
