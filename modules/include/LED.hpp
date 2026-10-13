#ifndef LED_HPP
#define LED_HPP
#include "gpio.h"

class LED
{
public:
    enum Color{RED=0,GREEN=1,BLUE=2};

    static void On(Color c);
    static void Off(Color c);
    static void Toggle(Color c);
    static void OffAll(); 
};
#endif