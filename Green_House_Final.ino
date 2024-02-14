#include <FirebaseRealtime.h>
#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define FIREBASE_REALTIME_URL "https://iotgreen-d0bab-default-rtdb.firebaseio.com/"
#define FIREBASE_REALTIME_SECRET "26dADICrCeOR445mT1u2zzFcvaFBQ8jbGCr6GvwZ"

#define RELAY D4
#define PUMP D3
#define DHTPIN D5
#define DHTTYPE DHT11

//D1 to display SCL
//D2 tp display SDA
//D3 water pump
//D4 fan
//D5 huminidy
//A0 soilMoisture

LiquidCrystal_I2C lcd(0x20, 16, 2); 
int soilMoisturePin = A0;

char ssid[] = "Dialog 4G 487";
char pass[] = "aF06FCb7";

FirebaseRealtime firebaseRealtime;
DHT dht(DHTPIN, DHTTYPE);

void setup() {

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);

  pinMode(RELAY, OUTPUT);
  pinMode(PUMP, OUTPUT);
  Serial.begin(9600);
  firebaseRealtime.begin(FIREBASE_REALTIME_URL, FIREBASE_REALTIME_SECRET, ssid, pass);
  dht.begin();
  
}

void loop() {
  String Rfan;
  String Rpump;

  DynamicJsonDocument fetchDoc(1024);
  int fetchResponseCode = firebaseRealtime.fetch("GreenHouse","nibm",fetchDoc);
  int fan = fetchDoc["fan"];
  int pump = fetchDoc["pump"];
  Serial.println("fan: " + fan);
  Serial.println("pump: " + pump);
  fetchDoc.clear();
//fan
  if (fan == 1) {
    Serial.println("on");
    digitalWrite(RELAY, HIGH);
    Rfan="ON";
  } else if (fan == 0) {
    Serial.println("off");
    digitalWrite(RELAY, LOW);
    Rfan="OFF";
  }
//pump
  if (pump == 1) {
    Serial.println("on");
    digitalWrite(PUMP, HIGH);
    Rpump="ON";
  } else if (pump == 0) {
    Serial.println("off");
    digitalWrite(PUMP, LOW);
    Rpump="OFF";
  }

  float Temperatue = dht.readTemperature();      //Temperatue
  float Humidity = dht.readHumidity();           //Humidity
  int soilMoistureValue = analogRead(soilMoisturePin);  //sild misture
  int myfan;
  int mypump;

//lcd display
  lcd.setCursor(0, 0);
  lcd.print("Temperature:" + String(Temperatue));
  lcd.setCursor(0, 1);
  lcd.print("Humidity:" + String(Humidity)+"%");


  Serial.print("Temperature: ");
  Serial.println(Temperatue);
  Serial.print("Humidity: ");
  Serial.println(Humidity);
  Serial.print("Soil Moisture: ");
  Serial.println(soilMoistureValue);

//fan
if(Temperatue>=30)
{
   digitalWrite(RELAY,HIGH);
   myfan=1;
   Rfan="ON";
   myfan=1;
}
else if(Temperatue<30)
{
   digitalWrite(RELAY,LOW);
   myfan=0;
   Rfan="OFF";
   myfan=0;
}

//pump
if(soilMoistureValue>=500)
{
   digitalWrite(PUMP,HIGH);
   mypump=1;
   Rpump="ON";
}
else if(soilMoistureValue<700)
{
   digitalWrite(PUMP,LOW);
   mypump=0;
   Rpump="OFF";
}

  DynamicJsonDocument saveDoc(1024);
  saveDoc["temperature"] = Temperatue;
  saveDoc["humidity"] = Humidity;
  saveDoc["SoilMoisture"] = soilMoistureValue;
  saveDoc["fan"] = myfan;
  saveDoc["pump"] = mypump;
  saveDoc["fanResalt"] = Rfan;
  saveDoc["pumpResalt"] = Rpump;
  
  String saveJSONData;
  serializeJson(saveDoc, saveJSONData);
  firebaseRealtime.save("GreenHouse","nibm", saveJSONData, true);

  delay(2000);
}