#include "Database.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"

//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
Database::Database (String firbase_host,String firebase_key,String user_email,String user_password) {
	_firbase_host = firbase_host;
	_firebase_key = firebase_key;
	_user_email = user_email;
	_user_password = user_password;
}

void Database::change_host (String host) {
	_firbase_host = host;
}
void Database::change_key (String key) {
	_firebase_key = key;
}
void Database::change_email (String email) {
	_user_email = email;
}
void Database::change_password (String password) {
	_user_password = password;
}


//this function creates the path to access the current user document
String Database::generate_document_path () {
   String documentPath = "/users/";
   documentPath.concat(auth.token.uid.c_str());
   return documentPath;
}

//when we call getDocument function it will provides only the data of the box inside the mask
String Database::generate_box_mask (int box_number) {
  //the hardware has exactly 6 boxes !
  if(box_number >6 or box_number <1) Serial.println("ERROR: INVALID BOX NUMBER !");
  //when we call getDocument function it will provides only the data of the box inside the mask
  String mask = "box1,state,title,open,close,weeks";
  //assigning the given box number
  String box = "box"+String(box_number);
  mask.replace("box1",box);
  return mask;
}

//this function generates the path to access the field containing a box name(the name of the medicament)
String Database::generate_box_name_path (int box_number) {
  if(box_number >6 or box_number <1) Serial.println("ERROR: INVALID BOX NUMBER !");
  //field path
  String mask ="fields/box1/mapValue/fields/title/stringValue";
  //assigning the giving box number
  String box = "box"+String(box_number);
  mask.replace("box1",box);
  return mask;
}

//this function generates the path to access the field containing a box state(activated or not)
String Database::generate_box_state_path (int box_number) {
  if(box_number >6 or box_number <1) Serial.println("ERROR: INVALID BOX NUMBER !");
  //field path
  String mask ="fields/box1/mapValue/fields/state/booleanValue";
  //assigning the giving box number
  String box = "box"+String(box_number);
  mask.replace("box1",box);
  return mask;
}

String Database::generate_open_state_path (int box_number) {
  if(box_number >6 or box_number <1) Serial.println("ERROR: INVALID BOX NUMBER !");
  //field path
  String mask ="fields/box1/mapValue/fields/open/booleanValue";
  //assigning the giving box number
  String box = "box"+String(box_number);
  mask.replace("box1",box);
  return mask;
}

String Database::generate_close_state_path (int box_number) {
  if(box_number >6 or box_number <1) Serial.println("ERROR: INVALID BOX NUMBER !");
  //field path
  String mask ="fields/box1/mapValue/fields/close/booleanValue";
  //assigning the giving box number
  String box = "box"+String(box_number);
  mask.replace("box1",box);
  return mask;
}




void Database::init_firestore () {
	// Assigning the api key 
    configg.api_key = _firebase_key;
    // Assigning the RTDB URL  
    configg.database_url = _firbase_host;
    //Assigning user sig-in credentials
    auth.user.email = _user_email;
    auth.user.password =_user_password;  
    // authentification
    Firebase.begin(&configg, &auth);
    Firebase.reconnectWiFi(true);
    //storing the user's document data as a json String in fbdo.payload()
    if (!(Firebase.Firestore.getDocument(&fbdo, _firbase_host, "", generate_document_path(), generate_box_mask(1).c_str())))
       Serial.println(fbdo.errorReason());
}

void Database::change_user (String email,String password) {
	_user_email = email;
	_user_password = password;
    //Close the session and clear all data
    fbdo.clear();

    //To make the token to expire immediately.
    configg.signer.lastReqMillis = 0;
    configg.signer.tokens.expires = 0;

    //Assigning new user sig-in credentials
    auth.user.email = email;
    auth.user.password = password;

    //Begin authenticate
    Firebase.begin(&configg, &auth);
    if (!(Firebase.Firestore.getDocument(&fbdo, _firbase_host, "", generate_document_path(), generate_box_mask(1).c_str())))
    Serial.println(fbdo.errorReason());
}

//get a value of field from document
String Database::getDocValue (String path) {
  //json instance creation
  FirebaseJson json;
  //the field value will be stored in result instance
  FirebaseJsonData result;
  //assigning the json String of user's document to json instance
  json.setJsonData(fbdo.payload().c_str());
  //accessing the field data
  json.get(result,path);
    if (result.success)
    {
       //returning field value as a string
       return result.to<String>().c_str();
    }
    else
    {
      Serial.println("failed");
      return "";
    }
    delay(50);
}


//this function checks if the medicament placed in a given box should be taken in a given day
bool Database::check_box_data (int box_number,int day_number) {
    if(box_number >6 or box_number <1) Serial.println("ERROR: INVALID BOX NUMBER !");
    String path = "fields/box"+String(box_number)+"/mapValue/fields/weeks/mapValue/fields/week"+String(day_number)+"/mapValue/fields/data/arrayValue";
    if(getDocValue(path) == "{}")
      return false;
    else 
      return true;
}

//this function convert a string containing the data array to a json String
String Database::_dataArrayToJson (String dataa) {
     String change;
     int i =dataa.indexOf("{\"mapValue\"");
     int p =0;
     //data map number
     int k=0;
     while (i!= -1){
            p=i;
            k++;
            dataa.setCharAt(i+5,'g');
            change = "\"data"+String(k)+"\":"+"{\"mapValue\"";
            dataa.replace ("{\"mapgalue\"",change);
            i = dataa.indexOf("{\"mapValue\"",p+20);
     }
     change = "{\"length\":\""+String(k)+"\",";
     dataa.replace("[",change);dataa.replace("]","}");
     return dataa;
}


//this function provides a json String containing all the alarm(time to take medicament)data related to a given box in a viven day
FirebaseJson Database::_get_box_data_json (int box_number,int day_number) { 
   //generating the path to access the data array
   String path = "fields/box"+String(box_number)+"/mapValue/fields/weeks/mapValue/fields/week"+String(day_number)+"/mapValue/fields/data/arrayValue/values";
   //storing the data array in dataa variable
   String dataa =getDocValue(path);
   //converting the data array to json String
   dataa = _dataArrayToJson(dataa);
   FirebaseJson json;
   json.setJsonData(dataa);
   return json;
}

// this function returns the data of a given field(path to the field) related to a given box and a given day
String Database::get_box_data (int box_number,int day_number,String path) {
         FirebaseJsonData result;
         _get_box_data_json(box_number,day_number).get(result,path);
         if (result.success) 
             return (result.to<String>().c_str());
         else
         {
             Serial.println("failed");
             return "";
         }
}

//this function returns the state of a given box(activated or not)
bool Database::getBoxState (int box_number) {
    String path =generate_box_state_path(box_number);
    if(getDocValue(path) == "true")
        return true;
    else 
        return false;
}


//this function returns the state of the open field of a given box(for manual apening or closing)
bool Database::getOpenState (int box_number) {
    String path = generate_open_state_path(box_number);
    if(getDocValue(path) == "true")
        return true;
    else 
        return false;
}

//this function returns the state of the open field of a given box(for manual apening or closing)
bool Database::getCloseState (int box_number) {
    String path = generate_close_state_path(box_number);
    if(getDocValue(path) == "true")
        return true;
    else 
        return false;
}

//this function returns the state of a given day(activated or not) of a given box
bool Database::getDayState (int box_number,int day_number) {
  String path = "fields/box"+String(box_number)+"/mapValue/fields/weeks/mapValue/fields/week"+String(day_number)+"/mapValue/fields/state/booleanValue";
    if(getDocValue(path) == "true") return true;
  else return false;
}


void Database::refresh(int box_number){
	    //storing the user's document data as a json String in fbdo.payload()
    if (!(Firebase.Firestore.getDocument(&fbdo, _firbase_host, "", generate_document_path(), generate_box_mask(box_number).c_str())))
       Serial.println(fbdo.errorReason());
}

String Database::get_firbase_host () {
	return _firbase_host;
}
String Database::get_firebase_key () {
	return _firebase_key;
}
String Database::get_user_email () {
	return _user_email;
}
String Database::get_user_password () {
	return _user_password;
}
