#include "createInstance.h"

#include "Pushbutton.h"
#include "Slider.h"
#include "ColorPicker.h"
#include "Central.h"
#include "Daytimer.h"

namespace Loxone
{
    std::shared_ptr<LoxoneControl> createInstance::createInstanceFromTypeString(PVariable control, std::unordered_map<std::string, std::string> &room, std::unordered_map<std::string, std::string> &cat)
    {
        auto type = control->structValue->at("type")->stringValue;

        if(type == "Pushbutton" || type == "Switch") return std::make_shared<Pushbutton>(control, room, cat);
        else if(type == "Slider") return std::make_shared<Slider>(control, room, cat);
        else if(type == "Dimmer") return std::make_shared<LoxoneControl>(control, room, cat, 0x102);
        else if(type == "LightControllerV2") return std::make_shared<LoxoneControl>(control, room, cat, 0x103);
        else if(type == "Jalousie") return std::make_shared<LoxoneControl>(control, room, cat, 0x104);
        else if(type == "ColorPickerV2") return std::make_shared<ColorPicker>(control, room, cat);
        else if(type == "ColorPicker") return std::make_shared<ColorPicker>(control, room, cat);
        else if(type == "Gate") return std::make_shared<LoxoneControl>(control, room, cat, 0x107);
        else if(type == "InfoOnlyAnalog") return std::make_shared<LoxoneControl>(control, room, cat, 0x108);
        else if(type == "InfoOnlyDigital") return std::make_shared<LoxoneControl>(control, room, cat, 0x109);
        else if(type == "Alarm") return std::make_shared<LoxoneControl>(control, room, cat, 0x200);
        else if(type == "MediaClient") return std::make_shared<LoxoneControl>(control, room, cat, 0x201);
        else if(type == "AlarmClock") return std::make_shared<LoxoneControl>(control, room, cat, 0x202);
        else if(type == "IRCV2Daytimer") return std::make_shared<Daytimer>(control, room, cat);
        else if(type == "Daytimer") return std::make_shared<Daytimer>(control, room, cat);
        //don't know if there are some more Daytimer Variants. The Api is not correct at this point
        else if(type == "Heatmixer") return std::make_shared<LoxoneControl>(control, room, cat, 0x204);
        else if(type == "Hourcounter") return std::make_shared<LoxoneControl>(control, room, cat, 0x205);
        else if(type == "ClimateController") return std::make_shared<LoxoneControl>(control, room, cat, 0x300);
        else if(type == "IRoomControllerV2") return std::make_shared<LoxoneControl>(control, room, cat, 0x301);
        else if(type == "IRoomController") return std::make_shared<LoxoneControl>(control, room, cat, 0x302);
        else if(type == "AalEmergency") return std::make_shared<LoxoneControl>(control, room, cat, 0x400);
        else if(type == "AalSmartAlarm") return std::make_shared<LoxoneControl>(control, room, cat, 0x401);
        else if(type == "CarCharger") return std::make_shared<LoxoneControl>(control, room, cat, 0x402);
        else if(type == "FanController") return std::make_shared<LoxoneControl>(control, room, cat, 0x403);
        else if(type == "EnergyMonitor") return std::make_shared<LoxoneControl>(control, room, cat, 0x404);
        else if(type == "CentralAlarm") return std::make_shared<Central>(control, room, cat, 0x500);
        else if(type == "CentralAudioZone") return std::make_shared<Central>(control, room, cat, 0x501);
        else if(type == "CentralGate") return std::make_shared<Central>(control, room, cat, 0x502);
        else if(type == "CentralJalousie") return std::make_shared<Central>(control, room, cat, 0x503);
        else if(type == "CentralLightController") return std::make_shared<Central>(control, room, cat, 0x504);
        //else if(type == "Intercom") return std::make_shared<LoxoneControl>(control, room, cat, 0x505);
        else return std::make_shared<LoxoneControl>(control, room, cat, 0x001);
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
        else if(typeNr == 0x105) return std::make_shared<ColorPicker>(rows, typeNr); //ColorPickerV2
        else if(typeNr == 0x106) return std::make_shared<ColorPicker>(rows, typeNr); //ColorPicker
        else if(typeNr == 0x107) return std::make_shared<LoxoneControl>(rows, typeNr); //Gate
        else if(typeNr == 0x108) return std::make_shared<LoxoneControl>(rows, typeNr); //InfoOnlyAnalog
        else if(typeNr == 0x109) return std::make_shared<LoxoneControl>(rows, typeNr); //InfoOnlyDigital
        else if(typeNr == 0x200) return std::make_shared<LoxoneControl>(rows, typeNr); //Alarm
        else if(typeNr == 0x201) return std::make_shared<LoxoneControl>(rows, typeNr); //MediaClient
        else if(typeNr == 0x202) return std::make_shared<LoxoneControl>(rows, typeNr); //AlarmClock
        else if(typeNr == 0x203) return std::make_shared<Daytimer>(rows, typeNr); //Daytimer, IRCV2Daytimer.....
        else if(typeNr == 0x204) return std::make_shared<LoxoneControl>(rows, typeNr); //Heatmixer
        else if(typeNr == 0x205) return std::make_shared<LoxoneControl>(rows, typeNr); //Hourcounter
        else if(typeNr == 0x300) return std::make_shared<LoxoneControl>(rows, typeNr); //ClimateController
        else if(typeNr == 0x301) return std::make_shared<LoxoneControl>(rows, typeNr); //IntelligentRoomControllerV2
        else if(typeNr == 0x302) return std::make_shared<LoxoneControl>(rows, typeNr); //IntelligentRoomController
        else if(typeNr == 0x400) return std::make_shared<LoxoneControl>(rows, typeNr); //AalEmergency
        else if(typeNr == 0x401) return std::make_shared<LoxoneControl>(rows, typeNr); //AalSmartAlarm
        else if(typeNr == 0x402) return std::make_shared<LoxoneControl>(rows, typeNr); //CarCharger
        else if(typeNr == 0x403) return std::make_shared<LoxoneControl>(rows, typeNr); //FanController
        else if(typeNr == 0x404) return std::make_shared<LoxoneControl>(rows, typeNr); //EnergyMonitor
        else if(typeNr == 0x500) return std::make_shared<Central>(rows, typeNr); //CentralAlarm
        else if(typeNr == 0x501) return std::make_shared<Central>(rows, typeNr); //CentralAudioZone
        else if(typeNr == 0x502) return std::make_shared<Central>(rows, typeNr); //CentralGate
        else if(typeNr == 0x503) return std::make_shared<Central>(rows, typeNr); //CentralJalousie
        else if(typeNr == 0x504) return std::make_shared<Central>(rows, typeNr); //CentralLightController
        //else if(typeNr == 0x505) return std::make_shared<LoxoneControl>(rows, typeNr); //Intercom
        else return std::make_shared<LoxoneControl>(rows, typeNr);
        //Alarm Sequence, this in in Doku, but not shown in struct file. don't know if this is a Loxone issu

        return nullptr;
    }
}