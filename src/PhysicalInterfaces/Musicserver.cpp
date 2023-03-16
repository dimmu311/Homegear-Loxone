#include "Musicserver.h"
#include "../Loxone.h"
#include "../GD.h"

namespace Loxone {
    Musicserver::Musicserver(
            std::shared_ptr<BaseLib::Systems::PhysicalInterfaceSettings> settings) //: IPhysicalInterface(GD::bl, GD::family->getFamily(), settings)
    {
        _out.init(GD::bl);
        _out.setPrefix(GD::out.getPrefix() + "Loxone Musicserver \"" + settings->id + "\": ");

        _jsonEncoder = std::unique_ptr<BaseLib::Rpc::JsonEncoder>(new BaseLib::Rpc::JsonEncoder(GD::bl));
       try {
            C1Net::TcpServer::TcpServerInfo serverInfo;
            serverInfo.packet_received_callback = std::bind(&Musicserver::packetReceived, this, std::placeholders::_1,
                                                          std::placeholders::_2);
            _server = std::make_unique<C1Net::TcpServer>(serverInfo);

            std::string address = BaseLib::Net::getMyIpAddress();
            std::string listenAddress;
            _server->startServer(address, "7091", listenAddress);
            _out.printInfo("Info: Server is now listening on address " + listenAddress + ".");
        }
        catch (BaseLib::Exception &ex) {
            _out.printError("Error starting server: " + std::string(ex.what()));
        }
    }

    Musicserver::~Musicserver() {
        _server->stopServer();
        _server->waitForServerStopped();
    }

    void Musicserver::packetReceived(const C1Net::TcpServer::PTcpClientData &client_data, const C1Net::TcpPacket &packet) {

        std::string x(packet.begin(), packet.end());
        _out.printInfo("MUSICSERVER_FAKE packetReceived-> " + x);

        if(_http.dataProcessingStarted() || !strncmp(reinterpret_cast<const char *>(packet.data()), "GET ", 4) || !strncmp(
                reinterpret_cast<const char *>(packet.data()), "HEAD ", 5)
           || !strncmp(reinterpret_cast<const char *>(packet.data()), "DELETE ", 7)) {

            uint32_t processedBytes = 0;
            while (processedBytes < packet.size()) {
                processedBytes += _http.process((char *) packet.data(), packet.size());
                if (_http.isFinished()) {
                    _out.printInfo("MUSICSERVER_FAKE http is ready");
                    if (_http.getHeader().responseCode == 101) {
                        std::vector<char> httpPacket;
                        if (!encodeWebsocketUpgrade(_http, httpPacket)) return;
                        _server->Send(client_data, httpPacket);

                        _out.printInfo("send WebsocketUpgrade");

                        std::string in ("LWSS V 2.3.9.2 | ~API:1.6~");
                        std::vector<char> input(in.begin(), in.end());
                        httpPacket.clear();
                        BaseLib::WebSocket::encode(input, WebSocket::Header::Opcode::Enum::text, httpPacket);
                        _server->Send(client_data, httpPacket);
                        _out.printInfo("send API");
                    }
                    _http.reset();
                }
            }
            return;
        }

        //if(_webSocket.dataProcessingStarted())
        uint32_t processedBytes = 0;
        while (processedBytes < packet.size()) {
            processedBytes += _webSocket.process((char *) packet.data(), packet.size());
            if (_webSocket.isFinished()) {
                std::string content(_webSocket.getContent().begin(), _webSocket.getContent().end());
                //_out.printInfo("MUSICSERVER_FAKE WS prcessed::::::" + content);

                std::vector<char> wsPacket;
                if (std::regex_match(content, std::regex("(?:^|/)audio/cfg/all"))) {
                    _out.printInfo("MATCH audio/cfg/all");
                    getAudioCfgAll(content, wsPacket);
                }
                if (std::regex_match(content, std::regex("(?:^|/)audio/cfg/getsyncedplayers"))) {
                    _out.printInfo("MATCH getsyncedplayers");
                    getAudioCfgGetSyncedPlayers(content, wsPacket);
                }
                if (std::regex_match(content, std::regex("(?:^|/)audio/cfg/iamaminiserver/(.*)"))) {
                    _out.printInfo("MATCH miniserver");
                    getEmpty(content, wsPacket);
                }
                if (std::regex_match(content, std::regex("(?:^|/)audio/cfg/miniserverport/(.*)"))) {
                    _out.printInfo("MATCH port");
                    getEmpty(content, wsPacket);
                }
                if (!wsPacket.empty()) _server->Send(client_data, wsPacket);
                _webSocket.reset();
                return;
            }
        }
    }

    bool Musicserver::encodeWebsocketUpgrade(BaseLib::Http http, std::vector<char> &packet) {
        _out.printInfo("encodeWebsocketUpgrade");

        std::string websocketKey =
                http.getHeader().fields["sec-websocket-key"] + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        std::vector<char> data(websocketKey.begin(), websocketKey.end());
        std::vector<char> sha1;
        BaseLib::Security::Hash::sha1(data, sha1);
        std::string websocketAccept;
        BaseLib::Base64::encode(sha1, websocketAccept);

        std::string header;
        header.reserve(133 + websocketAccept.size());
        header.append("HTTP/1.1 101 Switching Protocols\r\n");
        header.append("Connection: Upgrade\r\n");
        header.append("Upgrade: websocket\r\n");
        header.append("Sec-WebSocket-Accept: ").append(websocketAccept).append("\r\n");
        header.append("\r\n");

        packet = std::vector<char>(header.begin(), header.end());
        return true;
    }


    void Musicserver::getAudioCfgAll(const std::string &url, std::vector<char> &packet) {
        _out.printInfo("getAudioCfgAll");

        std::string contentString = "{\"configall_result\":[{\"airplay\":false,\"dns\":\"8.8.8.8\",\"errortts\":false,\"gateway\":\"0.0.0.0\",\"hostname\":\"loxberry-music-server-9090\",\"ip\":\"0.255.255.255\",\"language\":\"en\",\"lastconfig\":\"\",\"macaddress\":\"aa:bb:cc:dd:ee:ff\",\"mask\":\"255.255.255.255\",\"master\":true,\"maxplayers\":4,\"ntp\":\"0.europe.pool.ntp.org\",\"upnplicences\":0,\"usetrigger\":false,\"players\":[{\"playerid\":1,\"clienttype\":0,\"default_volume\":30,\"enabled\":true,\"internalname\":\"zone-1\",\"max_volume\":100,\"name\":\"Zone1\",\"upnpmode\":0,\"upnppredelay\":0},{\"playerid\":2,\"clienttype\":0,\"default_volume\":30,\"enabled\":true,\"internalname\":\"zone-1\",\"max_volume\":100,\"name\":\"Zone1\",\"upnpmode\":0,\"upnppredelay\":0},{\"playerid\":3,\"clienttype\":0,\"default_volume\":30,\"enabled\":true,\"internalname\":\"zone-1\",\"max_volume\":100,\"name\":\"Zone1\",\"upnpmode\":0,\"upnppredelay\":0},{\"playerid\":4,\"clienttype\":0,\"default_volume\":30,\"enabled\":true,\"internalname\":\"zone-1\",\"max_volume\":100,\"name\":\"Zone1\",\"upnpmode\":0,\"upnppredelay\":0}]},{\"command\":\"/audio/cfg/all\"}]}";
        std::vector<char> input(contentString.begin(), contentString.end());
        BaseLib::WebSocket::encode(input, WebSocket::Header::Opcode::Enum::text, packet);
    }

    void Musicserver::getAudioCfgEqualizer(const std::string &url, std::vector<char> &packet) {

    }

    void Musicserver::getAudioCfgMaster(const std::string &url, std::vector<char> &packet) {

    }

    void Musicserver::getAudioCfgPlayersDetails(const std::string &url, std::vector<char> &packet) {

    }

    void Musicserver::getAudioCfgGetRoomFavs(const std::string &url, std::vector<char> &packet) {

    }

    void Musicserver::getAudioCfgGetSyncedPlayers(const std::string &url, std::vector<char> &packet) {
        _out.printInfo("getAudioCfgGetSyncedPlayers");

        std::vector<std::string> strings;
        std::stringstream f(url);
        std::string s;
        while (getline(f, s, '/')) {
            strings.push_back(s);
            //_out.printInfo(s);
        }

        PVariable value = std::make_shared<Variable>(VariableType::tStruct);
        value->structValue->operator[]("command") = PVariable(new Variable(url));
        value->structValue->operator[](strings.at(2) + "_result") = PVariable(new Variable(VariableType::tArray));

        _jsonEncoder->encode(value, packet);

        std::string jstr(packet.begin(), packet.end());
        _out.printInfo("JSON STRING" + jstr);
        /*
        {
             "getsyncedplayers_result": null,
             "command": "audio/cfg/getsyncedplayers"
        }
        */
    }

    void Musicserver::getAudioCfgIAmAMiniserver(const std::string &url, std::vector<char> &packet) {

    }

    void Musicserver::getAudioCfgMac(const std::string &url, std::vector<char> &packet) {

    }

    void Musicserver::getEmpty(const std::string &url, std::vector<char> &packet) {
        _out.printInfo("getEmpty");

        std::vector<std::string> strings;
        std::stringstream f(url);
        std::string s;
        while (getline(f, s, '/')) {
            strings.push_back(s);
            //_out.printInfo(s);
        }

        PVariable value = std::make_shared<Variable>(VariableType::tStruct);
        value->structValue->operator[]("command") = PVariable(new Variable(url));
        value->structValue->operator[](strings.at(2) + "_result") = PVariable(new Variable(VariableType::tVoid));

        _jsonEncoder->encode(value, packet);

        std::string jstr(packet.begin(), packet.end());
        _out.printInfo("JSON STRING" + jstr);
    }
}