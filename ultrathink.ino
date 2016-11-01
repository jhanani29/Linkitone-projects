


#include <LTask.h>
#include <LWiFi.h>
#include <LWiFiServer.h>

#include <LWiFiClient.h>
#include <Grove_LED_Bar.h>

#define WIFI_AP "jan"             //wifi name
#define WIFI_PASSWORD "12345678"   // wifi password
#define WIFI_AUTH LWIFI_WPA  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP according to your WiFi AP configuration
#define trigPin 13
#define echoPin 12
Grove_LED_Bar bar(9, 8, 0);
long duration, distance;
// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";
String writeAPIKey = "5A3R2S5DI02DBQ8V";
const int updateThingSpeakInterval = 1 * 1000;      // Time interval in milliseconds to update ThingSpeak (number of seconds * 1000 = interval)

long lastConnectionTime = 0; 
boolean lastConnected = false;
int failedCounter = 0;

LWiFiClient client; 

void setup()
{
  LWiFi.begin();
  Serial.begin(115200);

  // keep retrying until connected to AP
  Serial.println("Connecting to AP");
  while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)))
  {
    delay(1000);
  }

  startWifi();
 pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  bar.begin();
}

void loop()
{
  // Read value from Analog Input Pin 0
  
  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }

  // Disconnect from ThingSpeak
  if (!client.connected() && lastConnected)
  {
    Serial.println("...disconnected");
    Serial.println();
    
    client.stop();
  }

  // Print Update Response to Serial Monitor

  // Update ThingSpeak
  if(!client.connected() && (millis() - lastConnectionTime > updateThingSpeakInterval))
  { ultrasonic();
    updateThingSpeak("field2="+String(distance));
  }
  
  // Check if Arduino Ethernet needs to be restarted
  if (failedCounter > 3 ) {startWifi();}
  
  lastConnected = client.connected();
}

void updateThingSpeak(String tsData)
{
  if (client.connect(thingSpeakAddress, 80))
  {         
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");

    client.print(tsData);
    
    lastConnectionTime = millis();
    
    if (client.connected())
    {
      Serial.println("Connecting to ThingSpeak...");
      Serial.println();
      
      failedCounter = 0;
    }
    else
    {
      failedCounter++;
  
      Serial.println("Connection to ThingSpeak failed ("+String(failedCounter, DEC)+")");   
      Serial.println();
    }
    
  }
  else
  {
    failedCounter++;
    
    Serial.println("Connection to ThingSpeak Failed ("+String(failedCounter, DEC)+")");   
    Serial.println();
    
    lastConnectionTime = millis(); 
  }
}

void ultrasonic()
{
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  delay(200);
     if (distance < 10) 
  {  bar.setLevel(10);
    delay(100); // This is where the LED On/Off happens
    }
  else if(distance < 15)  
   { bar.setLevel(8);
  }
   else if(distance < 25)  
   { bar.setLevel(6);
  }
  else if(distance < 35)  
   { bar.setLevel(4);
  }
  else if(distance < 45)  
   { bar.setLevel(2);
  }
  else if(distance >= 200 || distance <= 0)
  {bar.setLevel(0);
    Serial.println("Out of range");
  }
    Serial.print(distance);
    Serial.println(" cm");
 
    delay(200);
}
void startWifi()
{
  
  client.stop();

  Serial.println("Connecting Arduino to network...");
  Serial.println();  

  delay(1000);
  

  
}
