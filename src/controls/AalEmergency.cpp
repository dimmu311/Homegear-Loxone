#include "AalEmergency.h"

namespace Loxone
{
    AalEmergency::AalEmergency(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x0400)
    {
    }
    AalEmergency::AalEmergency(std::shared_ptr<BaseLib::Database::DataTable> rows) : LoxoneControl(rows, 0x0400)
    {
    }
    uint32_t AalEmergency::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
    {
        LoxoneControl::getDataToSave(list, peerID);
        return 0;
    }

    uint32_t AalEmergency::getExtraData(std::list<extraData> &extraData)
    {
        return -1;
    }
}