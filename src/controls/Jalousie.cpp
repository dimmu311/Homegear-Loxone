#include "Jalousie.h"

namespace Loxone
{
    Jalousie::Jalousie(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x0104)
    {

    }
    Jalousie::Jalousie(std::shared_ptr<BaseLib::Database::DataTable> rows) : LoxoneControl(rows, 0x0104)
    {

    }
    uint32_t Jalousie::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
    {
        LoxoneControl::getDataToSave(list, peerID);
        return 0;
    }
}