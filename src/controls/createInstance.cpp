#include "createInstance.h"

#include "Pushbutton.h"
#include "Slider.h"
#include "ColorPickerV2.h"

namespace Loxone
{
    std::shared_ptr<LoxoneControl> createInstance::createInstanceFromTypeString(PVariable control, std::string room, std::string cat)
    {
        auto type = control->structValue->at("type")->stringValue;

        if(type == "Pushbutton" || type == "Switch") return std::make_shared<Pushbutton>(control, room, cat);
        else if(type == "Slider") return std::make_shared<Slider>(control, room, cat);
        else if(type == "Dimmer") return std::make_shared<LoxoneControl>(control, room, cat, 0x102);
        else if(type == "LightControllerV2") return std::make_shared<LoxoneControl>(control, room, cat, 0x103);
        else if(type == "Jalousie") return std::make_shared<LoxoneControl>(control, room, cat, 0x104);
        else if(type == "ColorPickerV2") return std::make_shared<ColorPickerV2>(control, room, cat);
        else if(type == "Alarm") return std::make_shared<LoxoneControl>(control, room, cat, 0x200);
        else if(type == "MediaClient") return std::make_shared<LoxoneControl>(control, room, cat, 0x201);
        else if(type == "AlarmClock") return std::make_shared<LoxoneControl>(control, room, cat, 0x202);
        else if(type == "IRoomControllerV2") return std::make_shared<LoxoneControl>(control, room, cat, 0x301);
        else if(type == "AalEmergency") return std::make_shared<LoxoneControl>(control, room, cat, 0x400);
        else if(type == "AalSmartAlarm") return std::make_shared<LoxoneControl>(control, room, cat, 0x401);
        else if(type == "CarCharger") return std::make_shared<LoxoneControl>(control, room, cat, 0x402);
        //Alarm Sequence, this in in Doku, but not shown in struct file. don't know if this is a Loxone issu

        return nullptr;
    }

    std::shared_ptr<LoxoneControl> createInstance::createInstanceFromTypeNr(uint32_t typeNr, std::shared_ptr<BaseLib::Database::DataTable> rows)
    {
        if(typeNr == 0x100) return std::make_shared<Pushbutton>(rows); // Pushbutton || Switch
        else if(typeNr == 0x101) return std::make_shared<Slider>(rows); //Slider
        else if(typeNr == 0x102) return std::make_shared<LoxoneControl>(rows, typeNr); //Dimmer
        else if(typeNr == 0x103) return std::make_shared<LoxoneControl>(rows, typeNr); //LightControllerV2
        else if(typeNr == 0x104) return std::make_shared<LoxoneControl>(rows, typeNr); //Jalousie
        else if(typeNr == 0x105) return std::make_shared<ColorPickerV2>(rows); //ColorPickerV2
        else if(typeNr == 0x200) return std::make_shared<LoxoneControl>(rows, typeNr); //Alarm
        else if(typeNr == 0x201) return std::make_shared<LoxoneControl>(rows, typeNr); //MediaClient
        else if(typeNr == 0x202) return std::make_shared<LoxoneControl>(rows, typeNr); //AlarmClock
        else if(typeNr == 0x301) return std::make_shared<LoxoneControl>(rows, typeNr); //IntelligentRoomControllerV2
        else if(typeNr == 0x400) return std::make_shared<LoxoneControl>(rows, typeNr); //AalEmergency
        else if(typeNr == 0x401) return std::make_shared<LoxoneControl>(rows, typeNr); //AalSmartAlarm
        else if(typeNr == 0x402) return std::make_shared<LoxoneControl>(rows, typeNr); //CarCharger
        //Alarm Sequence, this in in Doku, but not shown in struct file. don't know if this is a Loxone issu

        return nullptr;
    }
}