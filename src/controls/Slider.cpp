#include "Slider.h"

namespace Loxone
{
    Slider::Slider(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x0101)
    {
        getValueFromStructFile("isFavorite", "", _isFavorite);
        getValueFromStructFile("defaultIcon", "", _defaultIcon);

        getValueFromStructFile("format", "details", _format);
        getValueFromStructFile("min", "details", _min);
        getValueFromStructFile("max", "details", _max);
        getValueFromStructFile("step", "details", _step);
    }
    Slider::Slider(std::shared_ptr<BaseLib::Database::DataTable> rows) : LoxoneControl(rows, 0x0101)
    {
        getValueFromDataTable(107, _isFavorite);
        getBinaryValueFromDataTable(108, _defaultIcon);
        getBinaryValueFromDataTable(111, _format);
        getValueFromDataTable(112, _min);
        getValueFromDataTable(113, _max);
        getValueFromDataTable(114, _step);
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
        //TODO, maybe this must not be safed anymore because it is safed to a config variable.
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(111)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("format")));
            std::vector<char> format(_format.begin(), _format.end());
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(format)));
            list.push_back(data);
        }
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(112)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(_min)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("min")));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            list.push_back(data);
        }
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(113)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(_max)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("max")));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            list.push_back(data);
        }
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(114)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(_step)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("step")));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            list.push_back(data);
        }
        return 0;
    }

    uint32_t Slider::getExtraData(std::list<extraData> &extraData)
    {
        {
            struct extraData data;
            data.variable = "FORMAT";
            data.channel = 1;
            data.value = PVariable(new Variable(_format));
            extraData.push_back(data);
        }
        {
            struct extraData data;
            data.variable = "MIN";
            data.channel = 1;
            data.value = PVariable(new Variable(_min));
            extraData.push_back(data);
        }
        {
            struct extraData data;
            data.variable = "MAX";
            data.channel = 1;
            data.value = PVariable(new Variable(_max));
            extraData.push_back(data);
        }
        {
            struct extraData data;
            data.variable = "STEP";
            data.channel = 1;
            data.value = PVariable(new Variable(_step));
            extraData.push_back(data);
        }
        return 0;
    }
}