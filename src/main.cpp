#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

const char* ssid = "C.I.A Surveillance Van";
const char* password = "1Ax8IXdbX3h5";

String serverAdress = "";

//define pin for RFID
#define RST_RFID 0
#define SS_RFID 2

//Instance of the class for the RFID
MFRC522 rfid(SS_RFID, RST_RFID);

int readUID( char arrayAddress[]);
int writeUID(byte* newUID, byte uidsize);
int httpPost(char* payload);


//Setting gpio pins
const int redLED = 5;//D1
const int greenLED = 4;//D2

void setup()
{
  // put your setup code here, to run once:
  //Set LEDs as output
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  Serial.begin(9600); //Initialize serial communication with the pc
  while (!Serial)
  Serial.println();

  SPI.begin();     //Init SPI bus
  rfid.PCD_Init(); //Init the mrc522 card
  delay(1000);
  rfid.PCD_DumpVersionToSerial(); //show details of rfid

  WiFi.begin(ssid, password);
  Serial.println("Connecting to wifi...");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    
  }
  Serial.println("connected to Wifi");
  
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (WiFi.status() == WL_CONNECTED)
  {

    //Look for new cards
    if (!rfid.PICC_IsNewCardPresent())
    {
      Serial.println("card unavailable");
      delay(5000);
      return;
    }

    //Select one of the cards
    if (!rfid.PICC_ReadCardSerial())
    {
      Serial.println("Bad read");
      return;
    }
    Serial1.println("Card selected ");
    Serial.println();
    

    //read uid
    Serial.println();
    int len = 64;
    char uid[len];

    int r = readUID(uid);
    if (r == 0)
    {
      uid[len] = '\0';
      Serial1.printf("ID: %s", uid);
      int h = httpPost(uid);
      if(h == 0){
        Serial.println("HTTP POST sucessful");
      }else{
        return;
      }
    }
    else
    {
      return;
    }
    delay(1000);

    //write new uid
    byte pass[6] = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x01};
    int s = writeUID(pass, sizeof(pass));
    if (s == 0)
    {
      Serial.println("Writing succesfull");
    }
    else
    {
      Serial.println("Error during wrinting");
    }

    //Dump debug info to serial ::PICC_HaltA is automitically called
    rfid.PICC_DumpDetailsToSerial(&(rfid.uid));
  }else{
    Serial.println("wifi not connected");
    WiFi.begin(ssid, password);
  }
}

//read function
int readUID( char arrayAddress[]) 
{
  if (rfid.uid.size == 0)
  {
    Serial.println("Bad card (size = 0");
  }
  else
  {
    char buff[16];
    //char tag[sizeof(rfid.uid.uidByte)] = {0};
    for (int i = 0; i < sizeof(rfid.uid.uidByte); ++i)
    {
      snprintf(buff, sizeof(buff), "%d", rfid.uid.uidByte[i]);
      strncat(arrayAddress, buff, sizeof(arrayAddress));
    }

    Serial.println("Good scan:");
    Serial.println(arrayAddress);
    delay(1000);
  }
}

//Write function
int writeUID(byte* newUID, byte uidsize){
  if ( rfid.MIFARE_SetUid(newUID, (byte)4, true) ) {
    Serial.println(F("Wrote new UID to card."));
  }
}

//HTTP POST
int httpPost(char* payload){
  HTTPClient http;
  http.begin(serverAdress);
  http.addHeader("Content-type", "application/octet-stream");

  //Send HTTP POST
  int status = http.POST((const uint8_t*) payload, sizeof(payload));
  Serial.println(status);
  if(status == 200){
    digitalWrite(greenLED,HIGH);
    delay(10000);
  }else{
    digitalWrite(redLED, HIGH);
    delay(10000);
  }
  http.end();
}