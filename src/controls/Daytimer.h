#ifndef LOXONE_MINISERVER_DAYTIMER_H
#define LOXONE_MINISERVER_DAYTIMER_H

#include "../LoxoneControl.h"

namespace Loxone {
    class Daytimer : public LoxoneControl {

    public:
        Daytimer(PVariable control, std::string room, std::string cat, uint32_t typeNr);
        Daytimer(std::shared_ptr<BaseLib::Database::DataTable> rows, uint32_t typeNr);

        bool setValue(PPacket frame, PVariable parameters, uint32_t channel, std::string &command, bool &isSecured);
        bool setValue(uint32_t channel, std::string valueKey, PVariable value, std::unordered_map<uint32_t, std::unordered_map<std::string, Systems::RpcConfigurationParameter>> &valuesCentral, std::string &command, bool &isSecured);
        bool packetReceived(uint32_t peerID, PLoxoneDaytimerStatesPacket loxonePacket, std::unordered_map<uint32_t, std::unordered_map<std::string, Systems::RpcConfigurationParameter>> &valuesCentral);

    protected:
    };
}


#endif //LOXONE_MINISERVER_DAYTIMER_H
