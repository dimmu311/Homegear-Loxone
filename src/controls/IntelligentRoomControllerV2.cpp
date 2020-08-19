#include "IntelligentRoomControllerV2.h"

namespace Loxone
{
    IntelligentRoomControllerV2::IntelligentRoomControllerV2(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x0301)
    {
        getValueFromStructFile("isFavorite", "", _isFavorite);

        getValueFromStructFile("timerModes", "details", _timerModes);
        getValueFromStructFile("format", "details", _format);
        getValueFromStructFile("connectedInputs", "details", _connectedInputs);
    }
    IntelligentRoomControllerV2::IntelligentRoomControllerV2(std::shared_ptr<BaseLib::Database::DataTable> rows) : LoxoneControl(rows, 0x0301)
    {
        getValueFromDataTable(107, _isFavorite);

        getBinaryValueFromDataTable(111, _timerModes);
        getBinaryValueFromDataTable(112, _format);
        getValueFromDataTable(113, _connectedInputs);
    }
    uint32_t IntelligentRoomControllerV2::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
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
        //TODO, maybe this must not be safed anymore because it is safed to a config variable.
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(111)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("timerModes")));
            std::vector<char> timerModes(_timerModes.begin(), _timerModes.end());
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(timerModes)));
            list.push_back(data);
        }
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(112)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("format")));
            std::vector<char> format(_format.begin(), _format.end());
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(format)));
            list.push_back(data);
        }
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(113)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(_connectedInputs)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("connectedInputs")));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            list.push_back(data);
        }
        return 0;
    }

    uint32_t IntelligentRoomControllerV2::getExtraData(std::list<extraData> &extraData)
    {
        {
            struct extraData data;
            data.variable = "TIMER_MODES";
            data.channel = 1;
            data.value = PVariable(new Variable(_timerModes));
            extraData.push_back(data);
        }
        {
            struct extraData data;
            data.variable = "FORMAT";
            data.channel = 1;
            data.value = PVariable(new Variable(_format));
            extraData.push_back(data);
        }
        {
            struct extraData data;
            data.variable = "CONNECTED_INPUTS";
            data.channel = 1;
            data.value = PVariable(new Variable(_connectedInputs));
            extraData.push_back(data);
        }
        return 0;
    }
}