#include "Box.h"
#include <Arduino.h>
#include <ESP32Servo.h>

Box::Box (int number,int servo_pin,int button_pin) {
	_number = number;
	_servo_pin = servo_pin;
	_button_pin = button_pin;
}

void Box::attach_servo () {
	servo.attach(_servo_pin);
}

void Box::detach_servo () {
	servo.detach();
}

void Box::init (int opened , int closed , int speed) {
	attach_servo();
	_opened_angle = opened;
	_closed_angle = closed;
	_speed = speed;
	_current_pose = _closed_angle;
}

void Box::change_speed(int new_speed) {
	_speed = new_speed;
}

int Box::get_number() {
	return _number;
}

bool Box::get_state() {
	return _state;
}

int Box::get_button_pin() {
	return _button_pin;
}

int Box::get_opened_angle() {
	return _opened_angle;
}

int Box::get_closed_angle() {
	return _closed_angle;
}

int Box::get_current_pose() {
	return _current_pose;
}

bool Box::get_open_order() {
	return _open_order;
}

bool Box::get_close_order() {
	return _close_order;
}

//this function rotates the servo1 to reach a given angle with a given speed
void Box::_rotate(int angle , int speedd,int sense){
  if(sense>0){
    for (int i = _current_pose; i <= angle; i=i+speedd) {  
    servo.write(i);              
    delay(15); 
    }
  }
  else{
    for (int i = _current_pose; i >= angle; i=i-speedd) {  
    servo.write(i);              
    delay(15); 
    }    
  }
  _current_pose = angle;
}

void Box::open() {
	
    _rotate(_opened_angle,_speed,1);
    _state = true;
  
}

void Box::close(){

    _rotate(_closed_angle,_speed,-1);
    _state = false;
    
}

void Box::change_open_order (bool neww) {
	_open_order = neww;
}

void Box::change_close_order (bool neww) {
	_close_order = neww;
}

void Box::toggle_open_order () {
	_open_order = !_open_order;
}

void Box::toggle_close_order () {
	_close_order = !_close_order;
}

