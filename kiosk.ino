// This #include statement was automatically added by the Particle IDE.
#pragma SPARK_NO_PREPROCESSOR
#include "application.h"
#include "xledcontrol.h"
#include "powercheck.h"

static const unsigned D_MOSI = A5;
static const unsigned D_SCK = A3;
static const unsigned D_SS = A2;
static const unsigned D_NUMDEVICES = 4;
static const unsigned D_INTERRUPT_PIN = D5;

XLedControl lc=XLedControl(D_MOSI,D_SCK,D_SS,D_NUMDEVICES);
PowerCheck pc;

//main vairables
String currentMessage="?";
int speed=25;

void endOfMessageHandler();
void motionHandler();
void setLowPowerMode(bool isLowPower);

//cloud function defs
int setMessage(String msg);
int setSpeed(String msg);

struct Memory
{
    uint8_t len;
    char message[63];
    uint8_t speed;
} m;

void setup() {
    
    //setup particle cloud interface
    Particle.function("set_message",setMessage);
    Particle.variable("current_message",currentMessage);
    Particle.function("set_speed",setSpeed);
    Particle.variable("current_speed",speed);
    
    pc.setup();
    
    EEPROM.get(0,m);
    currentMessage = String(m.message).substring(0,m.len);
    speed = m.speed;

    //setup motion detection handler
    pinMode(D_INTERRUPT_PIN, INPUT_PULLDOWN);
    attachInterrupt(D_INTERRUPT_PIN,motionHandler,RISING);
    lc.OnEndOfMessage(endOfMessageHandler);
    
    setLowPowerMode(true);

}

bool motionDetected = false;
bool motionLatched = false;

void loop() {
    

    if(motionDetected)
    {
        if(!motionLatched)
        {
            setLowPowerMode(false);
            motionLatched = true;
            lc.setScrollingMessage(String(currentMessage));
        }
        
        lc.updateScrollingMessage();
        delay(speed);
        
    }
    else
    {
        if(motionLatched)
        {
            setLowPowerMode(true);
            motionLatched = false;
        }
        
    }
    
#ifdef Wiring_Wire3 //determines if fuel gauge is present
    //TODO: power management
    // if(pc.getHasBattery()) 
    // {
    //     FuelGauge fg;
        
    //     float charge = fg.getSoC();
        
    //     if(charge < 20)
    //     {
            
    //     }
    //     else if (charge >= 20 && charge < 80)
    //     {
            
    //     }
    // }
    
    //TODO: notification on power up
    //TODO: setup notification for low battery
    
#endif

}

void motionHandler()
{
    motionDetected = true;
}

void endOfMessageHandler()
{
    motionDetected = false;
}

void setLowPowerMode(bool isLowPower)
{
    int devices=lc.getDeviceCount();
    
    for(int address=0;address<devices;address++) {
        /*The MAX72XX is in power-saving mode on startup*/
        lc.shutdown(address,isLowPower);
    }
    
    // if(isLowPower)
    // {
    //     System.sleep(D_INTERRUPT_PIN, RISING, 0, SLEEP_NETWORK_STANDBY);
    // }
    
}

int setMessage(String msg)
{
    currentMessage=msg;
    m.len = currentMessage.length();
    memcpy(&m.message,msg.c_str(),msg.length());
    EEPROM.put(0,m);
    return msg.toInt();
}

int setSpeed(String msg)
{
    speed=msg.toInt();
    m.speed = static_cast<uint8_t>(speed);
    EEPROM.put(0,m);
    return msg.toInt();
}