#ifndef __XLEDCONTROL__
#define __XLEDCONTROL__

//class derived from https://github.com/wayoda/LedControl

#include <LedControl-MAX7219-MAX7221.h>
#include "dotmatrixalphabet.h"

typedef void(*Callback)(void);

class XLedControl : public LedControl
{
public:
    XLedControl(int dataPin, int clkPin, int csPin, int numDevices);
        
    void writeSimpleMessage(String msg);
    
    void setScrollingMessage(String msg);
    
    void updateScrollingMessage();
    
    void clearScrollingMessage();
    
    void OnEndOfMessage(Callback func);
    
public:
    static const unsigned MAXMSGLEN = 4;
    
private:
    String _msg;
    bool _scrolling;
    unsigned _index,_charCount;
    byte _display[MAXMSGLEN+1][DISPLAY_WIDTH];
    Callback _endOfMessageFunc;
    
private:
    void _updateVideoBuffer(char letter);
    unsigned _getCharIndex(const char letter);
    
};
#endif //__XLEDCONTROL__