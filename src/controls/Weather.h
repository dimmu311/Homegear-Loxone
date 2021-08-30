#ifndef WEATHER_H
#define WEATHER_H

#include "../LoxoneControl.h"
#include "../LoxoneLoxApp3.h"

namespace Loxone
{
class Weather : public LoxoneControl
{
    public:
        Weather(LoxoneLoxApp3::weather weather);
        Weather(std::shared_ptr<BaseLib::Database::DataTable> rows);

        //bool setValue(PPacket frame, PVariable parameters, uint32_t channel, std::string &command, bool &isSecured);
        //bool setValue(uint32_t channel, std::string valueKey, PVariable value, std::unordered_map<uint32_t, std::unordered_map<std::string, Systems::RpcConfigurationParameter>> &valuesCentral, std::string &command, bool &isSecured);
        bool packetReceived(uint32_t peerID, PLoxoneDaytimerStatesPacket loxonePacket, std::unordered_map<uint32_t, std::unordered_map<std::string, Systems::RpcConfigurationParameter>> &valuesCentral);

    protected:
};
}

#endif //WEATHER_H
