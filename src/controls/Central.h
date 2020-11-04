#ifndef LOXONE_MINISERVER_CENTRAL_H
#define LOXONE_MINISERVER_CENTRAL_H

#include "../LoxoneControl.h"

namespace Loxone {

    class Central : public LoxoneControl {
    public:
        Central(PVariable control, std::string room, std::string cat, uint32_t typeNr);
        Central(std::shared_ptr<BaseLib::Database::DataTable> rows, uint32_t typeNr);

        uint32_t getExtraData(std::list<extraData> &extraData);
        bool setValue(PPacket frame, PVariable parameters, uint32_t channel, std::string &command, bool &isSecured);
    protected:
    };
}

#endif //LOXONE_MINISERVER_CENTRAL_H
