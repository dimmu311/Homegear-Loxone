#include "Pushbutton.h"

namespace Loxone
{
    Pushbutton::Pushbutton(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat,0x100)
    {
        getValueFromStructFile("isFavorite", "", _isFavorite);
        getValueFromStructFile("defaultIcon", "", _defaultIcon);
    }

    Pushbutton::Pushbutton(std::shared_ptr<BaseLib::Database::DataTable> rows) : LoxoneControl(rows, 0x100)
    {
        getValueFromDataTable(107, _isFavorite);
        getBinaryValueFromDataTable(108, _defaultIcon);
    }

    uint32_t Pushbutton::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID) {
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
        return 0;
    }
/*
    bool Pushbutton::processPacket(PLoxoneValueStatesPacket loxonePacket)
    {
        if (_uuidVariable_PeerIdMap.find(loxonePacket->getUuid()) == _uuidVariable_PeerIdMap.end()) return false;
        auto variable_PeerId = _uuidVariable_PeerIdMap.find(loxonePacket->getUuid());

        BaseLib::PVariable value;
        value.reset(new BaseLib::Variable((bool)loxonePacket->getDValue()));

        BaseLib::Systems::RpcConfigurationParameter& parameter = _valuesCentral[1]["ACTIVE"];

        std::vector<uint8_t> parameterData;
        parameter.rpcParameter->convertToPacket(value, parameter.mainRole() ,parameterData);
        parameter.setBinaryData(parameterData);

        GD::out.printDebug("JA+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

        if(parameter.databaseId > 0) saveParameter(parameter.databaseId, value);
        else saveParameter(0, ParameterGroup::Type::Enum::variables, *j, i->first, value);
    }
        */
}