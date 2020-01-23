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
		{"Alarm", &createInstance<Alarm>},
	};
    const std::unordered_map<uint32_t, LoxoneControl* (*)(PVariable, std::string, std::string)> LoxoneControl::_uintControlsMap =
    {
            {0x100, &createInstance<Pushbutton>},//Pushbutton
            //{0x100, &createInstance<Pushbutton>}, Switch
            {0x101, &createInstance<Slider>},
            {0x102, &createInstance<Dimmer>},
            {0x200, &createInstance<Alarm>},
    };

	LoxoneControl::LoxoneControl(PVariable control, std::string room, std::string cat, uint32_t typeNr)
	{
		try
		{
			_type = typeNr;
			_room = room;
			_cat = cat;
			_name = control->structValue->at("name")->stringValue;

			_uuidAction = control->structValue->at("uuidAction")->stringValue;
			_defaultRating = control->structValue->at("defaultRating")->integerValue;
			_isFavorite = control->structValue->at("isFavorite")->booleanValue;
			_isSecured = control->structValue->at("isSecured")->booleanValue;

			{
				UuidVariablePeer uuidVariablePeer;
				uuidVariablePeer.variable = "action";
				_uuidVariablePeerMap.emplace(_uuidAction, uuidVariablePeer);
			}

			for (auto i = control->structValue->at("states")->structValue->begin(); i != control->structValue->at("states")->structValue->end(); ++i)
			{
				UuidVariablePeer uuidVariablePeer;
				uuidVariablePeer.variable = i->first;
				_uuidVariablePeerMap.emplace(i->second->stringValue, uuidVariablePeer);
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

		if (_uuidVariablePeerMap.find(loxonePacket->getUuid()) == _uuidVariablePeerMap.end()) return false;

		auto uuidVariablePeer = _uuidVariablePeerMap.find(loxonePacket->getUuid());
		variable = (uuidVariablePeer->second.variable);

		if (variable == "action") variable = "active";

		_json = std::make_shared<Variable>(VariableType::tStruct);
		_json->structValue->operator[]("state") = PVariable(new Variable(VariableType::tStruct));

		switch (loxonePacket->getPacketType())
		{
		case LoxonePacketType::LoxoneValueStatesPacket:
		{
			PLoxoneValueStatesPacket loxoneValuePacket(std::dynamic_pointer_cast<LoxoneValueStatesPacket>(loxonePacket));
			_json->structValue->at("state")->structValue->operator[](variable) = PVariable(new Variable(loxoneValuePacket->getDValue()));
			addBooleanState(loxoneValuePacket->getDValue(), variable);
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

	Pushbutton::Pushbutton(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x100)
	{
		try
		{
			
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
				command = "jdev/sps/io/" + _uuidAction + "/" + parameters->arrayValue->at(0)->stringValue;
			}
			else if (method == "StateSet")
			{
				auto value = parameters->arrayValue->at(0)->booleanValue;
				command = "jdev/sps/io/" + _uuidAction + "/" + "off";
				if (value)
				{
					command = "jdev/sps/io/" + _uuidAction + "/" + "on";
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

	Slider::Slider(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x0101)
	{
		try
		{

		}
		catch (const std::exception & ex)
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
				command = "jdev/sps/io/" + _uuidAction + "/" + std::to_string(value);
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

	Dimmer::Dimmer(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x0102)
	{
		try
		{

		}
		catch (const std::exception & ex)
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
				command = "jdev/sps/io/" + _uuidAction + "/" + std::to_string(value);
			}
			else if (method == "setOn")
			{
				command = "jdev/sps/io/" + _uuidAction + "/" + "on";
			}
			else if (method == "setOff")
			{
				command = "jdev/sps/io/" + _uuidAction + "/" + "off";
			}
			else if (method == "StateSet")
			{
				auto value = parameters->arrayValue->at(0)->booleanValue;
				command = "jdev/sps/io/" + _uuidAction + "/" + "off";
				if (value)
				{
					command = "jdev/sps/io/" + _uuidAction + "/" + "on";
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
	Alarm::Alarm(PVariable control, std::string room, std::string cat) : LoxoneControl(control, room, cat, 0x0200)
	{
		try
		{
			
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

			if (_uuidVariablePeerMap.find(loxonePacket->getUuid()) == _uuidVariablePeerMap.end()) return false;

			PVariable json = std::make_shared<Variable>(VariableType::tStruct);
			json->structValue->operator[]("state") = PVariable(new Variable(VariableType::tStruct));
			return true;
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
				if(value != -1)	command = "jdev/sps/io/" + _uuidAction + "/on/" + std::to_string(value);
				else command = "jdev/sps/io/" + _uuidAction + "/on";
			}
			else if (method == "setOn")
			{
				command = "jdev/sps/io/" + _uuidAction + "/" + "on";
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
}