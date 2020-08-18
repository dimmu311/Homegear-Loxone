#include "createInstance.h"

#include "Pushbutton.h"
#include "Slider.h"
#include "Dimmer.h"
#include "LightControllerV2.h"
#include "Jalousie.h"
#include "Alarm.h"
#include "MediaClient.h"
#include "IntelligentRoomControllerV2.h"
#include "AalEmergency.h"

namespace Loxone
{
    template<typename T> LoxoneControl* createInstance3(PVariable control, std::string room, std::string cat) { return new T(control, room, cat); }
    const std::unordered_map<std::string, LoxoneControl* (*)(PVariable, std::string, std::string)> createInstance::_controlsMap =
    {
            {"Pushbutton", &createInstance3<Pushbutton>},
            {"Switch", &createInstance3<Pushbutton>},
            {"Slider", &createInstance3<Slider>},
            {"Dimmer", &createInstance3<Dimmer>},
            {"LightControllerV2", &createInstance3<LightControllerV2>},
            {"Jalousie", &createInstance3<Jalousie>},
            {"Alarm", &createInstance3<Alarm>},
            {"MediaClient", &createInstance3<MediaClient>},
            {"IRoomControllerV2", &createInstance3<IntelligentRoomControllerV2>},
            {"AalEmergency", &createInstance3<AalEmergency>},
    };

    template<typename T> LoxoneControl* createInstance2(std::shared_ptr<BaseLib::Database::DataTable> rows) { return new T(rows); }
    const std::unordered_map<uint32_t, LoxoneControl* (*)(std::shared_ptr<BaseLib::Database::DataTable>)> createInstance::_uintControlsMap =
    {
            {0x100, &createInstance2<Pushbutton>},//Pushbutton
            //{0x100, &createInstance2<Pushbutton>}, Switch
            {0x101, &createInstance2<Slider>},
            {0x102, &createInstance2<Dimmer>},
            {0x103, &createInstance2<LightControllerV2>},
            {0x104, &createInstance2<Jalousie>},
            {0x200, &createInstance2<Alarm>},
            {0x201, &createInstance2<MediaClient>},
            {0x301, &createInstance2<IntelligentRoomControllerV2>},
            {0x400, &createInstance2<AalEmergency>},
    };
}