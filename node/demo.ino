#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"       // including the library of DHT11 temperature and humidity sensor

#define DHTTYPE DHT11   // DHT 11
#define DHTPin 5

const char *ssid = "Chinmay";  
const char *password = "qwertyasd";

const char *host = "smart-snib2.000webhostapp.com";
const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80

//SHA1 finger print of certificate use web browser to view and copy
const char fingerprint[] PROGMEM = "F3 1B B7 47 29 59 39 C1 91 7D B4 61 DA 4D EC 0D 8C E1 E7 C1";


//Relay Pin setup
int relayOne = 13; // D7 
int value = LOW;

int relayTwo = 12; // D6
int valueTwo = LOW;

// Vibrator Sensor Setup
int vs = 16; // vibration sensor, D0
int sdata = 0; // sensor data will be stored in this variable.
String vdata = "";


// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);                

float t ;
float h ;

//=======================================================================
//                    Power on setup
//=======================================================================

void setup() {
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //Only Station No AP, This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
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


  pinMode(relayOne, OUTPUT);
  digitalWrite(relayOne, HIGH);

  pinMode(relayTwo, OUTPUT);
  digitalWrite(relayTwo, HIGH);

 // pinMode(vs ,INPUT);

  dht.begin();  
  delay(700);
  pinMode(DHTPin ,INPUT);
  
}

//=======================================================================
//                    Main Program Loop
//=======================================================================

void loop() {
  
  WiFiClientSecure httpsClient;    //Declare object of class WiFiClient

  Serial.println(host);

  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  httpsClient.setFingerprint(fingerprint);
  httpsClient.setTimeout(15000); // 15 Seconds
  delay(1000);
  
  Serial.print("HTTPS Connecting");
  int r=0; //retry counter
  
  while((!httpsClient.connect(host, httpsPort)) && (r < 30)){
      delay(100);
      Serial.print(".");
      r++;
  }
  if(r==30) {
    Serial.println("Connection failed");
  }
  else {
    Serial.println("Connected to web");
  }
  
  String ADCData, getData, Link;
  int adcvalue=analogRead(A0);  //Read Analog value of LDR
  ADCData = String(adcvalue);   //String to interger conversion

  pinMode(DHTPin,INPUT);
    h = dht.readHumidity();
    t = dht.readTemperature();         
    Serial.print("Current humidity = ");
    Serial.print(h);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(t); 
    Serial.println("C  ");


    String tempString = String(t);
    String humString = String(h) ;


  //GET Data

  long measurement = vibration();
  //delay(50);
  vdata = vdata + "Vibration: " + measurement;  

  Serial.println("Vibration: "+vdata);
  vdata = "";

  String vibrations = String(measurement);
  
  Link = "/website/qlda/Getstatus.php?Temperature="+tempString+"&Humidity="+humString+"&LockId=l2a13"+"&vibrationValue="+vibrations+"&isconnect=1";

  Serial.print("requesting URL: ");
  Serial.println(host+Link);

  httpsClient.print(String("GET ") + Link + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +               
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
    
  while (httpsClient.connected()) {
   String line = httpsClient.readStringUntil('\n');
    
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }

  String line[3];
  String onn1 = "00";
  String onn2 = "01";
  String onn3 = "10";
  String onn4 = "11";
  
  int i=0;
  while(httpsClient.available()){        
     line[i] = httpsClient.readStringUntil('\n');  //Read Line by Line
     i++ ;
   }
  line[1].trim();
  line[2].trim();
  
  String off = String(line[1]);
  Serial.println("lock status:"+off); //Print response

  
  if(off.equals(onn1)){
   digitalWrite(relayOne, HIGH);
   value = HIGH;

   digitalWrite(relayTwo, HIGH);
   valueTwo = HIGH;
   
  Serial.println("Opened State: 00");
  }
  
  
  if(off.equals(onn2)){

  digitalWrite(relayOne, HIGH);
   value = HIGH;

   digitalWrite(relayTwo, LOW);
   valueTwo = LOW;

   Serial.println("Opened State: 01");
  }
  if(off.equals(onn3)){

  digitalWrite(relayOne, LOW);
   value = LOW;

   digitalWrite(relayTwo, HIGH);
   valueTwo = HIGH;

   Serial.println("Opened State: 10");
  }
  if(off.equals(onn4)){

  digitalWrite(relayOne, LOW);
   value = LOW;

   digitalWrite(relayTwo, LOW);
   valueTwo = LOW;

   Serial.println("Opened State: 11");
  }

 
   
  Serial.println("closing connection");

  Serial.println("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
  delay(5000);  //GET Data at every 2 seconds
}

/*
void sensorvalue()
{
 
}
 */
long vibration(){
  long measurement = pulseIn(vs, HIGH);  //wait for the pin to get HIGH and returns measurement
  return measurement;
}
