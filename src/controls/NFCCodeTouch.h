#ifndef NFCCODETOUCH_H
#define NFCCODETOUCH_H

#include "../LoxoneControl.h"

namespace Loxone {
    class NFCCodeTouch : public LoxoneControl {

    public:
        NFCCodeTouch(PVariable control, std::unordered_map<std::string, std::string> &room, std::unordered_map<std::string, std::string> &cat);
        NFCCodeTouch(std::shared_ptr<BaseLib::Database::DataTable> rows, uint32_t typeNr);
        bool setValue(PPacket frame, PVariable parameters, uint32_t channel, std::string &command, bool &isSecured);
    protected:
    };
}

#endif //NFCCODETOUCH_H
