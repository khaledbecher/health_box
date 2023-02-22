#if not defined Box_h
#define Box_h

#include <ESP32Servo.h>


class Box {
	public:
		Box(int number,int servo_pin,int button_pin);
		void init(int opened , int closed,int speed);
		void attach_servo();
		void detach_servo();
		void open();
		void close();
		void change_speed(int new_speed);
		int get_number();
		int get_opened_angle();
		int get_closed_angle();
		int get_current_pose();
		int get_button_pin();
		bool get_open_order();
		bool get_close_order();
		bool get_state();
		void change_open_order(bool neww);
		void change_close_order(bool neww);
		void toggle_open_order();
		void toggle_close_order();

		Servo servo;
    private:
    	void _rotate(int angle , int speedd,int sense);
    	int _number;
    	int _servo_pin;
    	int _button_pin;
    	bool _state;
    	int _opened_angle;
    	int _closed_angle;
    	int _current_pose;
    	int _speed;
    	bool _open_order;
    	bool _close_order;
    	
	
};
#endif
