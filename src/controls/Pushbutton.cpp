#include "Pushbutton.h"

namespace Loxone
{
    Pushbutton::Pushbutton(PVariable control, std::unordered_map<std::string, std::string> &room, std::unordered_map<std::string, std::string> &cat) : LoxoneControl(control, room, cat,0x100)
    {
        getValueFromStructFile("defaultIcon", "", _defaultIcon);
    }

    Pushbutton::Pushbutton(std::shared_ptr<BaseLib::Database::DataTable> rows) : LoxoneControl(rows, 0x100)
    {
        getBinaryValueFromDataTable(109, _defaultIcon);
    }

    uint32_t Pushbutton::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID) {
        LoxoneControl::getDataToSave(list, peerID);
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(109)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("defaultIcon")));
            std::vector<char> defaultIcon(_defaultIcon.begin(), _defaultIcon.end());
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(defaultIcon)));
            list.push_back(data);
        }
        return 0;
    }
}