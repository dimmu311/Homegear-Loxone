#include "LightControllerV2.h"

namespace Loxone
{
    LightControllerV2::LightControllerV2(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x0103)
    {
        getValueFromStructFile("movementScene", "details", _MovementScene);
        getValueFromStructFile("masterValue", "details", _MasterValue);
        getValueFromStructFile("masterColor", "details", _MasterColor);
    }
    LightControllerV2::LightControllerV2(std::shared_ptr<BaseLib::Database::DataTable> rows): LoxoneControl(rows, 0x0103)
    {
        getValueFromDataTable(111, _MovementScene);
        getBinaryValueFromDataTable(112, _MasterValue);
        getBinaryValueFromDataTable(113, _MasterColor);
    }

    uint32_t LightControllerV2::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
    {
        LoxoneControl::getDataToSave(list, peerID);
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(111)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(_MovementScene)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("movementScene")));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            list.push_back(data);
        }
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(112)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("masterValue")));
            std::vector<char> masterValue(_MasterValue.begin(), _MasterValue.end());
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(masterValue)));
            list.push_back(data);
        }
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(113)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("masterColor")));
            std::vector<char> masterColor(_MasterColor.begin(), _MasterColor.end());
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(masterColor)));
            list.push_back(data);
        }
        return 0;
    }
}