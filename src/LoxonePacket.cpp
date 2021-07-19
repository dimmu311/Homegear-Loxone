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

		"close",
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
	    //this is a little nasty function, but it is required, because Loxone encode the response code in different ways.
        if (json->structValue->find("Code") != json->structValue->end()){
            if(json->structValue->at("Code")->type == BaseLib::VariableType::tInteger)
                return json->structValue->at("Code")->integerValue;
            else if(json->structValue->at("Code")->type == BaseLib::VariableType::tString)
                return std::stoi(json->structValue->at("Code")->stringValue);
        }
        else if (json->structValue->find("code") != json->structValue->end()){
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

	LoxoneHttpPacket::LoxoneHttpPacket(BaseLib::Http& http)
    {
        _packetType = LoxonePacketType::LoxoneHttpPacket;
        if(http.getHeader().responseCode == 200) { //ok
            GD::out.printDebug("Http Packet is :" + std::string(http.getContent().begin(), http.getContent().end()), 6);
            if(http.getHeader().contentType == "application/json"){
                PVariable json = BaseLib::Rpc::JsonDecoder::decode(http.getContent());
                if (!json) return;
                if(json->structValue->find("LL") == json->structValue->end()) return;
                _responseCode = getCodeFromPacket(json->structValue->at("LL"));
                if (_responseCode == 200)
                {
                    if(json->structValue->at("LL")->structValue->find("value") != json->structValue->at("LL")->structValue->end())
                        _value = json->structValue->at("LL")->structValue->at("value");
                    if(json->structValue->at("LL")->structValue->find("control") != json->structValue->at("LL")->structValue->end())
                        _control = json->structValue->at("LL")->structValue->at("control")->stringValue;
                }
            }
            //todo: maybe there are some more http packets with code 200 that are not handled because they are no json
        }
        else if(http.getHeader().responseCode == 101) { //Websocket Handshake
            _responseCode = 101;
            _control = "Web Socket Protocol Handshake";
        }
        else{
            GD::out.printDebug("Received Http Packet with Code not 200 and not 101", 6);
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
    }
	LoxoneWsPacket::LoxoneWsPacket()
	{
        _packetType = LoxonePacketType::LoxoneWsPacket;
		_responseCode = 0;
	}
	LoxoneWsPacket::LoxoneWsPacket(BaseLib::WebSocket& webSocket)
	{
        _packetType = LoxonePacketType::LoxoneWsPacket;
        GD::out.printDebug("Ws Packet is: " + std::string(webSocket.getContent().begin(), webSocket.getContent().end()), 6);
        if(webSocket.getHeader().opcode == WebSocket::Header::Opcode::close){
            _responseCode = 200;
            _control = "close";
            return;
        }
        else if(webSocket.getHeader().opcode == WebSocket::Header::Opcode::text || webSocket.getHeader().opcode == WebSocket::Header::Opcode::binary){
        PVariable json = BaseLib::Rpc::JsonDecoder::decode(webSocket.getContent());
            if (!json) return;
            if(json->structValue->find("LL") != json->structValue->end()) {
                _responseCode = getCodeFromPacket(json->structValue->at("LL"));
                if (json->structValue->at("LL")->structValue->find("value") != json->structValue->at("LL")->structValue->end()) _value = json->structValue->at("LL")->structValue->at("value");
                if (json->structValue->at("LL")->structValue->find("control") != json->structValue->at("LL")->structValue->end()) {
                    _control = json->structValue->at("LL")->structValue->at("control")->stringValue;
                    if(_control.compare(0, std::string("jdev/sys/enc/").size(), "jdev/sys/enc/") == 0){
                        _controlIsEncrypted = true;
                        return;
                    }
                    for (const std::string &command : _responseCommands) {
                        if (_control.compare(0, command.size(), command) == 0) {
                            _control = command;
                            return;
                        }
                    }
                }
            }
            else{
                GD::out.printDebug("LoxoneWsPacket with not LL at the beginning", 6);
                //This is kind of a workaround.
                //so the Problem is that a structfile did not have a fild to cleary identify a structfile
                //because of this we look for a lot of fields that are only in a structfile transmitted
                //maybe in future versions this could result in a problem.....
                if(json->structValue->find("lastModified") != json->structValue->end() && json->structValue->find("msInfo") != json->structValue->end() && json->structValue->find("globalStates") != json->structValue->end() && json->structValue->find("operatingModes") != json->structValue->end()) {
                    GD::out.printDebug("LoxoneWsPacket has fields that are only in strucfile. This packet should be a structfile", 4);
                    _responseCode = 200;
                    _control = "newStuctfile";
                    _value = json;
                }
            }
        }
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

		_rawPacketStruct = std::make_shared<Variable>(VariableType::tStruct);
        _rawPacketStruct->structValue->operator[]("packetType") = PVariable(new Variable("Value States Packet"));
        _rawPacketStruct->structValue->operator[]("uuid") = PVariable(new Variable(_uuid));
        _rawPacketStruct->structValue->operator[]("value") = PVariable(new Variable(_value));
	}
	LoxoneTextStatesPacket::LoxoneTextStatesPacket(char* packet, uint32_t len)
	{
		_packetType = LoxonePacketType::LoxoneTextStatesPacket;
		_uuid = getUuidFromPacket(packet);
		_uuidIcon = getUuidFromPacket(packet+16);
		_text = std::string(packet + 36, packet + len);

        _rawPacketStruct = std::make_shared<Variable>(VariableType::tStruct);
        _rawPacketStruct->structValue->operator[]("packetType") = PVariable(new Variable("Text States Packet"));
        _rawPacketStruct->structValue->operator[]("uuid") = PVariable(new Variable(_uuid));
        _rawPacketStruct->structValue->operator[]("uuidIcon") = PVariable(new Variable(_uuidIcon));
        _rawPacketStruct->structValue->operator[]("text") = PVariable(new Variable(_text));
	}
	LoxoneDaytimerStatesPacket::LoxoneTimeEntry::LoxoneTimeEntry(std::vector<uint8_t> data)
	{
		_mode = data.at(0) | data.at(1) << 8 | data.at(2) << 16 | data.at(3) << 24;
		_from = data.at(4) | data.at(5) << 8 | data.at(6) << 16 | data.at(7) << 24;
		_to = data.at(8) | data.at(9) << 8 | data.at(10) << 16 | data.at(11) << 24;
		_needActivate = data.at(12) | data.at(13) << 8 | data.at(14) << 16 | data.at(15) << 24;
		unsigned char ptr[]{ data.at(16), data.at(17), data.at(18), data.at(19), data.at(20) , data.at(21) , data.at(22) , data.at(23) };
		_value = *reinterpret_cast<double*>(ptr);

        _rawPacketStruct = std::make_shared<Variable>(VariableType::tStruct);
        _rawPacketStruct->structValue->operator[]("mode") = PVariable(new Variable(_mode));
        _rawPacketStruct->structValue->operator[]("from") = PVariable(new Variable(_from));
        _rawPacketStruct->structValue->operator[]("to") = PVariable(new Variable(_to));
        _rawPacketStruct->structValue->operator[]("needActivate") = PVariable(new Variable(_needActivate));
        _rawPacketStruct->structValue->operator[]("value") = PVariable(new Variable(_value));
	}
	LoxoneDaytimerStatesPacket::LoxoneDaytimerStatesPacket(char* packet, uint32_t nrEntrys)
	{
		_packetType = LoxonePacketType::LoxoneDaytimerStatesPacket;
		_uuid = getUuidFromPacket(packet);
		_devValue = getValueFromPacket(packet + 16);

        _rawPacketStruct = std::make_shared<Variable>(VariableType::tStruct);
        _rawPacketStruct->structValue->operator[]("packetType") = PVariable(new Variable("Daytimer States Packet"));
        _rawPacketStruct->structValue->operator[]("uuid") = PVariable(new Variable(_uuid));
        _rawPacketStruct->structValue->operator[]("defaultValue") = PVariable(new Variable(_devValue));
        _rawPacketStruct->structValue->operator[]("entrys") = PVariable(new Variable(VariableType::tArray));

		for(uint32_t i = 0; i<nrEntrys; i++)
		{
			std::vector<uint8_t> entry;
			entry.reserve(24);
			entry.insert(entry.begin(), packet + 28 + i*24, packet + 28 + 24 + i*24);
            std::shared_ptr<LoxoneTimeEntry> myTimeEntry = std::make_shared<LoxoneTimeEntry>(entry);
			_entrys.insert(std::pair<uint32_t, std::shared_ptr<LoxoneTimeEntry>>(i, myTimeEntry));
			_rawPacketStruct->structValue->at("entrys")->arrayValue->push_back(myTimeEntry->_rawPacketStruct);
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

        _rawPacketStruct = std::make_shared<Variable>(VariableType::tStruct);
        _rawPacketStruct->structValue->operator[]("timestamp") = PVariable(new Variable(_timestamp));
        _rawPacketStruct->structValue->operator[]("weatherType") = PVariable(new Variable(_weatherType));
        _rawPacketStruct->structValue->operator[]("windDirection") = PVariable(new Variable(_windDirection));
        _rawPacketStruct->structValue->operator[]("solarRadiation") = PVariable(new Variable(_solarRadiation));
        _rawPacketStruct->structValue->operator[]("relativeHumidity") = PVariable(new Variable(_relativeHumidity));
        _rawPacketStruct->structValue->operator[]("temperature") = PVariable(new Variable(_temperature));
        _rawPacketStruct->structValue->operator[]("perceivedTemperature") = PVariable(new Variable(_perceivedTemperature));
        _rawPacketStruct->structValue->operator[]("dewPoint") = PVariable(new Variable(_dewPoint));
        _rawPacketStruct->structValue->operator[]("precipitation") = PVariable(new Variable(_precipitation));
        _rawPacketStruct->structValue->operator[]("windSpeed") = PVariable(new Variable(_windSpeed));
        _rawPacketStruct->structValue->operator[]("barometicPressure") = PVariable(new Variable(_barometicPressure));
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

        _rawPacketStruct = std::make_shared<Variable>(VariableType::tStruct);
        _rawPacketStruct->structValue->operator[]("packetType") = PVariable(new Variable("Weather States Packet"));
        _rawPacketStruct->structValue->operator[]("uuid") = PVariable(new Variable(_uuid));
        _rawPacketStruct->structValue->operator[]("lastUpdate") = PVariable(new Variable(_lastUpdate));
        _rawPacketStruct->structValue->operator[]("entrys") = PVariable(new Variable(VariableType::tArray));

		for(uint32_t i = 0; i<nrEntrys; i++)
		{
			std::vector<uint8_t> entry;
			entry.reserve(68);
			entry.insert(entry.begin(), packet + 24 + i*68, packet + 24 + 24 + i*68);

			std::shared_ptr<LoxoneWeatherEntry> myWeatherEntry = std::make_shared<LoxoneWeatherEntry>(entry);
            _entrys.insert(std::pair<uint32_t, std::shared_ptr<LoxoneWeatherEntry>>(i, myWeatherEntry));
            _rawPacketStruct->structValue->at("entrys")->arrayValue->push_back(myWeatherEntry->_rawPacketStruct);
		}
	}
}

