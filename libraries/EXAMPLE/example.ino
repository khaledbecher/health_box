#include <Health_box.h>
#include <Database.h>
#include <Box.h>
#include <LiquidCrystal_I2C.h>
//WIFI connection library
#include <WiFi.h>

//firestore projtct id and API key
const String FIREBASE_HOST = "healthcare-b2767";
const String FIREBASE_KEY = "AIzaSyAUIWTnaEmPCEE8RLWlNYgktVHJK3ENnvc";

// user email and password
const String USER_EMAIL = "khaledbecher12345@gmail.com";
const String USER_PASSWORD = "#Password123";

//WIFI id and key
const char* ssid = "gnet347774";
const char* password = "347774347774";

const int  closed_servo1_angle = 59;
const int opened_servo1_angle = 178;
const int closed_servo2_angle = 72;
const int opened_servo2_angle = 178;
const int speed1 = 5;
const int speed2 = 3;
const int servo1_pin = 13;
const int servo2_pin = 12;
const int button1_pin = 26;
const int button2_pin = 27;

const int buzzerPin = 16;
const int alert_time = 2;



Database database(FIREBASE_HOST,FIREBASE_KEY,USER_EMAIL,USER_PASSWORD);



const int boxes_number = 2;
 Box boxes[boxes_number] = {Box(1,servo1_pin,button1_pin),Box(2,servo2_pin,button2_pin)};
//LiquidCrystal_I2C object
LiquidCrystal_I2C lcd(0x3F,20,4); 
Health_box health_box(&database,boxes,boxes_number,&lcd,buzzerPin,alert_time);

void init_wifi(){
    WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.print("connected!");
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  init_wifi();
  health_box.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  health_box.checkAlarm();
  delay(50);
}
