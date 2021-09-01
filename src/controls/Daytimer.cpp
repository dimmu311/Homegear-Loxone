#include "Daytimer.h"
namespace Loxone
{
    Daytimer::Daytimer(PVariable control, std::unordered_map<std::string, std::string> &room, std::unordered_map<std::string, std::string> &cat) : LoxoneControl(control, room, cat,0x203)
    {
    }

    Daytimer::Daytimer(std::shared_ptr<BaseLib::Database::DataTable> rows, uint32_t typeNr) : LoxoneControl(rows, typeNr)
    {
    }

    bool Daytimer::setValue(PPacket frame, BaseLib::PVariable parameters, uint32_t channel, std::string& command, bool &isSecured)
    {
        try {
            if (LoxoneControl::setValue(frame, parameters, channel, command, isSecured)) return true;
            if (parameters->type != VariableType::tArray) return false;
            if (frame->function1 != "special") return false;

            if (frame->function2 == "setOverride") {
                if (parameters->arrayValue->at(0)->type != VariableType::tFloat) return false;
                if (parameters->arrayValue->at(1)->type != VariableType::tFloat) return false;

                if((bool)parameters->arrayValue->at(0)->floatValue){
                    command += "startOverride/" + std::to_string(parameters->arrayValue->at(1)->floatValue);
                    return true;
                }
                command += "stopOverride";
                return true;
            }
        }
        catch (const std::exception& ex){
            GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
        }
        return false;
    }

    bool Daytimer::setValue(uint32_t channel, std::string valueKey, PVariable value, std::unordered_map<uint32_t, std::unordered_map<std::string, Systems::RpcConfigurationParameter>> &valuesCentral, std::string &command, bool &isSecured)
    {
        isSecured = _isSecured;
        command = "jdev/sps/io/" + _uuidAction + "/";
        command += "set/" + std::to_string(1) + "/";

        BaseLib::Systems::RpcConfigurationParameter& parameter = valuesCentral[channel]["MODE"];
        auto variable = parameter.rpcParameter->convertFromPacket(parameter.getBinaryData(), parameter.mainRole(), true);
        auto type = variable->type;
        //valuesCentral[channel]["MODE"].rpcParameter->convertFromPacket(i->second.value, parameter.mainRole(), true)
/*
        set/{numberOfEntries}/{entry}/{entry}/…,
        {entry} = {mode};{fromMin};{toMin};{needsActivation};{valueOfEntry}
        valueOfEntry will always be “0” in digital daytimers, or left out. Digital
        daytimers outputs are “On” as long as an entry exists.
        from and to are to be given as minutes since midnight
       */

        if(valuesCentral.find(channel) == valuesCentral.end()) return false;
        if(valuesCentral[channel].find(valueKey) == valuesCentral[channel].end()) return false;

        return false;

    }

    bool Daytimer::packetReceived(uint32_t peerID, PLoxoneDaytimerStatesPacket loxonePacket, std::unordered_map<uint32_t, std::unordered_map<std::string, Systems::RpcConfigurationParameter>> &valuesCentral)
    {
        try {
            //todo: maybe use this insted of generating json payload
            /*
            if(valuesCentral.find(1) == valuesCentral.end()) return false;
            if(valuesCentral[1].find("DEFAULT") == valuesCentral[1].end()) return false;
            {
                BaseLib::Systems::RpcConfigurationParameter &parameter = valuesCentral[1]["DEFAULT"];
                std::vector<uint8_t> parameterData;
                PVariable value = std::make_shared<Variable>(loxonePacket->getDevValue());
                parameter.rpcParameter->convertToPacket(value, parameter.mainRole(), parameterData);

                if (!parameter.equals(parameterData)) parameter.setBinaryData(parameterData);

                //if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
                //else saveParameter(0, ParameterGroup::Type::Enum::config, i->channel, i->variable, parameterData);

                GD::out.printInfo(
                        "Info: Parameter DEFAULT of peer " + std::to_string(peerID) + " and channel " +
                        std::to_string(1) + " was set to 0x" +
                        BaseLib::HelperFunctions::getHexString(parameterData) + ".");
            }
            */

            auto entrys = loxonePacket->getEntrys();
            uint32_t i = 3;
            for(auto entry = entrys.begin(); entry != entrys.end(); ++entry)
            {
                if(valuesCentral.find(i) == valuesCentral.end()) continue;
                if(valuesCentral[i].find("ID") == valuesCentral[i].end()) continue;
                {
                    BaseLib::Systems::RpcConfigurationParameter &parameter = valuesCentral[i]["ID"];
                    std::vector<uint8_t> parameterData;
                    PVariable value = std::make_shared<Variable>(entry->first);
                    parameter.rpcParameter->convertToPacket(value, parameter.mainRole(), parameterData);

                    if (!parameter.equals(parameterData)) parameter.setBinaryData(parameterData);

                    //if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
                    //else saveParameter(0, ParameterGroup::Type::Enum::config, i->channel, i->variable, parameterData);

                    GD::out.printInfo(
                            "Info: Parameter ID of peer " + std::to_string(peerID) + " and channel " +
                            std::to_string(i) + " was set to 0x" +
                            BaseLib::HelperFunctions::getHexString(parameterData) + ".");
                }
                if(valuesCentral.find(i) == valuesCentral.end()) continue;
                if(valuesCentral[i].find("FROM") == valuesCentral[i].end()) continue;
                {
                    BaseLib::Systems::RpcConfigurationParameter &parameter = valuesCentral[i]["FROM"];
                    std::vector<uint8_t> parameterData;
                    PVariable value = std::make_shared<Variable>(entry->second->_from);
                    parameter.rpcParameter->convertToPacket(value, parameter.mainRole(), parameterData);

                    if (!parameter.equals(parameterData)) parameter.setBinaryData(parameterData);

                    //if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
                    //else saveParameter(0, ParameterGroup::Type::Enum::config, i->channel, i->variable, parameterData);

                    GD::out.printInfo(
                            "Info: Parameter FROM of peer " + std::to_string(peerID) + " and channel " +
                            std::to_string(i) + " was set to 0x" +
                            BaseLib::HelperFunctions::getHexString(parameterData) + ".");
                }
                if(valuesCentral[i].find("TO") == valuesCentral[i].end()) continue;
                {
                    BaseLib::Systems::RpcConfigurationParameter &parameter = valuesCentral[i]["TO"];
                    std::vector<uint8_t> parameterData;
                    PVariable value = std::make_shared<Variable>(entry->second->_to);
                    parameter.rpcParameter->convertToPacket(value, parameter.mainRole(), parameterData);

                    if (!parameter.equals(parameterData)) parameter.setBinaryData(parameterData);

                    //if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
                    //else saveParameter(0, ParameterGroup::Type::Enum::config, i->channel, i->variable, parameterData);

                    GD::out.printInfo(
                            "Info: Parameter TO of peer " + std::to_string(peerID) + " and channel " +
                            std::to_string(i) + " was set to 0x" +
                            BaseLib::HelperFunctions::getHexString(parameterData) + ".");
                }
                if(valuesCentral[i].find("MODE") == valuesCentral[i].end()) continue;
                {
                    BaseLib::Systems::RpcConfigurationParameter &parameter = valuesCentral[i]["MODE"];
                    std::vector<uint8_t> parameterData;
                    PVariable value = std::make_shared<Variable>(entry->second->_mode);
                    parameter.rpcParameter->convertToPacket(value, parameter.mainRole(), parameterData);

                    if (!parameter.equals(parameterData)) parameter.setBinaryData(parameterData);

                    //if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
                    //else saveParameter(0, ParameterGroup::Type::Enum::config, i->channel, i->variable, parameterData);

                    GD::out.printInfo(
                            "Info: Parameter MODE of peer " + std::to_string(peerID) + " and channel " +
                            std::to_string(i) + " was set to 0x" +
                            BaseLib::HelperFunctions::getHexString(parameterData) + ".");
                }
                if(valuesCentral[i].find("NEED_ACTIVATE") == valuesCentral[i].end()) continue;
                {
                    BaseLib::Systems::RpcConfigurationParameter &parameter = valuesCentral[i]["NEED_ACTIVATE"];
                    std::vector<uint8_t> parameterData;
                    PVariable value = std::make_shared<Variable>(entry->second->_needActivate);
                    parameter.rpcParameter->convertToPacket(value, parameter.mainRole(), parameterData);

                    if (!parameter.equals(parameterData)) parameter.setBinaryData(parameterData);

                    //if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
                    //else saveParameter(0, ParameterGroup::Type::Enum::config, i->channel, i->variable, parameterData);

                    GD::out.printInfo(
                            "Info: Parameter NEED_ACTIVATE of peer " + std::to_string(peerID) + " and channel " +
                            std::to_string(i) + " was set to 0x" +
                            BaseLib::HelperFunctions::getHexString(parameterData) + ".");
                }
                if(valuesCentral[i].find("VALUE") == valuesCentral[i].end()) continue;
                {
                    BaseLib::Systems::RpcConfigurationParameter &parameter = valuesCentral[i]["VALUE"];
                    std::vector<uint8_t> parameterData;
                    PVariable value = std::make_shared<Variable>(entry->second->_value);
                    parameter.rpcParameter->convertToPacket(value, parameter.mainRole(), parameterData);

                    if (!parameter.equals(parameterData)) parameter.setBinaryData(parameterData);

                    //if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
                    //else saveParameter(0, ParameterGroup::Type::Enum::config, i->channel, i->variable, parameterData);

                    GD::out.printInfo(
                            "Info: Parameter VALUE of peer " + std::to_string(peerID) + " and channel " +
                            std::to_string(i) + " was set to 0x" +
                            BaseLib::HelperFunctions::getHexString(parameterData) + ".");
                }
                i++;
            }
            return true;
        }
        catch (const std::exception& ex)
        {
            GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
        }
        return false;
        }
    }
