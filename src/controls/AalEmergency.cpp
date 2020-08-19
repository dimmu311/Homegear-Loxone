#include "AalEmergency.h"

namespace Loxone
{
    AalEmergency::AalEmergency(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x0400)
    {
        getValueFromStructFile("isFavorite", "", _isFavorite);
    }
    AalEmergency::AalEmergency(std::shared_ptr<BaseLib::Database::DataTable> rows) : LoxoneControl(rows, 0x0400)
    {
        getValueFromDataTable(107, _isFavorite);
    }
    uint32_t AalEmergency::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
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

    uint32_t AalEmergency::getExtraData(std::list<extraData> &extraData)
    {
        return -1;
    }
}