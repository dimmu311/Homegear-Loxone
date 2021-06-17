#include "Central.h"

namespace Loxone
{
    Central::Central(PVariable control, std::unordered_map<std::string, std::string> &room, std::unordered_map<std::string, std::string> &cat, uint32_t typeNr) : LoxoneControl(control, room, cat,typeNr)
    {
    }

    Central::Central(std::shared_ptr<BaseLib::Database::DataTable> rows, uint32_t typeNr) : LoxoneControl(rows, typeNr)
    {
    }

    uint32_t Central::getExtraData(std::list<extraData> &extraData)
    {
        //if(LoxoneControl::getExtraData(extraData) != 0) return -1;
        if(_control->structValue->find("details") == _control->structValue->end()) return -1;
        auto details = _control->structValue->at("details");
        if(details->structValue->find("controls") == details->structValue->end()) return -1;
        auto controls = details->structValue->at("controls");

        for(auto i = controls->arrayValue->begin(); i != controls->arrayValue->end(); ++i)
        {
            struct extraData data;
            data.channel = (*i)->structValue->at("id")->integerValue + 2;
            data.variable = "CONTROL";
            data.value = (*i)->structValue->at("uuid");
            extraData.push_back(data);
        }
        return 0;
    }

    bool Central::setValue(PPacket frame, BaseLib::PVariable parameters, uint32_t channel, std::string& command, bool &isSecured)
    {
        try
        {
            if(LoxoneControl::setValue(frame, parameters, channel, command, isSecured)) return true;
            if(parameters->type != VariableType::tArray) return false;
            if(frame->function1 != "special") return false;

            if(frame->function2 == "setControl")
            {
                if (parameters->arrayValue->at(0)->type != VariableType::tString) return false;
                command += "selectedcontrols/" + std::to_string(channel -2) + "/" + parameters->arrayValue->at(0)->stringValue;
                return true;
            }
            if(frame->function2 == "setControls")
            {
                if (parameters->arrayValue->at(0)->type != VariableType::tString) return false;
                if (parameters->arrayValue->at(1)->type != VariableType::tString) return false;
                command += "selectedcontrols/" + parameters->arrayValue->at(1)->stringValue + "/" + parameters->arrayValue->at(0)->stringValue;
                return true;
            }
        }
        catch (const std::exception& ex)
        {
            GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
        }
        return false;
    }
}