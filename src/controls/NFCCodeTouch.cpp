#include "NFCCodeTouch.h"

namespace Loxone
{
    NFCCodeTouch::NFCCodeTouch(PVariable control, std::unordered_map<std::string, std::string> &room, std::unordered_map<std::string, std::string> &cat) : LoxoneControl(control, room, cat, 0x507)
    {
    }

    NFCCodeTouch::NFCCodeTouch(std::shared_ptr<BaseLib::Database::DataTable> rows, uint32_t typeNr) : LoxoneControl(rows, typeNr)
    {
    }

    bool NFCCodeTouch::setValue(PPacket frame, BaseLib::PVariable parameters, uint32_t channel, std::string& command, bool &isSecured)
    {
        try {
            if (LoxoneControl::setValue(frame, parameters, channel, command, isSecured)) return true;
            if (parameters->type != VariableType::tArray) return false;
            if (frame->function1 != "setStruct") return false;

            if (parameters->arrayValue->at(0)->type != VariableType::tString) return false;
            command += parameters->arrayValue->at(0)->stringValue + "/";
            if (parameters->arrayValue->at(1)->type != VariableType::tString) return false;
            command += parameters->arrayValue->at(1)->stringValue + "/";
            if(parameters->arrayValue->at(2)->type != VariableType::tStruct) return false;
            auto values = parameters->arrayValue->at(2)->structValue;

            if(values->find("name") == values->end()) return false;
            command += "/";
            if (!getValueFromVariable(values->at("name"), command)) return false;

            if(values->find("code") == values->end()) return false;
            command += "/";
            if (!getValueFromVariable(values->at("code"), command)) return false;

            if(values->find("type") == values->end()) return false;
            command += "/";
            if (!getValueFromVariable(values->at("type"), command)) return false;

            if(values->find("outputs") == values->end()) return false;
            command += "/";
            if (!getValueFromVariable(values->at("outputs"), command)) return false;

            if(values->find("standardOutput") == values->end()) return false;
            command += "/";
            if (!getValueFromVariable(values->at("standardOutput"), command)) return false;

            if(values->find("timeFrom") == values->end() && values->find("timeTo") == values->end()) return true;
            else if(values->find("timeFrom") == values->end() && values->find("timeTo") != values->end()) return false;
            else if(values->find("timeFrom") != values->end() && values->find("timeTo") == values->end()) return false;
            else{
                command += "/";
                if (!getValueFromVariable(values->at("timeFrom"), command)) return false;
                command += "/";
                if (!getValueFromVariable(values->at("timeFrom"), command)) return false;
                return true;
            }
        }
        catch (const std::exception& ex){
            GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
        }
        return false;
    }
}
