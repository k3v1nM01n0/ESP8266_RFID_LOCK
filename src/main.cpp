#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

const char *ssid = "C.I.A Surveillance Van";
const char *password = "1Ax8IXdbX3h5";

String TOKEN = "5a1c307ce50c97d2f3ed9590775fed6f449eafa84183557087b403fe84c411a1";

//define pin for RFID
#define RST_RFID 0
#define SS_RFID 2

//Instance of the class for the RFID
MFRC522 rfid(SS_RFID, RST_RFID);

int readUID(const char *verifyUid);
int writeUID(const char *serverCreate);

String responseRead = "";
String responseWrite = "";

//Setting gpio pins
const int redLED = 5;   //D1
const int greenLED = 4; //D2

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
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  Serial.println("connected to Wifi");
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (WiFi.status() == WL_CONNECTED)
  {
    const char *serverCreate = "http://192.168.0.21:5000/uuid/create";
    const char *serverVerify = "http://192.168.0.21:5000/uuid/verify/";

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
      return;
    }
    Serial1.println("Card selected ");
    Serial.println();

    //read uid

    //  int r = readUID(serverVerify);{
    //    if(r == 0){
    //      Serial.println();
    //    }
    //  }

    //write new uid

    int s = writeUID(serverCreate);
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
  }
  else
  {
    Serial.println("wifi not connected");
    WiFi.begin(ssid, password);
  }
}

//read function
int readUID(const char *verifyUid)
{
  char uid[16] = {0};
  char buff[4];

  if (rfid.uid.size == 0)
  {
    Serial.println("Bad card (size = 0");
  }
  else
  {

    //char tag[sizeof(rfid.uid.uidByte)] = {0};
    for (byte i = 0; i < sizeof(rfid.uid.uidByte); ++i)
    {

      //Serial.println();

      //Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      snprintf(buff, sizeof(buff), "%d", rfid.uid.uidByte[i]);
      strncat(uid, buff, sizeof(uid));
      //Serial.print(rfid.uid.uidByte[i]);
    }

    Serial.println();
    Serial.print(F("Card UID:"));Serial.println(uid);
    
    delay(1000);
  }

  HTTPClient http;
  char url[128];
  sprintf(url, "%s%s", verifyUid, uid);
  http.begin(url);

  int status = http.GET();
  responseRead = http.getString();
  Serial.println(status);
  if (status == 200)
  {
    Serial.println(responseRead);
    digitalWrite(greenLED, HIGH);
    delay(1000);
  }
  else
  {
    Serial.println(responseRead);
    digitalWrite(redLED, HIGH);
    delay(1000);
  }
  http.end();
}

//Write function
int writeUID(const char *serverCreate)
{
  byte newUid[16];
  //char header[5];
  HTTPClient http;
  http.begin(serverCreate);
  http.addHeader("Authorization", TOKEN);
  int status = http.GET();
  Serial.println(status);
  responseWrite = http.getString();

  const char *json_tmpl = "{\"name\":\"%s\"}";
  sscanf(responseRead.c_str(), json_tmpl, newUid);

  if (rfid.MIFARE_SetUid(newUid, (byte)4, true))
  {
    Serial.println(F("Wrote new UID to card."));
    digitalWrite(greenLED, HIGH);
    delay(1000);
  }
  else
  {
    digitalWrite(redLED, HIGH);
    delay(1000);
  }

  http.end();
}
