#include "LoxonePacket.h"
#include "GD.h"
#include "LoxoneCentral.h"

namespace Loxone
{
	const std::list<std::string> LoxonePacket::_responseCommands
	{
		"dev/sys/getPublicKey",
		"jdev/sys/keyexchange/",
		"jdev/sys/getkey2/",
		"jdev/sys/gettoken/",
        "jdev/sys/getjwt/",
        "dev/sys/refreshjwt/",
        "dev/sys/getvisusalt/",
        "authwithtoken/",
		"dev/sps/enablebinstatusupdate",

		"jdev/sys/enc/",
	};

    LoxonePacket::LoxonePacket(std::string& command, bool isSecured)
    {
        _command = command;
        _isSecured = isSecured;
    }
	std::string LoxonePacket::getUuidFromPacket(char* packet)
	{
		std::vector<uint8_t> data;
		data.reserve(26);
		data.insert(data.end(), packet, packet + 16);
		
		std::string uuid;
		{
			std::vector<uint8_t> uuidPart[4];
			uuidPart->push_back(data.at(3));
			uuidPart->push_back(data.at(2));
			uuidPart->push_back(data.at(1));
			uuidPart->push_back(data.at(0));

			uuid += BaseLib::HelperFunctions::getHexString(uuidPart->data(), uuidPart->size());
			uuid += "-";
		}
		{
			std::vector<uint8_t> uuidPart[2];
			uuidPart->push_back(data.at(5));
			uuidPart->push_back(data.at(4));

			uuid += BaseLib::HelperFunctions::getHexString(uuidPart->data(), uuidPart->size());
			uuid += "-";
		}
		{
			std::vector<uint8_t> uuidPart[2];
			uuidPart->push_back(data.at(7));
			uuidPart->push_back(data.at(6));

			uuid += BaseLib::HelperFunctions::getHexString(uuidPart->data(), uuidPart->size());
			uuid += "-";
		}
		{
			std::vector<uint8_t>::const_iterator first = data.begin() + 8;
			std::vector<uint8_t>::const_iterator last = data.begin() + 16;
			std::vector<uint8_t> uuidPart(first, last);

			uuid += BaseLib::HelperFunctions::getHexString(uuidPart.data(), uuidPart.size());
			uuid = BaseLib::HelperFunctions::toLower(uuid);
		}
		return uuid;
	}

	double LoxonePacket::getValueFromPacket(char* packet)
	{
		std::vector<uint8_t> data;
		data.reserve(8);
		data.insert(data.end(), packet, packet + 8);

		unsigned char ptr[]{ data.at(0), data.at(1), data.at(2), data.at(3), data.at(4) , data.at(5) , data.at(6) , data.at(7) };
		return *reinterpret_cast<double*>(ptr);
	}

    uint32_t LoxonePacket::getCodeFromPacket(PVariable& json)
    {
	    //this is a little nasty function, but it is requiered, because Loxone encode the response code in diffrent ways.
        if (json->structValue->find("Code") != json->structValue->end())
        {
            if(json->structValue->at("Code")->type == BaseLib::VariableType::tInteger)
                return json->structValue->at("Code")->integerValue;
            else if(json->structValue->at("Code")->type == BaseLib::VariableType::tString)
                return std::stoi(json->structValue->at("Code")->stringValue);
        }
        else if (json->structValue->find("code") != json->structValue->end())
        {
            if(json->structValue->at("code")->type == BaseLib::VariableType::tInteger)
                return json->structValue->at("code")->integerValue;
            else if(json->structValue->at("code")->type == BaseLib::VariableType::tString)
                return std::stoi(json->structValue->at("code")->stringValue);
        }
        return -1;
    }

	PVariable LoxonePacket::getJson(std::string& jsonString)
	{
		try
		{
		    _jsonDecoder.reset(new BaseLib::Rpc::JsonDecoder(GD::bl));
            return _jsonDecoder->decode(jsonString);
		}
		catch (const BaseLib::Rpc::JsonDecoderException& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, "Error parsing json: " + std::string(ex.what()) + ". Data was: " + jsonString);
		}
		catch (BaseLib::Exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		catch (...)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__);
		}
		return PVariable();
	}

	LoxoneHttpPacket::LoxoneHttpPacket(std::string jsonString, uint32_t responseCode)
	{
        _packetType = LoxonePacketType::LoxoneHttpPacket;

		if (responseCode == 200)// ok
		{
            if (GD::bl->debugLevel >= 6) GD::out.printDebug("httpPacket is " + jsonString);

			PVariable json = getJson(jsonString);
			if (!json) return;
			if(json->structValue->find("LL") != json->structValue->end()) _responseCode = getCodeFromPacket(json->structValue->at("LL"));
            if (_responseCode == 200)
            {
                if(json->structValue->at("LL")->structValue->find("value") != json->structValue->at("LL")->structValue->end())
				    _value = json->structValue->at("LL")->structValue->at("value");
                if(json->structValue->at("LL")->structValue->find("control") != json->structValue->at("LL")->structValue->end())
				    _control = json->structValue->at("LL")->structValue->at("control")->stringValue;
			}
		}
		else if (responseCode == 101)//Websocket Handshake
		{
			_responseCode = 101;
			_control = "Web Socket Protocol Handshake";
		}
		else
		{
			GD::out.printMessage("Received Http Command with Code not 200 and not 101" + jsonString, 0, true);
			//<html>
			//  <head>
			//      <title>error</title>
			//  </head>
			//  <body>
			//      <errorcode>403</errorcode>
			//      <errordetail>
			//          <errortext>Forbidden</errortext>
			//          <description>IP 192.168.41.42 temporarily blocked, too many failed login attempts;</description>
			//          <remaining>25</remaining>
			//      </errordetail>
			//  </body>
			//</html>
			//TODO Handle Responses like 404 and so
		}

        if (GD::bl->debugLevel >= 6) GD::out.printDebug("httpPacket parsed json is: responseCode " + std::to_string(_responseCode) + " control " + _control);
	}
	LoxoneWsPacket::LoxoneWsPacket()
	{
        _packetType = LoxonePacketType::LoxoneWsPacket;
		_responseCode = 0;
	}
	LoxoneWsPacket::LoxoneWsPacket(std::string jsonString)
	{
        _packetType = LoxonePacketType::LoxoneWsPacket;

        if (GD::bl->debugLevel >= 6) GD::out.printDebug("wsPacket is " + jsonString);

	    PVariable json = getJson(jsonString);
		if (!json) return;

        if(json->structValue->find("LL") != json->structValue->end())
        {
            _responseCode = getCodeFromPacket(json->structValue->at("LL"));
            if (_responseCode == 200)
            {
                if (json->structValue->at("LL")->structValue->find("value") != json->structValue->at("LL")->structValue->end())
                    _value = json->structValue->at("LL")->structValue->at("value");
                if (json->structValue->at("LL")->structValue->find("control") != json->structValue->at("LL")->structValue->end())
                {
                    _control = json->structValue->at("LL")->structValue->at("control")->stringValue;
                    for (const std::string &command : _responseCommands) {
                        if (_control.compare(0, command.size(), command) == 0) {
                            _control = command;

                            if (command == "jdev/sys/enc/") {
                                _controlIsEncrypted = true;
                                _control = json->structValue->at("LL")->structValue->at("control")->stringValue;
                            }
                            break;
                        }
                    }
                }
            }
		}
		else
		{
			GD::out.printMessage("LoxoneWsPacket with not LL at the beginning" + jsonString, 0, true);
			//ToDo, make a better solution to detect that this response is a new Structfile
			_responseCode = 200;
			_control = "newStuctfile";
			_value = json;
		}

        if (GD::bl->debugLevel >= 6) GD::out.printDebug("wsPacket parsed json is: responseCode " + std::to_string(_responseCode) + " control " + _control);
	}

	LoxoneTextmessagePacket::LoxoneTextmessagePacket(std::string command)
	{
	    _packetType = LoxonePacketType::LoxoneTextMessage;
		_command = command;
	}

	LoxoneValueStatesPacket::LoxoneValueStatesPacket(char* packet)
	{
		_packetType = LoxonePacketType::LoxoneValueStatesPacket;

		_uuid = getUuidFromPacket(packet);
		_value = getValueFromPacket(packet+16);
	}
	LoxoneTextStatesPacket::LoxoneTextStatesPacket(char* packet, uint32_t len)
	{
		_packetType = LoxonePacketType::LoxoneTextStatesPacket;

		_uuid = getUuidFromPacket(packet);
		_uuidIcon = getUuidFromPacket(packet+16);
		_text = std::string(packet + 36, packet + len);
		/*
		std::stringstream ss;
		for (uint32_t i = 36; i < len; i++)
		{
			ss << packet[i];
		}
		_text = ss.str();
		*/
	}
	LoxoneDaytimerStatesPacket::LoxoneTimeEntry::LoxoneTimeEntry(std::vector<uint8_t> data)
	{
		_mode = data.at(0) | data.at(1) << 8 | data.at(2) << 16 | data.at(3) << 24;
		_from = data.at(4) | data.at(5) << 8 | data.at(6) << 16 | data.at(7) << 24;
		_to = data.at(8) | data.at(9) << 8 | data.at(10) << 16 | data.at(11) << 24;
		_needActivate = data.at(12) | data.at(13) << 8 | data.at(14) << 16 | data.at(15) << 24;
		unsigned char ptr[]{ data.at(16), data.at(17), data.at(18), data.at(19), data.at(20) , data.at(21) , data.at(22) , data.at(23) };
		_value = *reinterpret_cast<double*>(ptr);
	}
	LoxoneDaytimerStatesPacket::LoxoneDaytimerStatesPacket(char* packet, uint32_t nrEntrys)
	{
		_packetType = LoxonePacketType::LoxoneDaytimerStatesPacket;
		_uuid = getUuidFromPacket(packet);
		_devValue = getValueFromPacket(packet + 16);

		for(uint32_t i = 0; i<nrEntrys; i++)
		{
			std::vector<uint8_t> entry;
			entry.reserve(24);
			entry.insert(entry.begin(), packet + 28 + i*24, packet + 28 + 24 + i*24);
            std::shared_ptr<LoxoneTimeEntry> myTimeEntry = std::make_shared<LoxoneTimeEntry>(entry);
			_entrys.insert(std::pair<uint32_t, std::shared_ptr<LoxoneTimeEntry>>(i, myTimeEntry));
		}
	}
	LoxoneWeatherStatesPacket::LoxoneWeatherEntry::LoxoneWeatherEntry(std::vector<uint8_t> data)
	{
		_timestamp = data.at(0) | data.at(1) << 8 | data.at(2) << 16 | data.at(3) << 24;
		_weatherType = data.at(4) | data.at(5) << 8 | data.at(6) << 16 | data.at(7) << 24;
		_windDirection = data.at(8) | data.at(9) << 8 | data.at(10) << 16 | data.at(11) << 24;
		_solarRadiation = data.at(12) | data.at(13) << 8 | data.at(14) << 16 | data.at(15) << 24;
		_relativeHumidity = data.at(16) | data.at(17) << 8 | data.at(18) << 16 | data.at(19) << 24;
		{
			uint32_t offset = 0;
			unsigned char ptr[]{ data.at(20 + offset), data.at(21 + offset), data.at(22 + offset), data.at(23 + offset), data.at(24 + offset) , data.at(25 + offset) , data.at(26 + offset) , data.at(27 + offset) };
			_temperature = *reinterpret_cast<double*>(ptr);
		}
		{
			uint32_t offset = 8;
			unsigned char ptr[]{ data.at(20 + offset), data.at(21 + offset), data.at(22 + offset), data.at(23 + offset), data.at(24 + offset) , data.at(25 + offset) , data.at(26 + offset) , data.at(27 + offset) };
			_perceivedTemperature = *reinterpret_cast<double*>(ptr);
		}
		{
			uint32_t offset = 16;
			unsigned char ptr[]{ data.at(20 + offset), data.at(21 + offset), data.at(22 + offset), data.at(23 + offset), data.at(24 + offset) , data.at(25 + offset) , data.at(26 + offset) , data.at(27 + offset) };
			_dewPoint = *reinterpret_cast<double*>(ptr);
		}
		{
			uint32_t offset = 24;
			unsigned char ptr[]{ data.at(20 + offset), data.at(21 + offset), data.at(22 + offset), data.at(23 + offset), data.at(24 + offset) , data.at(25 + offset) , data.at(26 + offset) , data.at(27 + offset) };
			_precipitation = *reinterpret_cast<double*>(ptr);
		}
		{
			uint32_t offset = 32;
			unsigned char ptr[]{ data.at(20 + offset), data.at(21 + offset), data.at(22 + offset), data.at(23 + offset), data.at(24 + offset) , data.at(25 + offset) , data.at(26 + offset) , data.at(27 + offset) };
			_windSpeed = *reinterpret_cast<double*>(ptr);
		}
		{
			uint32_t offset = 40;
			unsigned char ptr[]{ data.at(20 + offset), data.at(21 + offset), data.at(22 + offset), data.at(23 + offset), data.at(24 + offset) , data.at(25 + offset) , data.at(26 + offset) , data.at(27 + offset) };
			_barometicPressure = *reinterpret_cast<double*>(ptr);
		}
	}
	LoxoneWeatherStatesPacket::LoxoneWeatherStatesPacket(char* packet, uint32_t nrEntrys)
	{
		_packetType = LoxonePacketType::LoxoneWeatherStatesPacket;
		_uuid = getUuidFromPacket(packet);
		{
			std::vector<uint8_t> value;
			value.reserve(4);
			value.insert(value.begin(), packet + 16, packet + 20);
			_lastUpdate = value.at(0) | value.at(1) << 8 | value.at(2) << 16 | value.at(3) << 24;
		}
		for(uint32_t i = 0; i<nrEntrys; i++)
		{
			std::vector<uint8_t> entry;
			entry.reserve(68);
			entry.insert(entry.begin(), packet + 24 + i*68, packet + 24 + 24 + i*68);
			_entrys.insert(std::pair<uint32_t, LoxoneWeatherEntry>(i, LoxoneWeatherEntry(entry)));
		}
	}
}

