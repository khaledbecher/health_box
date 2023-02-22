#include "Health_box.h"
#include <Arduino.h>
#include <Database.h>
#include <Box.h>
//LCD display library
#include <LiquidCrystal_I2C.h>

//RTC libraries
#include "time.h"
#include <WiFiUDP.h>
Health_box::Health_box (Database* d,Box* b,int number,LiquidCrystal_I2C* l,int buzzer,int alert) {

    database = d;
	_boxes_number = number;
	_alarm_state = false;
	_buzzer_pin = buzzer;
	_pressed = false;
	_alert_time = alert;
	lcd = l;
}

int Health_box::get_boxes_number () {
	return _boxes_number;
}
void Health_box::init () {
	//RTC synchrinization
    const char* ntpServer = "in.pool.ntp.org";
    const long  gmtOffset_sec = 3600;
    const int   daylightOffset_sec = 0;//GMT
    lcd->init();     
    ledcSetup(0,1E5,_buzzer_pin);
	database->init_firestore ();
	init_orders();
	configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void Health_box::print_time () {
    _printLocalTime();
}


//this function stores the initial state of the open field for each box
void Health_box::init_orders () {
  for(int i=1;i<=_boxes_number;i++){
    database->refresh(i);
    bool o = database->getOpenState(i);
    bool c = database->getCloseState(i);
    boxes[i-1].change_open_order(o);
    boxes[i-1].change_close_order(c);
  }
  Serial.println("initiated orders ");
}


//this function checks if there are new opening or closing orders
void Health_box::checkOrders (int i) {
    bool openn = database->getOpenState(i);
    bool closee =  database->getCloseState(i);
    if(openn!= boxes[i-1].get_open_order() and !(boxes[i-1].get_state()) )
      { 
        boxes[i-1].toggle_open_order();
        boxes[i-1].open();
      }
      else if(boxes[i-1].get_state() and openn!= boxes[i-1].get_open_order())     boxes[i-1].toggle_open_order();

    if(closee!= boxes[i-1].get_close_order() and (boxes[i-1].get_state()) )
      { 
        boxes[i-1].toggle_open_order();
        boxes[i-1].close();
      }
      else if (!boxes[i-1].get_close_order() and closee!= boxes[i-1].get_close_order())     boxes[i-1].toggle_close_order();
}


String Health_box::getDateAndTimeInfo(String info){
      struct tm timeinfo;
      if(!getLocalTime(&timeinfo)){
         Serial.println("Failed to obtain "+info);
         return "";
      }
      if(info == "hour") return String(timeinfo.tm_hour);
      else if(info == "min") return String(timeinfo.tm_min);
      else if(info == "sec") return String(timeinfo.tm_sec);
      else if(info == "day") return String(timeinfo.tm_wday);//from 0 to 6
}

//this function checks if it's time to take the medicament
bool Health_box::compareTime(String hour,String minute){
  String  h = getDateAndTimeInfo("hour");
  String m = getDateAndTimeInfo("min");
  Serial.println(h+"   ,    "+m);
  Serial.println(hour+"    ,    "+minute);
   if (hour.toInt() == h.toInt() and minute.toInt() == m.toInt()) return true;
   else {
      Serial.println("noooooooooooo");
	  return false;
	}
}

//this function plays a tone with given frequency for a given duration
void Health_box::_play(int freq,int duration){
  ledcWriteTone(0,freq);
  delay(duration);
}

void Health_box::alarm_on(){
    _play(659,50);
  _play(0,50);
  _play(659,50);
  _play(0,50);
  _play(659,50);
  _play(0,50);
  _play(659,300);
  _play(0,50);
  _play(659,50);
  _play(0,1000);
}

void Health_box::_printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");  
}
//callback function
void Health_box::callBack(int box_number,int alarm_number){
        _alarm_minute = getDateAndTimeInfo("min").toInt();
        String dose_path = "data"+String(alarm_number)+"/mapValue/fields/dose/stringValue";
        Serial.println("It's medicament time !");
        Serial.print("take ");Serial.println(database->getDocValue(database->generate_box_name_path(box_number)));
        _printLocalTime();
        Serial.println("----------------------------");
        lcd->backlight();
        lcd->setCursor(0,0);
        lcd->print("Medicament time");
        lcd->setCursor(0,1);
        lcd->print(database->get_box_data(box_number,getDateAndTimeInfo("day").toInt(),dose_path));
        if(_alarm_state == false) _alarm_state = true;
            if(boxes[box_number-1].get_state()==false){ 
                boxes[box_number-1].open();
                for(int i=0;i<_boxes_number;i++){
                    boxes[i].detach_servo(); 
                }
                ledcAttachPin(_buzzer_pin,0);
                while(_alarm_state){
                   alarm_on();
                   if((digitalRead(boxes[0].get_button_pin())==LOW and box_number ==1)or(digitalRead(boxes[1].get_button_pin())==LOW and box_number ==2))
                   {
                      while((digitalRead(boxes[0].get_button_pin())==LOW and box_number ==1)or(digitalRead(boxes[1].get_button_pin())==LOW and box_number ==2));
                      _alarm_state = false;
                      _pressed = true;
                   }

                   if(_pressed){
                       while(getDateAndTimeInfo("min").toInt() == _alarm_minute){
                           for(int i=0;i<_boxes_number;i++){
                          	    if(digitalRead(boxes[i].get_button_pin())==LOW and boxes[i].get_state()) boxes[i].close();
                           }
                       }
                       _pressed = false;
                   }
                    
                  
                  else if(getDateAndTimeInfo("min").toInt()- _alarm_minute >= _alert_time){
                       _alarm_state = false;
                  }
                 }
                     ledcDetachPin(_buzzer_pin);
                     for(int i=0;i<_boxes_number;i++){
                     	boxes[i].attach_servo(); 
                     }
                }
                
}



//this functoin compares the current time to each activated alarm time(alarm time is the time to take a medicament) and calls the callback function if they are equal 
void Health_box::checkAlarm(){
  int day_number = getDateAndTimeInfo("day").toInt();
  //boxes loop
  for(int i=1;i<=_boxes_number;i++){
    database->refresh(i);
    checkOrders(i);
    if(database->getBoxState(i)){
        if(database->getDayState(i,day_number)){
          if(database->check_box_data(i,day_number)){
            //alarms loop
            for(int k =1;k<database->get_box_data(i,day_number,"length").toInt()+1;k++){
              String hour_path = "data"+String(k)+"/mapValue/fields/hour/stringValue";
              String minute_path  = "data"+String(k)+"/mapValue/fields/minute/stringValue";
              Serial.println("hour: "+database->get_box_data(i,day_number,hour_path)+"  ,  min: "+database->get_box_data(i,day_number,minute_path));
              if(compareTime(database->get_box_data(i,day_number,hour_path),database->get_box_data(i,day_number,minute_path))){
                   callBack(i,k);
              }
            }
          }
      }
    }    
  } 
  //allow boxe closing using buttons
   for(int i=0;i<_boxes_number;i++){
   	if(digitalRead(boxes[i].get_button_pin())==LOW and boxes[i].get_state()) boxes[i].close();
   }
}

