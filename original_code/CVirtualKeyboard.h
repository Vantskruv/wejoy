#ifndef CVIRTUAL_KEYBOARD_H
#define CVIRTUAL_KEYBOARD_H

class CVirtualKeyboard
{
	private:
		int fd;									//Device handler id
		//uint64_t keyFlags = 0;

	public:
		int get_key_status(int);												//Get defined key state
		//void set_key_flags(uint64_t _b){		keyFlags = _b;			};		//Set keys to on or off 
		//const uint64_t& get_key_flags(){		return keyFlags;		};		//Get the status of key states.


		void send_key_event(int, int);
		bool isOpen();
		CVirtualKeyboard();	//buttons, axes
		~CVirtualKeyboard();

};





#endif
