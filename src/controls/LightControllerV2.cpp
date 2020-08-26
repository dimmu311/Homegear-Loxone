#include "LightControllerV2.h"

namespace Loxone
{
    LightControllerV2::LightControllerV2(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x0103)
    {

    }
    LightControllerV2::LightControllerV2(std::shared_ptr<BaseLib::Database::DataTable> rows): LoxoneControl(rows, 0x0103)
    {

    }

    uint32_t LightControllerV2::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
    {
        LoxoneControl::getDataToSave(list, peerID);
        return 0;
    }
}