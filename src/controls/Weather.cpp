#include "Weather.h"

namespace Loxone
{
    Weather::Weather(LoxoneLoxApp3::weather weather)
    {
        _name = "Weather";
        _typeString = "Weather";
        _uuidAction = weather.getUuidActual();
        _defaultRating = 0;
        _isSecured = false;
        _isFavorite = false;
        _room = "noRoom";
        _cat = "noCat";

        _RpcEncoder = std::make_shared<BaseLib::Rpc::RpcEncoder>();

        _type = 0x50;
        //_control = control;
        _uuidVariableMap.emplace(weather.getUuidActual(), "weather.uuidActual");
        _uuidVariableMap.emplace(weather.getUuidForecast(), "weather.uuidForecast");
        {
            auto format = weather.getFormat();
            _detailsMap.emplace("format.barometricPressure", std::make_shared<Variable>(format.barometricPressure));
            _detailsMap.emplace("format.precipitation", std::make_shared<Variable>(format.precipitation));
            _detailsMap.emplace("format.relativeHumidity", std::make_shared<Variable>(format.relativeHumidity));
            _detailsMap.emplace("format.temperature", std::make_shared<Variable>(format.temperature));
            _detailsMap.emplace("format.windSpeed", std::make_shared<Variable>(format.windSpeed));
        }
        {
            auto variable = std::make_shared<Variable>(BaseLib::VariableType::tStruct);
            auto weatherTypeTexts = weather.getWeatherTypeTexts();
            for (auto const& weatherTypeText : weatherTypeTexts){
                variable->structValue->emplace(std::to_string(weatherTypeText.first), std::make_shared<Variable>(weatherTypeText.second));
            }
            _detailsMap.emplace("weatherTypeText", variable);
        }
        {
            auto variable = std::make_shared<Variable>(BaseLib::VariableType::tStruct);
            auto weatherFieldTypes = weather.getWeatherFieldTypes();
            for (auto const& weatherFieldType : weatherFieldTypes){
                auto myVariable = std::make_shared<Variable>(BaseLib::VariableType::tStruct);
                myVariable->structValue->emplace("id", std::make_shared<Variable>(weatherFieldType.second.id));
                myVariable->structValue->emplace("analog", std::make_shared<Variable>(weatherFieldType.second.analog));
                myVariable->structValue->emplace("unit", std::make_shared<Variable>(weatherFieldType.second.unit));
                myVariable->structValue->emplace("format", std::make_shared<Variable>(weatherFieldType.second.format));
                variable->structValue->emplace(weatherFieldType.second.name, myVariable);
            }
            _detailsMap.emplace("weatherFieldType", variable);
        }
    }

    Weather::Weather(std::shared_ptr<BaseLib::Database::DataTable> rows) : LoxoneControl(rows, 0x50)
    {
        //getBinaryValueFromDataTable(109, _defaultIcon);
    }

    bool Daytimer::packetReceived(uint32_t peerID, PLoxoneDaytimerStatesPacket loxonePacket, std::unordered_map<uint32_t, std::unordered_map<std::string, Systems::RpcConfigurationParameter>> &valuesCentral)
    {
        try {
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
                if(valuesCentral[i].find("TIMESTAMP") == valuesCentral[i].end()) continue;
                {
                    BaseLib::Systems::RpcConfigurationParameter &parameter = valuesCentral[i]["TIMESTAMP"];
                    std::vector<uint8_t> parameterData;
                    PVariable value = std::make_shared<Variable>(entry->second->_timestamp);
                    parameter.rpcParameter->convertToPacket(value, parameter.mainRole(), parameterData);

                    if (!parameter.equals(parameterData)) parameter.setBinaryData(parameterData);

                    //if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
                    //else saveParameter(0, ParameterGroup::Type::Enum::config, i->channel, i->variable, parameterData);

                    GD::out.printInfo(
                            "Info: Parameter TIMESTAMP of peer " + std::to_string(peerID) + " and channel " +
                            std::to_string(i) + " was set to 0x" +
                            BaseLib::HelperFunctions::getHexString(parameterData) + ".");
                }
                if(valuesCentral[i].find("WEATHER_TYPE") == valuesCentral[i].end()) continue;
                {
                    BaseLib::Systems::RpcConfigurationParameter &parameter = valuesCentral[i]["WEATHER_TYPE"];
                    std::vector<uint8_t> parameterData;
                    PVariable value = std::make_shared<Variable>(entry->second->_weatherType);
                    parameter.rpcParameter->convertToPacket(value, parameter.mainRole(), parameterData);

                    if (!parameter.equals(parameterData)) parameter.setBinaryData(parameterData);

                    //if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
                    //else saveParameter(0, ParameterGroup::Type::Enum::config, i->channel, i->variable, parameterData);

                    GD::out.printInfo(
                            "Info: Parameter WEATHER_TYPE of peer " + std::to_string(peerID) + " and channel " +
                            std::to_string(i) + " was set to 0x" +
                            BaseLib::HelperFunctions::getHexString(parameterData) + ".");
                }
                if(valuesCentral[i].find("WIND_DIRECTION") == valuesCentral[i].end()) continue;
                {
                    BaseLib::Systems::RpcConfigurationParameter &parameter = valuesCentral[i]["WIND_DIRECTION"];
                    std::vector<uint8_t> parameterData;
                    PVariable value = std::make_shared<Variable>(entry->second->_windDirection);
                    parameter.rpcParameter->convertToPacket(value, parameter.mainRole(), parameterData);

                    if (!parameter.equals(parameterData)) parameter.setBinaryData(parameterData);

                    //if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
                    //else saveParameter(0, ParameterGroup::Type::Enum::config, i->channel, i->variable, parameterData);

                    GD::out.printInfo(
                            "Info: Parameter WIND_DIRECTION of peer " + std::to_string(peerID) + " and channel " +
                            std::to_string(i) + " was set to 0x" +
                            BaseLib::HelperFunctions::getHexString(parameterData) + ".");
                }
                if(valuesCentral[i].find("SOLAR_RADIATION") == valuesCentral[i].end()) continue;
                {
                    BaseLib::Systems::RpcConfigurationParameter &parameter = valuesCentral[i]["SOLAR_RADIATION"];
                    std::vector<uint8_t> parameterData;
                    PVariable value = std::make_shared<Variable>(entry->second->_solarRadiation);
                    parameter.rpcParameter->convertToPacket(value, parameter.mainRole(), parameterData);

                    if (!parameter.equals(parameterData)) parameter.setBinaryData(parameterData);

                    //if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
                    //else saveParameter(0, ParameterGroup::Type::Enum::config, i->channel, i->variable, parameterData);

                    GD::out.printInfo(
                            "Info: Parameter SOLAR_RADIATION of peer " + std::to_string(peerID) + " and channel " +
                            std::to_string(i) + " was set to 0x" +
                            BaseLib::HelperFunctions::getHexString(parameterData) + ".");
                }
                if(valuesCentral[i].find("RELATIVE_HUMIDITY") == valuesCentral[i].end()) continue;
                {
                    BaseLib::Systems::RpcConfigurationParameter &parameter = valuesCentral[i]["RELATIVE_HUMIDITY"];
                    std::vector<uint8_t> parameterData;
                    PVariable value = std::make_shared<Variable>(entry->second->_relativeHumidity);
                    parameter.rpcParameter->convertToPacket(value, parameter.mainRole(), parameterData);

                    if (!parameter.equals(parameterData)) parameter.setBinaryData(parameterData);

                    //if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
                    //else saveParameter(0, ParameterGroup::Type::Enum::config, i->channel, i->variable, parameterData);

                    GD::out.printInfo(
                            "Info: Parameter RELATIVE_HUMIDITY of peer " + std::to_string(peerID) + " and channel " +
                            std::to_string(i) + " was set to 0x" +
                            BaseLib::HelperFunctions::getHexString(parameterData) + ".");
                }
                if(valuesCentral[i].find("TEMPERATURE") == valuesCentral[i].end()) continue;
                {
                    BaseLib::Systems::RpcConfigurationParameter &parameter = valuesCentral[i]["TEMPERATURE"];
                    std::vector<uint8_t> parameterData;
                    PVariable value = std::make_shared<Variable>(entry->second->_temperature);
                    parameter.rpcParameter->convertToPacket(value, parameter.mainRole(), parameterData);

                    if (!parameter.equals(parameterData)) parameter.setBinaryData(parameterData);

                    //if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
                    //else saveParameter(0, ParameterGroup::Type::Enum::config, i->channel, i->variable, parameterData);

                    GD::out.printInfo(
                            "Info: Parameter TEMPERATURE of peer " + std::to_string(peerID) + " and channel " +
                            std::to_string(i) + " was set to 0x" +
                            BaseLib::HelperFunctions::getHexString(parameterData) + ".");
                }
                if(valuesCentral[i].find("PERCEIVED_TEMPERATURE") == valuesCentral[i].end()) continue;
                {
                    BaseLib::Systems::RpcConfigurationParameter &parameter = valuesCentral[i]["PERCEIVED_TEMPERATURE"];
                    std::vector<uint8_t> parameterData;
                    PVariable value = std::make_shared<Variable>(entry->second->_perceivedTemperature);
                    parameter.rpcParameter->convertToPacket(value, parameter.mainRole(), parameterData);

                    if (!parameter.equals(parameterData)) parameter.setBinaryData(parameterData);

                    //if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
                    //else saveParameter(0, ParameterGroup::Type::Enum::config, i->channel, i->variable, parameterData);

                    GD::out.printInfo(
                            "Info: Parameter PERCEIVED_TEMPERATURE of peer " + std::to_string(peerID) + " and channel " +
                            std::to_string(i) + " was set to 0x" +
                            BaseLib::HelperFunctions::getHexString(parameterData) + ".");
                }
                if(valuesCentral[i].find("DEW_POINT") == valuesCentral[i].end()) continue;
                {
                    BaseLib::Systems::RpcConfigurationParameter &parameter = valuesCentral[i]["DEW_POINT"];
                    std::vector<uint8_t> parameterData;
                    PVariable value = std::make_shared<Variable>(entry->second->_dewPoint);
                    parameter.rpcParameter->convertToPacket(value, parameter.mainRole(), parameterData);

                    if (!parameter.equals(parameterData)) parameter.setBinaryData(parameterData);

                    //if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
                    //else saveParameter(0, ParameterGroup::Type::Enum::config, i->channel, i->variable, parameterData);

                    GD::out.printInfo(
                            "Info: Parameter DEW_POINT of peer " + std::to_string(peerID) + " and channel " +
                            std::to_string(i) + " was set to 0x" +
                            BaseLib::HelperFunctions::getHexString(parameterData) + ".");
                }
                if(valuesCentral[i].find("PRECIPITATION") == valuesCentral[i].end()) continue;
                {
                    BaseLib::Systems::RpcConfigurationParameter &parameter = valuesCentral[i]["PRECIPITATION"];
                    std::vector<uint8_t> parameterData;
                    PVariable value = std::make_shared<Variable>(entry->second->_precipitation);
                    parameter.rpcParameter->convertToPacket(value, parameter.mainRole(), parameterData);

                    if (!parameter.equals(parameterData)) parameter.setBinaryData(parameterData);

                    //if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
                    //else saveParameter(0, ParameterGroup::Type::Enum::config, i->channel, i->variable, parameterData);

                    GD::out.printInfo(
                            "Info: Parameter PRECIPITATION of peer " + std::to_string(peerID) + " and channel " +
                            std::to_string(i) + " was set to 0x" +
                            BaseLib::HelperFunctions::getHexString(parameterData) + ".");
                }
                if(valuesCentral[i].find("WIND_SPEED") == valuesCentral[i].end()) continue;
                {
                    BaseLib::Systems::RpcConfigurationParameter &parameter = valuesCentral[i]["WIND_SPEED"];
                    std::vector<uint8_t> parameterData;
                    PVariable value = std::make_shared<Variable>(entry->second->_windSpeed);
                    parameter.rpcParameter->convertToPacket(value, parameter.mainRole(), parameterData);

                    if (!parameter.equals(parameterData)) parameter.setBinaryData(parameterData);

                    //if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
                    //else saveParameter(0, ParameterGroup::Type::Enum::config, i->channel, i->variable, parameterData);

                    GD::out.printInfo(
                            "Info: Parameter WIND_SPEED of peer " + std::to_string(peerID) + " and channel " +
                            std::to_string(i) + " was set to 0x" +
                            BaseLib::HelperFunctions::getHexString(parameterData) + ".");
                }
                if(valuesCentral[i].find("BAROMETIC_PRESSURE") == valuesCentral[i].end()) continue;
                {
                    BaseLib::Systems::RpcConfigurationParameter &parameter = valuesCentral[i]["BAROMETIC_PRESSURE"];
                    std::vector<uint8_t> parameterData;
                    PVariable value = std::make_shared<Variable>(entry->second->_barometicPressure);
                    parameter.rpcParameter->convertToPacket(value, parameter.mainRole(), parameterData);

                    if (!parameter.equals(parameterData)) parameter.setBinaryData(parameterData);

                    //if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
                    //else saveParameter(0, ParameterGroup::Type::Enum::config, i->channel, i->variable, parameterData);

                    GD::out.printInfo(
                            "Info: Parameter BAROMETIC_PRESSURE of peer " + std::to_string(peerID) + " and channel " +
                            std::to_string(i) + " was set to 0x" +
                            BaseLib::HelperFunctions::getHexString(parameterData) + ".");
                }
            }
        }
        catch (const std::exception& ex){
            GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
        }
        return false;
    }
}