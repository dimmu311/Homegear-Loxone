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
#include "AalSmartAlarm.h"

namespace Loxone
{

    /*
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
            {"AalSmartAlarm", &createInstance3<AalSmartAlarm>},
    };
*/
    std::shared_ptr<LoxoneControl> createInstance::createInstanceFromTypeString(PVariable control, std::string room, std::string cat)
    {
        auto type = control->structValue->at("type")->stringValue;

        if(type == "Pushbutton" || type == "Switch") return std::make_shared<Pushbutton>(control, room, cat);
        else if(type == "Slider") return std::make_shared<Slider>(control, room, cat);
        else if(type == "Dimmer") return std::make_shared<LoxoneControl>(control, room, cat, 0x102);
        else if(type == "LightControllerV2") return std::make_shared<LoxoneControl>(control, room, cat, 0x103);
        else if(type == "Jalousie") return std::make_shared<LoxoneControl>(control, room, cat, 0x104);
        else if(type == "Alarm") return std::make_shared<LoxoneControl>(control, room, cat, 0x200);
        else if(type == "MediaClient") return std::make_shared<LoxoneControl>(control, room, cat, 0x201);
        else if(type == "AlarmClock") return std::make_shared<LoxoneControl>(control, room, cat, 0x202);
        else if(type == "IRoomControllerV2") return std::make_shared<LoxoneControl>(control, room, cat, 0x301);
        else if(type == "AalEmergency") return std::make_shared<LoxoneControl>(control, room, cat, 0x400);
        else if(type == "AalSmartAlarm") return std::make_shared<LoxoneControl>(control, room, cat, 0x401);
        //Alarm Sequence, this in in Doku, but not shown in struct file. don't know if this is a Loxone issu


        return nullptr;
    }
/*
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
            {0x401, &createInstance2<AalSmartAlarm>},
    };
    */

    std::shared_ptr<LoxoneControl> createInstance::createInstanceFromTypeNr(uint32_t typeNr, std::shared_ptr<BaseLib::Database::DataTable> rows)
    {
        if(typeNr == 0x100) return std::make_shared<Pushbutton>(rows); // Pushbutton || Switch
        else if(typeNr == 0x101) return std::make_shared<Slider>(rows); //Slider
        else if(typeNr == 0x102) return std::make_shared<LoxoneControl>(rows, typeNr); //Dimmer
        else if(typeNr == 0x103) return std::make_shared<LoxoneControl>(rows, typeNr); //LightControllerV2
        else if(typeNr == 0x104) return std::make_shared<LoxoneControl>(rows, typeNr); //Jalousie
        else if(typeNr == 0x200) return std::make_shared<LoxoneControl>(rows, typeNr); //Alarm
        else if(typeNr == 0x201) return std::make_shared<LoxoneControl>(rows, typeNr); //MediaClient
        else if(typeNr == 0x202) return std::make_shared<LoxoneControl>(rows, typeNr); //AlarmClock
        else if(typeNr == 0x301) return std::make_shared<LoxoneControl>(rows, typeNr); //IntelligentRoomControllerV2
        else if(typeNr == 0x400) return std::make_shared<LoxoneControl>(rows, typeNr); //AalEmergency
        else if(typeNr == 0x401) return std::make_shared<LoxoneControl>(rows, typeNr); //AalSmartAlarm
        //Alarm Sequence, this in in Doku, but not shown in struct file. don't know if this is a Loxone issu


        return nullptr;
    }

    /*
     "14004802-0210-102c-ffff14b9c0f9d460":{
        "name":"Wecker",
        "type":"AlarmClock",
        "uuidAction":"14004802-0210-102c-ffff14b9c0f9d460",
        "room":"140032b9-014b-2ed3-ffff14b9c0f9d460",
        "cat":"140032b9-0108-2df0-ffff14b9c0f9d460",
        "defaultRating":0,
        "isFavorite":false,
        "isSecured":false,
        "details":{
            "hasNightLight":true,
            "snoozeDurationConnected":false,
            "brightInactiveConnected":false,
            "brightActiveConnected":false,
            "wakeAlarmSoundConnected":false,
            "wakeAlarmVolumeConnected":false,
            "wakeAlarmSlopingConnected":false,
            "wakeAlarmSounds":[{"id":1,"name":"Tiefer 4-fach Piep"},{"id":2,"name":"Hoher 4-fach Piep"},{"id":3,"name":"Tiefer 2-fach Piep"},{"id":4,"name":"Hoher 2-fach Piep"},{"id":5,"name":"Sirene"}]
        },
        "states":{
            "isEnabled":"14004802-0210-1013-ffff6b6e5a2bad37",
            "isAlarmActive":"14004802-0210-1028-ffff6b6e5a2bad37",
            "confirmationNeeded":"14004802-0210-100b-ffff6b6e5a2bad37",
            "entryList":"14004802-0210-100c-ffff6b6e5a2bad37",
            "currentEntry":"14004802-0210-100d-ffff6b6e5a2bad37",
            "nextEntry":"14004802-0210-100e-ffff6b6e5a2bad37",
            "nextEntryMode":"14004802-0210-1014-ffff6b6e5a2bad37",
            "ringingTime":"14004802-0210-100f-ffff6b6e5a2bad37",
            "ringDuration":"14004802-0210-1010-ffff6b6e5a2bad37",
            "prepareDuration":"14004802-0210-1011-ffff6b6e5a2bad37",
            "snoozeTime":"14004802-0210-102b-ffff6b6e5a2bad37",
            "snoozeDuration":"14004802-0210-1012-ffff6b6e5a2bad37",
            "nextEntryTime":"14004802-0210-1015-ffff6b6e5a2bad37",
            "deviceState":"14004802-0210-1016-ffff6b6e5a2bad37",
            "deviceSettings":"14004802-0210-1017-ffff6b6e5a2bad37",
            "wakeAlarmSoundSettings":"14004802-0210-1018-ffff6b6e5a2bad37"
        }
    }
     */
}