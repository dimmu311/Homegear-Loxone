#include "LoxoneControl.h"

namespace Loxone
{
    MandatoryFields::MandatoryFields(PVariable mandatoryFields, std::string room, std::string cat)
    {
    	_name = mandatoryFields->structValue->at("name")->stringValue;
		_typeString = mandatoryFields->structValue->at("type")->stringValue;
		_uuidAction = mandatoryFields->structValue->at("uuidAction")->stringValue;
		_defaultRating = mandatoryFields->structValue->at("defaultRating")->integerValue;
		_isSecured = mandatoryFields->structValue->at("isSecured")->booleanValue;
		_isFavorite = mandatoryFields->structValue->at("isFavorite")->booleanValue;
    }
    void MandatoryFields::overwriteName(std::string name)
    {
    	_name = name;
    }
    MandatoryFields::MandatoryFields(std::shared_ptr<BaseLib::Database::DataTable>rows)
    {
    	for(BaseLib::Database::DataTable::iterator row = rows->begin(); row != rows->end(); ++row)
		{
			switch(row->second.at(2)->intValue)
			{
				case 101:
				{
					auto name = row->second.at(5)->binaryValue;
					_name = std::string (name->begin(), name->end());
					break;
				}
				case 102:
				{
					auto type = row->second.at(5)->binaryValue;
					_typeString = std::string (type->begin(), type->end());
					break;
				}
				case 103:
				{
					auto uuidAction = row->second.at(5)->binaryValue;
					_uuidAction = std::string (uuidAction->begin(), uuidAction->end());
					break;
				}
				case 104:
				{
					_defaultRating = row->second.at(3)->intValue;
					break;
				}
				case 105:
				{
					_isSecured = row->second.at(3)->intValue;
					break;
				}
			    case 106:
                {
                    _isFavorite = row->second.at(3)->intValue;
                }
			}
		}
    }
    uint32_t MandatoryFields::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
    {
    	{
			Database::DataRow data;
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(101)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("name")));
			std::vector<char> name(_name.begin(), _name.end());
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(name)));
			list.push_back(data);
    	}
    	{
			Database::DataRow data;
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(102)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("type")));
			std::vector<char> type(_typeString.begin(), _typeString.end());
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(type)));
			list.push_back(data);
		}
    	{
			Database::DataRow data;
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(103)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("uuidAction")));
			std::vector<char> uuidAction(_uuidAction.begin(), _uuidAction.end());
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(uuidAction)));
			list.push_back(data);
		}
    	{
			Database::DataRow data;
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(104)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(_defaultRating)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("defaultRating")));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
			list.push_back(data);
		}
		{
			Database::DataRow data;
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(105)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(_isSecured)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("isSecured")));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
			list.push_back(data);
		}
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(106)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(_isFavorite)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("isFavorite")));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            list.push_back(data);
        }
		return 0;
    }

    OptionalFields::OptionalFields(PVariable optionalFields, std::string room, std::string cat)
    {
    	_room = room;
		_cat = cat;
	}
    OptionalFields::OptionalFields(std::shared_ptr<BaseLib::Database::DataTable>rows)
	{
		for(BaseLib::Database::DataTable::iterator row = rows->begin(); row != rows->end(); ++row)
		{
			switch(row->second.at(2)->intValue)
			{
				case 107:
				{
					auto room = row->second.at(5)->binaryValue;
					_room = std::string (room->begin(), room->end());
					break;
				}
				case 108:
				{
					auto cat = row->second.at(5)->binaryValue;
					_cat = std::string (cat->begin(), cat->end());
					break;
				}
			}
		}
	}
    uint32_t OptionalFields::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
    {
    	{
			Database::DataRow data;
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(107)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("room")));
			std::vector<char> room(_room.begin(), _room.end());
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(room)));
			list.push_back(data);
		}
		{
			Database::DataRow data;
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(108)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("cat")));
			std::vector<char> cat(_cat.begin(), _cat.end());
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(cat)));
			list.push_back(data);
		}
		return 0;
    }

	LoxoneControl::LoxoneControl(PVariable control, std::string room, std::string cat, uint32_t typeNr) : MandatoryFields(control, room, cat),	OptionalFields(control, room, cat)
	{
		try
		{
            _RpcEncoder = std::make_shared<BaseLib::Rpc::RpcEncoder>();

            _type = typeNr;
			_control = control;

            if(control->structValue->find("states") != control->structValue->end()) {
                for (auto i = control->structValue->at("states")->structValue->begin();
                     i != control->structValue->at("states")->structValue->end(); ++i) {
                    std::shared_ptr<variable_PeerId> myVariable_PeerId(new variable_PeerId);
                    myVariable_PeerId->variable = i->first;
                    myVariable_PeerId->peerId = 0;
                    _uuidVariable_PeerIdMap.emplace(i->second->stringValue, myVariable_PeerId);
                }
            }

            if(control->structValue->find("details") != control->structValue->end())
            {
                auto details = control->structValue->at("details");
                for(auto j = details->structValue->begin(); j != details->structValue->end(); ++j)
                {
                    _detailsMap.emplace(j->first, j->second);
                }
            }
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
	}
	LoxoneControl::LoxoneControl(std::shared_ptr<BaseLib::Database::DataTable> rows, uint32_t typeNr) : MandatoryFields(rows),	OptionalFields(rows)
	{
		try
		{
            _RpcDecoder = std::make_shared<BaseLib::Rpc::RpcDecoder>();
		    _type = typeNr;
			_rows = rows;

			for(BaseLib::Database::DataTable::iterator row = rows->begin(); row != rows->end(); ++row)
			{
				switch(row->second.at(2)->intValue)
				{
				    case 121 ... 200:
				    {
                        std::vector<char> binData = *row->second.at(5)->binaryValue;
                        auto value = _RpcDecoder->decodeResponse(binData);
                        _detailsMap.emplace(row->second.at(4)->textValue, value);
				        break;
				    }
					case 201 ... 300:
					{
						auto uuid = row->second.at(5)->binaryValue;
						std::string uuidString(uuid->begin(), uuid->end());

                        std::shared_ptr<variable_PeerId> myVariable_PeerId(new variable_PeerId);
                        myVariable_PeerId->variable = row->second.at(4)->textValue;
                        myVariable_PeerId->peerId = row->second.at(1)->intValue;

						_uuidVariable_PeerIdMap.emplace(uuidString, myVariable_PeerId);
						break;
					}
				}
			}
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
	}

	bool LoxoneControl::processPacket(PLoxoneValueStatesPacket loxonePacket)
	{
		try
		{
			if (_uuidVariable_PeerIdMap.find(loxonePacket->getUuid()) == _uuidVariable_PeerIdMap.end()) return false;
			auto variable_PeerId = _uuidVariable_PeerIdMap.find(loxonePacket->getUuid());

			GD::out.printDebug("LoxoneControl::LoxoneValueStatesPacket at " + variable_PeerId->second->variable + " of peer " + std::to_string(variable_PeerId->second->peerId) + " and value is " + std::to_string(loxonePacket->getDValue()));

			_json = std::make_shared<Variable>(VariableType::tStruct);
			_json->structValue->operator[]("state") = PVariable(new Variable(VariableType::tStruct));
			_json->structValue->at("state")->structValue->operator[](variable_PeerId->second->variable) = PVariable(new Variable(loxonePacket->getDValue()));
			loxonePacket->setJsonString(_json);
			loxonePacket->setMethod("on.valueStatesPacket");
			return true;
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
			return false;
		}
	}

	bool LoxoneControl::processPacket(PLoxoneTextStatesPacket loxonePacket)
	{
		try
		{
			if (_uuidVariable_PeerIdMap.find(loxonePacket->getUuid()) == _uuidVariable_PeerIdMap.end()) return false;
			auto variable_PeerId = _uuidVariable_PeerIdMap.find(loxonePacket->getUuid());

			GD::out.printDebug("LoxoneControl::LoxoneTextStatesPacket at " + variable_PeerId->second->variable + " of peer " + std::to_string(variable_PeerId->second->peerId) + " and value is " + loxonePacket->getText());

			_json = std::make_shared<Variable>(VariableType::tStruct);
			_json->structValue->operator[]("state") = PVariable(new Variable(VariableType::tStruct));
			_json->structValue->at("state")->structValue->operator[](variable_PeerId->second->variable) = PVariable(new Variable(loxonePacket->getText()));

            auto value = BaseLib::Rpc::JsonDecoder::decode(loxonePacket->getText());
            _json->structValue->operator[]("json") = PVariable(new Variable(VariableType::tStruct));
            _json->structValue->at("json")->structValue->operator[](variable_PeerId->second->variable) = value;

            loxonePacket->setJsonString(_json);
            loxonePacket->setMethod("on.textStatesPacket");
			return true;
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
			return false;
		}
	}
	bool LoxoneControl::processPacket(PLoxoneBinaryFilePacket loxonePacket)
	{
		try
		{
			return false;
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
			return false;
		}
	}
	bool LoxoneControl::processPacket(PLoxoneTextmessagePacket loxonePacket)
	{
		try
		{
			return false;
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
			return false;
		}
	}
	bool LoxoneControl::processPacket(PLoxoneDaytimerStatesPacket loxonePacket)
	{
		try
		{
            if (_uuidVariable_PeerIdMap.find(loxonePacket->getUuid()) == _uuidVariable_PeerIdMap.end()) return false;
            auto variable_PeerId = _uuidVariable_PeerIdMap.find(loxonePacket->getUuid());

            GD::out.printDebug("LoxoneControl::LoxoneDaytimerStatesPacket at " + variable_PeerId->second->variable + " of peer " + std::to_string(variable_PeerId->second->peerId));

            _json = std::make_shared<Variable>(VariableType::tStruct);
            _json->structValue->operator[]("state") = PVariable(new Variable(VariableType::tStruct));
            _json->structValue->at("state")->structValue->operator[]("default") = PVariable(new Variable(loxonePacket->getDevValue()));

            GD::out.printDebug("LoxoneControl::LoxoneDaytimerStatesPacket at default of peer " + std::to_string(variable_PeerId->second->peerId) + " and value " + std::to_string(loxonePacket->getDevValue()));

            _json->structValue->at("state")->structValue->operator[]("entrys") = PVariable(new Variable(VariableType::tStruct));
            _json->structValue->at("state")->structValue->at("entrys") = PVariable(new Variable(VariableType::tArray));

            auto entrys = loxonePacket->getEntrys();
            for(auto entry = entrys.begin(); entry != entrys.end(); ++entry)
            {
                auto data = new Variable(VariableType::tStruct);

                data->structValue->operator[]("id") = PVariable(new Variable(entry->first));
                data->structValue->operator[]("from") = PVariable(new Variable(entry->second->_from));
                data->structValue->operator[]("to") = PVariable(new Variable(entry->second->_to));
                data->structValue->operator[]("mode") = PVariable(new Variable(entry->second->_mode));
                data->structValue->operator[]("needActivate") = PVariable(new Variable(entry->second->_needActivate));
                data->structValue->operator[]("value") = PVariable(new Variable(entry->second->_value));

                _json->structValue->at("state")->structValue->at("entrys")->arrayValue->push_back(
                        static_cast<const std::shared_ptr<Variable>>(data));
            }

            loxonePacket->setJsonString(_json);
            loxonePacket->setMethod("on.daytimerStatesPacket");

            std::string jstr;
            BaseLib::Rpc::JsonEncoder::encode(_json, jstr);
            GD::out.printDebug("LoxoneControl::LoxoneDaytimerStatesPacket of peer " + std::to_string(variable_PeerId->second->peerId) + " json: " + jstr);

            return true;
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
			return false;
		}
	}
	bool LoxoneControl::processPacket(PLoxoneWeatherStatesPacket loxonePacket)
	{
		try
		{
			return false;
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
			return false;
		}
	}
    uint32_t LoxoneControl::getExtraData(std::list<extraData> &extraData)
    {
        for(auto i = _detailsMap.begin(); i!= _detailsMap.end(); ++i)
        {
            struct extraData data;
            for(char c : i->first)
            {
                if(islower(c))
                {
                    data.variable.push_back(toupper(c));
                }
                else
                {
                    data.variable.push_back('_');
                    data.variable.push_back(c);
                }
            }

            data.channel = 1;
            data.value = i->second;
            extraData.push_back(data);
        }
        return 0;
    }
	uint32_t LoxoneControl::getDetailsToSave(std::list<Database::DataRow> &list, uint32_t peerID)
    {
        uint32_t variableID = 121;
        for(auto i = _detailsMap.begin(); i!= _detailsMap.end(); ++i)
        {
            Database::DataRow data;
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(variableID++)));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(i->first)));
            std::vector<uint8_t> binData;
            _RpcEncoder->encodeResponse(i->second, binData);
            data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(binData)));
            list.push_back(data);
        }
        return 0;
    }
	uint32_t LoxoneControl::getStatesToSave(std::list<Database::DataRow> &list, uint32_t peerID)
	{
		uint32_t variableID = 201;
		for(auto i = _uuidVariable_PeerIdMap.begin(); i != _uuidVariable_PeerIdMap.end(); ++i)
		{
			std::string variable = i->second->variable;
			std::string uuid = i->first;

			Database::DataRow data;
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(variableID++)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(variable)));
			std::vector<char> uuidVector(uuid.begin(), uuid.end());
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(uuidVector)));
			list.push_back(data);
		}
		return 0;
	}

	bool LoxoneControl::getValueFromVariable(BaseLib::PVariable variable, std::string& command)
    {
        if(variable->type == VariableType::tString)
        {
            command += variable->stringValue;
            return true;
        }
        else if (variable->type == VariableType::tInteger)
        {
            command += std::to_string(variable->integerValue);
            return true;
        }
        else if (variable->type == VariableType::tFloat)
        {
            command += std::to_string(variable->floatValue);
            return true;
        }
        return false;
    }

    bool LoxoneControl::setValue(PPacket frame, BaseLib::PVariable parameters, uint32_t channel, std::string& command, bool &isSecured)
    {
        try
        {
            isSecured = _isSecured;

            if(parameters->type != VariableType::tArray) return false;
            command = "jdev/sps/io/" + _uuidAction + "/";
            if(frame->function1 == "activeSetOn" || frame->function1 == "activeSetOff")
            {
                if (parameters->arrayValue->at(0)->type != VariableType::tString) return false;
                command += parameters->arrayValue->at(0)->stringValue;
                return true;
            }
            if(frame->function1 == "setNext" || frame->function1 == "setPrevious")
            {
                if (parameters->arrayValue->at(0)->type != VariableType::tString) return false;
                command += parameters->arrayValue->at(0)->stringValue;
                return true;
            }
            else if(frame->function1 == "stepUp" || frame->function1 == "stepDown")
            {
                if (parameters->arrayValue->at(0)->type != VariableType::tFloat) return false;
                if (parameters->arrayValue->at(1)->type != VariableType::tFloat) return false;
                float value = parameters->arrayValue->at(0)->floatValue;
                float step = parameters->arrayValue->at(1)->floatValue;
                value += step;
                if(frame->function1 == "stepDown")
                {
                    value -= step;
                    value -= step;
                }
                command += std::to_string(value);
                return true;
            }
            else if(frame->function1 == "activeSet")
            {
                if (parameters->arrayValue->at(0)->type != VariableType::tFloat) return false;
                std::string doCommand = "off";
                if ((bool)parameters->arrayValue->at(0)->floatValue) doCommand = "on";
                command += doCommand;
                return true;
            }
            else if(frame->function1 == "booleanSet")
            {
                if (parameters->arrayValue->at(0)->type != VariableType::tFloat)
                {
                    if(parameters->arrayValue->at(0)->type != VariableType::tString) return false;
                }
                if (parameters->arrayValue->at(1)->type != VariableType::tString) return false;
                if (parameters->arrayValue->at(2)->type != VariableType::tString) return false;

                if(parameters->arrayValue->at(0)->type == VariableType::tFloat)
                {
                    std::string doCommand = parameters->arrayValue->at(1)->stringValue;
                    if (!(bool)parameters->arrayValue->at(0)->floatValue) doCommand = parameters->arrayValue->at(2)->stringValue;
                    command += doCommand;
                    return true;
                }
                if(parameters->arrayValue->at(0)->type == VariableType::tString)
                {
                    std::string doCommand = parameters->arrayValue->at(1)->stringValue;
                    if (parameters->arrayValue->at(0)->stringValue == "false") doCommand = parameters->arrayValue->at(2)->stringValue;
                    command += doCommand;
                    return true;
                }
                return false;
            }
            else if(frame->function1 == "valueSet")
            {
                return getValueFromVariable(parameters->arrayValue->at(0), command);
            }
            else if(frame->function1 == "valueSetToPath")
            {
                if (parameters->arrayValue->at(0)->type != VariableType::tString) return false;
                command += parameters->arrayValue->at(0)->stringValue + "/";
                return getValueFromVariable(parameters->arrayValue->at(1), command);
            }
            else if(frame->function1 == "2valueSetToPath")
            {
                if (parameters->arrayValue->at(0)->type != VariableType::tString) return false;
                command += parameters->arrayValue->at(0)->stringValue + "/";
                if (!getValueFromVariable(parameters->arrayValue->at(1), command)) return false;
                command += "/";
                return getValueFromVariable(parameters->arrayValue->at(2), command);
            }
            else if(frame->function1 == "3valueSetToPath")
            {
                if (parameters->arrayValue->at(0)->type != VariableType::tString) return false;
                command += parameters->arrayValue->at(0)->stringValue + "/";
                if (!getValueFromVariable(parameters->arrayValue->at(1), command)) return false;
                command += "/";
                if (!getValueFromVariable(parameters->arrayValue->at(2), command)) return false;
                command += "/";
                return getValueFromVariable(parameters->arrayValue->at(3), command);
            }
            else if (frame->function1 == "stringSetToPath"){
                if (parameters->arrayValue->at(0)->type != VariableType::tString) return false;
                command += parameters->arrayValue->at(0)->stringValue + "/";
                return getValueFromVariable(parameters->arrayValue->at(1), command);
            }
            else if(frame->function1 == "valueStringSetToPath") {
                if (parameters->arrayValue->at(0)->type != VariableType::tString) return false;
                command += parameters->arrayValue->at(0)->stringValue + "/";
                if (!getValueFromVariable(parameters->arrayValue->at(1), command)) return false;
                command += "/";
                if (parameters->arrayValue->at(2)->type != VariableType::tString) return false;
                command += parameters->arrayValue->at(2)->stringValue;
                return true;
            }
        }
        catch (const std::exception& ex)
        {
            GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
        }
        return false;
    }
	uint32_t LoxoneControl::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
	{
		if(peerID == 0)return 0;
		MandatoryFields::getDataToSave(list, peerID);
		OptionalFields::getDataToSave(list, peerID);
		getStatesToSave(list, peerID);
		getDetailsToSave(list, peerID);
		return 0;
	}
	bool LoxoneControl::getValueFromStructFile(const std::string& variableId, const std::string& path, bool& value)
	{
		try
		{
			if(path != "")
			{
				if(_control->structValue->find(path) != _control->structValue->end())
				{
					if(_control->structValue->at(path)->structValue->find(variableId) != _control->structValue->end())
					{
						value = _control->structValue->at(path)->structValue->at(variableId)->booleanValue;
						return true;
					}
				}
			}

			if(_control->structValue->find(variableId) != _control->structValue->end())
			{
				value = _control->structValue->at(variableId)->booleanValue;
				return true;
			}
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		value = false;
		if (GD::bl->debugLevel >= 5) GD::out.printInfo("could not get boolean variable from Struct File. variable id= " + variableId);
		return false;
	}
	bool LoxoneControl::getValueFromStructFile(const std::string& variableId, const std::string& path, uint32_t& value)
	{
		try
		{
			if(path != "")
			{
				if(_control->structValue->find(path) != _control->structValue->end())
				{
					if(_control->structValue->at(path)->structValue->find(variableId) != _control->structValue->end())
					{
						value = _control->structValue->at(path)->structValue->at(variableId)->integerValue;
						return true;
					}
				}
			}

			if(_control->structValue->find(variableId) != _control->structValue->end())
			{
				value = _control->structValue->at(variableId)->integerValue;
				return true;
			}
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		value = 0;
		if (GD::bl->debugLevel >= 5) GD::out.printInfo("could not get int variable from Struct File. variable id= " + variableId);
		return false;
	}
	bool LoxoneControl::getValueFromStructFile(const std::string& variableId, const std::string& path, float& value)
	{
		try
		{
			if(path != "")
			{
				if(_control->structValue->find(path) != _control->structValue->end())
				{
					if(_control->structValue->at(path)->structValue->find(variableId) != _control->structValue->end())
					{
						value = _control->structValue->at(path)->structValue->at(variableId)->floatValue;
						return true;
					}
				}
			}

			if(_control->structValue->find(variableId) != _control->structValue->end())
			{
				value = _control->structValue->at(variableId)->floatValue;
				return true;
			}
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		value = 0;
		if (GD::bl->debugLevel >= 5) GD::out.printInfo("could not get float variable from Struct File. variable id= " + variableId);
		return false;
	}
	bool LoxoneControl::getValueFromStructFile(const std::string& variableId, const std::string& path, std::string& value)
	{
		try
		{
			if(path != "")
			{
				if(_control->structValue->find(path) != _control->structValue->end())
				{
					if(_control->structValue->at(path)->structValue->find(variableId) != _control->structValue->end())
					{
						value = _control->structValue->at(path)->structValue->at(variableId)->stringValue;
						return true;
					}
				}
			}

			if(_control->structValue->find(variableId) != _control->structValue->end())
			{
				value = _control->structValue->at(variableId)->stringValue;
				return true;
			}
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		value = "nA";
		if (GD::bl->debugLevel >= 5) GD::out.printInfo("could not get string variable from Struct File. variable id= " + variableId);
		return false;
	}
	bool LoxoneControl::getValueFromDataTable(const uint32_t& variableId, uint32_t& value)
	{
		try
		{
			for(BaseLib::Database::DataTable::iterator row = _rows->begin(); row != _rows->end(); ++row)
			{
				if(row->second.at(2)->intValue == variableId)
				{
					value = row->second.at(3)->intValue;
					return true;
				}
			}
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		value = 0;
		if (GD::bl->debugLevel >= 5) GD::out.printInfo("could not get int variable from database. variable id= " + std::to_string(variableId));
		return false;
	}
	bool LoxoneControl::getValueFromDataTable(const uint32_t& variableId, bool& value)
	{
		try
		{
			for(BaseLib::Database::DataTable::iterator row = _rows->begin(); row != _rows->end(); ++row)
			{
				if(row->second.at(2)->intValue == variableId)
				{
					value = (bool)row->second.at(3)->intValue;
					return true;
				}
			}
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		value = false;
		if (GD::bl->debugLevel >= 5) GD::out.printInfo("could not get boolean variable from database. variable id= " + std::to_string(variableId));
		return false;
	}
	bool LoxoneControl::getValueFromDataTable(const uint32_t &variableId, float& value)
	{
		try
		{
			for(BaseLib::Database::DataTable::iterator row = _rows->begin(); row != _rows->end(); ++row)
			{
				if(row->second.at(2)->intValue == variableId)
				{
					value = row->second.at(3)->floatValue;
					return true;
				}
			}
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		value = 0;
		if (GD::bl->debugLevel >= 5) GD::out.printInfo("could not get float variable from database. variable id= " + std::to_string(variableId));
		return false;
	}
	bool LoxoneControl::getValueFromDataTable(const uint32_t& variableId, std::string& value)
	{
		try
		{
			for(BaseLib::Database::DataTable::iterator row = _rows->begin(); row != _rows->end(); ++row)
			{
				if(row->second.at(2)->intValue == variableId)
				{
					value = row->second.at(4)->textValue;
					return true;
				}
			}
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		value ="nA";
		if (GD::bl->debugLevel >= 5) GD::out.printInfo("could not get string variable from database. variable id= " + std::to_string(variableId));
		return false;
	}
	bool LoxoneControl::getBinaryValueFromDataTable(const uint32_t& variableId, std::string& value)
	{
		try
		{
			for(BaseLib::Database::DataTable::iterator row = _rows->begin(); row != _rows->end(); ++row)
			{
				if(row->second.at(2)->intValue == variableId)
				{
					auto temp = row->second.at(5)->binaryValue;
					value = std::string(temp->begin(), temp->end());
					return true;
				}
			}
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		value = "nA";
		if (GD::bl->debugLevel >= 5) GD::out.printInfo("could not get binary variable from database. variable id= " + std::to_string(variableId));
		return false;
	}
}
