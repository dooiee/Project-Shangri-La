#include "arduino_secrets.h"
/**
 * Author: Nick Doolittle (dooiee)
 * 
 * Project: Project-Shangri-La
 * 
 * Description: Arduino MKR WiFi 1010 code to collect and upload pond sensor values to Firebase RTDB to to provide iOS application with up-to-date water quality for monitoring and control
*/

#include <TimeLib.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include "thingProperties.h"

///////////// Firebase Libraries /////////////////
#include "Firebase_Arduino_WiFiNINA.h"
#include <FirebaseJson.h>
#define DATABASE_URL SECRET_DATABASE_URL //"Firebase base URL"
#define DATABASE_SECRET SECRET_DATABASE_SECRET //"Firebase API key"

//Define Firebase data object
FirebaseData fbdo;

String pondParameters = "/Pond Parameters";
String dataLog = "/Sensor Data Log";
String mockDataLog = "/Mock Data Log";
String rfTransmitterCode = "/rfTransmitterCode";
String dataLogLastUpdate = "/Data Log Last Update";
String arduinoPowerStatus = "/Arduino Status";
String arduinoResetLog = "/Arduino Reset Log";

String temperatureDbPath = "/Temperature";
String waterLevelDbPath = "/Water Level";
String turbidityDbPath = "/Turbidity";
String tdsDbPath = "/Total Dissolved Solids";
String phDbPath = "/pH";

///////////// RF Transmitter Libraries //////////////
// rc-switch - Version: Latest 
#include <RCSwitch.h>
#include <RTCZero.h>

///////// Creating variables for RC-Switch to produce transmitter code
RCSwitch mySwitch = RCSwitch();
RTCZero rtc2;

char* onButton = "111111111111111100000001";
char* offButton = "111111111111111100000011";

//////////// LCD Display Library //////////////
// LiquidCrystal I2C - Version: Latest 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd2(0x27, 20, 4);
//////////// Creating a custom degree symbol for lcd screen
byte customChar[8] = {
	0b01110,
	0b01010,
	0b01110,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000
};

//////////// Sensor Libraries ///////////////
#include <Wire.h>
// OneWire(Edison) for i686 - Version: Latest 
#include <OneWire.h>
// DallasTemperature(Edison) for i686 - Version: Latest 
#include <DallasTemperature.h>
// Data wire is plugged into pin #5 on MKR 1010
#define ONE_WIRE_BUS 5

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature ds18b20(&oneWire);

///////////// SR-HC04 Sensor (Water Level) Variables //////////////
const int trigPin = 6;
const int echoPin = 7;
float duration, distanceCm, distanceIn;

///////////////// TDS Variables ///////////////
//// Source - https://how2electronics.com/aquarium-water-quality-monitor-with-tds-sensor-esp32/
#define tdsSensorPin A2
#define VREF 3.3    // analog reference voltage(Volt) of the ADC
#define SCOUNT  5    // sum of sample point 
int analogBuffer[SCOUNT];   // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
int copyIndex = 0;
float averageVoltage = 0;
float tdsValue = 0;
float temperature = 0;

//////////// TDS Variables for pH style function ////////////////
unsigned long int avgValueTDS;
float bTDS;
int bufTDS[10],tempTDS;
////////////////////////

//////////// pH Variables ////////////
#define phSensorPin A3
#define Offset 0.20
unsigned long int avgValue;
float b;
int buf[10],temp;

/////////////// RF Transmitter Control /////////////// 
// String rfPowerCode; // already declared
int rfColorCodeDecimal;
String previousPowerCode;
int previousColorCodeDecimal;
unsigned long dataMillis = 0;
const int led = 4;
const int blueButtonDecimal = 334860;
const char* blueButton = "111111111111111100001100";

// String previousSolenoidPowerState; // added for Solenoid Control
// String solenoidPowerState; // added for Solenoid Control

// const int greenLED = 3;

char Mymessage[4] = "101";
int byteToSend;
unsigned long elapsedMillis = 0;
int status = WL_IDLE_STATUS;
unsigned long millisElapsed = 0;
unsigned long epoch;

///////// ESP32 Reset Variables /////////
const int ESP32_RESET_PIN = 3;
int resetPinValue;
int esp32ResetStatus;
bool resetComplete;

int mkrCallForHelp = 0;

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  Serial1.begin(9600);
  delay(2500);
  Serial.println("Powering Up...");
  
  //////////// LCD setup
  lcd2.init();  // initialize the lcd 
  lcd2.init();
  ///////// Set up the LCD displays
  lcd2.backlight();
  lcd2.setCursor(3,1);
  lcd2.blink();
  ////////// Setting up loop that animates how the words on the display are revealed.
  String dataPrintout = "Powering Up..."; 
  for (int i = 0; i < dataPrintout.length(); i++) {
    lcd2.print(dataPrintout.charAt(i));
    delay(50);
    if (i >= dataPrintout.length() - 4) {
      delay(250);
    }
  }
  Serial.println("Connecting to Wi-Fi/Firebase...");
  delay(3000);
  lcd2.clear(); // clear message off LCD
  lcd2.setCursor(0,1);
  String dataPrintout3 = "Connecting to Wi-Fi"; 
  for (int i = 0; i < dataPrintout3.length(); i++) {
    lcd2.print(dataPrintout3.charAt(i));
    delay(50);
  }
  lcd2.setCursor(0,2);
  String dataPrintout9 = "..."; 
  for (int i = 0; i < dataPrintout9.length(); i++) {
    lcd2.print(dataPrintout9.charAt(i));
    delay(50);
    if (i >= dataPrintout9.length() - 3) {
      delay(250);
    }
  }
  
  rtc2.begin();
  //Provide the autentication data
  Firebase.begin(DATABASE_URL, DATABASE_SECRET, SECRET_SSID, SECRET_PASS);
  Firebase.reconnectWiFi(true);
  status = WiFi.begin(SECRET_SSID, SECRET_PASS);
  delay(10000);
  Serial.print("Wi-Fi status (after 10 second delay): ");
  Serial.println(status);
  int numberOfTriesWiFi = 0, maxTriesWiFi = 6;
  // int numberOfTriesWiFiReconnect = 0; // can try for one more reconnect until we sit and wait for external reset.
  do {
    // Serial.print("Wi-Fi status (in do-while loop): ");
    // Serial.println(WiFi.status());
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print("failed...");
      delay(500);
      Serial.print("retrying(");
      Serial.print(numberOfTriesWiFi + 1);
      Serial.println(")...");
      if (numberOfTriesWiFi > 5) {
        WiFi.disconnect();
        delay(5000); // give it time to disconnect
      }
      status = WiFi.begin(SECRET_SSID, SECRET_PASS);
      delay(10000);
      Serial.print("Wi-Fi status: ");
      Serial.println(WiFi.status());
      lcd2.setCursor(18,0);
      lcd2.print(numberOfTriesWiFi + 1);
      numberOfTriesWiFi++;
    }
  }
  while ((WiFi.status() != WL_CONNECTED) && (numberOfTriesWiFi < maxTriesWiFi));
  if (numberOfTriesWiFi >= maxTriesWiFi) {
    Serial.println("Cannot connect to Wi-Fi");
    lcd2.setCursor(3,2);
    String dataPrintout18 = "Failed..."; 
    for (int i = 0; i < dataPrintout18.length(); i++) {
      lcd2.print(dataPrintout18.charAt(i));
      delay(50);
      if (i >= dataPrintout18.length() - 1) {
        delay(250);
      }
    }
    reconnectToWiFi(); 
    Serial1.write(mkrCallForHelp);
    while (1); // just hold it here until reset occurs.
    // maybe can send signal to ESP here that interrupts whatever it is doing and it can send back a reset signal
  }
  else {
    printWiFiStatus();
    lcd2.setCursor(3,2);
    String dataPrintout4 = "Connected!"; 
    for (int i = 0; i < dataPrintout4.length(); i++) {
      lcd2.print(dataPrintout4.charAt(i));
      delay(50);
      if (i >= dataPrintout4.length() - 1) {
        delay(250);
      }
    }
    delay(1500);
    lcd2.setCursor(0,3);
    String dataPrintout12 = "Epoch:"; 
    for (int i = 0; i < dataPrintout12.length(); i++) {
      lcd2.print(dataPrintout12.charAt(i));
      delay(50);
      if (i >= dataPrintout12.length() - 1) {
        delay(250);
      }
    }
    int numberOfEpochTries = 0, maxEpochTries = 6;
    do {
      epoch = WiFi.getTime();
      delay(4000);
      // epoch = 0; // TEST (Succeeded)
      // delay(3000);
      Serial.print("Epoch (in do-while loop): ");
      Serial.println(epoch);
      // Serial.print("Epoch number of tries: (");
      // Serial.print(numberOfEpochTries + 1);
      // Serial.println(")...");
      // Firebase.setInt(fbdo, arduinoPowerStatus + "/Epoch Check", epoch);
      numberOfEpochTries++;
    }
    while ((epoch <= 1659564822) && (numberOfEpochTries < maxEpochTries));
    if (numberOfEpochTries >= maxEpochTries) {
      Firebase.setInt(fbdo, arduinoPowerStatus + "/Epoch Check", -1);
      Serial.println("Epoch not received... Reconnecting to Wi-Fi/Firebase.");
      // reconnectToWiFiAndFirebase(); // commented out for now
      getCurrentEpoch();
    }
    else {
      Firebase.setInt(fbdo, arduinoPowerStatus + "/Epoch Check", epoch);
      rtc2.setEpoch(epoch);
    }
    Serial.print("Epoch received: ");
    Serial.println(epoch);
    
    lcd2.setCursor(6,3);
    lcd2.print(epoch);
    delay(2000);
    lcd2.clear(); // clear message off LCD
  }
  
  // Defined in thingProperties.h
  // initProperties();

  analogReadResolution(12); // change to 12 bits for compatibility with MKR 1010 
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(tdsSensorPin, INPUT);
  pinMode(led, OUTPUT);
  pinMode(ESP32_RESET_PIN, OUTPUT);
  digitalWrite(ESP32_RESET_PIN, HIGH);
  
  // Start up the library + Set up the Temperature Sensor
  ds18b20.begin();
  
  /// Creating custom degree symbol for LCD screen
  lcd2.createChar(0, customChar);
  lcd2.setCursor(0,0);
  lcd2.blink();
  ////////// Setting up loop that animates how the words on the display are revealed.
  String dataPrintout2 = "Beginning data"; 
  for (int i = 0; i < dataPrintout2.length(); i++) {
    lcd2.print(dataPrintout2.charAt(i));
    delay(50);
  }
  lcd2.setCursor(0,1);
  String dataPrintout5 = "collection & upload"; 
  for (int i = 0; i < dataPrintout5.length(); i++) {
    lcd2.print(dataPrintout5.charAt(i));
    delay(50);
  }
  lcd2.setCursor(0,2);
  String dataPrintout6 = "to Firebase RTDB..."; 
  for (int i = 0; i < dataPrintout6.length(); i++) {
    lcd2.print(dataPrintout6.charAt(i));
    delay(50);
    if (i >= dataPrintout6.length() - 3) {
      delay(250);
    }
  }
  
  ///////////// RF Transmitter setup /////////////
  // Transmitter is connected to Arduino MKR Pin #2
  mySwitch.enableTransmit(2);
  // Optional set protocol (default is 1, will work for most outlets)
  mySwitch.setProtocol(1);
  // Optional set pulse length.
  mySwitch.setPulseLength(389);
  // Optional set number of transmission repetitions.
  mySwitch.setRepeatTransmit(5);
  
  /////////// RF Transmitter Code - Initial Reads from Firebase ///////////////
  if (Firebase.getString(fbdo, rfTransmitterCode + "/Power")) {
    previousPowerCode = fbdo.stringData();
    Serial.println("Retreiving Current RF Codes... " + previousPowerCode);
  }
  if (Firebase.getInt(fbdo, rfTransmitterCode + "/Code(Decimal)")) {
    previousColorCodeDecimal = fbdo.intData();
    Serial.println("Current RF Color Code: " + String(previousColorCodeDecimal));
  }
  
  /////////// Arduino Status Parameters for Connectivity Check //////////
  if (epoch == 0) { // letting epoch go if it doesn't return the correct value, because it seems to keep program running rather than getting permenantly stuck.
  // so just doing simple check here that gives it more time to correctly return val
    epoch = WiFi.getTime();
    delay(2000);
    Firebase.setInt(fbdo, arduinoPowerStatus + "/Epoch Check", epoch);
    rtc2.setEpoch(epoch);
  }
  String onlineSinceLatest = String(rtc2.getEpoch());
  if (Firebase.getString(fbdo, arduinoPowerStatus + "/Online Since" + "/Latest")) {
    String onlineSinceTimeBefore = fbdo.stringData();
    // Serial.println("Online Since - Time before that: " + fbdo.stringData());
    Firebase.setString(fbdo, arduinoPowerStatus + "/Online Since" + "/Latest", onlineSinceLatest);
    
    ///////ADDING ... STILL NEED TO UPLOAD AND TEST
    Firebase.pushString(fbdo, arduinoResetLog + "/MKR 1010", onlineSinceLatest);
    //////////////////
    
    // Serial.println("Online Since: " + onlineSinceLatest);
    // Serial.println("Online Since - Time before that: " + onlineSinceTimeBefore);
    Firebase.setString(fbdo, arduinoPowerStatus + "/Online Since" + "/Time Before That", onlineSinceTimeBefore);
    int totalRuntime = (onlineSinceLatest.toInt() - onlineSinceTimeBefore.toInt())/60;
    Firebase.setInt(fbdo, arduinoPowerStatus + "/Online Since" + "/Total runtime of last power cycle (minutes)", totalRuntime);
  } else {
    Firebase.setString(fbdo, arduinoPowerStatus + "/Online Since" + "/Latest", onlineSinceLatest);
  }
  
  Firebase.setInt(fbdo, arduinoPowerStatus + "/Resetting", 0);
  Serial.println("MKR Reset Status set to 0"); 
  
  delay(2000);
  lcd2.noBlink();
  lcd2.clear(); // clear message off LCD
  
  Serial.println("Beginning Sensor Readings...");
  delay(1000);
  
  Serial.print("fbdo.httpConnected(): ");
  Serial.println(fbdo.httpConnected()); // maybe can use as self-reset option if it indicates that Firebase is not connected...
}

void loop() {
  if (millis() - millisElapsed > 5000)
  {
    millisElapsed = millis();
    sensorDataAcquisitionAndUploadToFirebase(); // function to hold all loop code for easier scrolling
    int inByte = abs(WiFi.RSSI());
    // Serial.print("MKR RSSI:");
    // Serial.println(-inByte);
    lcd2.setCursor(18, 3);
    lcd2.print(inByte);
    Serial1.write(inByte);

    if (Serial1.available()) {
      // read from Serial1 output to Serial
      // Serial.write(Serial1.read()); // leads to random symbols showing up on serial monitor.
    }
    checkIfESP32NeedsExternalReset(); // this causes MKR to reset because it draws the connected Reset Pin to LOW
    /// maybe need to make this just a WiFi/Firebase reconnect rather than a hard reset.
  }
}

void sensorDataAcquisitionAndUploadToFirebase() {
  const unsigned long oneMinute = 1 * 60 * 1000UL;
  static unsigned long lastSampleTime = 0 - oneMinute;  // initialize such that a reading is due the first time through loop()
  unsigned long now = millis();
  ///////////// DATA ACQUISITION + LCD DISPLAY //////////////
  
  /////////// Temperature Sensor (ds18b20) data acquisition
  ds18b20.requestTemperatures(); // Send the command to get temperatures
  temperature = ds18b20.getTempCByIndex(0); // Gets temp in Celcius for TDS calculation
  temperatureSensor = ds18b20.getTempFByIndex(0);
  
  /////////// Temperature Sensor (ds18b20) LCD printout
  lcd2.setCursor(0, 0);
  lcd2.print("Temp: ");
  lcd2.print(temperatureSensor, 1);
  lcd2.write(byte(0));

  /////// Depth Sensor (HC-SR04) data acquisition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * 0.034 / 2; // distance in centimeters
  distanceIn = distanceCm / 2.54; // distance in inches
  
  //////////// Calculation to present water level variable as a means of acceptable levels
  /////////////// i.e. its 10 inches below where it should be
  const float totalPondDepth = 30.0; // total pond depth in inches
  depthSensor = distanceIn; // will change later to determine correct depth.
  
  /////// Depth Sensor (HC-SR04) LCD printout
  lcd2.setCursor(0, 1);
  lcd2.print("Pond Level: ");
  if (depthSensor <= 100) {
    lcd2.print(depthSensor, 1);
    lcd2.print(" in ");
  } else {
    lcd2.print("N/A ");
  }
  
  //////// Turbidity Sensor data acquisition
  // units are NTU, range from 0 - 3000 (measure of cloudiness) - Sensor is connected to 5V and GND and A2
  int turbiditySensorPin = analogRead(A1);// read the input on analog pin 0:
  float turbidityOffset = 0.46;
  float ntuVoltage = turbiditySensorPin * (5.0 / 4095.0) - turbidityOffset; // Convert the analog reading (which goes from 0 - 4095 for MKR) to a voltage (0 - 5V): 
  float ntuValue;
  
  //////// Turbidity Sensor LCD printout
  ///// https://www.mdpi.com/1424-8220/14/4/7142/htm#SD1 - maybe a good source for NTU calculation.
  ///////// other papers on Turbidity:
    //////////// https://www.mdpi.com/1424-8220/19/14/3039/htm
    //////////// https://www.mdpi.com/1424-8220/19/14/3039/htm#B22-sensors-19-03039
    //////////// https://www.mdpi.com/1424-8220/18/3/750/htm
    //////////// https://www.mdpi.com/1424-8220/18/4/1115/htm
    //////////// https://www.mdpi.com/1424-8220/18/4/1115/htm#B1-sensors-18-01115
    //////////// https://www.fondriest.com/environmental-measurements/parameters/water-quality/turbidity-total-suspended-solids-water-clarity/
  /// NTU quadratic equation only works between 2.5 - 4.2 V. Equation not fit to voltages outside of that.
  lcd2.setCursor(0, 2);
  lcd2.print("Turb: ");
  lcd2.print(ntuVoltage, 2);
  lcd2.print("V ");
  
  if (ntuVoltage <= 2.5) {
      turbiditySensor = 3000;
      lcd2.print(turbiditySensor);
      lcd2.print(" NTU"); // spaces added for padding based on output to overwrite unused spaces 
  }
  else if (ntuVoltage > 4.21) {
      turbiditySensor = 0;
      lcd2.print(turbiditySensor);
      lcd2.print(" NTU   "); // spaces added for padding based on output to overwrite unused spaces 
  }
  else {
      ntuValue = -1120.4*sq(ntuVoltage) + 5742.3*ntuVoltage - 4352.9; 
      // this polynomial equation was given by DFRobot and calculates the relationship between voltage and turbidity value
      // for 3.3V output this is the quadratic equation: y = -2572.2x² + 8700.5x - 4352.9
      //// source: https://forum.arduino.cc/t/getting-ntu-from-turbidity-sensor-on-3-3v/658067/14
      // however since we are powering with 5V we should not need 3.3V equation.
      turbiditySensor = round(ntuValue);
      lcd2.print(turbiditySensor);
      if (ntuValue >= 1000) {
        lcd2.print(" NTU");
      }
      else if (ntuValue >= 100 && ntuValue < 1000 || ntuValue < -9) {
        lcd2.print(" NTU "); // spaces added for padding based on output to overwrite unused spaces
      }
      else if (ntuValue <= 99 && ntuValue >= -9) {
        lcd2.print(" NTU  ");
      }
  }
  
  ///////// Total Dissolved Solids (TDS) Sensor data acquisition / LCD printout
  lcd2.setCursor(0, 3);
  lcd2.print("TDS Value: "); //// printout first to reserve spot on LCD while average measurement is calculated
  //// Calculating TDS the same way as pH - by taking average of 10 samples and then calculating the TDS of the average.
  ////// that way long term average values are not skewed by zeros that were initially generated by other TDS calculation.
  /////// this is okay because even on startup/board reset the probe is already submerged so it does not need time to steady out.
  for(int n=0;n<10;n++)       
  { 
    bufTDS[n]=analogRead(tdsSensorPin);
    delay(10);
  }
  for(int n=0;n<9;n++)      
  {
    for(int m=n+1;m<10;m++)
    {
      if(bufTDS[n]>bufTDS[m])
      {
        tempTDS=bufTDS[n];
        bufTDS[n]=bufTDS[m];
        bufTDS[m]=tempTDS;
      }
    }
  }
  avgValueTDS=0;
  for(int n=2;n<8;n++)                   
    avgValueTDS+=bufTDS[n];
  averageVoltage = ((float)avgValueTDS/6.0) * ((float)VREF / 4095.0); // divided by 6 for middle 6 measurements
  float compensationCoefficient = 1.0 + 0.0191 * (temperature - 25.0); //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
  float compensationVolatge = averageVoltage / compensationCoefficient; //temperature compensation
  tdsValue = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) * 0.5; //convert voltage value to tds value
  /// Source - https://iopscience.iop.org/article/10.1088/1755-1315/118/1/012019/pdf#:~:text=There%20are%20many%20standards%20that,level%20%5B21%2C22%5D.
  //// Other Sources/Useful info - https://pubs.usgs.gov/wsp/2311/report.pdf
  tdsSensor = round(tdsValue);
  lcd2.setCursor(11, 3);
  lcd2.print(tdsSensor);
  if (tdsSensor > 999) {
    lcd2.print(" ppm  ");
  } else {
    lcd2.print(" ppm ");
  }
  /////////////////////
  
  /////////// pH data acquisition ///////////////
  for(int i=0;i<10;i++)       
  { 
    buf[i]=analogRead(phSensorPin);
    delay(10);
  }
  for(int i=0;i<9;i++)      
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                   
    avgValue+=buf[i];
  // float phVol=(float)avgValue*5.0/1024/6;
  float phVol=(float)avgValue*3.3/4096/6; // changed to 4096 for MKR 1010 12 bit resolution
  // equation is divided by 6 because we are taking middle 6 measurements.
  
  /* ////// pH calibration results //////
  For Arduino Uno:
  pH    Voltage
  4.01  3.1
  6.86  2.62
  9.18  2.2
  
  For MKR 1010:
  pH actual   pH measured   Voltage
  4.01        3.81          3.14
  6.86        6.66          2.65
  9.18        8.98          2.24
  */
  
  float phValue = -5.749*phVol+21.861+Offset;
  phSensor = phValue;
  //////////// pH LCD printout ////////////
  lcd2.setCursor(12, 0);
  lcd2.print("pH: ");
  if (phSensor >= 10.0) {
    lcd2.print(phSensor, 1);
  } else {
    lcd2.print(phSensor, 2);
  }
  
  ////////////// UPLOADING DATA TO FIREBASE RTDB /////////////
  
  //////// RF Transmitter Data /////////
  if (Firebase.getString(fbdo, rfTransmitterCode + "/Power")) {
      rfPowerCode = fbdo.stringData();
    if ((rfPowerCode == "111111111111111100000001") && (rfPowerCode != previousPowerCode)) {
      digitalWrite(led, HIGH);
      // Serial.println("POWER ON!");
      mySwitch.send(onButton);
      previousPowerCode = onButton;
      delay(250); 
    } else if ((rfPowerCode == "111111111111111100000011") && (rfPowerCode != previousPowerCode)) {
      digitalWrite(led, HIGH);
      // Serial.println("POWER OFF!");
      mySwitch.send(offButton);
      previousPowerCode = offButton;
    } else {
      digitalWrite(led, LOW);
    }
  }
  if (Firebase.getInt(fbdo, rfTransmitterCode + "/Code(Decimal)")) {
    rfColorCodeDecimal = fbdo.intData();
    if (rfColorCodeDecimal != previousColorCodeDecimal) {
          switch(rfColorCodeDecimal) {
            case blueButtonDecimal:  
              //Serial.println("Blue!");
              digitalWrite(led, HIGH);
              mySwitch.send(blueButton);
              break;
            case 0 : //Serial.println("Color Code not yet programmed");
              digitalWrite(led, HIGH);
              break;
            default : //Serial.println("Color signal sent!");
              digitalWrite(led, HIGH);
              mySwitch.send(rfColorCodeDecimal, 24);
          }
          previousColorCodeDecimal = rfColorCodeDecimal;
        } else {
          digitalWrite(led, LOW);
        }
  }
  if (Firebase.getString(fbdo, rfTransmitterCode + "/Code")) {
    //Serial.println("Firebase Transmitter Color Code: " + fbdo.stringData());
    //Serial.println("Type: "+ fbdo.dataType());
    rfColorCode = fbdo.stringData();
  }

  ///////// Sensor Data Most Recent Value //////////
  if (Firebase.setFloat(fbdo, pondParameters + temperatureDbPath, temperatureSensor)) {
      // Serial.println(fbdo.dataPath() + " = " + temperatureSensor);
  }
  if (Firebase.setFloat(fbdo, pondParameters + waterLevelDbPath, depthSensor)) {
      // Serial.println(fbdo.dataPath() + " = " + depthSensor);
  }
  if (Firebase.setInt(fbdo, pondParameters + turbidityDbPath + " Value", turbiditySensor)) {
      // Serial.println(fbdo.dataPath() + " = " + turbiditySensor);
  }
  if (Firebase.setFloat(fbdo, pondParameters + turbidityDbPath + " Voltage", ntuVoltage)) {
      // Serial.println(fbdo.dataPath() + " = " + ntuVoltage);
  }
  if (Firebase.setInt(fbdo, pondParameters + tdsDbPath, tdsSensor)) {
      // Serial.println(fbdo.dataPath() + " = " + tdsSensor);
  }
  if (Firebase.setFloat(fbdo, pondParameters + phDbPath, phSensor)) {
      // Serial.println(fbdo.dataPath() + " = " + phSensor);
  }
  ////////// Arduino Status Last Upload //////////
  if (Firebase.setTimestamp(fbdo, arduinoPowerStatus + "/Last Upload")) {
  }
  ////////// Arduino Connection Status -- Checked every 20 seconds //////////
  if (millis() - elapsedMillis > 20000)
  {
    elapsedMillis = millis();
    int wifiRSSI = WiFi.RSSI();
    status = WiFi.status();
    Serial.print("WiFi RSSI:");
    Serial.println(-wifiRSSI); // checking to see if this is triggered twice before we see firebase upload serial print..
    if (Firebase.setInt(fbdo, arduinoPowerStatus + "/Wi-Fi RSSI", wifiRSSI)) {
    }
    if (Firebase.setInt(fbdo, arduinoPowerStatus + "/Wi-Fi Status", status)) {
      if ((status == 3) && (millis() <= 120000)) { // check for first two minutes of powerup to ensure reset variable is set back to 0.
        Firebase.setInt(fbdo, arduinoPowerStatus + "/Resetting", 0);
      }
    }
  }
  
  ///////// Sensor Data Historical Log //////////
  if (now - lastSampleTime >= oneMinute)
  {
    lastSampleTime += oneMinute;
    String currentTimestamp = String(rtc2.getEpoch());
    Firebase.setFloat(fbdo, dataLog + "/" + currentTimestamp + temperatureDbPath, temperatureSensor);
    Firebase.setFloat(fbdo, dataLog + "/" + currentTimestamp + waterLevelDbPath, depthSensor);
    Firebase.setInt(fbdo, dataLog + "/" + currentTimestamp + turbidityDbPath, turbiditySensor);
    Firebase.setInt(fbdo, dataLog + "/" + currentTimestamp + tdsDbPath, tdsSensor);
    Firebase.setFloat(fbdo, dataLog + "/" + currentTimestamp + phDbPath, phSensor);
    Firebase.setTimestamp(fbdo, dataLog + "/" + currentTimestamp + "/Timestamp");
    // Serial.print("Sensor Values Uploaded to Firebase @ ");
    // Serial.println(currentTimestamp);
  }
  ///////////////////////////////////
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("Signal Strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void reconnectToWiFi() {
  WiFi.disconnect();
  delay(5000);
  lcd2.clear(); // clear message off LCD
  lcd2.setCursor(0,1);
  String dataPrintout7 = "Reconnecting Wi-Fi."; 
  for (int i = 0; i < dataPrintout7.length(); i++) {
    lcd2.print(dataPrintout7.charAt(i));
    delay(50);
    if (i >= dataPrintout7.length() - 1) {
      delay(250);
    }
  }
  lcd2.setCursor(0,2);
  String dataPrintout8 = "..."; 
  for (int i = 0; i < dataPrintout8.length(); i++) {
    lcd2.print(dataPrintout8.charAt(i));
    delay(50);
    if (i >= dataPrintout8.length() - 3) {
      delay(250);
    }
  }
  status = WiFi.begin(SECRET_SSID, SECRET_PASS);
  delay(10000);
  
  int numberOfTriesWiFi = 0, maxTriesWiFi = 6;
  do {
    Serial.print("Wi-Fi status (in do-while loop): ");
    Serial.println(WiFi.status());
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print("failed...");
      delay(500);
      Serial.print("retrying(");
      Serial.print(numberOfTriesWiFi + 1);
      Serial.println(")...");
      // WiFi.disconnect();
      status = WiFi.begin(SECRET_SSID, SECRET_PASS);
      delay(10000);
      Serial.print("Wi-Fi status: ");
      Serial.println(WiFi.status());
      lcd2.setCursor(18,0);
      lcd2.print(numberOfTriesWiFi + 1);
      numberOfTriesWiFi++;
    }
  }
  while ((WiFi.status() != WL_CONNECTED) && (numberOfTriesWiFi < maxTriesWiFi));
  if (numberOfTriesWiFi >= maxTriesWiFi) {
    Serial.println("Cannot connect to Wi-Fi");
    lcd2.setCursor(3,2);
    String dataPrintout17 = "Failed..."; 
    for (int i = 0; i < dataPrintout17.length(); i++) {
      lcd2.print(dataPrintout17.charAt(i));
      delay(50);
      if (i >= dataPrintout17.length() - 4) {
        delay(250);
      }
    }
    lcd2.setCursor(0,3);
    String dataPrintout19 = "ESP32 to the rescue!"; 
    for (int i = 0; i < dataPrintout19.length(); i++) {
      lcd2.print(dataPrintout19.charAt(i));
      delay(50);
      if (i >= dataPrintout19.length() - 1) {
        delay(250);
      }
    }
    Serial1.write(mkrCallForHelp); // should trigger ESP32 in 60 seconds (once timeout timer has been hit).
    while (1);
  }
  else {
    printWiFiStatus();
    lcd2.setCursor(5,2);
    String dataPrintout10 = "Connected!"; 
    for (int i = 0; i < dataPrintout10.length(); i++) {
      lcd2.print(dataPrintout10.charAt(i));
      delay(50);
      if (i >= dataPrintout10.length() - 1) {
        delay(250);
      }
    }
    delay(2500);
    lcd2.clear(); // clear message off LCD
  }
}

void reconnectToWiFiAndFirebase() {
  WiFi.disconnect();
  delay(5000);
  lcd2.clear(); // clear message off LCD
  lcd2.setCursor(0,1);
  String dataPrintout7 = "Reconnecting to WiFi"; 
  for (int i = 0; i < dataPrintout7.length(); i++) {
    lcd2.print(dataPrintout7.charAt(i));
    delay(50);
  }
  lcd2.setCursor(0,2);
  String dataPrintout8 = "and Firebase..."; 
  for (int i = 0; i < dataPrintout8.length(); i++) {
    lcd2.print(dataPrintout8.charAt(i));
    delay(50);
    if (i >= dataPrintout8.length() - 3) {
      delay(250);
    }
  }
  status = WiFi.begin(SECRET_SSID, SECRET_PASS);
  Firebase.begin(DATABASE_URL, DATABASE_SECRET, SECRET_SSID, SECRET_PASS);
  Firebase.reconnectWiFi(true);
  delay(10000);
  
  int numberOfTriesWiFi = 0, maxTriesWiFi = 6;
  do {
    Serial.print("Wi-Fi status (in do-while loop): ");
    Serial.println(WiFi.status());
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print("failed...");
      delay(500);
      Serial.print("retrying(");
      Serial.print(numberOfTriesWiFi + 1);
      Serial.println(")...");
      // WiFi.disconnect();
      status = WiFi.begin(SECRET_SSID, SECRET_PASS);
      delay(10000);
      Serial.print("Wi-Fi status: ");
      Serial.println(WiFi.status());
      numberOfTriesWiFi++;
    }
  }
  while ((WiFi.status() != WL_CONNECTED) && (numberOfTriesWiFi < maxTriesWiFi));
  if (numberOfTriesWiFi >= maxTriesWiFi) {
    Serial.println("Cannot connect to Wi-Fi");
    lcd2.setCursor(3,2);
    String dataPrintout16 = "Failed..."; 
    for (int i = 0; i < dataPrintout16.length(); i++) {
      lcd2.print(dataPrintout16.charAt(i));
      delay(50);
      if (i >= dataPrintout16.length() - 4) {
        delay(250);
      }
    }
    Serial1.write(mkrCallForHelp); // should trigger ESP32 two minutes after (once timeout timer has been hit).
    while (1);
  }
  else {
    printWiFiStatus();
    lcd2.setCursor(5,3);
    String dataPrintout11 = "Connected!"; 
    for (int i = 0; i < dataPrintout11.length(); i++) {
      lcd2.print(dataPrintout11.charAt(i));
      delay(50);
      if (i >= dataPrintout11.length() - 1) {
        delay(250);
      }
    }
    delay(1500);
    lcd2.clear(); // clear message off LCD
  }
}

///// function to check if epoch time is correct, will use ESP32 as interrupter to activate this function
void getCurrentEpoch() {
  // unsigned long epoch;
  int numberOfEpochTries = 0, maxEpochTries = 3;
  do {
    epoch = WiFi.getTime();
    delay(4000);
    Firebase.setInt(fbdo, arduinoPowerStatus + "/Epoch Check", epoch);
    numberOfEpochTries++;
  }
  while ((epoch <= 1659564822) && (numberOfEpochTries < maxEpochTries));
  if (numberOfEpochTries >= maxEpochTries) {
    Firebase.setInt(fbdo, arduinoPowerStatus + "/Epoch Check", -1);
    Serial1.write(mkrCallForHelp); // should trigger ESP32 two minutes after (once timeout timer has been hit).
    while (1);
  }
  else {
    Firebase.setInt(fbdo, arduinoPowerStatus + "/Epoch Check", epoch);
    rtc2.setEpoch(epoch);
  }
}

void checkIfESP32NeedsExternalReset() {
  // resetPinValue = digitalRead(ESP32_RESET_PIN);
  // Serial.print("Reset Pin Value: ");
  // Serial.println(resetPinValue);  
  Firebase.getInt(fbdo, arduinoPowerStatus + "/ESP32 Resetting");
  esp32ResetStatus = fbdo.intData();
  if (esp32ResetStatus == 1) {
    resetComplete = false;
    lcd2.clear();
    lcd2.setCursor(1,1);
    lcd2.blink();
    ////////// Setting up loop that animates how the words on the display are revealed.
    String dataPrintout13 = "Resetting ESP32..."; 
    for (int i = 0; i < dataPrintout13.length(); i++) {
      lcd2.print(dataPrintout13.charAt(i));
      delay(50);
      if (i >= dataPrintout13.length() - 4) {
        delay(250);
      }
    }
    digitalWrite(ESP32_RESET_PIN, LOW);
    // resetPinValue = digitalRead(ESP32_RESET_PIN);
    // Serial.println("ESP32 is resetting " + String(esp32ResetStatus));
    // (resetPinValue == 0) ? Serial.println("ESP32_RESET_PIN = LOW") : Serial.println("ESP32_RESET_PIN = HIGH");
    delay(1500);
    digitalWrite(ESP32_RESET_PIN, HIGH);
    // resetPinValue = digitalRead(ESP32_RESET_PIN);
    // (resetPinValue == 1) ? Serial.println("ESP32_RESET_PIN = LOW") : Serial.println("ESP32_RESET_PIN = HIGH");
    do {
      Firebase.getInt(fbdo, arduinoPowerStatus + "/ESP32 Resetting");
      esp32ResetStatus = fbdo.intData();
      delay(1000);
      Serial.print(".");
      delay(1000);
      Serial.print(".");
      delay(1000);
      Serial.println(".");
      delay(5000);
      if (esp32ResetStatus == 0) {
        resetComplete = true;
      }
    }
    while (resetComplete != true);
    //digitalWrite(RESET_PIN, HIGH); // ESP will set itself to conclude reset has completed.
    // Serial.println("ESP32 Reset Complete!");
    // Serial.print("Reset pin value is now: ");
    // Serial.println(resetPinValue); 
    lcd2.setCursor(2,2);
    String dataPrintout14 = "Reset Complete!"; 
    for (int i = 0; i < dataPrintout14.length(); i++) {
      lcd2.print(dataPrintout14.charAt(i));
      delay(50);
      if (i >= dataPrintout14.length() - 1) {
        delay(250);
      }
    }
    delay(1500);
    lcd2.clear();
  }
}

/////// loops below used in TDS average calculation // no longer used
int getMedianNum(int bArray[], int iFilterLen)
{
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++)
  {
    for (i = 0; i < iFilterLen - j - 1; i++)
    {
      if (bTab[i] > bTab[i + 1])
      {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
  else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  return bTemp;
}

