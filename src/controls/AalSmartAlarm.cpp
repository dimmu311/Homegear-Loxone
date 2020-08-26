#include "AalSmartAlarm.h"

namespace Loxone
{
    AalSmartAlarm::AalSmartAlarm(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x0401)
    {
    }
    AalSmartAlarm::AalSmartAlarm(std::shared_ptr<BaseLib::Database::DataTable> rows): LoxoneControl(rows, 0x0401)
    {
    }
}