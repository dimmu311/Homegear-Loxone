#include "Jalousie.h"

namespace Loxone
{
    Jalousie::Jalousie(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x0104)
    {
        getValueFromStructFile("isAutomatic", "details", _isAutomatic);
        uint32_t animation;
        getValueFromStructFile("animation", "details", animation);
        _animation = static_cast<enum animation>(animation);
    }
    Jalousie::Jalousie(std::shared_ptr<BaseLib::Database::DataTable> rows) : LoxoneControl(rows, 0x0104)
    {
        getValueFromDataTable(111, _isAutomatic);
        uint32_t animation;
        getValueFromDataTable(112, animation);
        _animation = static_cast<enum animation>(animation);
    }
    uint32_t Jalousie::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
    {
        LoxoneControl::getDataToSave(list, peerID);
        //TODO, maybe this must not be safed anymore because it is safed to a config variable.
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(111)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(_isAutomatic)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("isAutomatic")));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            list.push_back(data);
        }
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(112)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(static_cast<double>(_animation))));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("animation")));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            list.push_back(data);
        }
        return 0;
    }

    uint32_t Jalousie::getExtraData(std::list<extraData> &extraData)
    {
        {
            struct extraData data;
            data.variable = "IS_AUTOMATIC";
            data.channel = 1;
            data.value = PVariable(new Variable(_isAutomatic));
            extraData.push_back(data);
        }
        {
            struct extraData data;
            data.variable = "ANIMATION";
            data.channel = 1;
            data.value = PVariable(new Variable(static_cast<double>(_animation)));
            extraData.push_back(data);
        }
        return 0;
    }
}