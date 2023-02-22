#if not defined Database_h
#define Database_h

#include <Firebase_ESP_Client.h>


class Database {
	public:
		Database(String firbase_host,String firebase_key,String user_email,String user_password);
		void init_firestore ();
		void change_user (String email,String password);
		void change_host(String host);
		void change_key (String key);
		void change_email(String email);
		void change_password(String password);
    	String generate_document_path ();
    	String generate_box_mask (int box_number);
    	String generate_box_name_path (int box_number);
    	String generate_box_state_path (int box_number);
    	String generate_open_state_path (int box_number);
    	String generate_close_state_path (int box_number);
		String getDocValue (String path);
		void refresh(int box_number);
		bool check_box_data (int box_number,int day_number);
		String get_box_data (int box_number,int day_number,String path);
		bool getBoxState (int box_number) ;
		bool getOpenState (int box_number);
		bool getCloseState (int box_number);
		bool getDayState (int box_number,int day_number);
    	String get_firbase_host();
    	String get_firebase_key();
    	String get_user_email();
    	String get_user_password();
		
        //firebase authentification and data configuration 
        FirebaseData fbdo;
        FirebaseAuth auth;
        FirebaseConfig configg;
        
        
    private:

    	String _dataArrayToJson(String dataa);
    	FirebaseJson _get_box_data_json (int box_number,int day_number);
    	String _firbase_host;
    	String _firebase_key;
    	String _user_email;
    	String _user_password;

    	
	
};
#endif
