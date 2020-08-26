#include "Alarm.h"

namespace Loxone
{
    Alarm::Alarm(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x0200)
    {
    }
    Alarm::Alarm(std::shared_ptr<BaseLib::Database::DataTable> rows): LoxoneControl(rows, 0x0200)
    {
    }
    uint32_t Alarm::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
    {
        LoxoneControl::getDataToSave(list, peerID);
        return 0;
    }
}