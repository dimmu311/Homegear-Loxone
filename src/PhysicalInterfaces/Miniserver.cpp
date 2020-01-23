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



#include "Miniserver.h"
#include "../Loxone.h"
#include "../GD.h"
#include "../LoxoneEncryption.h"


namespace Loxone
{

Miniserver::Miniserver(std::shared_ptr<BaseLib::Systems::PhysicalInterfaceSettings> settings) : IPhysicalInterface(GD::bl, GD::family->getFamily(), settings)
{
    _out.init(GD::bl);
    _out.setPrefix(GD::out.getPrefix() + "Loxone Miniserver \"" + settings->id + "\": ");

    signal(SIGPIPE, SIG_IGN);

    _stopped = true;

    if(!settings)
    {
        _out.printCritical("Critical: Error initializing. Settings pointer is empty.");
        return;
    }
    _hostname = settings->host;
    _port = BaseLib::Math::getNumber(settings->port);
    if(_port < 1 || _port > 65535) _port = 80;

	_user = settings->user;
	_password = settings->password;
}

Miniserver::~Miniserver()
{
    stopListening();
    _bl->threadManager.join(_initThread);
	_bl->threadManager.join(_keepAliveThread);
}

uint16_t Miniserver::getMessageCounter()
{
    return _messageCounter++;
}

void Miniserver::sendPacket(std::shared_ptr<BaseLib::Systems::Packet> packet)
{
	try
    {
        PLoxonePacket loxonePacket(std::dynamic_pointer_cast<LoxonePacket>(packet));
        if(!loxonePacket) return;
		
		std::string command = "jdev/sys/enc/" + _loxoneEncryption.encryptCommand(loxonePacket->getCommand());
		try
		{
            if(GD::bl->debugLevel >= 5) GD::out.printInfo("Info: Sending packet " + command);

			std::vector<char> output;
			std::vector<char> input(command.begin(), command.end());
			BaseLib::WebSocket::encode(input, BaseLib::WebSocket::Header::Opcode::Enum::text, output);

			std::string sendCommand(output.begin(), output.end());
			_tcpSocket->proofwrite(sendCommand);

			_lastPacketSent = BaseLib::HelperFunctions::getTime();
		}
		catch (const BaseLib::SocketOperationException & ex)
		{
			_out.printError("Error sending packet: " + std::string(ex.what()));
		}
    }
    catch(const std::exception& ex)
    {
        _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
}

void Miniserver::startListening()
{
    try
    {
        stopListening();

        if(_hostname.empty())
        {
            _out.printError("Error: Configuration of Loxone is incomplete (hostname is missing). Please correct it in \"loxone.conf\".");
            return;
        }

		if (_settings->user.empty())
		{
			_out.printError("Error: Configuration of Loxone is incomplete (user is missing). Please correct it in \"loxone.conf\".");
			return;
		}

        if(_settings->password.empty())
        {
            _out.printError("Error: Configuration of Loxone is incomplete (password is missing). Please correct it in \"loxone.conf\".");
            return;
        }

        _tcpSocket = std::make_shared<BaseLib::TcpSocket>(_bl, _hostname, std::to_string(_port), false, std::string(), false);
        _tcpSocket->setConnectionRetries(1);
        _tcpSocket->setReadTimeout(1000000);
        _tcpSocket->setWriteTimeout(1000000);
        _stopCallbackThread = false;
        if(_settings->listenThreadPriority > -1) _bl->threadManager.start(_listenThread, true, _settings->listenThreadPriority, _settings->listenThreadPolicy, &Miniserver::listen, this);
        else _bl->threadManager.start(_listenThread, true, &Miniserver::listen, this);
        IPhysicalInterface::startListening();
    }
    catch(const std::exception& ex)
    {
        _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
}

void Miniserver::stopListening()
{
    try
    {
        _stopCallbackThread = true;
        if(_tcpSocket) _tcpSocket->close();
        _bl->threadManager.join(_listenThread);
        _stopped = true;
        IPhysicalInterface::stopListening();
    }
    catch(const std::exception& ex)
    {
        _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
}

void Miniserver::init()
{
	try
    {
        _out.printDebug("init Connection to Miniserver");

        {
            std::lock_guard<std::mutex> requestsGuard(_responsesMutex);
            _responses.clear();
            _responseCollections.clear();
        }

		{
			{
				httpPacket = true;
				
				//BaseLib::Http::constructHeader();	
				//"GET /jdev/sys/getPublicKey / HTTP / 1.1\r\n"
				//"Host: homegear\r\n"
				//"User-Agent: homegear\r\n"
				//"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
				//"Accept-Language: en-us,en;q=0.5\r\n"
				//"Accept-Encoding: gzip,deflate\r\n"
				//"Accept-Charset: ISO-8859-1,utf-8;\r\n"
				//"Keep-Alive: 1000\r\n"
				//"Connection: keep-alive\r\n"
				//"\r\n"

                if (GD::bl->debugLevel >= 5) _out.printDebug("Step 1: getPublicKey");

                //auto loxonePacket = std::make_shared<LoxonePacket>(LoxonePacket::httpGetPublicKey());
				auto loxonePacket = LoxonePacket::_commands.at("getPublicKey");
				auto responsePacket = getResponse(loxonePacket);
				auto loxoneHttpPacket = std::dynamic_pointer_cast<LoxoneHttpPacket>(responsePacket);

				//if (!responsePacket || responsePacket->getPayload().at(0) == 1)
				if (loxoneHttpPacket->getResponseCode() != 200)
				{
					_out.printError("Error: Could not get Public Key from Miniserver.");
					_stopped = true;
					return;
				}
				_loxoneEncryption.setPublicKey(loxoneHttpPacket->getValue()->stringValue);
			}

			{
                if (GD::bl->debugLevel >= 5) _out.printDebug("Step 2: openWebsocket Connection");

				auto loxonePacket = LoxonePacket::_commands.at("openWebsocket");
				auto responsePacket = getResponse(loxonePacket);
				auto loxoneHttpPacket = std::dynamic_pointer_cast<LoxoneHttpPacket>(responsePacket);

				//if (!responsePacket || responsePacket->getPayload().at(0) == 1)
				if (loxoneHttpPacket->getResponseCode() != 101)
				{
					_out.printError("Error: Could not open Websocket Connection to Miniserver.");
					_stopped = true;
					return;
				}
			}

			{
                if (GD::bl->debugLevel >= 5) _out.printDebug("Step 3: start keyexchange");

				httpPacket = false;
				webSocketPacket = true;
				
				auto loxonePacket = LoxonePacket::_commands.at("keyexchange");
		
				_loxoneEncryption.makeAES256();
				std::string RSA_encrypted;
				_loxoneEncryption.buildSessionKey(RSA_encrypted);
								
				loxonePacket._value = RSA_encrypted;
				auto responsePacket = getResponse(loxonePacket);
				auto loxoneTextmessagePacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);

				//if (!responsePacket || responsePacket->getPayload().at(0) == 1)
				if (loxoneTextmessagePacket->getResponseCode() != 200)
				{
					_out.printError("Error: Could not exchange AES Keys with Miniserver.");
					_stopped = true;
					return;
				}
			}

			{
                if (GD::bl->debugLevel >= 5) _out.printDebug("Step 4: getkey2");

                auto loxonePacket = LoxonePacket::_commands.at("getkey2");

				loxonePacket._value = _loxoneEncryption.encryptCommand(loxonePacket._command + _user);
				loxonePacket._command = "jdev/sys/enc/";

				auto responsePacket = getResponse(loxonePacket);
				auto loxoneTextmessagePacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);

				//if (!responsePacket || responsePacket->getPayload().at(0) == 1)
				if (loxoneTextmessagePacket->getResponseCode() != 200)
				{
					_out.printError("Error: Could not get Tokens from Miniserver.");
					_stopped = true;
					return;
				}

				_loxoneEncryption.setKey(loxoneTextmessagePacket->getValue()->structValue->at("key")->stringValue);
				_loxoneEncryption._salt = loxoneTextmessagePacket->getValue()->structValue->at("salt")->stringValue;
			}
			{
                if (GD::bl->debugLevel >= 5) _out.printDebug("Step 5: getToken");

				auto loxonePacket = LoxonePacket::_commands.at("getToken");
				loxonePacket._value = _loxoneEncryption.hashPassword(_user, _password);
				loxonePacket._value = _loxoneEncryption.encryptCommand("jdev/sys/gettoken/" + loxonePacket._value + "/"+ _user + "/2/edfc5f9a-df3f-4cad-9dddcdc42c732be2/homegearloxwsapi");
				loxonePacket._command = "jdev/sys/enc/";

				auto responsePacket = getResponse(loxonePacket);
				auto loxoneTextmessagePacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);

				if (loxoneTextmessagePacket->getResponseCode() != 200)
				{
					_out.printError("Error: Could not get Tokens from Miniserver.");
					_stopped = true;
					return;
				}
			}
			{
                if (GD::bl->debugLevel >= 5) _out.printDebug("Step 6: enableUpdates");

				auto loxonePacket = LoxonePacket::_commands.at("enableUpdates");
				auto responsePacket = getResponse(loxonePacket);
				auto loxoneTextmessagePacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
				if (loxoneTextmessagePacket->getResponseCode() != 200)
				{
					_out.printError("Error: Could not enable Updates from Miniserver.");
					_stopped = true;
					return;
				}
			}
		}
        _out.printInfo("Info: Initialization complete.");
		_out.printInfo("Info: Starting Keep Alive Thread.");
		_bl->threadManager.start(_keepAliveThread, true, &Miniserver::keepAlive, this);
    }
    catch(const std::exception& ex)
    {
        _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
}

void Miniserver::keepAlive()
{
	try
	{
		while (!_stopCallbackThread)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //Sleep for 60sec
			{
				if (_keepAliveCounter >= 60)
				{
					_keepAliveCounter = 0;

					auto loxonePacket = LoxonePacket::_commands.at("keepalive");
					auto responsePacket = getResponse(loxonePacket);

					auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);

					if (loxoneWsPacket->getResponseCode() != 200)
					{
						_out.printError("Error: Could not keepalive the connection to the miniserver.");
						_stopped = true;
						return;
					}
				}
				_keepAliveCounter++;
			}
		}
	}
	catch (const std::exception& ex)
	{
		_out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
}
void Miniserver::listen()
{
	LoxoneHeader loxoneHeader;
	BaseLib::Http http;
	BaseLib::WebSocket websocket;
    try
    {
        try
        {
            _tcpSocket->open();
            if(_tcpSocket->connected())
            {
                _out.printInfo("Info: Successfully connected.");
                _stopped = false;
                _bl->threadManager.start(_initThread, true, &Miniserver::init, this);
            }
        }
        catch(const std::exception& ex)
        {
            _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
        }

        while(!_stopCallbackThread)
        {
			try
            {
				uint32_t processed = 0;

				if(_stopped || !_tcpSocket->connected())
                {
                    if(_stopCallbackThread) return;
                    if(_stopped) _out.printWarning("Warning: Connection to device closed. Trying to reconnect...");
                    _tcpSocket->close();
                    for(int32_t i = 0; i < 15; i++)
                    {
                        if(_stopCallbackThread) continue;
                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    }
                    _tcpSocket->open();
                    if(_tcpSocket->connected())
                    {
                        _out.printInfo("Info: Successfully connected.");
                        _stopped = false;
                        _bl->threadManager.start(_initThread, true, &Miniserver::init, this);
                    }
                    continue;
                }

                std::vector<uint8_t> buffer(1024);
                int32_t bytesRead = 0;
                try
                {
                    bytesRead = _tcpSocket->proofread((char*)buffer.data(), buffer.size());
                    if (GD::bl->debugLevel >= 5) _out.printDebug("listen -> " + std::string(buffer.begin(), buffer.begin()+ bytesRead));
                }
                catch (BaseLib::SocketTimeOutException& ex)
                {
                    if (_stopCallbackThread) continue;
                    continue;
                }
                if (bytesRead <= 0) continue;
                if (bytesRead > 1024) bytesRead = 1024;

                if (httpPacket)
				{
					do
					{
						processed = http.process((char*)buffer.data() + processed, bytesRead - processed, false, true);
						if (http.isFinished())
						{
							uint32_t responseCode = http.getHeader().responseCode;
							
							if (responseCode == 101)
							{
								httpPacket = false;
								webSocketPacket = true;
							}

							processHttpPacket(http.getContent(), http.getHeader().responseCode);
							http.reset();
						}
					} while (processed < bytesRead);
				}
				else if (webSocketPacket)
				{
					do
					{
						processed += websocket.process((char*)buffer.data() + processed, bytesRead - processed);
						
						if (websocket.getContentSize() == websocket.getHeader().length)
						{
							auto content = websocket.getContent();
							if (websocket.getHeader().opcode == BaseLib::WebSocket::Header::Opcode::Enum::binary)
							{
								if (content.at(0) == 3 && content.size() == 8)//Loxone Header
								{
									loxoneHeader.identifier = (Identifier)content.at(1);
									loxoneHeader.loxonePayloadLength = content.at(4) | content.at(5) << 8 | content.at(6) << 16 | content.at(7) << 24;
								
									if (loxoneHeader.identifier == Identifier::Keepalive_Response)
									{
										processKeepAlivePacket(loxoneHeader.identifier);
									}
								}
								else if (loxoneHeader.identifier == Identifier::Textmessage)
								{
									processTextmessagePacket(content);
								}
								else if (loxoneHeader.identifier == Identifier::Binary_File)
								{
									processBinaryFilePacket(content);
								}
								else if (loxoneHeader.identifier == Identifier::EventTable_of_Value_States)
								{
									processEventTableOfValueStatesPacket(content);
								}
								else if (loxoneHeader.identifier == Identifier::EventTable_of_Text_States)
								{
									//processEventTableOfTextStatesPacket(content);
								}
								else if (loxoneHeader.identifier == Identifier::EventTable_of_Daytimer_States)
								{
									processEventTableOfDaytimerStatesPacket(content);
								}
								else if (loxoneHeader.identifier == Identifier::Out_Of_Service_Indicator)
								{
									processOutOfServiceIndicatorPacket(loxoneHeader.identifier);
								}
								else if (loxoneHeader.identifier == Identifier::Keepalive_Response)
								{
									//this Condition would never get true;
									//this is because the Keepalive Response is only a Loxone Header Telegramm.
									//The normale way is to receive a Loxone Header and after a Message wich has to parse differently -> see the different identifiers
									//So this means that we have to do a special check at the LoxoneHeader generation if the received header is a keepalive header
									processKeepAlivePacket(loxoneHeader.identifier);
								}
								else if (loxoneHeader.identifier == Identifier::EventTable_of_Weather_States)
								{
									processEventTableOfWeatherStatesPacket(content);
								}
							}
							if (websocket.getHeader().opcode == BaseLib::WebSocket::Header::Opcode::Enum::text)
							{
								processWsPacket(content);
							}

							websocket.reset();
						}
					} while (processed < bytesRead);
				}
			}
            catch(const std::exception& ex)
            {
                _stopped = true;
                _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
            }
        }
    }
    catch(const std::exception& ex)
    {
        _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
}

void Miniserver::processHttpPacket(const std::vector<char>& data, uint32_t responseCode)
{
    if (GD::bl->debugLevel >= 5) _out.printDebug("processHttpPacket");

    std::string content(data.begin(),data.end());
	auto loxoneHttpPacket = std::make_shared<LoxoneHttpPacket>(content, responseCode);
	
	std::unique_lock<std::mutex> requestsGuard(_responsesMutex);
	auto responsesIterator = _responses.find(loxoneHttpPacket->getControl());
	if (responsesIterator != _responses.end())
	{
		auto request = responsesIterator->second;
		requestsGuard.unlock();
		request->response = loxoneHttpPacket;
		{
			std::lock_guard<std::mutex> lock(request->mutex);
			request->mutexReady = true;
		}
		request->conditionVariable.notify_one();
		return;
	}
	else requestsGuard.unlock();
}

void Miniserver::processWsPacket(std::vector<char>& data)
{
    if (GD::bl->debugLevel >= 5) _out.printDebug("processWsPacket");

    std::string content(data.begin(), data.end());
	auto loxoneWsPacket = std::make_shared<LoxoneWsPacket>(content);

	std::unique_lock<std::mutex> requestsGuard(_responsesMutex);

	auto responsesIterator = _responses.find(loxoneWsPacket->getControl());
	if (responsesIterator != _responses.end())
	{
		auto request = responsesIterator->second;
		requestsGuard.unlock();
		request->response = loxoneWsPacket;
		{
			std::lock_guard<std::mutex> lock(request->mutex);
			request->mutexReady = true;
		}
		request->conditionVariable.notify_one();
		return;
	}
	else requestsGuard.unlock();
}


void Miniserver::processTextmessagePacket(std::vector<char>& data)
{
    if (GD::bl->debugLevel >= 5) _out.printDebug("processTextmessagePacket");
}

void Miniserver::processBinaryFilePacket(std::vector<char>& data)
{
    if (GD::bl->debugLevel >= 5) _out.printDebug("processBinaryFilePacket");
}
void Miniserver::processEventTableOfValueStatesPacket(std::vector<char>& data)
{
    if (GD::bl->debugLevel >= 5) _out.printDebug("processEventTableOfValueStatesPacket");
	uint32_t processed = 0;
	do
	{ 
		std::vector<char> packet(data.begin() + processed, data.begin() + processed + 24);
		
		processed += packet.size();
		auto loxonePacket = std::make_shared<LoxoneValueStatesPacket>(packet.data());
		raisePacketReceived(loxonePacket);
	} while (processed < data.size());
}
void Miniserver::processEventTableOfTextStatesPacket(std::vector<char>& data)
{
    if (GD::bl->debugLevel >= 5) _out.printDebug("processEventTableOfTextStatesPacket");
	uint32_t processed = 0;
	do
	{
		std::vector<char> packet(data.begin() + processed, data.begin() + processed + 36); //insert the UUID, UUID_ICON and the TEXTLENGTH
		processed += packet.size();

		uint32_t strLength = packet.at(32) | packet.at(33) << 8 | packet.at(34) << 16 | packet.at(35) << 24;
		packet.reserve(packet.size() + strLength);
		packet.insert(packet.end(), data.begin() + processed, data.begin() + processed + strLength); 
		auto loxonePacket = std::make_shared<LoxoneTextStatesPacket>((char*)packet.data(), strLength);
		uint32_t padds = strLength % 4;
		if (padds != 0)
		{
			padds = 4 - padds;
			strLength += padds;
		}
		processed += strLength;
		raisePacketReceived(loxonePacket);
	} while (processed < data.size());
}
void Miniserver::processEventTableOfDaytimerStatesPacket(std::vector<char>& data)
{
    if (GD::bl->debugLevel >= 5) _out.printDebug("processEventTableOfDaytimerStatesPacket");
	/*
	else if (loxoneHeader.identifier == Identifier::EventTable_of_Daytimer_States)
	{
	while (buffer2.size() > 0)
	{
		if (buffer2.size() >= 28)
		{
			std::vector<uint8_t> packet(buffer2.begin(), buffer2.begin() + 24);
			buffer2.erase(buffer2.begin(), buffer2.begin() + 24);

			uint32_t numEntrys = buffer2.at(0) | buffer2.at(1) << 8 | buffer2.at(2) << 16 | buffer2.at(3) << 24;
			GD::out.printMessage("die Anzahl der Zeiteintr�ge" + std::to_string(numEntrys), 0, true);//from TR

			packet.insert(packet.end(), buffer2.begin(), buffer2.begin() + 4);
			buffer2.erase(buffer2.begin(), buffer2.begin() + 4);

			if (buffer2.size() >= numEntrys * 24)
			{
				packet.insert(packet.end(), buffer2.begin(), buffer2.begin() + numEntrys * 24);
				buffer2.erase(buffer2.begin(), buffer2.begin() + numEntrys * 24);

				auto loxonePacket = std::make_shared<LoxonePacket>(packet, (Identifier2)(int)loxoneHeader.identifier);
				raisePacketReceived(loxonePacket);
			}
			else
				if (GD::bl->debugLevel >= 5) _out.printDebug("Debug: Problem Parcing EventTable_of_Daytimer_States Packet: Size is to short");
		}
		else
			if (GD::bl->debugLevel >= 5) _out.printDebug("Debug: Problem Parcing EventTable_of_Daytimer_States Packet: Size is to short");
	}
	}
	*/
}

void Miniserver::processOutOfServiceIndicatorPacket(Identifier identifier)
{
    if (GD::bl->debugLevel >= 5) _out.printDebug("processOutOfServiceIndicatorPacket");
}
void Miniserver::processKeepAlivePacket(Identifier identifier)
{
    if (GD::bl->debugLevel >= 5) _out.printDebug("processKeepAlivePacket");

	auto loxoneWsPacket = std::make_shared<LoxoneWsPacket>();

	loxoneWsPacket->setResponseCode(200);

	std::unique_lock<std::mutex> requestsGuard(_responsesMutex);
	auto responsesIterator = _responses.find("keepalive");
	if (responsesIterator != _responses.end())
	{
		auto request = responsesIterator->second;
		requestsGuard.unlock();
		request->response = loxoneWsPacket;
		{
			std::lock_guard<std::mutex> lock(request->mutex);
			request->mutexReady = true;
		}
		request->conditionVariable.notify_one();
		return;
	}
	else requestsGuard.unlock();
}
void Miniserver::processEventTableOfWeatherStatesPacket(std::vector<char>& data)
{
    if (GD::bl->debugLevel >= 5) _out.printDebug("processEventTableOfWeatherStatesPacket");
}

PLoxonePacket Miniserver::getResponse(const LoxoneHttpCommand& requestPacket, int32_t waitForSeconds)
{
	try
    {
        if(_stopped) return PLoxonePacket();

        std::lock_guard<std::mutex> sendPacketGuard(_sendPacketMutex);
        std::lock_guard<std::mutex> getResponseGuard(_getResponseMutex);
        std::shared_ptr<Request> request = std::make_shared<Request>();
        std::unique_lock<std::mutex> requestsGuard(_responsesMutex);
		_responses[requestPacket._responseCommand] = request;
		requestsGuard.unlock();
        std::unique_lock<std::mutex> lock(request->mutex);

		std::string sendCommand = "";
		std::string command ="";
		if (requestPacket._type == commandType::http)
		{
			command = requestPacket._command;
			sendCommand = command;
		}
		else if (requestPacket._type == commandType::ws)
		{
			command = requestPacket._command + requestPacket._value;

			std::vector<char> output;
			std::vector<char> input(command.begin(), command.end());
			BaseLib::WebSocket::encode(input, BaseLib::WebSocket::Header::Opcode::Enum::text, output);

			std::string sendCommand2(output.begin(), output.end());
			sendCommand = sendCommand2;
		}
		else if (requestPacket._type == commandType::header) {}
		else {}

        try
        {
            GD::out.printInfo("Info: Sending packet " + command);
            _tcpSocket->proofwrite(sendCommand);

			_lastPacketSent = BaseLib::HelperFunctions::getTime();
        }
        catch(const BaseLib::SocketOperationException& ex)
        {
            _out.printError("Error sending packet: " + std::string(ex.what()));
            return PLoxonePacket();
        }

		int32_t i = 0;
        while(!request->conditionVariable.wait_for(lock, std::chrono::milliseconds(1000), [&]
        {
			i++;
            return request->mutexReady || _stopped || i == waitForSeconds;
        }));

		if (i == waitForSeconds || !request->response)
		//if (i == waitForSeconds || (request->response._code != 200 && request->_httpCommand._code != 101))
		{
			_out.printError("Error: No response received to packet: " + requestPacket._command);
			return PLoxonePacket();
		}
		auto responsePacket = request->response;

        requestsGuard.lock();
		_responses.erase(requestPacket._responseCommand);
        requestsGuard.unlock();

        return responsePacket;
    }
    catch(const std::exception& ex)
    {
        _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
    return PLoxonePacket();
}

PVariable Miniserver::getNewStructfile()
{
    if (GD::bl->debugLevel >= 5) _out.printDebug("getNewStructfile");

	auto loxonePacket = LoxonePacket::_commands.at("newStuctfile");
	auto responsePacket = getResponse(loxonePacket);
	auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);

	if (loxoneWsPacket->getResponseCode() != 200)
	{
		_out.printError("Error: Could not get new Structfile from miniserver.");
		_stopped = true;
		return PVariable();
	}
	return loxoneWsPacket->getValue();
}

PVariable Miniserver::getLoxApp3Version()
{
    if (GD::bl->debugLevel >= 5) _out.printDebug("getLoxApp3Version");

	auto loxonePacket = LoxonePacket::_commands.at("LoxApp3Version");
	auto responsePacket = getResponse(loxonePacket);
	auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);

	if (loxoneWsPacket->getResponseCode() != 200)
	{
		_out.printError("Error: Could not get LoxApp3Version from miniserver.");
		_stopped = true;
		return PVariable();
	}
	return loxoneWsPacket->getValue();
}

}
