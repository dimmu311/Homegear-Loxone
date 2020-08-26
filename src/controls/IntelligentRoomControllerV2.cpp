#include "IntelligentRoomControllerV2.h"

namespace Loxone
{
    IntelligentRoomControllerV2::IntelligentRoomControllerV2(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x0301)
    {

    }
    IntelligentRoomControllerV2::IntelligentRoomControllerV2(std::shared_ptr<BaseLib::Database::DataTable> rows) : LoxoneControl(rows, 0x0301)
    {

    }
    uint32_t IntelligentRoomControllerV2::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
    {
        LoxoneControl::getDataToSave(list, peerID);
        return 0;
    }
}