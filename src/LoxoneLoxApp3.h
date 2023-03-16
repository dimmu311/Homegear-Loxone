#ifndef LOXONELOXAPP3_H_
#define LOXONELOXAPP3_H_

#include <homegear-base/BaseLib.h>
#include "GD.h"
#include "LoxoneControl.h"

namespace Loxone
{

class LoxoneLoxApp3
{
public:
    struct weather{
        struct format{
            std::string relativeHumidity;
            std::string temperature;
            std::string windSpeed;
            std::string precipitation;
            std::string barometricPressure;
        };
        struct sWeatherFieldTypes{
            uint8_t id;
            std::string name;
            bool analog = false;
            std::string unit;
            std::string format;
        };
        std::string getUuidActual() {return uuidActual;}
        std::string getUuidForecast() {return uuidForecast;}
        format getFormat() {return format;}
        std::map<uint8_t, std::string> getWeatherTypeTexts(){return weatherTypeTexts;}
        std::map<uint8_t, sWeatherFieldTypes> getWeatherFieldTypes(){return weatherFieldTypes;}

        std::string uuidActual;
        std::string uuidForecast;
        format format;
        std::map<uint8_t, std::string> weatherTypeTexts;
        std::map<uint8_t, sWeatherFieldTypes> weatherFieldTypes;
    };

	LoxoneLoxApp3();
	int32_t parseStructFile(BaseLib::PVariable structFile);
	std::vector<std::shared_ptr<LoxoneControl>> getControls() { return _controls; }
    std::string getlastModified() { return _lastModified; }
protected:
	BaseLib::PVariable _structFile;
	std::unordered_map<std::string, std::string> _cats;
	std::unordered_map<std::string, std::string> _rooms;
	std::vector<std::shared_ptr<LoxoneControl>> _controls;
	weather _weather;
	std::string _lastModified;

	void loadlastModified();
	void loadCats();
	void loadRooms();
	void loadControls();
	void loadWeatherServer();
private:
    BaseLib::Output _out;
};
}
#endif
