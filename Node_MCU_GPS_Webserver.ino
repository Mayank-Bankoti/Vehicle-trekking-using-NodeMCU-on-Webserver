#include <TinyGPS++.h>
#include <TinyGPSPlus.h>

#include<ESP8266WiFi.h>
#include<ESP8266WebServer.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "html.h"
/* Create object named bt of the class SoftwareSerial */
SoftwareSerial GPS_SoftSerial(D1,D2);/* (Rx, Tx) */
/* Create an object named gps of the class TinyGPSPlus */
TinyGPSPlus gps; 

ESP8266WebServer server(80);

const char* ssid = "Narzo 50A";         /*Enter Your SSID*/
const char* password = "Youcutie"; /*Enter Your Password*/

volatile float minutes, seconds;
volatile int degree, secs, mins;

double lat_val, lng_val, alt_m_val;
uint8_t hr_val, min_val, sec_val;
bool loc_valid, alt_valid, time_valid;

void MainPage(){
  String _html_page = html_page;              /*Read The HTML Page*/
  server.send(200, "text/html", _html_page);  /*Send the code to the web server*/
}

void webGPS() {
  String data = "[\""+String(lat_val)+"\",\""+String(lng_val)+"\"]";
  server.send(200, "text/plane", data);
}

void setup() {
  Serial.begin(9600);               /*Set the baudrate to 115200*/
  WiFi.mode(WIFI_STA);                /*Set the WiFi in STA Mode*/
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while(WiFi.waitForConnectResult() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("Your Local IP address is: ");
  Serial.println(WiFi.localIP());     /*Print the Local IP*/

  server.on("/", MainPage);           /*Display the Web/HTML Page*/
  server.on("/readgps", webGPS);       /*Display the updated Temperature and Humidity value*/
  server.begin();                    /*Start Server*/
  delay(1000);

  GPS_SoftSerial.begin(9600); /* Define baud rate for software serial communication */
}

void loop() {
        server.handleClient();
        smartDelay(1000); /* Generate precise delay of 1ms */
        lat_val = gps.location.lat(); /* Get latitude data */
        loc_valid = gps.location.isValid(); /* Check if valid location data is available */
        lng_val = gps.location.lng(); /* Get longtitude data */
        alt_m_val = gps.altitude.meters();  /* Get altitude data in meters */
        alt_valid = gps.altitude.isValid(); /* Check if valid altitude data is available */
        hr_val = gps.time.hour(); /* Get hour */
        min_val = gps.time.minute();  /* Get minutes */
        sec_val = gps.time.second();  /* Get seconds */
        time_valid = gps.time.isValid();  /* Check if valid time data is available */
        if (!loc_valid)
        {         
          Serial.print("Latitude : ");
          Serial.println("*****");
          Serial.print("Longitude : ");
          Serial.println("*****");
        }
        else
        {
          DegMinSec(lat_val);
          Serial.print("Latitude in Decimal Degrees : ");
          Serial.println(lat_val, 6);
          Serial.print("Latitude in Degrees Minutes Seconds : ");
          Serial.print(degree);
          Serial.print("\t");
          Serial.print(mins);
          Serial.print("\t");
          Serial.println(secs);
          DegMinSec(lng_val); /* Convert the decimal degree value into degrees minutes seconds form */
          Serial.print("Longitude in Decimal Degrees : ");
          Serial.println(lng_val, 6);
          Serial.print("Longitude in Degrees Minutes Seconds : ");
          Serial.print(degree);
          Serial.print("\t");
          Serial.print(mins);
          Serial.print("\t");
          Serial.println(secs);
        }
        if (!alt_valid)
        {
          Serial.print("Altitude : ");
          Serial.println("*****");
        }
        else
        {
          Serial.print("Altitude : ");
          Serial.println(alt_m_val, 6);   
        }
        if (!time_valid)
        {
          Serial.print("Time : ");
          Serial.println("*****");
        }
        else
        {
          char time_string[32];
          sprintf(time_string, "Time : %02d/%02d/%02d \n", hr_val, min_val, sec_val);
          Serial.print(time_string);   
        }
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (GPS_SoftSerial.available())  /* Encode data read from GPS while data is available on serial port */
      gps.encode(GPS_SoftSerial.read());
/* Encode basically is used to parse the string received by the GPS and to store it in a buffer so that information can be extracted from it */
  } while (millis() - start < ms);
}

void DegMinSec( double tot_val)   /* Convert data in decimal degrees into degrees minutes seconds form */
{ 
  degree = (int)tot_val;
  minutes = tot_val - degree;
  seconds = 60 * minutes;
  minutes = (int)seconds;
  mins = (int)minutes;
  seconds = seconds - minutes;
  seconds = 60 * seconds;
  secs = (int)seconds;
} 