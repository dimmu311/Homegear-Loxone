#ifndef LOXONE_MINISERVER_MUSICSERVER_H
#define LOXONE_MINISERVER_MUSICSERVER_H

#include <memory>
#include <homegear-base/Systems/PhysicalInterfaceSettings.h>
#include <homegear-base/Sockets/TcpSocket.h>
#include <homegear-base/Encoding/Http.h>
#include <homegear-base/Encoding/JsonEncoder.h>
#include <homegear-base/Encoding/WebSocket.h>


namespace Loxone {
    class Musicserver
            {
    public:
        Musicserver(std::shared_ptr<BaseLib::Systems::PhysicalInterfaceSettings> settings);
        ~Musicserver();

    protected:
        BaseLib::Output _out;
        std::unique_ptr<BaseLib::Rpc::JsonEncoder> _jsonEncoder;

        std::unique_ptr <BaseLib::TcpSocket> _server;
        BaseLib::Http _http;
        BaseLib::WebSocket _webSocket;

        void packetReceived(int32_t clientId, BaseLib::TcpSocket::TcpPacket &packet);
        bool encodeWebsocketUpgrade(BaseLib::Http http, std::vector<char> &packet);

        void getAudioCfgAll(const std::string &url, std::vector<char> &packet);
        void getAudioCfgEqualizer(const std::string &url, std::vector<char> &packet);
        void getAudioCfgMaster(const std::string &url, std::vector<char> &packet);
        void getAudioCfgPlayersDetails(const std::string &url, std::vector<char> &packet);
        void getAudioCfgGetRoomFavs(const std::string &url, std::vector<char> &packet);
        void getAudioCfgGetSyncedPlayers(const std::string &url, std::vector<char> &packet);
        void getAudioCfgIAmAMiniserver(const std::string &url, std::vector<char> &packet);
        void getAudioCfgMac(const std::string &url, std::vector<char> &packet);

        void getEmpty(const std::string &url, std::vector<char> &packet);

        /*
        /audio/cfg/all -> getAudioCfgAll
        /audio/cfg/equalizer -> getAudiCfgEqualizer
        /audio/cfg/getfavorites -> getEmpty
        /audio/cfg/getinputs -> getEmpty
        /audio/cfg/getmaster -> getAudioCfgMaster
        /audio/cfg/getpairedmaster -> getAudioCfgMaster
        /audio/cfg/getplayersdetails -> getAudioCfgPlayersDetails
        /audio/cfg/getradios -> getEmpty
        /audio/cfg/getroomfavs -> getAudioCfgGetRoomFavs
        /audio/cfg/getservices -> getEmpty
        /audio/cfg/getsyncedplayers -> getAudioCfgSyncedPlayers
        /audio/cfg/iamaminiserver/192.168.41.131 -> getAudioCfgIAmAMiniserver
        /audio/cfg/mac -> getAudioCfgMac

        /audio/1/pause -> getAudioPause
        /audio/1/play -> getAudioPlay
        /audio/1/position -> getAudioPosition
        /audio/1/queueminus -> getAudioQueueMinus
        /audio/1/queueplus -> getAudioQueuePlus
        /audio/1/repeat -> getAudioRepeat
        /audio/1/shuffle -> getAudioShuffle
        /audio/1/volume -> getAudioVolume

        /audio/cfg/miniserverinit/192.168.41.131
        /audio/cfg/miniserverport/80

        */
    };
}

#endif //LOXONE_MINISERVER_MUSICSERVER_H
