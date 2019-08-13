#ifndef CKEYBOARD_HEADER
#define CKEYBOARD_HEADER

#include <string>


class CKeyboardEvent
{
    public:
        bool isPressed;
        char code;
};

class CKeyboard
{
    private:
        int fd = -1;
        std::string eventPath;

    public:
        std::string getEventPath();
        bool isOpen();
        bool readEvent(CKeyboardEvent*);
        
        CKeyboard(std::string eventPath);
        ~CKeyboard();
};

#endif
