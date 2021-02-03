/*
 * inspired by:
 * https://github.com/timvw74/Kelvin2RGB
 */
#include "KelvinRgb.h"

KelvinRgb::KelvinRgb(uint32_t temperature, uint32_t brightness) {
    if(temperature < 0) _temperature = 0;
    else if(temperature >65500) _temperature = 65500;
    else _temperature = temperature;
    _temperature /=100;

    if(brightness < 0 ) _brightness = 0;
    else if (brightness > 100) _brightness = 100;
    else _brightness = brightness;

    calculateRed();
    calculateGreen();
    calculateBlue();
}

void KelvinRgb::calculateRed() {
    float red = 255;
    if(_temperature>66) {
        red = _temperature - 60;
        red = 329.698727466 * pow(red,-0.1332047592);
    }
    if(red < 0) red = 0;
    else if(red > 255) red = 255;

    _red = map(_brightness, 0, 100, 0, red);
}

void KelvinRgb::calculateGreen() {
    float green;
    if(_temperature<= 66){
        green = _temperature;
        green = (99.4708025861 * log(green)) - 161.1195681661;
    } else {
        green = _temperature - 60;
        green = 288.1221695283 * pow(green, -0.0755148492);
    }
    if(green < 0) green = 0;
    else if(green > 255) green = 255;

    _green = map(_brightness, 0, 100, 0, green);
}

void KelvinRgb::calculateBlue() {
    float blue = 255;
    if(_temperature<65){
        if(_temperature <= 19){
            blue = 0;
        } else {
            blue = _temperature - 10;
            blue = (138.5177312231 * log(blue)) - 305.0447927307;
        }
    }
    if(blue < 0) blue = 0;
    else if(blue > 255) blue = 255;

    _blue = map(_brightness, 0, 100, 0, blue);
}

float KelvinRgb::map(float value, float start1, float stop1, float start2, float stop2) {
    return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}
