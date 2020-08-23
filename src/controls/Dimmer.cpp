#include "Dimmer.h"

namespace Loxone
{
    Dimmer::Dimmer(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x0102)
    {
    }
    Dimmer::Dimmer(std::shared_ptr<BaseLib::Database::DataTable> rows): LoxoneControl(rows, 0x0102)
    {
    }
    uint32_t Dimmer::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
    {
        LoxoneControl::getDataToSave(list, peerID);
        return 0;
    }
}