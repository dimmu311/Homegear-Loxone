#ifndef LOXONE_MINISERVER_KELVINRGB_H
#define LOXONE_MINISERVER_KELVINRGB_H

#include <string>
#include <cmath>

class KelvinRgb {
public:
    KelvinRgb();
    KelvinRgb(uint32_t temperature, uint32_t brightness);
    uint8_t getRed(){return _red;}
    uint8_t getGreen(){return _green;}
    uint8_t getBlue(){return _blue;}

    void setRed(uint8_t red){_red = red;}
    void setGreen(uint8_t green){_green = green;}
    void setBlue(uint8_t blue){_blue = blue;}

protected:
    void calculateRed();
    void calculateGreen();
    void calculateBlue();

private:
    float map(float value, float start1, float stop1, float start2, float stop2);

    uint32_t _temperature;
    uint32_t _brightness;

    uint8_t _red;
    uint8_t _green;
    uint8_t _blue;

};


#endif //LOXONE_MINISERVER_KELVINRGB_H
