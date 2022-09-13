#include "arduino_secrets.h"
/**
 * Author: Nick Doolittle (dooiee)
 * 
 * Project: Project-Shangri-La
 * 
 * Description: ESP32 code to read and write state values to Firebase RTDB to provide remote control via iOS application
 ** The program utilizes connection to Firebase, performs failsafe functions to maintain online connection based on read values, 
 ** as well as performs state changes based on serial communication between the ESP32 and the Arduino MKR 1010. 
 ** It also performs transmissions of RF signals to control underwater lights by monitoring value changes in Firebase. 
*/

#define RXD2 16
#define TXD2 17
#include <HardwareSerial.h>
char rssiMKR;
String message = "";
char Mymessage[10];
HardwareSerial SerialPort(2); // use uart2

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <Firebase_ESP_Client.h>
#include <RCSwitch.h>

//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
// Defined in Secret tab

/* 2. Define the RTDB URL */
// #define SECRET_DATABASE_URL SECRET_DATABASE_URL

/* 3. Define the Firebase Data object */
FirebaseData fbdo;
RCSwitch mySwitch = RCSwitch();

/* 4, Define the FirebaseAuth data for authentication data */
FirebaseAuth auth;

/* Define the FirebaseConfig data for config data */
FirebaseConfig config;

String rfTransmitterCode = "/rfTransmitterCode";
String rfPowerCode;
//String rfColorCode;
//const char* rfColorCode;
int rfColorCodeDecimal;
const char* onButton = "111111111111111100000001";
const char* offButton = "111111111111111100000011";
const char* redButton = "111111111111111100001010";
const char* greenButton = "000001010001110000001011";
const char* blueButton = "111111111111111100001100";
const int redButtonDecimal = 16776970;
const int greenButtonDecimal = 16776971;
const int blueButtonDecimal = 334860;
//const char* previousColorCode;
String previousPowerCode;
//String previousColorCode; // not using Binary Color Codes b/c switch case architecture.
int previousColorCodeDecimal;

String previousSolenoidPowerState; // added for Solenoid Control
String solenoidPowerState; // added for Solenoid Control

unsigned long dataMillis = 0;
int count = 0;

unsigned long previousMillis;
unsigned long currentMillis;
unsigned long interval = 30000;

int status = WL_IDLE_STATUS;     // the Wifi radio's status

char mkrRSSI = ' ';
int mkrWeakRSSITest;
const int RESET_PIN = 19;
int resetPinValue;
bool mkrFirstRSSI = true;
unsigned long firstDataEntryTime;
unsigned long nextDataEntryTime;
unsigned long elapsedTime;
unsigned long timeoutTimer = 0;
int arduinoResetStatus;
bool resetComplete;
int mkrRSSIViaSerial = -1;

const int EXTERNAL_RESET_PIN = 5;

void IRAM_ATTR isr() {
  ESP.restart();
}

void setup() {
  
  pinMode (4, OUTPUT);
  pinMode (18, OUTPUT);
  pinMode (0, OUTPUT); // added for Solenoid Control /// on init, solenoid valve open so need to change to pin that does not start on LOW on boot.
  digitalWrite(RESET_PIN, HIGH);
  pinMode(RESET_PIN, OUTPUT); 
  pinMode (EXTERNAL_RESET_PIN, INPUT);

    Serial.begin(9600);
//    Serial.begin(9600); //open serial via USB to PC on default port
    SerialPort.begin(9600, SERIAL_8N1, 16, 17);

    previousMillis = millis();

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
    }
    display.display();
    delay(2000); // Pause for 2 seconds
    display.clearDisplay();
    display.setTextColor(WHITE);

    WiFi.begin(SECRET_WIFI_SSID, SECRET_WIFI_PASSWORD);
    Serial.println("\nConnecting to Wi-Fi");
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Connecting to Wi-Fi");
    display.display();
    delay(1000);
//    display.clearDisplay();
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
      display.setTextSize(1);
      display.setCursor(0, 8);
      display.print(".");
      display.display();
      delay(300);
    }
    delay(200);
//    display.clearDisplay();
    display.setCursor(0, 20);
    display.print("Connected with IP:");
    display.setCursor(0, 30);
    display.print(WiFi.localIP());
    display.display();
    delay(3000);
    display.clearDisplay();
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    
    Serial.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    /* Assign the certificate file (optional) */
    //config.cert.file = "/cert.cer";
    //config.cert.file_storage = StorageType::FLASH;

    /* Assign the database URL(required) */
    config.database_url = SECRET_DATABASE_URL;

    config.signer.test_mode = true;

    /**
     Set the database rules to allow public read and write.

       {
	      "rules": {
		      ".read": true,
		      ".write": true
	      }
        }

    */

    Firebase.reconnectWiFi(true);

    /* Initialize the library with the Firebase authen and config */
    Firebase.begin(&config, &auth);

    //Or use legacy authenticate method
    //Firebase.begin(SECRET_DATABASE_URL, SECRET_DATABASE_SECRET);

    // Transmitter is connected to ESP GPIO2 
    mySwitch.enableTransmit(2);
    // Optional set protocol (default is 1, will work for most outlets)
    mySwitch.setProtocol(1);
    // Optional set pulse length.
    mySwitch.setPulseLength(390);
    // Optional set number of transmission repetitions.
    mySwitch.setRepeatTransmit(10);

//    Serial.printf("Getting latest Power Code... previousPowerCode = %s\n", Firebase.RTDB.getString(&fbdo, F("/rfTransmitterCode/Power")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
//    previousPowerCode = fbdo.to<const char *>();
    Firebase.RTDB.getString(&fbdo, F("/rfTransmitterCode/Power"));
    previousPowerCode = fbdo.to<String>();

    Firebase.RTDB.getInt(&fbdo, F("/rfTransmitterCode/Code(Decimal)"));
    previousColorCodeDecimal = fbdo.to<int>();
//    Serial.println("previousColorCodeDecimal " + String(previousColorCodeDecimal));

  // adding Solenoid Firebase Section
    Firebase.RTDB.getString(&fbdo, F("/Solenoid Control"));
    previousSolenoidPowerState = fbdo.to<String>();
    Serial.println("previousSolenoidPowerState " + String(previousSolenoidPowerState));

    resetPinValue = digitalRead(RESET_PIN);
    Serial.print("ESP32/MKR: ");
    (resetPinValue == 0) ? Serial.println("RESET_PIN = LOW") : Serial.println("RESET_PIN = HIGH");
    
    attachInterrupt(EXTERNAL_RESET_PIN, isr, FALLING);   
    Firebase.RTDB.setInt(&fbdo, F("/Arduino Status/ESP32 Resetting"), 0);

    //////// ADDING
    Firebase.RTDB.pushTimestamp(&fbdo, F("/Arduino Reset Log/ESP32"));  
    ////////////////
}

void loop()
{
  while (millis() < 4147200000) { // 48 days so it resets number before it overloads.
    if (WiFi.status() == WL_CONNECTED) {
      if (millis() - dataMillis > 2000)
      {
        dataMillis = millis();
        Firebase.RTDB.setInt(&fbdo, "/test/int", count++); 

        Firebase.RTDB.getString(&fbdo, F("/rfTransmitterCode/Power"));
        rfPowerCode = fbdo.to<String>();

        if ((rfPowerCode == "111111111111111100000001") && (rfPowerCode != previousPowerCode)) {
          digitalWrite(4, HIGH);
          Serial.println("POWER ON!");
          mySwitch.send(onButton);
          mySwitch.send(onButton);
          previousPowerCode = onButton;
        } else if ((rfPowerCode == "111111111111111100000011") && (rfPowerCode != previousPowerCode)) {
          digitalWrite(4, HIGH);
          Serial.println("POWER OFF!");
          //  mySwitch.send(16776963, 24); // offButton
          mySwitch.send(offButton);
          previousPowerCode = offButton;
        } else {
          digitalWrite(4, LOW);
        }
        
        Firebase.RTDB.getInt(&fbdo, F("/rfTransmitterCode/Code(Decimal)"));
        rfColorCodeDecimal = fbdo.to<int>();

        if (rfColorCodeDecimal != previousColorCodeDecimal) {
          switch(rfColorCodeDecimal) {
            case blueButtonDecimal:  
              Serial.println("Blue!");
              digitalWrite(18, HIGH);
              mySwitch.send(blueButton);
              break;
            case 0 : Serial.println("Color Code not yet programmed");
              digitalWrite(18, HIGH);
//              mySwitch.send(rfColorCode);
              break;
            default : Serial.println("Color signal sent!");
              digitalWrite(18, HIGH);
              mySwitch.send(rfColorCodeDecimal, 24);
          }
          previousColorCodeDecimal = rfColorCodeDecimal;
        } else {
          digitalWrite(18, LOW);
        }

        ////////// Solenoid Control Section ///////////
        Firebase.RTDB.getString(&fbdo, F("/Solenoid Control"));
        solenoidPowerState = fbdo.to<String>();

        if (solenoidPowerState == "ON") {
          digitalWrite(0, LOW);
          if (solenoidPowerState != previousSolenoidPowerState) {
            Serial.println("SOLENOID ON!");
            digitalWrite(4, HIGH);
            digitalWrite(18, HIGH);
          }
          // Will send out signal eventually to have other arduino pick it up and execute command (i.e. open valve)
          //mySwitch.send(offButton);
//          mySwitch.setRepeatTransmit(5);
//          delay(1000);
          previousSolenoidPowerState = solenoidPowerState;
        } else if ((solenoidPowerState == "OFF") && (solenoidPowerState != previousSolenoidPowerState)) {
          digitalWrite(0, HIGH);
          Serial.println("SOLENOID OFF!");
          digitalWrite(4, HIGH);
          digitalWrite(18, HIGH);
          previousSolenoidPowerState = solenoidPowerState;
        } else {
          digitalWrite(0, HIGH);
          digitalWrite(4, LOW);
          digitalWrite(18, LOW);
        }
//        CheckIfMKRNeedsExternalReset();
        
////  so each loop we will check millis() to get that elapsed time regardless of if statements. from when we started counting which will be as soon as we get first message.
////  if if statements are hit we will reset elapsed time.

        unsigned long timeElapsedTest = millis() - timeoutTimer;

        if (SerialPort.available()) {
          display.setTextSize(2);
          display.setCursor(0, 0);
          display.print("MKR RSSI:"); 
          mkrRSSIViaSerial = SerialPort.read();
          if (mkrFirstRSSI == true) {
            mkrFirstRSSI = false;
            firstDataEntryTime = millis();
          }
          else {
            nextDataEntryTime = millis();
            elapsedTime = (nextDataEntryTime - firstDataEntryTime)/1000;
            firstDataEntryTime = nextDataEntryTime;
            Serial.print("MKR RSSI Via Serial: ");
            Serial.println(-mkrRSSIViaSerial);
            Serial.print("elasped time: ");
            Serial.print(elapsedTime);
            Serial.println(" seconds");
          }          
          timeoutTimer = firstDataEntryTime;
          display.setTextSize(2);
          display.setCursor(0, 18);
          display.print(-mkrRSSIViaSerial);
//          display.setCursor(50, 18);
//          display.print(WiFi.RSSI());
//          display.display();
          }
          display.setTextSize(2);
          display.setCursor(0, 18);
          display.print(-mkrRSSIViaSerial);
          display.setCursor(50, 18);
          display.print(WiFi.RSSI());
          display.display();
//          Serial.print("mkrRSSIViaSerial:");
//          Serial.println(mkrRSSIViaSerial);
          
          if ((millis() - timeoutTimer >= 180000) || (mkrRSSIViaSerial <= -85 && mkrRSSIViaSerial > -100) || (mkrRSSIViaSerial == 0 && (millis() - timeoutTimer >= 120000)))  {
            (mkrRSSIViaSerial <= -85 && mkrRSSIViaSerial > -100) ? Serial.println("MKR Wi-Fi Signal Too Weak... Triggering reconnection.") : Serial.println("MKR Unresponsive For: "+ String(timeElapsedTest/1000) + " Seconds... Triggering reset."); 
            display.setTextSize(1);
            display.setCursor(0, 40);
            display.print("Resetting Arduino...");
            display.display();
            autoMKRExternalReset();
            Firebase.RTDB.setTimestamp(&fbdo, F("/Arduino Status/Last External Reset"));
            display.setTextSize(1);
            display.setCursor(0, 50);
            display.print("Arduino Reset Complete");
            display.display();
            timeoutTimer = millis();
          }
        }
        display.clearDisplay();
    } else {
      currentMillis = millis();
      if (currentMillis - previousMillis >= 30000) {
        display.setTextSize(1);
        display.setCursor(0, 40);
        display.print("Wi-Fi not connected...");
        display.setCursor(0, 50);
        display.print("Resetting in");
        display.display();
        display.setCursor(0, 60);
        display.print("3.");
        display.display();
        delay(1000);
        display.setCursor(10, 60);
        display.print("2.");
        display.display();
        delay(1000);
        display.setCursor(20, 60);
        display.print("1..");
        display.display();
        delay(1000);
        Serial.println("Wi-Fi Connection has been interrupted");
        ESP.restart();
//        getRSSI();
//        previousMillis = currentMillis;
//        Serial.println(previousMillis);
      }
    }
  }
  Serial.print("time to restart...");
  ESP.restart();
}

void CheckIfMKRNeedsExternalReset() {
  Firebase.RTDB.getInt(&fbdo, F("/Arduino Status/Resetting"));
    arduinoResetStatus = fbdo.to<int>();
    resetPinValue = digitalRead(RESET_PIN);
    if (arduinoResetStatus == 1) {
      resetComplete = false;
      digitalWrite(RESET_PIN, LOW);
      delay(1500);
      digitalWrite(RESET_PIN, HIGH);
      do {
        Firebase.RTDB.getInt(&fbdo, F("/Arduino Status/Resetting"));
        arduinoResetStatus = fbdo.to<int>();
        delay(1000);
        Serial.print(".");
        delay(1000);
        Serial.print(".");
        delay(1000);
        Serial.println(".");
        delay(5000);
        if (arduinoResetStatus == 0) {
          resetComplete = true;
        }
      }
      while (resetComplete != true);
      Firebase.RTDB.setTimestamp(&fbdo, F("/Arduino Status/Last External Reset"));
      Serial.println("MKR Reset Complete!");  
    }
}

void autoMKRExternalReset() {
  timeoutTimer = millis();
  Firebase.RTDB.setInt(&fbdo, F("/Arduino Status/Resetting"), 1);
  
  //////// ADDING
  Firebase.RTDB.pushTimestamp(&fbdo, F("/Arduino Reset Log/MKR Reset by ESP32"));  
  ////////////////
  
    resetComplete = false;
    digitalWrite(RESET_PIN, LOW);
    delay(1500);
    digitalWrite(RESET_PIN, HIGH);
    do {
      Firebase.RTDB.getInt(&fbdo, F("/Arduino Status/Resetting"));
      arduinoResetStatus = fbdo.to<int>();
      delay(1000);
      Serial.print(".");
      delay(1000);
      Serial.print(".");
      delay(1000);
      Serial.println(".");
      delay(10000);
      if (arduinoResetStatus == 0) {
        resetComplete = true;
      }
    }
    while ((resetComplete != true) && (millis() - timeoutTimer < 180000));
    if (millis() - timeoutTimer >= 180000) {
      // Arduino taking too long, trying again...
      
      //////// ADDING
      Firebase.RTDB.pushTimestamp(&fbdo, F("/Arduino Reset Log/MKR Reset by ESP32"));  
      ////////////////
      
      Serial.println("MKR took too long... Resetting again.");
      digitalWrite(RESET_PIN, LOW);
      delay(1500);
      digitalWrite(RESET_PIN, HIGH);
      // here we exit the loop without confirming Arduino is back online, but if it is then Serial.port will work and ESP won't timeout so its fine.
    } else {
      Serial.println("MKR Reset Complete!");
    }
    
}

void getRSSI() {
  int rssi = WiFi.RSSI();
        display.clearDisplay();
        Serial.print("signal strength (RSSI):");
        Serial.println(rssi);
        display.setTextSize(2);
        display.setCursor(0, 0);
        display.print("RSSI:");
        display.setTextSize(2);
        display.setCursor(65, 0);
        display.print(rssi);
//        display.display();
        display.setTextSize(1);
        display.setCursor(0, 20);
        display.print("Wi-Fi Status:");
        display.setTextSize(2);
        display.setCursor(0, 30);
        display.print(WiFi.status());
        display.display();
}

void resetMKR() {
  resetPinValue = digitalRead(RESET_PIN);
  Serial.print("Reset Pin Value: ");
  Serial.println(resetPinValue);      
  resetComplete = false;
  digitalWrite(RESET_PIN, LOW);
  resetPinValue = digitalRead(RESET_PIN);
  Serial.println("MKR is resetting " + String(arduinoResetStatus));
  (resetPinValue == 0) ? Serial.println("RESET_PIN = LOW") : Serial.println("RESET_PIN = HIGH");
  delay(1500);
  digitalWrite(RESET_PIN, HIGH);
  resetPinValue = digitalRead(RESET_PIN);
  (resetPinValue == 1) ? Serial.println("RESET_PIN = LOW") : Serial.println("RESET_PIN = HIGH");
  do {
    Firebase.RTDB.getInt(&fbdo, F("/Arduino Status/Resetting"));
    arduinoResetStatus = fbdo.to<int>();
    resetPinValue = digitalRead(RESET_PIN);
    delay(1000);
    Serial.print(".");
    delay(1000);
    Serial.print(".");
    delay(1000);
    Serial.println(".");
    delay(5000);
    if (arduinoResetStatus == 0) {
      resetComplete = true;
    }
  }
  while (resetComplete != true);
  //digitalWrite(RESET_PIN, HIGH);
  Serial.println("MKR Reset Complete!");
  Serial.print("Reset pin value is now: ");
  Serial.println(resetPinValue);      
}
