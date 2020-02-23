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
		_type = mandatoryFields->structValue->at("type")->stringValue;
		_uuidAction = mandatoryFields->structValue->at("uuidAction")->stringValue;
		_defaultRating = mandatoryFields->structValue->at("defaultRating")->integerValue;
		_isSecured = mandatoryFields->structValue->at("isSecured")->booleanValue;
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
					_type = std::string (type->begin(), type->end());
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
			std::vector<char> type(_type.begin(), _type.end());
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

	LoxoneControl::LoxoneControl(PVariable control, std::string room, std::string cat, uint32_t typeNr)
	{
		try
		{
			_type = typeNr;

			_mandatoryFields = std::make_shared<MandatoryFields>(control, room, cat);
			_optionalFields = std::make_shared<OptionalFields>(control, room, cat);

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
	LoxoneControl::LoxoneControl(std::shared_ptr<BaseLib::Database::DataTable> rows, uint32_t typeNr)
	{
		try
		{
			_type = typeNr;
			_mandatoryFields = std::make_shared<MandatoryFields>(rows);
			_optionalFields = std::make_shared<OptionalFields>(rows);

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

	bool LoxoneControl::preProcessPacket(PLoxonePacket loxonePacket, std::string& variable)
	{
		GD::out.printDebug("LoxoneControl::preProcessPacket " + loxonePacket->getUuid());

		if (_uuidVariable_PeerIdMap.find(loxonePacket->getUuid()) == _uuidVariable_PeerIdMap.end()) return false;

		auto variable_PeerId = _uuidVariable_PeerIdMap.find(loxonePacket->getUuid());
		variable = (variable_PeerId->second.variable);

		_json = std::make_shared<Variable>(VariableType::tStruct);
		_json->structValue->operator[]("state") = PVariable(new Variable(VariableType::tStruct));

		switch (loxonePacket->getPacketType())
		{
		case LoxonePacketType::LoxoneValueStatesPacket:
		{
			PLoxoneValueStatesPacket loxoneValuePacket(std::dynamic_pointer_cast<LoxoneValueStatesPacket>(loxonePacket));
			_json->structValue->at("state")->structValue->operator[](variable) = PVariable(new Variable(loxoneValuePacket->getDValue()));
			addBooleanState(loxoneValuePacket->getDValue(), variable);
			GD::out.printDebug("LoxoneControl::preProcessPacket " + loxonePacket->getUuid() + std::to_string(loxonePacket->getDValue()));
			return true;
		}
		case LoxonePacketType::LoxoneTextStatesPacket:
		{
			PLoxoneTextStatesPacket loxoneTextPacket(std::dynamic_pointer_cast<LoxoneTextStatesPacket>(loxonePacket));
			_json->structValue->at("state")->structValue->operator[](variable) = PVariable(new Variable(loxoneTextPacket->getText()));
			return true;
		}
		default:
			return false;
		}
		
		return false;
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
	uint32_t LoxoneControl::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
	{
		if(peerID == 0)return 0;
		_mandatoryFields->getDataToSave(list, peerID);
		_optionalFields->getDataToSave(list, peerID);
		getStatesToSave(list, peerID);
		return 1;
	}

	Pushbutton::Pushbutton(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x100)
	{
		try
		{
			_isFavorite = control->structValue->at("isFavorite")->booleanValue;
			_defaultIcon = control->structValue->at("defaultIcon")->stringValue;
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
	}
	Pushbutton::Pushbutton(std::shared_ptr<BaseLib::Database::DataTable> rows) : LoxoneControl(rows, 0x100)
	{
		try
		{
			for(BaseLib::Database::DataTable::iterator row = rows->begin(); row != rows->end(); ++row)
			{
				switch(row->second.at(2)->intValue)
				{
					case 107:
					{
						_isFavorite = row->second.at(3)->intValue;
						break;
					}
					case 108:
					{
						auto defaultIcon = row->second.at(5)->binaryValue;
						_defaultIcon = std::string(defaultIcon->begin(), defaultIcon->end());
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
	bool Pushbutton::processPacket(PLoxonePacket loxonePacket)
	{
		try
		{
			GD::out.printDebug("Pushbutton::processPacket " + loxonePacket->getUuid());

			std::string variable;
			if (LoxoneControl::preProcessPacket(loxonePacket, variable))
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

	bool Pushbutton::setValue(std::string method, BaseLib::PVariable parameters, std::shared_ptr<LoxonePacket> packet)
	{
		try
		{
			std::string command;
			if (method == "setConstString")
			{
				GD::out.printInfo("setValueTests: " + parameters->arrayValue->at(0)->stringValue);
				command = "jdev/sps/io/" + _mandatoryFields->_uuidAction + "/" + parameters->arrayValue->at(0)->stringValue;
			}
			else if (method == "StateSet")
			{
				auto value = parameters->arrayValue->at(0)->booleanValue;
				command = "jdev/sps/io/" + _mandatoryFields->_uuidAction + "/" + "off";
				if (value)
				{
					command = "jdev/sps/io/" + _mandatoryFields->_uuidAction + "/" + "on";
				}
			}

			packet->setCommand(command);
			return true;
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
		try
		{
			_isFavorite = control->structValue->at("isFavorite")->booleanValue;
			_defaultIcon = control->structValue->at("defaultIcon")->stringValue;

			_detFormat = control->structValue->at("details")->structValue->at("format")->stringValue;
			_detMin = control->structValue->at("details")->structValue->at("min")->floatValue;
			_detMax = control->structValue->at("details")->structValue->at("max")->floatValue;
			_detStep = control->structValue->at("details")->structValue->at("step")->floatValue;
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
	}
	Slider::Slider(std::shared_ptr<BaseLib::Database::DataTable> rows) : LoxoneControl(rows, 0x0101)
	{
		try
		{
			for(BaseLib::Database::DataTable::iterator row = rows->begin(); row != rows->end(); ++row)
			{
				switch(row->second.at(2)->intValue)
				{
					case 107:
					{
						_isFavorite = row->second.at(3)->intValue;
						break;
					}
					case 108:
					{
						auto defaultIcon = row->second.at(5)->binaryValue;
						_defaultIcon = std::string(defaultIcon->begin(), defaultIcon->end());
						break;
					}
					case 111:
					{
						auto detFormat = row->second.at(5)->binaryValue;
						_detFormat = std::string(detFormat->begin(), detFormat->end());
						break;
					}
					case 112:
					{
						_detMin = row->second.at(3)->floatValue;
						break;
					}
					case 113:
					{
						_detMax = row->second.at(3)->floatValue;
						break;
					}
					case 114:
					{
						_detStep = row->second.at(3)->floatValue;
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
	bool Slider::processPacket(PLoxonePacket loxonePacket)
	{
		try
		{
			GD::out.printDebug("Slider::processPacket " + loxonePacket->getUuid());

			std::string variable;
			if (LoxoneControl::preProcessPacket(loxonePacket, variable))
			{
				loxonePacket->setJsonString(_json);
				return true;
			}
		}
		catch (const std::exception & ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		return false;
	}
	bool Slider::setValue(std::string method, BaseLib::PVariable parameters, std::shared_ptr<LoxonePacket> packet)
	{
		try
		{
			std::string command;
			if (method == "ValueSet")
			{
				auto value = parameters->arrayValue->at(0)->floatValue;
				command = "jdev/sps/io/" + _mandatoryFields->_uuidAction + "/" + std::to_string(value);
			}
			packet->setCommand(command);
			return true;
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		return false;
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
		try
		{
			_isFavorite = control->structValue->at("isFavorite")->booleanValue;
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
	}
	Dimmer::Dimmer(std::shared_ptr<BaseLib::Database::DataTable> rows): LoxoneControl(rows, 0x0102)
	{
		try
		{
			for(BaseLib::Database::DataTable::iterator row = rows->begin(); row != rows->end(); ++row)
			{
				switch(row->second.at(2)->intValue)
				{
					case 107:
					{
						_isFavorite = row->second.at(3)->intValue;
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
	bool Dimmer::processPacket(PLoxonePacket loxonePacket)
	{
		try
		{
			GD::out.printDebug("Dimmer::processPacket " + loxonePacket->getUuid());

			std::string variable;
			if (LoxoneControl::preProcessPacket(loxonePacket, variable))
			{
				loxonePacket->setJsonString(_json);
				return true;
			}
		}
		catch (const std::exception & ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		return false;
	}
	bool Dimmer::setValue(std::string method, BaseLib::PVariable parameters, std::shared_ptr<LoxonePacket> packet)
	{
		try
		{
			std::string command;
			if (method == "ValueSet")
			{
				auto value = parameters->arrayValue->at(0)->floatValue;
				command = "jdev/sps/io/" + _mandatoryFields->_uuidAction + "/" + std::to_string(value);
			}
			else if (method == "setOn")
			{
				command = "jdev/sps/io/" + _mandatoryFields->_uuidAction + "/" + "on";
			}
			else if (method == "setOff")
			{
				command = "jdev/sps/io/" + _mandatoryFields->_uuidAction + "/" + "off";
			}
			else if (method == "StateSet")
			{
				auto value = parameters->arrayValue->at(0)->booleanValue;
				command = "jdev/sps/io/" + _mandatoryFields->_uuidAction + "/" + "off";
				if (value)
				{
					command = "jdev/sps/io/" + _mandatoryFields->_uuidAction + "/" + "on";
				}
			}
			packet->setCommand(command);
			return true;
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		return false;
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
		try
		{
			_isFavorite = control->structValue->at("isFavorite")->booleanValue;

			_detMovementScene = control->structValue->at("details")->structValue->at("movementScene")->integerValue;
			_detMasterValue = control->structValue->at("details")->structValue->at("masterValue")->stringValue;
			_detMasterColor = control->structValue->at("details")->structValue->at("masterColor")->stringValue;
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
	}
	LightControllerV2::LightControllerV2(std::shared_ptr<BaseLib::Database::DataTable> rows): LoxoneControl(rows, 0x0103)
	{
		try
		{
			for(BaseLib::Database::DataTable::iterator row = rows->begin(); row != rows->end(); ++row)
			{
				switch(row->second.at(2)->intValue)
				{
					case 107:
					{
						_isFavorite = row->second.at(3)->intValue;
						break;
					}
					case 111:
					{
						_detMovementScene = row->second.at(3)->intValue;
						break;
					}
					case 112:
					{
						auto MasterValue = row->second.at(5)->binaryValue;
						_detMasterValue = std::string(MasterValue->begin(), MasterValue->end());
						break;
					}
					case 113:
					{
						auto MasterColor = row->second.at(5)->binaryValue;
						_detMasterColor = std::string(MasterColor->begin(), MasterColor->end());
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
	bool LightControllerV2::processPacket(PLoxonePacket loxonePacket)
	{
		try
		{
			GD::out.printDebug("LightControllerV2::processPacket " + loxonePacket->getUuid());

			std::string variable;
			if (LoxoneControl::preProcessPacket(loxonePacket, variable))
			{
				loxonePacket->setJsonString(_json);
				return true;
			}
		}
		catch (const std::exception & ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		return false;
	}
	bool LightControllerV2::setValue(std::string method, BaseLib::PVariable parameters, std::shared_ptr<LoxonePacket> packet)
	{
		try
		{
			std::string command;
			if (method == "ValueSet")
			{
				auto value = parameters->arrayValue->at(0)->floatValue;
				command = "jdev/sps/io/" + _mandatoryFields->_uuidAction + "/" + std::to_string(value);
			}
			packet->setCommand(command);
			return true;
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		return false;
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
		try
		{
			_detAlert = control->structValue->at("details")->structValue->at("alert")->booleanValue;
			_detPresenceConnected = control->structValue->at("details")->structValue->at("presenceConnected")->booleanValue;
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
	}
	Alarm::Alarm(std::shared_ptr<BaseLib::Database::DataTable> rows): LoxoneControl(rows, 0x0200)
	{
		try
		{
			for(BaseLib::Database::DataTable::iterator row = rows->begin(); row != rows->end(); ++row)
			{
				switch(row->second.at(2)->intValue)
				{
					case 111:
					{
						_detAlert = row->second.at(3)->intValue;
						break;
					}
					case 112:
					{
						_detPresenceConnected = row->second.at(3)->intValue;
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
	bool Alarm::processPacket(PLoxonePacket loxonePacket)
	{
		try
		{
			GD::out.printDebug("Alarm::processPacket " + loxonePacket->getUuid());

			std::string variable;
			if (LoxoneControl::preProcessPacket(loxonePacket, variable))
			{
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
	bool Alarm::setValue(std::string method, BaseLib::PVariable parameters, std::shared_ptr<LoxonePacket> packet)
	{
		try
		{
			std::string command;
			if (method == "Arm")
			{
				auto value = parameters->arrayValue->at(0)->integerValue;
				if(value != -1)	command = "jdev/sps/io/" + _mandatoryFields->_uuidAction + "/on/" + std::to_string(value);
				else command = "jdev/sps/io/" + _mandatoryFields->_uuidAction + "/on";
			}
			else if (method == "setOn")
			{
				command = "jdev/sps/io/" + _mandatoryFields->_uuidAction + "/" + "on";
			}
			
			packet->setCommand(command);
			return true;
		}
		catch (const std::exception & ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		return false;
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
	bool MediaClient::processPacket(PLoxonePacket loxonePacket)
	{
		try
		{
			GD::out.printDebug("MediaClient::processPacket " + loxonePacket->getUuid());

			std::string variable;
			if (LoxoneControl::preProcessPacket(loxonePacket, variable))
			{
				if (_json->structValue->at("state")->structValue->find("power") != _json->structValue->at("state")->structValue->end())
				{
					auto value = _json->structValue->at("state")->structValue->at("power")->floatValue;
					_json->structValue->at("state")->structValue->at("power") = PVariable(new Variable((bool)value));
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

	bool MediaClient::setValue(std::string method, BaseLib::PVariable parameters, std::shared_ptr<LoxonePacket> packet)
	{
		try
		{
			/*
			std::string command;
			if (method == "setConstString")
			{
				GD::out.printInfo("setValueTests: " + parameters->arrayValue->at(0)->stringValue);
				command = "jdev/sps/io/" + _mandatoryFields._uuidAction + "/" + parameters->arrayValue->at(0)->stringValue;
			}
			else if (method == "StateSet")
			{
				auto value = parameters->arrayValue->at(0)->booleanValue;
				command = "jdev/sps/io/" + _mandatoryFields._uuidAction + "/" + "off";
				if (value)
				{
					command = "jdev/sps/io/" + _mandatoryFields._uuidAction + "/" + "on";
				}
			}

			packet->setCommand(command);
			*/
			return true;
		}
		catch (const std::exception& ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		return false;
	}
	uint32_t MediaClient::getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID)
	{
		LoxoneControl::getDataToSave(list, peerID);
		{
			return 0;
		}
	}
}
