#include "LightControllerV2.h"

namespace Loxone
{
    LightControllerV2::LightControllerV2(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x0103)
    {
        getValueFromStructFile("isFavorite", "", _isFavorite);

        getValueFromStructFile("movementScene", "details", _detMovementScene);
        getValueFromStructFile("masterValue", "details", _detMasterValue);
        getValueFromStructFile("masterColor", "details", _detMasterColor);
    }
    LightControllerV2::LightControllerV2(std::shared_ptr<BaseLib::Database::DataTable> rows): LoxoneControl(rows, 0x0103)
    {
        getValueFromDataTable(107, _isFavorite);
        getValueFromDataTable(111, _detMovementScene);
        getBinaryValueFromDataTable(112, _detMasterValue);
        getBinaryValueFromDataTable(113, _detMasterColor);
    }

    uint32_t LightControllerV2::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
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
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(111)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(_detMovementScene)));
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
            std::vector<char> masterValue(_detMasterValue.begin(), _detMasterValue.end());
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(masterValue)));
            list.push_back(data);
        }
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(113)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("masterColor")));
            std::vector<char> masterColor(_detMasterColor.begin(), _detMasterColor.end());
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(masterColor)));
            list.push_back(data);
        }
        return 0;
    }
}