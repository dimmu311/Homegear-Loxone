#include "Miniserver.h"
#include "../Loxone.h"

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
    //_msVersion = settings->type;
	_loxoneEncryption = std::make_shared<LoxoneEncryption>(_user, settings->password, settings->passwordS21, _bl);

    std::string token;
    {
        std::string name = "token";
        auto setting = GD::family->getFamilySetting(name);
        if(setting) token = setting->stringValue;
    }
    if(token.size() >0) _loxoneEncryption->setToken(token);

    //_musicserver = std::make_shared<Musicserver>(settings);
}

Miniserver::~Miniserver()
{
    stopListening();
    _bl->threadManager.join(_initThread);
	_bl->threadManager.join(_keepAliveThread);
	_bl->threadManager.join(_refreshTokenThread);
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

        std::string commandToEncrypt = loxonePacket->getCommand();
        if(loxonePacket->needToSecure())
        {
            //ToDo: maybe find a better solution than just insert the hashed passwort into the command
            prepareSecuredCommand();
            commandToEncrypt.insert(11 ,"s/" + _loxoneEncryption->getHashedVisuPassword());
        }

        std::string command;
        if(_loxoneEncryption->encryptCommand(commandToEncrypt,command)<0)
        {
            _out.printError("Error: Could not encrypt Command.");
            _stopped = true;
            return;
        }
		try
		{
            if(GD::bl->debugLevel >= 5) GD::out.printInfo("Info: Sending packet " + command);

			auto webSocket = encodeWebSocket(command, WebSocket::Header::Opcode::Enum::text);
            std::string sendCommand(webSocket);
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
        _bl->threadManager.join(_keepAliveThread);

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

                if (GD::bl->debugLevel >= 5) _out.printDebug("Step 0: getVersion");

                auto responsePacket = getResponse("dev/cfg/apiKey/", "GET /jdev/cfg/apiKey/ HTTP/1.1\r\n");
                if (!responsePacket) {
                    _out.printError("Error: Could not get Version from Miniserver.");
                    _stopped = true;
                    return;
                }
                auto loxoneHttpPacket = std::dynamic_pointer_cast<LoxoneHttpPacket>(responsePacket);
                if (!loxoneHttpPacket || loxoneHttpPacket->getResponseCode() != 200) {
                    _out.printError("Error: Could not get Version from Miniserver.");
                    _stopped = true;
                    return;
                }
                std::string value = loxoneHttpPacket->getValue()->stringValue;
                std::replace(value.begin(),value.end(), '\'', '"');
                PVariable json = BaseLib::Rpc::JsonDecoder::decode(value);
                if (GD::bl->debugLevel >= 5) _out.printDebug("Miniserver Version = " + json->structValue->at("version")->stringValue);
                std::string ver = json->structValue->at("version")->stringValue;
                ver = ver.substr(0, ver.find("."));
                uint32_t version = std::stoi(ver);
                if(version < _msVersion)
                {
                    _out.printError("Error: Miniserver Firmware not Supported by this Module. Please update the Miniserver to min Firmware Version 11.x.x.");
                    _stopped = true;
                    return;
                }
            }
			{
				if (GD::bl->debugLevel >= 5) _out.printDebug("Step 1: getPublicKey");

				auto responsePacket = getResponse("dev/sys/getPublicKey/", "GET /jdev/sys/getPublicKey/ HTTP/1.1\r\n");
                if(!responsePacket)
				{
					_out.printError("Error: Could not get Public Key from Miniserver.");
					_stopped = true;
					return;
				}
				auto loxoneHttpPacket = std::dynamic_pointer_cast<LoxoneHttpPacket>(responsePacket);
				if (!loxoneHttpPacket || loxoneHttpPacket->getResponseCode() != 200)
				{
					_out.printError("Error: Could not get Public Key from Miniserver.");
					_stopped = true;
					return;
				}
				_loxoneEncryption->setPublicKey(loxoneHttpPacket->getValue()->stringValue);
			}
			{
                if (GD::bl->debugLevel >= 5) _out.printDebug("Step 2: openWebsocket Connection");

                auto responsePacket = getResponse("Web Socket Protocol Handshake", "GET /ws/rfc6455/ HTTP/1.1\r\n");
				if (!responsePacket)
				{
					_out.printError("Error: Could not open Websocket Connection to Miniserver.");
					_stopped = true;
					return;
				}
				auto loxoneHttpPacket = std::dynamic_pointer_cast<LoxoneHttpPacket>(responsePacket);
				if (!loxoneHttpPacket || loxoneHttpPacket->getResponseCode() != 101)
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

				std::string RSA_encrypted;
				if(_loxoneEncryption->buildSessionKey(RSA_encrypted)< 0)
                {
                    _out.printError("Error: Could not encrypt AES Keys.");
                    _stopped = true;
                    return;
                }

                std::string command = "jdev/sys/keyexchange/" + RSA_encrypted;
                auto webSocket = encodeWebSocket(command, WebSocket::Header::Opcode::Enum::text);
                auto responsePacket = getResponse("jdev/sys/keyexchange/", webSocket);
				if(!responsePacket)
				{
					_out.printError("Error: Could not exchange AES Keys with Miniserver.");
					_stopped = true;
					return;
				}
				auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
				if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200)
				{
					_out.printError("Error: Could not exchange AES Keys with Miniserver.");
					_stopped = true;
					return;
				}
			}
            {
                if (GD::bl->debugLevel >= 5) _out.printDebug("Step 4: getkey2");

                std::string command;
                if(_loxoneEncryption->encryptCommand("jdev/sys/getkey2/" + _user, command)<0)
                {
                    _out.printError("Error: Could not encrypt command.");
                    _stopped = true;
                    return;
                }

                auto webSocket = encodeWebSocket(command, WebSocket::Header::Opcode::Enum::text);
                auto responsePacket = getResponse("jdev/sys/getkey2/", webSocket);
                if (!responsePacket)
                {
                    _out.printError("Error: Could not get Key from Miniserver.");
                    _stopped = true;
                    return;
                }
                auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
                if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200)
                {
                    _out.printError("Error: Could not get Key from Miniserver.");
                    _stopped = true;
                    return;
                }

                _loxoneEncryption->setKey(loxoneWsPacket->getValue()->structValue->at("key")->stringValue);
                _loxoneEncryption->setSalt(loxoneWsPacket->getValue()->structValue->at("salt")->stringValue);
                if(_loxoneEncryption->setHashAlgorithm(loxoneWsPacket->getValue()->structValue->at("hashAlg")->stringValue)<0)
                {
                    _out.printError("Error: Could not set Hash Algorithm.");
                    _stopped = true;
                    return;
                }
            }

            if (GD::bl->debugLevel >= 5) _out.printDebug("Step 4: Check if there is a valid token that can be used for authentication");
            std::string token;
            std::time_t lifeTime;
            if(_loxoneEncryption->getToken(token, lifeTime) == 0)
            {
                std::time_t t = std::time(0);
                if(lifeTime > t)
                {
                    if (GD::bl->debugLevel >= 5) _out.printDebug("Step 5: there is a valid token!");
                    authenticateUsingTokens();
                }
                else acquireToken();
            }
            else acquireToken();

            prepareSecuredCommand();

			{
                if (GD::bl->debugLevel >= 5) _out.printDebug("Step 6: enableUpdates");

				std::string command = "jdev/sps/enablebinstatusupdate";
				auto webSocket = encodeWebSocket(command, WebSocket::Header::Opcode::Enum::text);
                auto responsePacket = getResponse("dev/sps/enablebinstatusupdate", webSocket);
                if(!responsePacket)
				{
					_out.printError("Error: Could not enable Updates from Miniserver.");
					_stopped = true;
					return;
				}
				auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
				if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200)
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
        _out.printInfo("Info: Starting Refresh Token Thread.");
        _bl->threadManager.start(_refreshTokenThread, true, &Miniserver::refreshToken, this);
    }
    catch(const std::exception& ex)
    {
        _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
}
void Miniserver:: authenticateUsingTokens()
{
    if (GD::bl->debugLevel >= 5) _out.printDebug("authenticateUsingTokens");
    {
        if (GD::bl->debugLevel >= 5) _out.printDebug("Step 2: authenticate");
        std::string hashedToken;
        if (_loxoneEncryption->hashToken(hashedToken) < 0) {
            _out.printError("Error: Could not hash the Token.");
            _stopped = true;
            return;
        }
        std::string command;
        if (_loxoneEncryption->encryptCommand("authwithtoken/" + hashedToken + "/" + _user,command) < 0) {
            _out.printError("Error: Could not encrypt command.");
            _stopped = true;
            return;
        }
        auto webSocket = encodeWebSocket(command, WebSocket::Header::Opcode::Enum::text);
        auto responsePacket = getResponse("authwithtoken/", webSocket);
        if (!responsePacket)
        {
            _out.printError("Error: Could not authenticate with token.");
            _stopped = true;
            return;
        }
        auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
        if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200)
        {
            _out.printError("Error: Could not authenticate with token.");
            if(loxoneWsPacket->getResponseCode() == 401) _loxoneEncryption->setToken("");
            _stopped = true;
            return;
        }
    }
}
void Miniserver:: acquireToken()
{
    {
        if (GD::bl->debugLevel >= 5) _out.printDebug("Step 5: getToken");
        std::string hashedPassword;
        if(_loxoneEncryption->hashPassword(hashedPassword)<0)
        {
            _out.printError("Error: Could not hash password.");
            _stopped = true;
            return;
        }
        std::string command;
        //if(_loxoneEncryption->encryptCommand("jdev/sys/gettoken/" + hashedPassword + "/"+ _user + "/2/edfc5f9a-df3f-4cad-9dddcdc42c732be2/homegearloxwsapi", command)<0)
        if(_loxoneEncryption->encryptCommand("jdev/sys/getjwt/" + hashedPassword + "/"+ _user + "/2/edfc5f9a-df3f-4cad-9dddcdc42c732be2/homegearloxwsapi", command)<0)
        {
            _out.printError("Error: Could not encrypt command.");
            _stopped = true;
            return;
        }

        auto webSocket = encodeWebSocket(command, WebSocket::Header::Opcode::Enum::text);
        //auto responsePacket = getResponse("jdev/sys/gettoken/", std::string(output.begin(), output.end()));
        auto responsePacket = getResponse("jdev/sys/getjwt/", webSocket);
        if(!responsePacket)
        {
            _out.printError("Error: Could not get Token from Miniserver.");
            _stopped = true;
            return;
        }
        auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
        if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200)
        {
            _out.printError("Error: Could not get Token from Miniserver.");
            _stopped = true;
            return;
        }

        if(_loxoneEncryption->setToken(loxoneWsPacket->getValue())<0)
        {
            _out.printError("Error: Could not import Token.");
            _stopped = true;
            return;
        }
        saveToken();
    }
}

    void Miniserver::prepareSecuredCommand()
    {
        {
            if (GD::bl->debugLevel >= 5) _out.printDebug("Step 1: Request Visu Salt");
            std::string command;
            if (_loxoneEncryption->encryptCommand("jdev/sys/getvisusalt/" + _user,command) < 0) {
                _out.printError("Error: Could not encrypt command.");
                _stopped = true;
                return;
            }
            auto webSocket = encodeWebSocket(command, WebSocket::Header::Opcode::Enum::text);
            auto responsePacket = getResponse("dev/sys/getvisusalt/", webSocket);
            if (!responsePacket)
            {
                _out.printError("Error: Could get Visu Salt.");
                _stopped = true;
                return;
            }
            auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
            if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200)
            {
                _out.printError("Error: Could get Visu Salt.");
                _stopped = true;
                return;
            }

            _loxoneEncryption->setVisuKey(loxoneWsPacket->getValue()->structValue->at("key")->stringValue);
            _loxoneEncryption->setVisuSalt(loxoneWsPacket->getValue()->structValue->at("salt")->stringValue);
            if(_loxoneEncryption->setVisuHashAlgorithm(loxoneWsPacket->getValue()->structValue->at("hashAlg")->stringValue)<0)
            {
                _out.printError("Error: Could not set Hash Algorithm.");
                _stopped = true;
                return;
            }
        }
        {
            if (GD::bl->debugLevel >= 5) _out.printDebug("Step 2: create Visu Password Hash");
            std::string hashedPassword;
            if(_loxoneEncryption->hashVisuPassword(hashedPassword)<0)
            {
                _out.printError("Error: Could not hash password.");
                _stopped = true;
                return;
            }
            _loxoneEncryption->setHashedVisuPassword(hashedPassword);
        }
     }

void Miniserver::saveToken()
{
    _out.printInfo("Info: Save Token.");
    std::string token;
    std::time_t lifeTime;
    if(_loxoneEncryption->getToken(token, lifeTime)==0)
    {
        GD::family->setFamilySetting("token", token);
    }
}
void Miniserver::refreshToken()
{
    try
    {
        uint32_t refreshToken = 3600;
        while (!_stopCallbackThread)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //Sleep for 1sec
            {
                if (refreshToken >= 3600)
                {
                    refreshToken = 0;
                    if (GD::bl->debugLevel >= 5) _out.printDebug("Refresh Token");
                    if (GD::bl->debugLevel >= 5) _out.printDebug("Step 1: getkey");
                    {
                        std::string command;
                        if(_loxoneEncryption->encryptCommand("jdev/sys/getkey/", command)<0)
                        {
                            _out.printError("Error: Could not encrypt command.");
                            _stopped = true;
                            return;
                        }

                        auto webSocket = encodeWebSocket(command, WebSocket::Header::Opcode::Enum::text);
                        auto responsePacket = getResponse("jdev/sys/getkey/", webSocket);
                        if(!responsePacket)
                        {
                            _out.printError("Error: Could not get Key from Miniserver.");
                            _stopped = true;
                            return;
                        }
                        auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
                        if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200)
                        {
                            _out.printError("Error: Could not get Key from Miniserver.");
                            _stopped = true;
                            return;
                        }
                        _loxoneEncryption->setKey(loxoneWsPacket->getValue()->stringValue);
                    }
                    {
                        if (GD::bl->debugLevel >= 5) _out.printDebug("Step 2: refresh Token");
                        std::string hashedToken;
                        if (_loxoneEncryption->hashToken(hashedToken) < 0) {
                            _out.printError("Error: Could not hash the Token.");
                            _stopped = true;
                            return;
                        }
                        std::string command;
                        if (_loxoneEncryption->encryptCommand("jdev/sys/refreshjwt/" + hashedToken + "/" + _user,command) < 0) {
                            _out.printError("Error: Could not encrypt command.");
                            _stopped = true;
                            return;
                        }

                        auto webSocket = encodeWebSocket(command, WebSocket::Header::Opcode::Enum::text);
                        auto responsePacket = getResponse("dev/sys/refreshjwt/", webSocket);
                        if (!responsePacket) {
                            _out.printError("Error: Could not refresh token.");
                            _stopped = true;
                            return;
                        }
                        auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
                        if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200) {
                            _out.printError("Error: Could not refresh token.");
                            _stopped = true;
                            return;
                        }

                        if(_loxoneEncryption->setToken(loxoneWsPacket->getValue())<0)
                        {
                            _out.printError("Error: Could not import Token.");
                            _stopped = true;
                            return;
                        }
                        saveToken();
                    }
                }
                refreshToken++;
            }
        }
    }
    catch (const std::exception& ex)
    {
        _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
}
void Miniserver::keepAlive()
{
	try
	{
        uint32_t keepAliveCounter = 0;
        while (!_stopCallbackThread)
		//_stopped
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //Sleep for 1sec
			{
				if (keepAliveCounter >= 60)
				{
					keepAliveCounter = 0;

					auto webSocket = encodeWebSocket("keepalive", WebSocket::Header::Opcode::Enum::text);
                    auto responsePacket = getResponse("keepalive", webSocket);
                    if (!responsePacket)
					{
						_out.printError("Error: Could not keepalive the connection to the miniserver.");
						_stopped = true;
						return;
					}
					auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
					if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200)
					{
						_out.printError("Error: Could not keepalive the connection to the miniserver.");
						_stopped = true;
						return;
					}
				}
				keepAliveCounter++;
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
	auto loxoneHeader = std::make_shared<LoxoneHeader>();
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
                uint32_t bytesRead = 0;
                try
                {
                    bytesRead = _tcpSocket->proofread((char*)buffer.data(), buffer.size());
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
                                if (GD::bl->debugLevel >= 5) _out.printDebug("Websocket Opcode is typ BINARY");

								if (content.at(0) == 3 && content.size() == 8)//Loxone Header
								{
                                    if (GD::bl->debugLevel >= 5) _out.printDebug("Websocket BINARY Packet is typ Loxone Header");

									loxoneHeader->identifier = (LoxoneHeader::Identifier)content.at(1);
									loxoneHeader->loxonePayloadLength = content.at(4) | content.at(5) << 8 | content.at(6) << 16 | content.at(7) << 24;
								
									if (loxoneHeader->identifier == LoxoneHeader::Identifier::Keepalive_Response) processKeepAlivePacket();
									else if (loxoneHeader->identifier == LoxoneHeader::Identifier::Out_Of_Service_Indicator) processOutOfServiceIndicatorPacket();
								}
								else if (loxoneHeader->identifier == LoxoneHeader::Identifier::Textmessage)
								{
									processTextmessagePacket(content);
								}
								else if (loxoneHeader->identifier == LoxoneHeader::Identifier::Binary_File)
								{
									processBinaryFilePacket(content);
								}
								else if (loxoneHeader->identifier == LoxoneHeader::Identifier::EventTable_of_Value_States)
								{
									processEventTableOfValueStatesPacket(content);
								}
								else if (loxoneHeader->identifier == LoxoneHeader::Identifier::EventTable_of_Text_States)
								{
									processEventTableOfTextStatesPacket(content);
								}
								else if (loxoneHeader->identifier == LoxoneHeader::Identifier::EventTable_of_Daytimer_States)
								{
									processEventTableOfDaytimerStatesPacket(content);
								}
								else if (loxoneHeader->identifier == LoxoneHeader::Identifier::EventTable_of_Weather_States)
								{
									processEventTableOfWeatherStatesPacket(content);
								}
								else if (loxoneHeader->identifier == LoxoneHeader::Identifier::Out_Of_Service_Indicator)
								{
									//this Condition would never get true;
									//this is because the Out of Service Response is only a Loxone Header Telegramm.
									//The normale way is to receive a Loxone Header and after a Message wich has to parse differently -> see the different identifiers
									//So this means that we have to do a special check at the LoxoneHeader generation if the received header is a Out of Service header
									processOutOfServiceIndicatorPacket();
								}
								else if (loxoneHeader->identifier == LoxoneHeader::Identifier::Keepalive_Response)
								{
									//this Condition would never get true;
									//this is because the Keepalive Response is only a Loxone Header Telegramm.
									//The normale way is to receive a Loxone Header and after a Message wich has to parse differently -> see the different identifiers
									//So this means that we have to do a special check at the LoxoneHeader generation if the received header is a keepalive header
									processKeepAlivePacket();
								}
							}
							else if (websocket.getHeader().opcode == BaseLib::WebSocket::Header::Opcode::Enum::text)
							{
                                if (GD::bl->debugLevel >= 5) _out.printDebug("Websocket Opcode is typ TEXT");
								processWsPacket(content);
							}
							else if(websocket.getHeader().opcode == BaseLib::WebSocket::Header::Opcode::Enum::close)
							{
                                if (GD::bl->debugLevel >= 5) _out.printDebug("Websocket Opcode is typ CLOSE");
								if (GD::bl->debugLevel >= 5) _out.printDebug("Opcode CLOSE -> " + std::string(content.begin(), content.end()));
								_stopped = true;
								_loxoneEncryption->setToken("");
								websocket.reset();
								break;
							}

							websocket.reset();
						}
					} while (processed < bytesRead);
				}
			}
            catch(const std::exception& ex)
            {
                _stopped = true;
                _loxoneEncryption->setToken("");
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

    if(loxoneWsPacket->isControlEncypted())
    {
        std::string control = loxoneWsPacket->getControl();
        //remove "jdev/sys/enc/" at the beginning
        control.erase(0, 13);
        std::string decrypted;
        if(_loxoneEncryption->decryptCommand(control, decrypted)<0)
        {
            _out.printError("Error: Could not decrypt Packet");
            _stopped = true;
            return;
        }
        loxoneWsPacket->setControl(decrypted);
        if (GD::bl->debugLevel >= 7) _out.printDebug("decrypted Command = " + decrypted);
        if (GD::bl->debugLevel >= 7) _out.printDebug("decrypted Command as hex= " + BaseLib::HelperFunctions::getHexString(decrypted));
    }
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
	try
	{
		uint32_t processed = 0;
		do
		{
			std::vector<char> packet(data.begin() + processed, data.begin() + processed + 24);

			processed += packet.size();
			auto loxonePacket = std::make_shared<LoxoneValueStatesPacket>(packet.data());
			raisePacketReceived(loxonePacket);
		} while (processed < data.size());
	}
	catch(const std::exception& ex)
	{
		_out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
}
void Miniserver::processEventTableOfTextStatesPacket(std::vector<char>& data)
{
    if (GD::bl->debugLevel >= 5) _out.printDebug("processEventTableOfTextStatesPacket");
    try
	{
		uint32_t processed = 0;
		do
		{
			std::vector<uint8_t> packet(data.begin() + processed, data.begin() + processed + 36); //insert the UUID, UUID_ICON and the TEXTLENGTH
			processed += 36;

			uint32_t strLength = packet.at(32) | packet.at(33) << 8 | packet.at(34) << 16 | packet.at(35) << 24;
			packet.reserve(packet.size() + strLength);
			packet.insert(packet.end(), data.begin() + processed, data.begin() + processed + strLength);

			auto loxonePacket = std::make_shared<LoxoneTextStatesPacket>((char*)packet.data(), strLength + 36);

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
	catch(const std::exception& ex)
	{
		_out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
}
void Miniserver::processEventTableOfDaytimerStatesPacket(std::vector<char>& data)
{
    if (GD::bl->debugLevel >= 5) _out.printDebug("processEventTableOfDaytimerStatesPacket");
    try
    {
		uint32_t processed = 0;
		do
		{
			std::vector<uint8_t> packet(data.begin() + processed, data.begin() + processed + 28); //insert the UUID, defValue and nrEntrys
			processed += 28;

			uint32_t nrEntrys = packet.at(24) | packet.at(25) << 8 | packet.at(26) << 16 | packet.at(27) << 24;
			if(nrEntrys >0)
			{
				uint32_t lenEntrys = 24*nrEntrys;

				packet.reserve(packet.size() + lenEntrys);
				packet.insert(packet.end(), data.begin() + processed, data.begin() + processed + lenEntrys);

				processed += lenEntrys;
			}
			auto loxonePacket = std::make_shared<LoxoneDaytimerStatesPacket>((char*)packet.data(), nrEntrys);
			raisePacketReceived(loxonePacket);
		} while (processed < data.size());
	}
	catch(const std::exception& ex)
	{
		_out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
}

void Miniserver::processOutOfServiceIndicatorPacket()
{
    if (GD::bl->debugLevel >= 5) _out.printDebug("processOutOfServiceIndicatorPacket");
}
void Miniserver::processKeepAlivePacket()
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
    try
	{
		uint32_t processed = 0;
		do
		{
			std::vector<uint8_t> packet(data.begin() + processed, data.begin() + processed + 24); //insert the UUID, lastUpdate and nrEntrys
			processed += 24;

			uint32_t nrEntrys = packet.at(20) | packet.at(21) << 8 | packet.at(22) << 16 | packet.at(23) << 24;
			if(nrEntrys >0)
			{
				uint32_t lenEntrys = 68*nrEntrys;

				packet.reserve(packet.size() + lenEntrys);
				packet.insert(packet.end(), data.begin() + processed, data.begin() + processed + lenEntrys);

				processed += lenEntrys;
			}
			auto loxonePacket = std::make_shared<LoxoneWeatherStatesPacket>((char*)packet.data(), nrEntrys);
			raisePacketReceived(loxonePacket);
		} while (processed < data.size());
	}
	catch(const std::exception& ex)
	{
		_out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
}

PLoxonePacket Miniserver::getResponse(const std::string& responseCommand, const std::string& command, int32_t waitForSeconds)
{
    try
    {
        if(_stopped) return PLoxonePacket();

        std::lock_guard<std::mutex> sendPacketGuard(_sendPacketMutex);
        std::lock_guard<std::mutex> getResponseGuard(_getResponseMutex);
        std::shared_ptr<Request> request = std::make_shared<Request>();
        std::unique_lock<std::mutex> requestsGuard(_responsesMutex);
        _responses[responseCommand] = request;
        requestsGuard.unlock();
        std::unique_lock<std::mutex> lock(request->mutex);

        try
        {
            GD::out.printInfo("Info: Sending command " + command);
            _tcpSocket->proofwrite(command);

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
        {
            _out.printError("Error: No response received to command: " + command);
            return PLoxonePacket();
        }
        auto responsePacket = request->response;

        requestsGuard.lock();
        _responses.erase(responseCommand);
        requestsGuard.unlock();

        return responsePacket;
    }
    catch(const std::exception& ex)
    {
        _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
    return PLoxonePacket();
}

std::string Miniserver::encodeWebSocket(const std::string& command, WebSocket::Header::Opcode::Enum messageType)
{
    std::vector<char> output;
    std::vector<char> input(command.begin(), command.end());
    BaseLib::WebSocket::encode(input, messageType, output);
    return std::string(output.begin(), output.end());
}

PVariable Miniserver::getNewStructfile()
{
    if (GD::bl->debugLevel >= 5) _out.printDebug("getNewStructfile");

    auto webSocket = encodeWebSocket("data/LoxAPP3.json", WebSocket::Header::Opcode::Enum::text);
    auto responsePacket = getResponse("newStuctfile", webSocket);
    if(!responsePacket)
	{
		_out.printError("Error: Could not get new Structfile from miniserver.");
		_stopped = true;
		return PVariable();
	}
	auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
	if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200)
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

	auto webSocket = encodeWebSocket("jdev/sps/LoxAPPversion3", WebSocket::Header::Opcode::Enum::text);
    auto responsePacket = getResponse("dev/sps/LoxAPPversion3", webSocket);
    if(!responsePacket)
    {
        _out.printError("Error: Could not get LoxApp3Version from miniserver.");
        _stopped = true;
        return PVariable();
    }
    auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
    if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200)
	{
		_out.printError("Error: Could not get LoxApp3Version from miniserver.");
		_stopped = true;
		return PVariable();
	}
	return loxoneWsPacket->getValue();
}
}
