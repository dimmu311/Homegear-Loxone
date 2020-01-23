#include <utility>

/* Copyright 2013-2019 Homegear GmbH
 *
 * Homegear is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Homegear is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Homegear.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

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
			,
			""
			,
			commandType::http,
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
			,
			""
			,
			commandType::http,
			}
		},
		{"keyexchange", LoxoneHttpCommand{
			"jdev/sys/keyexchange/"
			,
			"jdev/sys/keyexchange/" 
			,
			""
			,
			commandType::ws,
			}
		},
		{"getkey2", LoxoneHttpCommand{
			"jdev/sys/getkey2/"
			,
			"jdev/sys/getkey2/"
			,
			""
			,
			commandType::ws,
			}
		},
		{"getToken", LoxoneHttpCommand{
			"jdev/sys/gettoken/"
			,
			"jdev/sys/gettoken/"
			,
			""
			,
			commandType::ws,
			}
		},
		{"enableUpdates", LoxoneHttpCommand{
			"jdev/sps/enablebinstatusupdate"
			,
			"dev/sps/enablebinstatusupdate"
			,
			""
			,
			commandType::ws,
			}
		},
		{"keepalive", LoxoneHttpCommand{
			"keepalive"
			,
			"keepalive"
			,
			""
			,
			commandType::ws,
			}
		},
		{"newStuctfile", LoxoneHttpCommand{
			"data/LoxAPP3.json"
			,
			"newStuctfile"
			,
			""
			,
			commandType::ws,
			}
		},
		{"LoxApp3Version", LoxoneHttpCommand{
			"jdev/sps/LoxAPPversion3"
			,
			"dev/sps/LoxAPPversion3"
			,
			""
			,
			commandType::ws,
			}
		},
	};

	std::string LoxonePacket::getUuidFromPacket(char* packet)
	{
		std::vector<char> data;
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
			std::vector<char>::const_iterator first = data.begin() + 8;
			std::vector<char>::const_iterator last = data.begin() + 16;
			std::vector<uint8_t> uuidPart(first, last);

			uuid += BaseLib::HelperFunctions::getHexString(uuidPart.data(), uuidPart.size());
			uuid = BaseLib::HelperFunctions::toLower(uuid);
		}
		return uuid;
	}

	double LoxonePacket::getValueFromPacket(char* packet)
	{
		std::vector<char> data;
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
			//ToDO, zähle die Zeilen solange bis 2 Leerzeilen kommen. Die Zeile danach ist dann der Json String.
			//Das ist dann sicherer wie das Zeigen auf Zeile 7. Wer weiß wann Loxone den Header umbaut.
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
			GD::out.printMessage("Received Http Command with Code not 200 and not 101" + jsonString, 0, true);//from TR
			//TODO Handle Responses wike 404 and so
		}
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
			GD::out.printMessage("LoxoneWsPacket with not LL at the beginning" + jsonString, 0, true);//from TR
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
}

