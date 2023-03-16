#include "Miniserver.h"
#include "../Loxone.h"

namespace Loxone {

Miniserver::Miniserver(std::shared_ptr<BaseLib::Systems::PhysicalInterfaceSettings> settings) : IPhysicalInterface(GD::bl, GD::family->getFamily(), settings) {
  _out.init(GD::bl);
  _out.setPrefix(GD::out.getPrefix() + "\"" + settings->id + "\": ");

  signal(SIGPIPE, SIG_IGN);

  _stopped = true;
  _connected = false;

  if (!settings) {
    _out.printCritical("Critical: Error initializing. Settings pointer is empty.");
    return;
  }

  _hostname = settings->host;
  _port = BaseLib::Math::getNumber(settings->port);
  if (_port < 1 || _port > 65535) _port = 80;

  _user = settings->user;
  //_msVersion = settings->type;
  _loxoneEncryption = std::make_shared<LoxoneEncryption>(settings);
  //_musicserver = std::make_shared<Musicserver>(settings);
}

Miniserver::~Miniserver() {
  stopListening();
  _bl->threadManager.join(_initThread);
  _bl->threadManager.join(_keepAliveThread);
  _bl->threadManager.join(_refreshTokenThread);
}

void Miniserver::sendPacket(std::shared_ptr<BaseLib::Systems::Packet> packet) {
  try {
    PLoxonePacket loxonePacket(std::dynamic_pointer_cast<LoxonePacket>(packet));
    if (!loxonePacket) return;

    std::string commandToEncrypt = loxonePacket->getCommand();
    if (loxonePacket->needToSecure()) {
      //ToDo: maybe find a better solution to handle the encryption in the packet
      prepareSecuredCommand();
      commandToEncrypt.insert(11, "s/" + _loxoneEncryption->getHashedVisuPassword());
    }

    std::string command;
    if (_loxoneEncryption->encryptCommand(commandToEncrypt, command) < 0) {
      _out.printError("Error: Could not encrypt Command.");
      _stopped = true;
      return;
    }

    try {
      GD::out.printInfo("Info: Sending packet " + command);
      auto encoded_packet = encodeWebSocket(command, WebSocket::Header::Opcode::Enum::text);
      _tcpSocket->Send((uint8_t *)encoded_packet.data(), encoded_packet.size());
      /*
      auto responsePacket = getResponse(commandToEncrypt, encodeWebSocket(command, WebSocket::Header::Opcode::Enum::text));
      if(!responsePacket){
          _out.printError("Error: did not receive response for " + command);
          _stopped = true;
          return;
      }
      auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
      if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200){
          _out.printError("Error: did not receive response for " + command);
          _stopped = true;
          return;
      }
      */
      _lastPacketSent = BaseLib::HelperFunctions::getTime();
    }
    catch (const C1Net::Exception &ex) {
      _out.printError("Error sending packet: " + std::string(ex.what()));
      startListening();
    }
  }
  catch (const std::exception &ex) {
    _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
  }
}

void Miniserver::startListening() {
  try {
    stopListening();

    if (_hostname.empty()) {
      _out.printError("Error: Configuration of Loxone is incomplete (hostname is missing). Please correct it in \"loxone.conf\".");
      return;
    }

    if (_settings->user.empty()) {
      _out.printError("Error: Configuration of Loxone is incomplete (user is missing). Please correct it in \"loxone.conf\".");
      return;
    }

    if (_settings->password.empty()) {
      _out.printError("Error: Configuration of Loxone is incomplete (password is missing). Please correct it in \"loxone.conf\".");
      return;
    }

    C1Net::TcpSocketInfo tcp_socket_info;
    tcp_socket_info.read_timeout = 1000;
    tcp_socket_info.write_timeout = 1000;

    C1Net::TcpSocketHostInfo tcp_socket_host_info{
        .host = _hostname,
        .port = (uint16_t)_port,
        .connection_retries = 1
    };

    _tcpSocket = std::make_shared<C1Net::TcpSocket>(tcp_socket_info, tcp_socket_host_info);
    _stopCallbackThread = false;
    if (_settings->listenThreadPriority > -1) _bl->threadManager.start(_listenThread, true, _settings->listenThreadPriority, _settings->listenThreadPolicy, &Miniserver::listen, this);
    else _bl->threadManager.start(_listenThread, true, &Miniserver::listen, this);
    IPhysicalInterface::startListening();
  }
  catch (const std::exception &ex) {
    _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
  }
}

void Miniserver::stopListening() {
  try {
    _stopCallbackThread = true;
    if (_tcpSocket) _tcpSocket->Shutdown();
    _bl->threadManager.join(_listenThread);
    _bl->threadManager.join(_keepAliveThread);

    _stopped = true;
    IPhysicalInterface::stopListening();
  }
  catch (const std::exception &ex) {
    _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
  }
}

void Miniserver::init() {
  try {
    _out.printDebug("Init Connection to Miniserver");
    _connected = false;
    {
      std::lock_guard<std::mutex> requestsGuard(_responsesMutex);
      _responses.clear();
      _responseCollections.clear();
    }
    {
      {
        _out.printDebug("Step 1: get Miniserver Version", 4);
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
        std::replace(value.begin(), value.end(), '\'', '"');
        PVariable json = BaseLib::Rpc::JsonDecoder::decode(value);
        if (!json) {
          _out.printError("Error: Could not get Version from Miniserver.");
          _stopped = true;
          return;
        }
        if (json->structValue->find("version") == json->structValue->end()) {
          _out.printError("Error: Could not get Version from Miniserver.");
          _stopped = true;
          return;
        }
        _out.printDebug("Miniserver Version is: " + json->structValue->at("version")->stringValue);
        std::string ver = json->structValue->at("version")->stringValue;
        ver = ver.substr(0, ver.find("."));
        uint32_t version = std::stoi(ver);
        if (version < _msVersion) {
          _out.printError("Error: Miniserver Firmware not Supported by this Module. Please update the Miniserver to min Firmware Version 11.x.x.");
          _stopped = true;
          return;
        }
      }
      {
        _out.printDebug("Step 2: getPublicKey", 4);
        auto responsePacket = getResponse("dev/sys/getPublicKey/", "GET /jdev/sys/getPublicKey/ HTTP/1.1\r\n");
        if (!responsePacket) {
          _out.printError("Error: Could not get Public Key from Miniserver.");
          _stopped = true;
          return;
        }
        auto loxoneHttpPacket = std::dynamic_pointer_cast<LoxoneHttpPacket>(responsePacket);
        if (!loxoneHttpPacket || loxoneHttpPacket->getResponseCode() != 200) {
          _out.printError("Error: Could not get Public Key from Miniserver.");
          _stopped = true;
          return;
        }
        _loxoneEncryption->setPublicKey(loxoneHttpPacket->getValue()->stringValue);
      }
      {
        _out.printDebug("Step 3: open Websocket Connection", 4);
        auto responsePacket = getResponse("Web Socket Protocol Handshake", "GET /ws/rfc6455/ HTTP/1.1\r\n");
        if (!responsePacket) {
          _out.printError("Error: Could not open Websocket Connection to Miniserver.");
          _stopped = true;
          return;
        }
        auto loxoneHttpPacket = std::dynamic_pointer_cast<LoxoneHttpPacket>(responsePacket);
        if (!loxoneHttpPacket || loxoneHttpPacket->getResponseCode() != 101) {
          _out.printError("Error: Could not open Websocket Connection to Miniserver.");
          _stopped = true;
          return;
        }
      }
      {
        _out.printDebug("Step 3: start key exchange", 4);
        std::string RSA_encrypted;
        if (_loxoneEncryption->buildSessionKey(RSA_encrypted) < 0) {
          _out.printError("Error: Could not encrypt AES Keys.");
          _stopped = true;
          return;
        }
        std::string command = "jdev/sys/keyexchange/" + RSA_encrypted;
        auto responsePacket = getResponse("jdev/sys/keyexchange/", encodeWebSocket(command, WebSocket::Header::Opcode::Enum::text));
        if (!responsePacket) {
          _out.printError("Error: Could not exchange AES Keys with Miniserver.");
          _stopped = true;
          return;
        }
        auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
        if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200) {
          _out.printError("Error: Could not exchange AES Keys with Miniserver.");
          _stopped = true;
          return;
        }
      }
      {
        _out.printDebug("Step 4: send getkey2 request");
        std::string command;
        if (_loxoneEncryption->encryptCommand("jdev/sys/getkey2/" + _user, command) < 0) {
          _out.printError("Error: Could not encrypt command.");
          _stopped = true;
          return;
        }
        auto responsePacket = getResponse("jdev/sys/getkey2/", encodeWebSocket(command, WebSocket::Header::Opcode::Enum::text));
        if (!responsePacket) {
          _out.printError("Error: Could not get Key from Miniserver.");
          _stopped = true;
          return;
        }
        auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
        if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200) {
          _out.printError("Error: Could not get Key from Miniserver.");
          _stopped = true;
          return;
        }

        _loxoneEncryption->setKey(loxoneWsPacket->getValue()->structValue->at("key")->stringValue);
        _loxoneEncryption->setSalt(loxoneWsPacket->getValue()->structValue->at("salt")->stringValue);
        if (_loxoneEncryption->setHashAlgorithm(loxoneWsPacket->getValue()->structValue->at("hashAlg")->stringValue) < 0) {
          _out.printError("Error: Could not set Hash Algorithm.");
          _stopped = true;
          return;
        }
      }
      _out.printDebug("Step 5: Check if there is a valid token that can be used for authentication", 4);
      std::string token;
      std::time_t lifeTime;
      if (_loxoneEncryption->getToken(token, lifeTime) != 0) acquireToken();
      else {
        std::time_t t = std::time(0);
        if (lifeTime > t) {
          _out.printDebug("Step 5: there is a valid token!", 4);
          authenticateUsingTokens();
        }
      }
      prepareSecuredCommand();
      {
        _out.printDebug("Step 6: enableUpdates", 4);
        auto responsePacket = getResponse("dev/sps/enablebinstatusupdate", encodeWebSocket("jdev/sps/enablebinstatusupdate", WebSocket::Header::Opcode::Enum::text));
        if (!responsePacket) {
          _out.printError("Error: Could not enable Updates from Miniserver.");
          _stopped = true;
          return;
        }
        auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
        if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200) {
          _out.printError("Error: Could not enable Updates from Miniserver.");
          _stopped = true;
          return;
        }
      }
    }
    _connected = true;
    _out.printDebug("Info: Initialization complete.", 3);
    _out.printDebug("Info: Starting Keep Alive Thread.", 4);
    _bl->threadManager.start(_keepAliveThread, true, &Miniserver::keepAlive, this);
    _out.printDebug("Info: Starting Refresh Token Thread.", 4);
    _bl->threadManager.start(_refreshTokenThread, true, &Miniserver::refreshToken, this);
  }
  catch (const std::exception &ex) {
    _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    _connected = false;
  }
}
void Miniserver::authenticateUsingTokens() {
  _out.printDebug("Step 5: authenticate using token");
  std::string hashedToken;
  if (_loxoneEncryption->hashToken(hashedToken) < 0) {
    _out.printError("Error: Could not hash the Token.");
    _stopped = true;
    _connected = false;
    return;
  }
  std::string command;
  if (_loxoneEncryption->encryptCommand("authwithtoken/" + hashedToken + "/" + _user, command) < 0) {
    _out.printError("Error: Could not encrypt command.");
    _stopped = true;
    _connected = false;
    return;
  }
  auto responsePacket = getResponse("authwithtoken/", encodeWebSocket(command, WebSocket::Header::Opcode::Enum::text));
  if (!responsePacket) {
    _out.printError("Error: Could not authenticate with token.");
    _stopped = true;
    _connected = false;
    return;
  }
  auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
  if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200) {
    _out.printError("Error: Could not authenticate with token.");
    if (loxoneWsPacket->getResponseCode() == 401) _loxoneEncryption->setToken("");
    _stopped = true;
    _connected = false;
    return;
  }
}
void Miniserver::acquireToken() {
  _out.printDebug("Step 5: getToken", 4);
  std::string hashedPassword;
  if (_loxoneEncryption->hashPassword(hashedPassword) < 0) {
    _out.printError("Error: Could not hash password.");
    _stopped = true;
    _connected = false;
    return;
  }
  std::string command;
  if (_loxoneEncryption->encryptCommand("jdev/sys/getjwt/" + hashedPassword + "/" + _user + "/2/edfc5f9a-df3f-4cad-9dddcdc42c732be2/homegearloxwsapi", command) < 0) {
    _out.printError("Error: Could not encrypt command.");
    _stopped = true;
    _connected = false;
    return;
  }
  auto responsePacket = getResponse("jdev/sys/getjwt/", encodeWebSocket(command, WebSocket::Header::Opcode::Enum::text));
  if (!responsePacket) {
    _out.printError("Error: Could not get Token from Miniserver.");
    _stopped = true;
    _connected = false;
    return;
  }
  auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
  if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200) {
    _out.printError("Error: Could not get Token from Miniserver.");
    _stopped = true;
    _connected = false;
    return;
  }
  if (_loxoneEncryption->setToken(loxoneWsPacket->getValue()) < 0) {
    _out.printError("Error: Could not import Token.");
    _stopped = true;
    _connected = false;
    return;
  }
}

void Miniserver::prepareSecuredCommand() {
  _out.printDebug("Step 1: Request Visu Salt");
  std::string command;
  if (_loxoneEncryption->encryptCommand("jdev/sys/getvisusalt/" + _user, command) < 0) {
    _out.printError("Error: Could not encrypt command.");
    _stopped = true;
    _connected = false;
    return;
  }
  auto responsePacket = getResponse("dev/sys/getvisusalt/", encodeWebSocket(command, WebSocket::Header::Opcode::Enum::text));
  if (!responsePacket) {
    _out.printError("Error: Could get Visu Salt.");
    _stopped = true;
    _connected = false;
    return;
  }
  auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
  if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200) {
    _out.printError("Error: Could get Visu Salt.");
    _stopped = true;
    _connected = false;
    return;
  }
  _loxoneEncryption->setVisuKey(loxoneWsPacket->getValue()->structValue->at("key")->stringValue);
  _loxoneEncryption->setVisuSalt(loxoneWsPacket->getValue()->structValue->at("salt")->stringValue);
  if (_loxoneEncryption->setVisuHashAlgorithm(loxoneWsPacket->getValue()->structValue->at("hashAlg")->stringValue) < 0) {
    _out.printError("Error: Could not set Hash Algorithm.");
    _stopped = true;
    _connected = false;
    return;
  }
  _out.printDebug("Step 2: create Visu Password Hash");
  std::string hashedPassword;
  if (_loxoneEncryption->hashVisuPassword(hashedPassword) < 0) {
    _out.printError("Error: Could not hash password.");
    _stopped = true;
    _connected = false;
    return;
  }
  _loxoneEncryption->setHashedVisuPassword(hashedPassword);
}
void Miniserver::refreshToken() {
  try {
    uint32_t refreshToken = 0;
    while (!_stopCallbackThread) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //Sleep for 1sec
      if (_stopped) return;
      if (_stopCallbackThread) return;
      refreshToken++;
      if (refreshToken < 3600) continue;
      refreshToken = 0;
      _out.printDebug("Refresh Token");
      {
        _out.printDebug("Step 1: getkey");
        std::string command;
        if (_loxoneEncryption->encryptCommand("jdev/sys/getkey/", command) < 0) {
          _out.printError("Error: Could not encrypt command.");
          _stopped = true;
          _connected = false;
          return;
        }
        auto responsePacket = getResponse("jdev/sys/getkey/", encodeWebSocket(command, WebSocket::Header::Opcode::Enum::text));
        if (!responsePacket) {
          _out.printError("Error: Could not get Key from Miniserver.");
          _stopped = true;
          _connected = false;
          return;
        }
        auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
        if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200) {
          _out.printError("Error: Could not get Key from Miniserver.");
          _stopped = true;
          _connected = false;
          return;
        }
        _loxoneEncryption->setKey(loxoneWsPacket->getValue()->stringValue);
      }
      {
        _out.printDebug("Step 2: refresh Token");
        std::string hashedToken;
        if (_loxoneEncryption->hashToken(hashedToken) < 0) {
          _out.printError("Error: Could not hash the Token.");
          _stopped = true;
          _connected = false;
          return;
        }
        std::string command;
        if (_loxoneEncryption->encryptCommand("jdev/sys/refreshjwt/" + hashedToken + "/" + _user, command) < 0) {
          _out.printError("Error: Could not encrypt command.");
          _stopped = true;
          _connected = false;
          return;
        }
        auto responsePacket = getResponse("dev/sys/refreshjwt/", encodeWebSocket(command, WebSocket::Header::Opcode::Enum::text));
        if (!responsePacket) {
          _out.printError("Error: Could not refresh token.");
          _stopped = true;
          _connected = false;
          return;
        }
        auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
        if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200) {
          _out.printError("Error: Could not refresh token.");
          _stopped = true;
          _connected = false;
          return;
        }
        if (_loxoneEncryption->setToken(loxoneWsPacket->getValue()) < 0) {
          _out.printError("Error: Could not import Token.");
          _stopped = true;
          _connected = false;
          return;
        }
      }
    }
  }
  catch (const std::exception &ex) {
    _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
  }
}
void Miniserver::keepAlive() {
  try {
    uint32_t keepAliveCounter = 0;
    while (!_stopCallbackThread) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //Sleep for 1sec
      if (_stopped) return;
      if (_stopCallbackThread) return;
      keepAliveCounter++;
      if (keepAliveCounter < 60) continue;
      _out.printDebug("keepalive", 4);
      keepAliveCounter = 0;
      auto responsePacket = getResponse("keepalive", encodeWebSocket("keepalive", WebSocket::Header::Opcode::Enum::text));
      if (!responsePacket) {
        _out.printError("Error: Could not keepalive the connection to the miniserver.");
        _stopped = true;
        _connected = false;
        return;
      }
      auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
      if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200) {
        _out.printError("Error: Could not keepalive the connection to the miniserver.");
        _stopped = true;
        _connected = false;
        return;
      }
    }
  }
  catch (const std::exception &ex) {
    _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
  }
}
void Miniserver::listen() {
  auto loxoneHeader = std::make_shared<LoxoneHeader>();
  BaseLib::Http http;
  BaseLib::WebSocket websocket;
  try {
    try {
      _tcpSocket->Open();
      if (_tcpSocket->Connected()) {
        _out.printInfo("Info: Successfully connected.");
        _stopped = false;
        _connected = false;
        _bl->threadManager.start(_initThread, true, &Miniserver::init, this);
      }
    }
    catch (const std::exception &ex) {
      _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }

    std::vector<char> buffer(1024);
    uint32_t bytesRead;
    bool more_data = false;
    while (!_stopCallbackThread) {
      try {
        if (_stopped || !_tcpSocket->Connected()) {
          if (_stopCallbackThread) return;
          if (_stopped) _out.printWarning("Warning: Connection to device closed. Trying to reconnect...");
          _out.printDebug("Info: Stop Keep Alive Thread.", 4);
          _bl->threadManager.join(_keepAliveThread);
          _out.printDebug("Info: Stop Refresh Token Thread.", 4);
          _bl->threadManager.join(_refreshTokenThread);
          _tcpSocket->Shutdown();
          http.reset();
          websocket.reset();
          for (int32_t i = 0; i < 30; i++) {
            if (_stopCallbackThread) continue;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
          }
          _tcpSocket->Open();
          if (_tcpSocket->Connected()) {
            _out.printInfo("Info: Successfully connected.");
            _stopped = false;
            _connected = false;
            _bl->threadManager.start(_initThread, true, &Miniserver::init, this);
          }
          continue;
        }

        try {
          bytesRead = _tcpSocket->Read((uint8_t *)buffer.data(), buffer.size(), more_data);
          _out.printDebug(std::string(buffer.begin(), buffer.begin() + bytesRead), 8);
          _out.printDebug(BaseLib::HelperFunctions::getHexString(buffer.data(), bytesRead), 8);
        }
        catch (C1Net::TimeoutException &ex) {
          if (_stopCallbackThread) continue;
          continue;
        }
        if (bytesRead <= 0) continue;
        if (bytesRead > 1024) bytesRead = 1024;

        uint32_t processed = 0;
        do {
          if (!http.headerProcessingStarted() && !http.dataProcessingStarted() && !websocket.dataProcessingStarted()) {
            char *raw = buffer.data() + processed;
            if (!strncmp(raw, "HTTP/", 5)) {
              _out.printDebug("Packet is type http....", 6);
              processed = http.process(buffer.data() + processed, bytesRead - processed, false, true);
            }
          }

          if (http.headerProcessingStarted() || http.dataProcessingStarted()) {
            _out.printDebug("Process Http Packet: Http Packet isFinished = " + std::to_string(http.isFinished()), 6);
            if (http.isFinished()) {
              processHttpPacket(http);
              http.reset();
              continue;
            }
            processed = http.process(buffer.data() + processed, bytesRead - processed, false, true);
          } else {
            _out.printDebug("Packet is NOT type http.... it should be websocket", 6);
            _out.printDebug(std::string(buffer.begin() + processed, buffer.begin() + bytesRead), 8);
            _out.printDebug(BaseLib::HelperFunctions::getHexString(buffer.data() + processed, bytesRead - processed), 7);
            processed += websocket.process(buffer.data() + processed, bytesRead - processed);
            _out.printDebug("Process Websocket Packet: Header says content length should be " + std::to_string(websocket.getHeader().length) + " and content length is " + std::to_string(websocket.getContentSize()), 6);
            _out.printDebug("Process Websocket Packet: Processed Bytes are: " + std::to_string(processed), 6);
            _out.printDebug("Remaining Websocket Packet is: " + BaseLib::HelperFunctions::getHexString(buffer.data() + processed, bytesRead - processed), 7);
            if (!websocket.getHeader().parsed) continue;
            if (!websocket.isFinished()) continue;
            if (websocket.getHeader().opcode == BaseLib::WebSocket::Header::Opcode::Enum::binary) {
              _out.printDebug("Websocket Opcode is typ BINARY", 6);
              auto content = websocket.getContent();
              if (content.at(0) == 3 && content.size() == 8) { //Loxone Header starts with 0x03 and is 8 byte long
                _out.printDebug("Websocket BINARY Packet is typ of Loxone Header");

                loxoneHeader->identifier = (LoxoneHeader::Identifier)content.at(1);
                loxoneHeader->loxonePayloadLength = content.at(4) | content.at(5) << 8 | content.at(6) << 16 | content.at(7) << 24;

                if (loxoneHeader->identifier == LoxoneHeader::Identifier::Keepalive_Response) processKeepAlivePacket();
                else if (loxoneHeader->identifier == LoxoneHeader::Identifier::Out_Of_Service_Indicator) processOutOfServiceIndicatorPacket();
              }
                /*fixme
                 * this results in a segfault
                else if(websocket.getContentSize() != loxoneHeader->loxonePayloadLength){
                    _out.printWarning("Websocket content length do not match Loxone Header Payload length. Skipping....");
                    continue;
                }
                if (loxoneHeader->identifier == LoxoneHeader::Identifier::Textmessage) processTextmessagePacket(websocket.getContent());
                */
              else if (loxoneHeader->identifier == LoxoneHeader::Identifier::Textmessage) processTextmessagePacket(websocket.getContent());
              else if (loxoneHeader->identifier == LoxoneHeader::Identifier::Binary_File) processBinaryFilePacket(websocket.getContent());
              else if (loxoneHeader->identifier == LoxoneHeader::Identifier::EventTable_of_Value_States) processEventTableOfValueStatesPacket(websocket.getContent());
              else if (loxoneHeader->identifier == LoxoneHeader::Identifier::EventTable_of_Text_States) processEventTableOfTextStatesPacket(websocket.getContent());
              else if (loxoneHeader->identifier == LoxoneHeader::Identifier::EventTable_of_Daytimer_States) processEventTableOfDaytimerStatesPacket(websocket.getContent());
              else if (loxoneHeader->identifier == LoxoneHeader::Identifier::EventTable_of_Weather_States) processEventTableOfWeatherStatesPacket(websocket.getContent());
              websocket.reset();
            } else if (websocket.getHeader().opcode == BaseLib::WebSocket::Header::Opcode::Enum::text) {
              _out.printDebug("Websocket Opcode is typ TEXT", 6);
              processWsPacket(websocket);
              websocket.reset();
            } else if (websocket.getHeader().opcode == BaseLib::WebSocket::Header::Opcode::Enum::close) {
              _out.printDebug("Websocket Opcode is typ CLOSE: " + std::string(websocket.getContent().begin(), websocket.getContent().end()), 6);
              _stopped = true;
              _connected = false;
              _loxoneEncryption->setToken("");
              processWsPacket(websocket);
            } else {
              _out.printDebug("Websocket Opcode is typ: " + std::to_string((int)websocket.getHeader().opcode));
              websocket.reset();
            }
          }
        } while (processed < bytesRead);
      }
      catch (const std::exception &ex) {
        _stopped = true;
        _connected = false;
        _loxoneEncryption->setToken("");
        _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
      }
    }
  }
  catch (const std::exception &ex) {
    _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
  }
}

void Miniserver::processHttpPacket(BaseLib::Http &http) {
  _out.printDebug("Process Http Packet. Response Code is: " + std::to_string(http.getHeader().responseCode));
  auto loxoneHttpPacket = std::make_shared<LoxoneHttpPacket>(http);
  if (!loxoneHttpPacket) return;
  std::unique_lock<std::mutex> requestsGuard(_responsesMutex);
  auto responsesIterator = _responses.find(loxoneHttpPacket->getControl());
  if (responsesIterator != _responses.end()) {
    auto request = responsesIterator->second;
    requestsGuard.unlock();
    request->response = loxoneHttpPacket;
    {
      std::lock_guard<std::mutex> lock(request->mutex);
      request->mutexReady = true;
    }
    request->conditionVariable.notify_one();
    return;
  } else requestsGuard.unlock();
}

void Miniserver::processWsPacket(BaseLib::WebSocket &webSocket) {
  _out.printDebug("Process Websocket Packet.");
  auto loxoneWsPacket = std::make_shared<LoxoneWsPacket>(webSocket);
  if (!loxoneWsPacket)return;
  if (loxoneWsPacket->isControlEncypted()) {
    std::string control = loxoneWsPacket->getControl();
    //remove "jdev/sys/enc/" at the beginning
    control.erase(0, 13);
    std::string decrypted;
    if (_loxoneEncryption->decryptCommand(control, decrypted) < 0) {
      _out.printError("Error: Could not decrypt Packet");
      _stopped = true;
      _connected = false;
      return;
    }
    loxoneWsPacket->setControl(decrypted);
    _out.printDebug("decrypted Command = " + decrypted, 7);
    _out.printDebug("decrypted Command as hex= " + BaseLib::HelperFunctions::getHexString(decrypted), 7);
  }
  std::unique_lock<std::mutex> requestsGuard(_responsesMutex);

  auto responsesIterator = _responses.find(loxoneWsPacket->getControl());
  if (responsesIterator != _responses.end()) {
    auto request = responsesIterator->second;
    requestsGuard.unlock();
    request->response = loxoneWsPacket;
    {
      std::lock_guard<std::mutex> lock(request->mutex);
      request->mutexReady = true;
    }
    request->conditionVariable.notify_one();
    return;
  } else requestsGuard.unlock();
}

void Miniserver::processTextmessagePacket(std::vector<char> &data) {
  //todo
  if (GD::bl->debugLevel >= 5) _out.printDebug("processTextmessagePacket");
}

void Miniserver::processBinaryFilePacket(std::vector<char> &data) {
  //todo
  if (GD::bl->debugLevel >= 5) _out.printDebug("processBinaryFilePacket");
}
void Miniserver::processEventTableOfValueStatesPacket(std::vector<char> &data) {
  _out.printDebug("processEventTableOfValueStatesPacket");
  try {
    uint32_t processed = 0;
    do {
      std::vector<char> packet(data.begin() + processed, data.begin() + processed + 24);
      processed += packet.size();
      auto loxonePacket = std::make_shared<LoxoneValueStatesPacket>(packet.data());
      raisePacketReceived(loxonePacket);
    } while (processed < data.size());
  }
  catch (const std::exception &ex) {
    _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
  }
}
void Miniserver::processEventTableOfTextStatesPacket(std::vector<char> &data) {
  _out.printDebug("processEventTableOfTextStatesPacket");
  try {
    uint32_t processed = 0;
    do {
      std::vector<uint8_t> packet(data.begin() + processed, data.begin() + processed + 36); //insert the UUID, UUID_ICON and the TEXTLENGTH
      processed += 36;

      uint32_t strLength = packet.at(32) | packet.at(33) << 8 | packet.at(34) << 16 | packet.at(35) << 24;
      packet.reserve(packet.size() + strLength);
      packet.insert(packet.end(), data.begin() + processed, data.begin() + processed + strLength);

      auto loxonePacket = std::make_shared<LoxoneTextStatesPacket>((char *)packet.data(), strLength + 36);

      uint32_t padds = strLength % 4;
      if (padds != 0) {
        padds = 4 - padds;
        strLength += padds;
      }

      processed += strLength;
      raisePacketReceived(loxonePacket);
    } while (processed < data.size());
  }
  catch (const std::exception &ex) {
    _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
  }
}
void Miniserver::processEventTableOfDaytimerStatesPacket(std::vector<char> &data) {
  _out.printDebug("processEventTableOfDaytimerStatesPacket");
  try {
    uint32_t processed = 0;
    do {
      std::vector<uint8_t> packet(data.begin() + processed, data.begin() + processed + 28); //insert the UUID, defValue and nrEntrys
      processed += 28;

      uint32_t nrEntrys = packet.at(24) | packet.at(25) << 8 | packet.at(26) << 16 | packet.at(27) << 24;
      if (nrEntrys > 0) {
        uint32_t lenEntrys = 24 * nrEntrys;

        packet.reserve(packet.size() + lenEntrys);
        packet.insert(packet.end(), data.begin() + processed, data.begin() + processed + lenEntrys);

        processed += lenEntrys;
      }
      auto loxonePacket = std::make_shared<LoxoneDaytimerStatesPacket>((char *)packet.data(), nrEntrys);
      raisePacketReceived(loxonePacket);
    } while (processed < data.size());
  }
  catch (const std::exception &ex) {
    _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
  }
}

void Miniserver::processOutOfServiceIndicatorPacket() {
  //todo: wait a while until next connect retry
  _out.printDebug("processOutOfServiceIndicatorPacket");
}
void Miniserver::processKeepAlivePacket() {
  _out.printDebug("processKeepAlivePacket");

  auto loxoneWsPacket = std::make_shared<LoxoneWsPacket>();
  loxoneWsPacket->setResponseCode(200);
  std::unique_lock<std::mutex> requestsGuard(_responsesMutex);
  auto responsesIterator = _responses.find("keepalive");
  if (responsesIterator != _responses.end()) {
    auto request = responsesIterator->second;
    requestsGuard.unlock();
    request->response = loxoneWsPacket;
    {
      std::lock_guard<std::mutex> lock(request->mutex);
      request->mutexReady = true;
    }
    request->conditionVariable.notify_one();
    return;
  } else requestsGuard.unlock();
}
void Miniserver::processEventTableOfWeatherStatesPacket(std::vector<char> &data) {
  _out.printDebug("processEventTableOfWeatherStatesPacket");
  try {
    uint32_t processed = 0;
    do {
      std::vector<uint8_t> packet(data.begin() + processed, data.begin() + processed + 24); //insert the UUID, lastUpdate and nrEntrys
      processed += 24;

      uint32_t nrEntrys = packet.at(20) | packet.at(21) << 8 | packet.at(22) << 16 | packet.at(23) << 24;
      if (nrEntrys > 0) {
        uint32_t lenEntrys = 68 * nrEntrys;

        packet.reserve(packet.size() + lenEntrys);
        packet.insert(packet.end(), data.begin() + processed, data.begin() + processed + lenEntrys);

        processed += lenEntrys;
      }
      auto loxonePacket = std::make_shared<LoxoneWeatherStatesPacket>((char *)packet.data(), nrEntrys);
      raisePacketReceived(loxonePacket);
    } while (processed < data.size());
  }
  catch (const std::exception &ex) {
    _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
  }
}

PLoxonePacket Miniserver::getResponse(const std::string &responseCommand, const std::string &command, int32_t waitForSeconds) {
  try {
    if (_stopped) return PLoxonePacket();

    std::lock_guard<std::mutex> sendPacketGuard(_sendPacketMutex);
    std::lock_guard<std::mutex> getResponseGuard(_getResponseMutex);
    std::shared_ptr<Request> request = std::make_shared<Request>();
    std::unique_lock<std::mutex> requestsGuard(_responsesMutex);
    _responses[responseCommand] = request;
    requestsGuard.unlock();
    std::unique_lock<std::mutex> lock(request->mutex);

    try {
      GD::out.printInfo("Info: Sending command " + command);
      _tcpSocket->Send((uint8_t *)command.data(), command.size());

      _lastPacketSent = BaseLib::HelperFunctions::getTime();
    }
    catch (const C1Net::Exception &ex) {
      _out.printError("Error sending packet: " + std::string(ex.what()));
      startListening();
      return PLoxonePacket();
    }

    int32_t i = 0;
    while (!request->conditionVariable.wait_for(lock, std::chrono::milliseconds(1000), [&] {
      i++;
      return request->mutexReady || _stopped || i == waitForSeconds;
    }));

    if (i == waitForSeconds || !request->response) {
      _out.printError("Error: No response received to command: " + command);
      return PLoxonePacket();
    }
    auto responsePacket = request->response;

    requestsGuard.lock();
    _responses.erase(responseCommand);
    requestsGuard.unlock();

    return responsePacket;
  }
  catch (const std::exception &ex) {
    _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
  }
  return PLoxonePacket();
}

std::string Miniserver::encodeWebSocket(const std::string &command, WebSocket::Header::Opcode::Enum messageType) {
  std::vector<char> output;
  std::vector<char> input(command.begin(), command.end());
  BaseLib::WebSocket::encode(input, messageType, output);
  return std::string(output.begin(), output.end());
}

PVariable Miniserver::getNewStructfile() {
  _out.printDebug("getNewStructfile");
  auto responsePacket = getResponse("newStuctfile", encodeWebSocket("data/LoxAPP3.json", WebSocket::Header::Opcode::Enum::text));
  if (!responsePacket) {
    _out.printError("Error: Could not get new Structfile from miniserver.");
    _stopped = true;
    _connected = false;
    return PVariable();
  }
  auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
  if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200) {
    _out.printError("Error: Could not get new Structfile from miniserver.");
    _stopped = true;
    _connected = false;
    return PVariable();
  }
  return loxoneWsPacket->getValue();
}

PVariable Miniserver::getLoxApp3Version() {
  _out.printDebug("getLoxApp3Version");
  auto responsePacket = getResponse("dev/sps/LoxAPPversion3", encodeWebSocket("jdev/sps/LoxAPPversion3", WebSocket::Header::Opcode::Enum::text));
  if (!responsePacket) {
    _out.printError("Error: Could not get LoxApp3Version from miniserver.");
    _stopped = true;
    _connected = false;
    return PVariable();
  }
  auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
  if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200) {
    _out.printError("Error: Could not get LoxApp3Version from miniserver.");
    _stopped = true;
    _connected = false;
    return PVariable();
  }
  return loxoneWsPacket->getValue();
}
void Miniserver::disconnect() {
  _out.printDebug("Disconnect from Miniserver");
  std::vector<char> output;
  BaseLib::WebSocket::encodeClose(output);
  std::string command(output.begin(), output.end());

  auto responsePacket = getResponse("close", command);
  if (!responsePacket) {
    _out.printError("Error: Could not disconnect from miniserver.");
    _stopped = true;
    _connected = false;
    return;
  }
  auto loxoneWsPacket = std::dynamic_pointer_cast<LoxoneWsPacket>(responsePacket);
  if (!loxoneWsPacket || loxoneWsPacket->getResponseCode() != 200) {
    _out.printError("Error: Could not disconnect from miniserver.");
    _stopped = true;
    _connected = false;
    return;
  }
  stopListening();
}
}
