#if not defined Health_box_h
#define Health_box_h

#include <Database.h>
#include <Box.h>
#include <LiquidCrystal_I2C.h>


class Health_box {
	public:
		Health_box(Database* d,Box* b,int number,LiquidCrystal_I2C* l,int buzzer,int alert);
		
		void init() ;
		void print_time ();
		int get_boxes_number();
		void init_orders();
		void checkOrders (int i);
		String getDateAndTimeInfo(String info);
		void checkAlarm();
		void alarm_on();
		void callBack(int box_number,int alarm_number);
		bool compareTime(String hour,String minute);
        Database* database;
        Box* boxes;
        LiquidCrystal_I2C* lcd;
        
    private:

    	String _dataArrayToJson(String dataa);
        int _boxes_number;
        int _buzzer_pin;
        bool _alarm_state;
        int _alarm_minute;
        bool _pressed;
        int _alert_time;
        void _play(int freq,int duration);
        void _printLocalTime();
    	
	
};
#endif
