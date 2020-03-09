#include "LoxonePacket.h"
#include "GD.h"
#include "homegear-base/Encoding/JsonDecoder.h"
#include "homegear-base/Encoding/JsonEncoder.h"
#include "LoxoneCentral.h"

namespace Loxone
{
	const std::list<std::string> LoxonePacket::_responseCommands
	{
		"dev/sys/getPublicKey",
		"jdev/sys/keyexchange/",
		"jdev/sys/getkey2/",
		"jdev/sys/gettoken/",
		"dev/sps/enablebinstatusupdate",

		"jdev/sys/enc/",
	};

	const std::unordered_map<std::string, LoxoneHttpCommand> LoxonePacket::_commands =
	{
		{"getPublicKey", LoxoneHttpCommand{
			"GET /jdev/sys/getPublicKey / HTTP / 1.1\r\n"
			"Host: homegear\r\n"
			"User-Agent: homegear\r\n"
			"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
			"Accept-Language: en-us,en;q=0.5\r\n"
			"Accept-Encoding: gzip,deflate\r\n"
			"Accept-Charset: ISO-8859-1,utf-8;\r\n"
			"Keep-Alive: 1000\r\n"
			"Connection: keep-alive\r\n"
			"\r\n"
			,
			"dev/sys/getPublicKey"
			}
		},
		{"openWebsocket", LoxoneHttpCommand{
			"GET /ws/rfc6455/ HTTP/1.1\r\n"
			"Host: miniserver\r\n"
			"Upgrade: websocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
			"Sec-WebSocket-Protocol: remotecontrol\r\n"
			"\r\n"
			,
			"Web Socket Protocol Handshake"
			}
		},
		{"keyexchange", LoxoneHttpCommand{
			"jdev/sys/keyexchange/"
			,
			"jdev/sys/keyexchange/" 
			}
		},
		{"getkey2", LoxoneHttpCommand{
			"jdev/sys/getkey2/"
			,
			"jdev/sys/getkey2/"
			}
		},
        {"getkey", LoxoneHttpCommand{
            "jdev/sys/getkey/"
            ,
            "jdev/sys/enc/"
            }
        },
		{"getToken", LoxoneHttpCommand{
			"jdev/sys/gettoken/"
			,
			"jdev/sys/gettoken/"
			}
		},
		{"enableUpdates", LoxoneHttpCommand{
			"jdev/sps/enablebinstatusupdate"
			,
			"dev/sps/enablebinstatusupdate"
			}
		},
		{"keepalive", LoxoneHttpCommand{
			"keepalive"
			,
			"keepalive"
			}
		},
		{"newStuctfile", LoxoneHttpCommand{
			"data/LoxAPP3.json"
			,
			"newStuctfile"
			}
		},
		{"LoxApp3Version", LoxoneHttpCommand{
			"jdev/sps/LoxAPPversion3"
			,
			"dev/sps/LoxAPPversion3"
			}
		},
        {"refreshToken", LoxoneHttpCommand{
            "jdev/sys/refreshtoken/",
            "dev/sps/LoxAPPversion3",
            }
        },
	};

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

	PVariable LoxonePacket::getJson(std::string& jsonString)
	{
		try
		{
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
	/*
	LoxoneCommand LoxonePacket::getResponseCommand()
	{
		auto iterator = _requestResponseMapping.find(_command);
		if (iterator != _requestResponseMapping.end()) return iterator->second;
		return LoxoneCommand;
	}
	*/
	LoxoneHttpPacket::LoxoneHttpPacket(std::string jsonString, uint32_t responseCode)
	{
		PVariable json = getJson(jsonString);
		if (!json) return;

		if (responseCode == 200)// ok
		{
			PVariable json = getJson(jsonString);
			if (!json) return;

			uint32_t code = 0;
			if (json->structValue->at("LL")->structValue->find("Code") != json->structValue->at("LL")->structValue->end())
			{
				code = json->structValue->at("LL")->structValue->at("Code")->integerValue;
				if (code == 0) code = std::stoi(json->structValue->at("LL")->structValue->at("Code")->stringValue);
			}
			else if (json->structValue->at("LL")->structValue->find("code") != json->structValue->at("LL")->structValue->end())
			{
				code = json->structValue->at("LL")->structValue->at("code")->integerValue;
				if (code == 0)code = std::stoi(json->structValue->at("LL")->structValue->at("code")->stringValue);
			}

			if (code == 200)
			{
				_responseCode = 200;
				_value = json->structValue->at("LL")->structValue->at("value");
				_control = json->structValue->at("LL")->structValue->at("control")->stringValue;
			}
		}
		else if (responseCode == 101)//Websocket Handshake
		{
			_responseCode = 101;
			//_value = "wird das hier gebraucht?";
			_control = "Web Socket Protocol Handshake";
		}
		else
		{
			GD::out.printMessage("Received Http Command with Code not 200 and not 101" + jsonString, 0, true);
			//TODO Handle Responses like 404 and so
		}
	}
	LoxoneWsPacket::LoxoneWsPacket()
	{
		_responseCode = 0;
	}
	LoxoneWsPacket::LoxoneWsPacket(std::string jsonString)
	{
		PVariable json = getJson(jsonString);
		if (!json) return;

		uint32_t code = 0;

		if (json->structValue->find("LL") != json->structValue->end())
		{
			if (json->structValue->at("LL")->structValue->find("Code") != json->structValue->at("LL")->structValue->end())
			{
				code = json->structValue->at("LL")->structValue->at("Code")->integerValue;
				if (code == 0) code = std::stoi(json->structValue->at("LL")->structValue->at("Code")->stringValue);
			}
			else if (json->structValue->at("LL")->structValue->find("code") != json->structValue->at("LL")->structValue->end())
			{
				code = json->structValue->at("LL")->structValue->at("code")->integerValue;
				if (code == 0)code = std::stoi(json->structValue->at("LL")->structValue->at("code")->stringValue);
			}

			if (code == 200)
			{
				_responseCode = 200;
				_control = json->structValue->at("LL")->structValue->at("control")->stringValue;
				_value = json->structValue->at("LL")->structValue->at("value");

				for (const std::string& command : _responseCommands)
				{
					if (_control.compare(0, command.size(), command) == 0)
					{
						_control = command;
						break;
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
	}

	LoxoneTextmessagePacket::LoxoneTextmessagePacket(std::string jsonString)
	{
		PVariable json = getJson(jsonString);
		if (!json) return;
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

		std::stringstream ss;

		for (uint32_t i = 36; i < len; i++)
		{
			ss << packet[i];
		}
		_text = ss.str();
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
			_entrys.insert(std::pair<uint32_t, LoxoneTimeEntry>(i, LoxoneTimeEntry(entry)));
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

