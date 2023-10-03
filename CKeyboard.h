#ifndef CKEYBOARD_HEADER
#define CKEYBOARD_HEADER

#include <string>
#include <set>

class CKeyboardEvent
{
public:
  static const int RELEASED = 0;
  static const int PRESSED = 1;
  static const int REPEATED = 2; // Not used
  int state;
  char code;
};

class CKeyboard
{
private:
  int fd = -1;
  std::string eventPath;

public:
  std::set<int> pressedKeys;

  std::string getEventPath();
  bool isOpen();
  bool readEvent(CKeyboardEvent*);
        
  CKeyboard(std::string eventPath);
  ~CKeyboard();
};

#endif
