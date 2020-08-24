#ifndef HOMEGEAR_LOXONE_MINISERVER_MEDIACLIENT_H
#define HOMEGEAR_LOXONE_MINISERVER_MEDIACLIENT_H

#include "../LoxoneControl.h"

namespace Loxone
{
    class MediaClient : public LoxoneControl
    {
        /*
            "14eef2d6-03ce-80a8-ffff1239b12ff514":{
                "name":"Music Server Zone",
                "type":"MediaClient",
                "uuidAction":"14eef2d6-03ce-80a8-ffff1239b12ff514",
                "room":"0df9e099-020c-03a6-ffff1239b12ff514",
                "cat":"0df9e099-01dd-0320-ffff1239b12ff514",
                "defaultRating":0,
                "isFavorite":false,
                "isSecured":false,
                "details":{"
                    playerid":-1,
                    "clientType":0,
                    "parameterConnections":0
                },
                "states":{
                    "power":"14eef2d6-03ce-8057-ffff9bb655d4afc8",
                    "serverState":"14eef2d6-03ce-8067-ffff9bb655d4afc8",
                    "playState":"14eef2d6-03ce-805b-ffff9bb655d4afc8",
                    "clientState":"14eef2d6-03ce-8068-ffff9bb655d4afc8",
                    "volume":"14eef2d6-03ce-8058-ffff9bb655d4afc8",
                    "maxVolume":"14eef2d6-03ce-8059-ffff9bb655d4afc8",
                    "mute":"14eef2d6-03ce-805c-ffff9bb655d4afc8",
                    "shuffle":"14eef2d6-03ce-805f-ffff9bb655d4afc8",
                    "repeat":"14eef2d6-03ce-8060-ffff9bb655d4afc8",
                    "source":"14eef2d6-03ce-805a-ffff9bb655d4afc8",
                    "sourceList":"14eef2d6-03ce-805e-ffff9bb655d4afc8",
                    "songName":"14eef2d6-03ce-805d-ffff9bb655d4afc8",
                    "duration":"14eef2d6-03ce-8062-ffff9bb655d4afc8",
                    "progress":"14eef2d6-03ce-8061-ffff9bb655d4afc8",
                    "album":"14eef2d6-03ce-8063-ffff9bb655d4afc8",
                    "artist":"14eef2d6-03ce-8064-ffff9bb655d4afc8",
                    "station":"14eef2d6-03ce-806a-ffff9bb655d4afc8",
                    "genre":"14eef2d6-03ce-8065-ffff9bb655d4afc8",
                    "cover":"14eef2d6-03ce-8066-ffff9bb655d4afc8",
                    "volumeStep":"14eef2d6-03ce-8069-ffff9bb655d4afc8",
                    "queueIndex":"14eef2d6-03ce-806d-ffff9bb655d4afc8",
                    "enableAirPlay":"14eef2d6-03ce-806e-ffff9bb655d4afc8",
                    "enableSpotifyConnect":"14eef2d6-03ce-806f-ffff9bb655d4afc8",
                    "alarmVolume":"14eef2d6-03ce-8098-ffff9bb655d4afc8",
                    "bellVolume":"14eef2d6-03ce-8099-ffff9bb655d4afc8",
                    "buzzerVolume":"14eef2d6-03ce-809a-ffff9bb655d4afc8",
                    "ttsVolume":"14eef2d6-03ce-809b-ffff9bb655d4afc8",
                    "ttsVolume":"14eef2d6-03ce-809b-ffff9bb655d4afc8",
                    "defaultVolume":"14eef2d6-03ce-8097-ffff9bb655d4afc8",
                    "equalizerSettings":"14eef2d6-03ce-8070-ffff9bb655d4afc8"
                }
            }
        */
    public:
        MediaClient(PVariable control, std::string room, std::string cat);
        MediaClient(std::shared_ptr<BaseLib::Database::DataTable> rows);
        uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
    protected:
    };
}

#endif //HOMEGEAR_LOXONE_MINISERVER_MEDIACLIENT_H
