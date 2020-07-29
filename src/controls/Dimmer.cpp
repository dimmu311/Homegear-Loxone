#include "Dimmer.h"

namespace Loxone
{
    Dimmer::Dimmer(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x0102)
    {
        getValueFromStructFile("isFavorite", "", _isFavorite);
    }
    Dimmer::Dimmer(std::shared_ptr<BaseLib::Database::DataTable> rows): LoxoneControl(rows, 0x0102)
    {
        getValueFromDataTable(107, _isFavorite);
    }
    uint32_t Dimmer::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
    {
        LoxoneControl::getDataToSave(list, peerID);
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(107)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(_isFavorite)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("isFavorite")));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            list.push_back(data);
        }
        return 0;
    }
}