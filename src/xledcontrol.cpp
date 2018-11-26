#include "xledcontrol.h"

XLedControl::XLedControl(int dataPin, int clkPin, int csPin, int numDevices) 
    : LedControl(dataPin,clkPin,csPin,numDevices),
      _msg('?'),
      _scrolling(false),
      _index(0),
      _charCount(0),
      _endOfMessageFunc(0)
{
    for(unsigned i = 0; i < MAXMSGLEN+1; i++)
    {
        memset(&_display[i],0,sizeof(byte));
    }
}

void XLedControl::writeSimpleMessage(String msg)
{
    unsigned len = msg.length() > MAXMSGLEN ? MAXMSGLEN : msg.length();
    
    //update display buffer
    
    //render
    
    //iterate through message
    for(unsigned i=0; (i<MAXMSGLEN) && (i<len) ;i++)
    {
        unsigned index = _getCharIndex(msg.charAt(i));
        
        //draw character
        for(unsigned j=0;j<DISPLAY_WIDTH;j++)
        {
            setRow(MAXMSGLEN-i-1,j,CHAR_DICTIONARY[index][j]);
        }
        
    }
    
}

void XLedControl::setScrollingMessage(String msg)
{
    _msg=msg;
    _scrolling=true;
    _charCount=0;
    _index=0;
    // _updateVideoBuffer(CHAR_DICTIONARY[_getCharIndex(_msg.charAt(_charCount))]);
    _updateVideoBuffer(_msg.charAt(_charCount));
}

// void XLedControl::_updateVideoBuffer(const byte letter[DISPLAY_WIDTH])
void XLedControl::_updateVideoBuffer(char letter)
{
    for(unsigned i = 0;i<DISPLAY_WIDTH;i++)
    {
        _display[0][i]=CHAR_DICTIONARY[_getCharIndex(letter)][i];
    }
    
}

void XLedControl::updateScrollingMessage()
{
    if(_scrolling)
    {
        
        //update display buffer and render
        for(unsigned display_cnt = 0; display_cnt < MAXMSGLEN; display_cnt++) //for each display
        {
            unsigned current_display = MAXMSGLEN-display_cnt; //start with last display
            
            for(unsigned line = 0; line < DISPLAY_WIDTH; line++) //update the display
            {
                
                //current line = current line shifted and masked with the next available pixel from adjacent display
                _display[current_display][line] = (_display[current_display][line] << 1) | (_display[current_display-1][line] >> (DISPLAY_WIDTH-1));
                setRow(current_display-1,line,_display[current_display][line]);
            }
        }
        
        //update the end of the display buffer
        for(unsigned line = 0; line < DISPLAY_WIDTH; line++) //update the display
        {
            //current line = current line shifted and masked with the next available pixel from adjacent display
            _display[0][line] = _display[0][line] << 1;
        }
        
        
        //check to see if we've scrolled through a single character
        if(_index < DISPLAY_WIDTH)
            _index++;
        else
        {
            _index=0;
            
            //check to see if we've scrolled through the entire message
            if(_charCount < _msg.length()+MAXMSGLEN)
            {
                _charCount++;
            }
            else
            {
                _charCount = 0;
                _endOfMessageFunc();
            }
            
            // _updateVideoBuffer(CHAR_DICTIONARY[_getCharIndex(_msg.charAt(_charCount))]);
            _updateVideoBuffer(_msg.charAt(_charCount));
                
        }
    }
}

void XLedControl::clearScrollingMessage()
{
    for(unsigned i=0;i<MAXMSGLEN;i++)
    {
        clearDisplay(i);
    }
    
    _scrolling=false;
}

unsigned XLedControl::_getCharIndex(const char letter)
{
    unsigned index = ALPHABET.indexOf(letter);
        
    if(index==-1)
        index = CHAR_DICTIONARY_LEN-1; //question mark acts in place of unsupported char
        
    return index;
}

void XLedControl::OnEndOfMessage(Callback func)
{
    _endOfMessageFunc = func;
}

