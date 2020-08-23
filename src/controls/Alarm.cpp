#include "Alarm.h"

namespace Loxone
{
    Alarm::Alarm(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x0200)
    {
        getValueFromStructFile("alert", "details", _Alert);
        getValueFromStructFile("presenceConnected", "details", _PresenceConnected);
    }
    Alarm::Alarm(std::shared_ptr<BaseLib::Database::DataTable> rows): LoxoneControl(rows, 0x0200)
    {
        getValueFromDataTable(111, _Alert);
        getValueFromDataTable(112, _PresenceConnected);
    }
    uint32_t Alarm::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
    {
        LoxoneControl::getDataToSave(list, peerID);
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(111)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(_Alert)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("detAlert")));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            list.push_back(data);
        }
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(112)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(_PresenceConnected)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("detPresenceConnected")));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            list.push_back(data);
        }
        return 0;
    }
}