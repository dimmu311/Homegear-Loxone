#include "LoxoneControl.h"

namespace Loxone
{
	template<typename T> LoxoneControl* createInstance(PVariable control, std::string room, std::string cat) { return new T(control, room, cat); }
	const std::unordered_map<std::string, LoxoneControl* (*)(PVariable, std::string, std::string)> LoxoneControl::_controlsMap =
	{
		{"Pushbutton", &createInstance<Pushbutton>},
		{"Switch", &createInstance<Pushbutton>},
		{"Slider", &createInstance<Slider>},
		{"Dimmer", &createInstance<Dimmer>},
		{"LightControllerV2", &createInstance<LightControllerV2>},
		{"Alarm", &createInstance<Alarm>},
		{"MediaClient", &createInstance<MediaClient>},
	};

	template<typename T> LoxoneControl* createInstance2(std::shared_ptr<BaseLib::Database::DataTable> rows) { return new T(rows); }
    const std::unordered_map<uint32_t, LoxoneControl* (*)(std::shared_ptr<BaseLib::Database::DataTable>)> LoxoneControl::_uintControlsMap =
    {
		{0x100, &createInstance2<Pushbutton>},//Pushbutton
		//{0x100, &createInstance2<Pushbutton>}, Switch
		{0x101, &createInstance2<Slider>},
		{0x102, &createInstance2<Dimmer>},
		{0x103, &createInstance2<LightControllerV2>},
		{0x200, &createInstance2<Alarm>},
		{0x201, &createInstance2<MediaClient>},
    };

    MandatoryFields::MandatoryFields(PVariable mandatoryFields, std::string room, std::string cat)
    {
    	_name = mandatoryFields->structValue->at("name")->stringValue;
		_typeString = mandatoryFields->structValue->at("type")->stringValue;
		_uuidAction = mandatoryFields->structValue->at("uuidAction")->stringValue;
		_defaultRating = mandatoryFields->structValue->at("defaultRating")->integerValue;
		_isSecured = mandatoryFields->structValue->at("isSecured")->booleanValue;
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
				case 106:
				{
					auto room = row->second.at(5)->binaryValue;
					_room = std::string (room->begin(), room->end());
					break;
				}
				case 107:
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
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(106)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("room")));
			std::vector<char> room(_room.begin(), _room.end());
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(room)));
			list.push_back(data);
		}
		{
			Database::DataRow data;
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(107)));
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
			_type = typeNr;
			_control = control;

			for (auto i = control->structValue->at("states")->structValue->begin(); i != control->structValue->at("states")->structValue->end(); ++i)
			{
				variable_PeerId variable_PeerId;
				variable_PeerId.variable = i->first;
				_uuidVariable_PeerIdMap.emplace(i->second->stringValue, variable_PeerId);
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
			_type = typeNr;
			_rows = rows;

			for(BaseLib::Database::DataTable::iterator row = rows->begin(); row != rows->end(); ++row)
			{
				switch(row->second.at(2)->intValue)
				{
					case 201 ... 300:
					{
						auto uuid = row->second.at(5)->binaryValue;
						std::string uuidString(uuid->begin(), uuid->end());

						variable_PeerId variable_PeerId;
						variable_PeerId.variable = row->second.at(4)->textValue;
						variable_PeerId.peerId = row->second.at(1)->intValue;

						_uuidVariable_PeerIdMap.emplace(uuidString, variable_PeerId);
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

	void LoxoneControl::addBooleanState(double value, std::string& variable)
	{
		GD::out.printDebug("LoxoneControl::addBooleanState ");

		if(variable == "value") _json->structValue->at("state")->structValue->operator[]("state") = PVariable(new Variable((bool)value));
		else if (variable == "position") _json->structValue->at("state")->structValue->operator[]("state") = PVariable(new Variable((bool)value));
	}

	bool LoxoneControl::processPacket(PLoxoneValueStatesPacket loxonePacket)
	{
		try
		{
			if (_uuidVariable_PeerIdMap.find(loxonePacket->getUuid()) == _uuidVariable_PeerIdMap.end()) return false;
			auto variable_PeerId = _uuidVariable_PeerIdMap.find(loxonePacket->getUuid());

			GD::out.printDebug("LoxoneControl::LoxoneValueStatesPacket at " + variable_PeerId->second.variable + " of peer " + std::to_string(variable_PeerId->second.peerId) + " and value is " + std::to_string(loxonePacket->getDValue()));

			_json = std::make_shared<Variable>(VariableType::tStruct);
			_json->structValue->operator[]("state") = PVariable(new Variable(VariableType::tStruct));
			_json->structValue->at("state")->structValue->operator[](variable_PeerId->second.variable) = PVariable(new Variable(loxonePacket->getDValue()));
			addBooleanState(loxonePacket->getDValue(), variable_PeerId->second.variable);

			loxonePacket->setJsonString(_json);
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

			GD::out.printDebug("LoxoneControl::LoxoneTextStatesPacket at " + variable_PeerId->second.variable + " of peer " + std::to_string(variable_PeerId->second.peerId) + " and value is " + loxonePacket->getText());

			_json = std::make_shared<Variable>(VariableType::tStruct);
			_json->structValue->operator[]("state") = PVariable(new Variable(VariableType::tStruct));
			_json->structValue->at("state")->structValue->operator[](variable_PeerId->second.variable) = PVariable(new Variable(loxonePacket->getText()));

			loxonePacket->setJsonString(_json);
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
			return false;
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
	uint32_t LoxoneControl::getStatesToSave(std::list<Database::DataRow> &list, uint32_t peerID)
	{
		uint32_t variableID = 201;
		for(auto i = _uuidVariable_PeerIdMap.begin(); i != _uuidVariable_PeerIdMap.end(); ++i)
		{
			std::string variable = i->second.variable;
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
	bool LoxoneControl::setValue(std::string method, BaseLib::PVariable parameters, std::shared_ptr<LoxonePacket> packet)
	{
		try
		{
			std::string command = "jdev/sps/io/" + _uuidAction;
			switch(parameters->type)
			{
				case VariableType::tArray:
				{
					for(auto value1 = parameters->arrayValue->begin(); value1 != parameters->arrayValue->end(); ++value1)
					{
						switch(value1.operator *()->type)
						{
							case VariableType::tString:
							{
								command += "/";
								command += value1.operator *()->stringValue;
								break;
							}
							case VariableType::tInteger:
							{
								command += "/";
								command += std::to_string(value1.operator *()->integerValue);
								break;
							}
							case VariableType::tFloat:
							{
								command += "/";
								command += std::to_string(value1.operator *()->floatValue);
								break;
							}
							case VariableType::tBoolean:
							{
								std::string doCommand = "off";
								if(value1.operator *()->booleanValue) doCommand = "on";
								command += "/";
								command += doCommand;
							}
							default:
								break;
						}
					}
					break;
				}
				default:
					break;
			}
			if (GD::bl->debugLevel >= 5) GD::out.printInfo("build command from packet: " + command);
			packet->setCommand(command);
			return true;
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
		return 1;
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

	Pushbutton::Pushbutton(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x100)
	{
		getValueFromStructFile("isFavorite","", _isFavorite);
		getValueFromStructFile("defaultIcon","", _defaultIcon);
	}
	Pushbutton::Pushbutton(std::shared_ptr<BaseLib::Database::DataTable> rows) : LoxoneControl(rows, 0x100)
	{
		getValueFromDataTable(107, _isFavorite);
		getBinaryValueFromDataTable(108, _defaultIcon);
	}
	bool Pushbutton::processPacket(PLoxoneValueStatesPacket loxonePacket)
	{
		try
		{
			if (LoxoneControl::processPacket(loxonePacket))
			{
				if (_json->structValue->at("state")->structValue->find("active") != _json->structValue->at("state")->structValue->end())
				{
					auto value = _json->structValue->at("state")->structValue->at("active")->floatValue;
					_json->structValue->at("state")->structValue->at("active") = PVariable(new Variable((bool)value));
				}
				loxonePacket->setJsonString(_json);
				return true;
			}
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		return false;
	}
	uint32_t Pushbutton::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
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
		return 0;
	}
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

	Dimmer::Dimmer(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x0102)
	{
		getValueFromStructFile("isFavorite", "", _isFavorite);
	}
	Dimmer::Dimmer(std::shared_ptr<BaseLib::Database::DataTable> rows): LoxoneControl(rows, 0x0102)
	{
		getValueFromDataTable(107, _isFavorite);
	}
	uint32_t Dimmer::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
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
		return 0;
	}

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

	Alarm::Alarm(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x0200)
	{
		getValueFromStructFile("alert", "details", _detAlert);
		getValueFromStructFile("presenceConnected", "details", _detPresenceConnected);
	}
	Alarm::Alarm(std::shared_ptr<BaseLib::Database::DataTable> rows): LoxoneControl(rows, 0x0200)
	{
		getValueFromDataTable(111, _detAlert);
		getValueFromDataTable(112, _detPresenceConnected);
	}
	uint32_t Alarm::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
	{
		LoxoneControl::getDataToSave(list, peerID);
		{
			Database::DataRow data;
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(111)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(_detAlert)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("detAlert")));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
			list.push_back(data);
		}
		{
			Database::DataRow data;
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(peerID)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(112)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn(_detPresenceConnected)));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn("detPresenceConnected")));
			data.push_back(std::shared_ptr<Database::DataColumn>(new Database::DataColumn()));
			list.push_back(data);
		}
		return 0;
	}


	MediaClient::MediaClient(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x201){}
	MediaClient::MediaClient(std::shared_ptr<BaseLib::Database::DataTable> rows) : LoxoneControl(rows, 0x201){}
	uint32_t MediaClient::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
	{
		LoxoneControl::getDataToSave(list, peerID);
		{
			return 0;
		}
	}
}
