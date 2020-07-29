#include "Slider.h"

namespace Loxone
{
    Slider::Slider(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x0101)
    {
        getValueFromStructFile("isFavorite", "", _isFavorite);
        getValueFromStructFile("defaultIcon", "", _defaultIcon);

        getValueFromStructFile("format", "details", _detFormat);
        getValueFromStructFile("min", "details", _detMin);
        getValueFromStructFile("max", "details", _detMax);
        getValueFromStructFile("step", "details", _detStep);
    }
    Slider::Slider(std::shared_ptr<BaseLib::Database::DataTable> rows) : LoxoneControl(rows, 0x0101)
    {
        getValueFromDataTable(107, _isFavorite);
        getBinaryValueFromDataTable(108, _defaultIcon);
        getBinaryValueFromDataTable(111, _detFormat);
        getValueFromDataTable(112, _detMin);
        getValueFromDataTable(113, _detMax);
        getValueFromDataTable(114, _detStep);
    }
    uint32_t Slider::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
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
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(108)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("defaultIcon")));
            std::vector<char> defaultIcon(_defaultIcon.begin(), _defaultIcon.end());
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(defaultIcon)));
            list.push_back(data);
        }
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(111)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("format")));
            std::vector<char> detFormat(_detFormat.begin(), _detFormat.end());
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(_detFormat)));
            list.push_back(data);
        }
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(112)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(_detMin)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("min")));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            list.push_back(data);
        }
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(113)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(_detMax)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("max")));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            list.push_back(data);
        }
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(114)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(_detStep)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("step")));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            list.push_back(data);
        }
        return 0;
    }
}