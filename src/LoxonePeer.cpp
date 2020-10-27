#include "LoxonePeer.h"
#include "LoxoneCentral.h"
#include "Loxone.h"
#include "PhysicalInterfaces/Miniserver.h"
#include "controls/createInstance.h"

namespace Loxone
{
std::shared_ptr<BaseLib::Systems::ICentral> LoxonePeer::getCentral()
{
	try
	{
		if(_central) return _central;
		_central = GD::family->getCentral();
		return _central;
	}
	catch(const std::exception& ex)
	{
		GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	return std::shared_ptr<BaseLib::Systems::ICentral>();
}
//wird aufgerufen wenn ein peer gepairt wird
LoxonePeer::LoxonePeer(uint32_t parentID, IPeerEventSink* eventHandler, std::shared_ptr<LoxoneControl> control) : Peer(GD::bl, parentID, eventHandler)
{
	_binaryEncoder.reset(new BaseLib::Rpc::RpcEncoder(GD::bl));
	_binaryDecoder.reset(new BaseLib::Rpc::RpcDecoder(GD::bl));
	_control = control;
}
//wird aufgerufen wenn ein vorhandener peer neu geladen wird
//danach siehe LoxonePeer::load()
LoxonePeer::LoxonePeer(int32_t id, int32_t address, std::string serialNumber, uint32_t parentID, IPeerEventSink* eventHandler) : Peer(GD::bl, id, address, serialNumber, parentID, eventHandler)
{
	_binaryEncoder.reset(new BaseLib::Rpc::RpcEncoder(GD::bl));
	_binaryDecoder.reset(new BaseLib::Rpc::RpcDecoder(GD::bl));
}

LoxonePeer::~LoxonePeer()
{
	dispose();
}

std::string LoxonePeer::handleCliCommand(std::string command)
{
	try
	{
		std::ostringstream stringStream;

		if(command == "help")
		{
			stringStream << "List of commands:" << std::endl << std::endl;
			stringStream << "For more information about the individual command type: COMMAND help" << std::endl << std::endl;
			stringStream << "unselect\t\tUnselect this peer" << std::endl;
			return stringStream.str();
		}
		return "Unknown command.\n";
	}
	catch(const std::exception& ex)
    {
        GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
    return "Error executing command. See log file for more details.\n";
}

void LoxonePeer::save(bool savePeer, bool variables, bool centralConfig)
{
	try
	{
		Peer::save(savePeer, variables, centralConfig);
	}
	catch(const std::exception& ex)
    {
    	GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
}

void LoxonePeer::saveUuids()
{
	try
	{
		std::list<Database::DataRow> list;
		if(_control->getDataToSave(list, _peerID) != 0) return;

		for(auto i = list.begin(); i != list.end(); ++i)
		{
			_bl->db->savePeerVariableAsynchronous(*i);
		}
	}
	catch (const std::exception & ex)
	{
		GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
}

void LoxonePeer::loadUuids()
{
    try
    {
        std::shared_ptr<BaseLib::Database::DataTable> rows = _bl->db->getPeerVariables(_peerID);
        if(!rows) return;
        //_control = std::shared_ptr<LoxoneControl>(createInstance::_uintControlsMap.at(_deviceType)(rows));
        _control = createInstance::createInstanceFromTypeNr(_deviceType, rows);
        if(!_control)return ;
        _uuidVariable_PeerIdMap = _control->getVariables();
    }
    catch(const std::exception& ex)
    {
        GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
}
void LoxonePeer::setPeerIdToVariableList()
{
    _uuidVariable_PeerIdMap = _control->getVariables();
    for(auto i = _uuidVariable_PeerIdMap.begin(); i != _uuidVariable_PeerIdMap.end(); ++i)
    {
        i->second->peerId = _peerID;
    }
}
void LoxonePeer::setConfigParameters()
{
    //This do not set anything to homegear rooms or roles. this are just to variables wich shows how this peer is configred in Loxone Config.
    //If you want to add a room or role then do this in homegear.
    { //Set the Roomname wich is configured in Loxone Config
        BaseLib::Systems::RpcConfigurationParameter &parameter = configCentral[1]["ROOM"];
        std::vector<uint8_t> parameterData;
        parameter.rpcParameter->convertToPacket(_control->getRoom(), parameter.mainRole(), parameterData);
        parameter.setBinaryData(parameterData);
        if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
        else saveParameter(0, ParameterGroup::Type::Enum::config, 1, "ROOM", parameterData);
        GD::out.printInfo("Info: Parameter ROOM of peer " + std::to_string(_peerID) + " and channel " + std::to_string(1) +" was set to 0x" + BaseLib::HelperFunctions::getHexString(parameterData) + ".");
        raiseRPCUpdateDevice(_peerID, 1, _serialNumber + ":" + std::to_string(1), 0);
    }
    { //Set the Categorie wich is configured in Loxone Config
        BaseLib::Systems::RpcConfigurationParameter &parameter = configCentral[1]["CATEGORY"];
        std::vector<uint8_t> parameterData;
        parameter.rpcParameter->convertToPacket(_control->getCat(), parameter.mainRole(), parameterData);
        parameter.setBinaryData(parameterData);
        if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
        else saveParameter(0, ParameterGroup::Type::Enum::config, 1, "CATEGORY", parameterData);
        GD::out.printInfo("Info: Parameter CATEGORY of peer " + std::to_string(_peerID) + " and channel " + std::to_string(1) + " was set to 0x" + BaseLib::HelperFunctions::getHexString(parameterData) + ".");
        raiseRPCUpdateDevice(_peerID, 1, _serialNumber + ":" + std::to_string(1), 0);
    }
    { //Set ExtraData  from this peer
        std::list<extraData> data;
        if(_control->getExtraData(data) == 0)
        {
            for(auto i = data.begin(); i != data.end(); ++i)
            {
                BaseLib::Systems::RpcConfigurationParameter &parameter = configCentral[i->channel][i->variable];
                std::vector<uint8_t> parameterData;
                parameter.rpcParameter->convertToPacket(i->value, parameter.mainRole(), parameterData);
                parameter.setBinaryData(parameterData);
                if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
                else saveParameter(0, ParameterGroup::Type::Enum::config, i->channel, i->variable, parameterData);
                GD::out.printInfo("Info: Parameter "+ i->variable +" of peer " + std::to_string(_peerID) + " and channel " + std::to_string(i->channel) +" was set to 0x" + BaseLib::HelperFunctions::getHexString(parameterData) + ".");
                raiseRPCUpdateDevice(_peerID, i->channel, _serialNumber + ":" + std::to_string(1), 0);
            }
        }
    }
}
void LoxonePeer::homegearStarted()
{
    Peer::homegearStarted();
    //serviceMessages->setUnreach(true,false);
}
void LoxonePeer::setPhysicalInterfaceId(std::string id)
{
    if(id.empty() || (GD::physicalInterfaces.find(id) != GD::physicalInterfaces.end() && GD::physicalInterfaces.at(id)))
    {
        _physicalInterfaceId = id;
        setPhysicalInterface(id.empty() ? GD::defaultPhysicalInterface : GD::physicalInterfaces.at(_physicalInterfaceId));
        saveVariable(19, _physicalInterfaceId);
    }
    if(!_physicalInterface) _physicalInterface = GD::defaultPhysicalInterface;
}

void LoxonePeer::setPhysicalInterface(std::shared_ptr<Miniserver> interface)
{
	try
	{
		if(!interface) return;
		_physicalInterface = interface;
	}
	catch(const std::exception& ex)
	{
		GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
}

void LoxonePeer::loadVariables(BaseLib::Systems::ICentral* central, std::shared_ptr<BaseLib::Database::DataTable>& rows)
{
	try
	{
		if(!rows) rows = _bl->db->getPeerVariables(_peerID);
		Peer::loadVariables(central, rows);

		for(BaseLib::Database::DataTable::iterator row = rows->begin(); row != rows->end(); ++row)
		{
			switch(row->second.at(2)->intValue)
			{
			case 19:
                _physicalInterfaceId = row->second.at(4)->textValue;
                if(!_physicalInterfaceId.empty() && GD::physicalInterfaces.find(_physicalInterfaceId) != GD::physicalInterfaces.end()) setPhysicalInterface(GD::physicalInterfaces.at(_physicalInterfaceId));
                break;
			}
		}

		if(!_physicalInterface)
		{
			GD::out.printError("Error: Could not find correct physical interface for peer " + std::to_string(_peerID) + ". The peer might not work correctly. The expected interface ID is: " + _physicalInterfaceId);
            _physicalInterface = GD::defaultPhysicalInterface;
		}
	}
	catch(const std::exception& ex)
    {
    	GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
}

bool LoxonePeer::load(BaseLib::Systems::ICentral* central)
{
    try
    {
        std::shared_ptr<BaseLib::Database::DataTable> rows;
        loadVariables(central, rows);
        _rpcDevice = GD::family->getRpcDevices()->find(_deviceType, _firmwareVersion, -1);
		if(!_rpcDevice)
		{
			GD::out.printError("Error loading peer " + std::to_string(_peerID) + ": Device type not found: 0x" + BaseLib::HelperFunctions::getHexString(_deviceType) + " Firmware version: " + std::to_string(_firmwareVersion));
			return false;
		}
		initializeTypeString();
		loadConfig();
		initializeCentralConfig();

		serviceMessages.reset(new BaseLib::Systems::ServiceMessages(_bl, _peerID, _serialNumber, this));
		serviceMessages->load();

		return true;
	}
	catch(const std::exception& ex)
    {
    	GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
    return false;
}

void LoxonePeer::saveVariables()
{
	try
	{
		if(_peerID == 0) return;
		Peer::saveVariables();

		saveVariable(19, _physicalInterfaceId);
	}
	catch(const std::exception& ex)
    {
    	GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
}

PParameterGroup LoxonePeer::getParameterSet(int32_t channel, ParameterGroup::Type::Enum type)
{
	try
	{
		PParameterGroup parameterGroup = _rpcDevice->functions.at(channel)->getParameterGroup(type);
		if(!parameterGroup || parameterGroup->parameters.empty())
		{
			GD::out.printDebug("Debug: Parameter set of type " + std::to_string(type) + " not found for channel " + std::to_string(channel));
			return PParameterGroup();
		}
		return parameterGroup;
	}
	catch(const std::exception& ex)
	{
		GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	return PParameterGroup();
}

void LoxonePeer::getValuesFromPacket(PLoxonePacket packet, std::vector<FrameValues>& frameValues)
{
	try
    {
        if(!_rpcDevice) return;
        if (_rpcDevice->packetsByFunction1.find(packet->getMethod()) == _rpcDevice->packetsByFunction1.end()) return;
        std::pair<PacketsByFunction::iterator, PacketsByFunction::iterator> range = _rpcDevice->packetsByFunction1.equal_range(packet->getMethod());
		if(range.first == _rpcDevice->packetsByFunction1.end()) return;
		PacketsByFunction::iterator i = range.first;
        do
        {
			FrameValues currentFrameValues;
            PPacket frame(i->second);
            if(!frame) continue;
			if (frame->direction != BaseLib::DeviceDescription::Packet::Direction::Enum::toCentral) continue;
			int32_t channel = -1;
			if (frame->channel > -1) channel = frame->channel;
			currentFrameValues.frameID = frame->id;

			for (JsonPayloads::iterator j = frame->jsonPayloads.begin(); j != frame->jsonPayloads.end(); ++j)
			{
				BaseLib::PVariable value;
				if ((*j)->constValueBooleanSet) value.reset(new BaseLib::Variable((*j)->constValueBoolean));
				else if ((*j)->constValueIntegerSet) value.reset(new BaseLib::Variable((*j)->constValueInteger));
				else if ((*j)->constValueDecimalSet) value.reset(new BaseLib::Variable((*j)->constValueDecimal));
				else if ((*j)->constValueStringSet) value.reset(new BaseLib::Variable((*j)->constValueString));
				else
				{
					value = packet->getJsonString();
					if (!value) continue;
					bool notFound = false;
					for (std::vector<std::string>::iterator k = (*j)->keyPath.begin(); k != (*j)->keyPath.end(); ++k)
					{
						BaseLib::Struct::iterator jsonValueIterator = value->structValue->find(*k);
						if (jsonValueIterator == value->structValue->end() || !jsonValueIterator->second)
						{
							notFound = true;
							break;
						}
						value = jsonValueIterator->second;
					}
					if (notFound) continue;
				}

				if (!value) continue;

				for (std::vector<PParameter>::iterator k = frame->associatedVariables.begin(); k != frame->associatedVariables.end(); ++k)
				{
					if ((*k)->physical->groupId != (*j)->parameterId) continue;
					currentFrameValues.parameterSetType = (*k)->parent()->type();
					
					bool setValues = false;

					if (currentFrameValues.paramsetChannels.empty()) //Fill paramsetChannels
					{
						int32_t startChannel = (channel < 0) ? 0 : channel;
						int32_t endChannel;
						//When fixedChannel is -2 (means '*') cycle through all channels
						if (frame->channel == -2)
						{
							startChannel = 0;
							endChannel = _rpcDevice->functions.rbegin()->first;
						}
						else endChannel = startChannel;

						for (int32_t l = startChannel; l <= endChannel; l++)
						{
							Functions::iterator functionIterator = _rpcDevice->functions.find(1);
							if (functionIterator == _rpcDevice->functions.end()) continue;
							PParameterGroup parameterGroup = functionIterator->second->getParameterGroup(currentFrameValues.parameterSetType);
							
							if (!parameterGroup) continue;
							if (!parameterGroup || parameterGroup->parameters.find((*k)->id) == parameterGroup->parameters.end()) continue;
							currentFrameValues.paramsetChannels.push_back(l);
							currentFrameValues.values[(*k)->id].channels.push_back(l);
							setValues = true;
						}
					}
					else //Use paramsetChannels
					{
						for (std::list<uint32_t>::const_iterator l = currentFrameValues.paramsetChannels.begin(); l != currentFrameValues.paramsetChannels.end(); ++l)
						{
							Functions::iterator functionIterator = _rpcDevice->functions.find(*l);
							if (functionIterator == _rpcDevice->functions.end()) continue;
							PParameterGroup parameterGroup = functionIterator->second->getParameterGroup(currentFrameValues.parameterSetType);
							if (!parameterGroup || parameterGroup->parameters.find((*k)->id) == parameterGroup->parameters.end()) continue;
							currentFrameValues.values[(*k)->id].channels.push_back(*l);
							setValues = true;
						}
					}

					if (setValues)
					{
						//This is a little nasty and costs a lot of resources, but we need to run the data through the packet converter
						std::vector<uint8_t> encodedData;
						_binaryEncoder->encodeResponse(value, encodedData);
						PVariable data = (*k)->convertFromPacket(encodedData, Role(), true);
						(*k)->convertToPacket(data, Role(), currentFrameValues.values[(*k)->id].value);
					}
				}
			}
            if(!currentFrameValues.values.empty()) frameValues.push_back(currentFrameValues);
        } while(++i != range.second && i != _rpcDevice->packetsByFunction1.end());
    }
    catch(const std::exception& ex)
    {
        GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
}

void LoxonePeer::packetReceived(std::shared_ptr<LoxonePacket> packet)
{
	try
    {
        if(_disposing || !packet || !_rpcDevice) return;
		GD::out.printDebug("Loxone Peer: packetReceived-> at peer " + std::to_string(_peerID));
		switch(packet->getPacketType())
		{
			case LoxonePacketType::LoxoneValueStatesPacket:
			{
				auto cPacket = std::dynamic_pointer_cast<LoxoneValueStatesPacket>(packet);
				if(!cPacket) return;
				if(!_control->processPacket(cPacket)) return;
				break;
			}
			case LoxonePacketType::LoxoneTextStatesPacket:
			{
				auto cPacket = std::dynamic_pointer_cast<LoxoneTextStatesPacket>(packet);
				if(!cPacket) return;
                if(!_control->processPacket(cPacket)) return;
				break;
			}
			case LoxonePacketType::LoxoneBinaryFilePacket:
			{
				auto cPacket = std::dynamic_pointer_cast<LoxoneBinaryFilePacket>(packet);
				if(!cPacket) return;
                if(!_control->processPacket(cPacket)) return;
				break;
			}
			case LoxonePacketType::LoxoneTextMessage:
			{
				auto cPacket = std::dynamic_pointer_cast<LoxoneTextmessagePacket>(packet);
				if(!cPacket) return;
                if(!_control->processPacket(cPacket)) return;
				break;
			}
			case LoxonePacketType::LoxoneWeatherStatesPacket:
			{
				auto cPacket = std::dynamic_pointer_cast<LoxoneWeatherStatesPacket>(packet);
				if(!cPacket) return;
                if(!_control->processPacket(cPacket)) return;
				break;
			}
			case LoxonePacketType::LoxoneDaytimerStatesPacket:
			{
				auto cPacket = std::dynamic_pointer_cast<LoxoneDaytimerStatesPacket>(packet);
				if(!cPacket) return;
                if(!_control->processPacket(cPacket)) return;
				break;
			}
		    default:
            {
                break;
            }
		}

        auto central = std::dynamic_pointer_cast<LoxoneCentral>(getCentral());
        if(!central) return;
        setLastPacketReceived();
        serviceMessages->endUnreach();
        
		std::vector<FrameValues> frameValues;
        getValuesFromPacket(packet, frameValues);
        std::map<uint32_t, std::shared_ptr<std::vector<std::string>>> valueKeys;
        std::map<uint32_t, std::shared_ptr<std::vector<PVariable>>> rpcValues;

        //Loop through all matching frames
        for(std::vector<FrameValues>::iterator a = frameValues.begin(); a != frameValues.end(); ++a)
        {
			PPacket frame;
            if(!a->frameID.empty()) frame = _rpcDevice->packetsById.at(a->frameID);
            if(!frame) continue;
			
			for(std::map<std::string, FrameValue>::iterator i = a->values.begin(); i != a->values.end(); ++i)
            {
				for(std::list<uint32_t>::const_iterator j = a->paramsetChannels.begin(); j != a->paramsetChannels.end(); ++j)
                {
					if(std::find(i->second.channels.begin(), i->second.channels.end(), *j) == i->second.channels.end()) continue;
					
					BaseLib::Systems::RpcConfigurationParameter& parameter = valuesCentral[*j][i->first];
					if (parameter.equals(i->second.value)) continue;
					
					if(!valueKeys[*j] || !rpcValues[*j])
                    {
                        valueKeys[*j].reset(new std::vector<std::string>());
                        rpcValues[*j].reset(new std::vector<PVariable>());
                    }

					parameter.setBinaryData(i->second.value);
                    if(parameter.databaseId > 0) saveParameter(parameter.databaseId, i->second.value);
                    else saveParameter(0, ParameterGroup::Type::Enum::variables, *j, i->first, i->second.value);
                    if(_bl->debugLevel >= 4) GD::out.printInfo("Info: " + i->first + " on channel " + std::to_string(*j) + " of peer " + std::to_string(_peerID) + " with serial number " + _serialNumber  + " was set to 0x" + BaseLib::HelperFunctions::getHexString(i->second.value) + ".");

                    if(parameter.rpcParameter)
                    {
                        //Process service messages
                        if(parameter.rpcParameter->service && !i->second.value.empty())
                        {
                            if(parameter.rpcParameter->logical->type == ILogical::Type::Enum::tEnum)
                            {
                                serviceMessages->set(i->first, i->second.value.at(0), *j);
                            }
                            else if(parameter.rpcParameter->logical->type == ILogical::Type::Enum::tBoolean)
                            {
								serviceMessages->set(i->first, parameter.rpcParameter->convertFromPacket(i->second.value, parameter.mainRole(), true)->booleanValue);
							}
                        }
                        valueKeys[*j]->push_back(i->first);
						rpcValues[*j]->push_back(parameter.rpcParameter->convertFromPacket(i->second.value, parameter.mainRole(), true));
					}
                }
            }
        }

        if(!rpcValues.empty())
        {
            for(std::map<uint32_t, std::shared_ptr<std::vector<std::string>>>::iterator j = valueKeys.begin(); j != valueKeys.end(); ++j)
            {
                if(j->second->empty()) continue;
                std::string eventSource = "device-" + std::to_string(_peerID);
                std::string address(_serialNumber + ":" + std::to_string(j->first));
                raiseEvent(eventSource, _peerID, j->first, j->second, rpcValues.at(j->first));
                raiseRPCEvent(eventSource, _peerID, j->first, address, j->second, rpcValues.at(j->first));
            }
        }
    }
    catch(const std::exception& ex)
    {
        GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
}

//RPC Methods
PVariable LoxonePeer::getDeviceInfo(BaseLib::PRpcClientInfo clientInfo, std::map<std::string, bool> fields)
{
	try
	{
		PVariable info(Peer::getDeviceInfo(clientInfo, fields));
		if(info->errorStruct) return info;

		if(fields.empty() || fields.find("INTERFACE") != fields.end()) info->structValue->insert(StructElement("INTERFACE", PVariable(new Variable(_physicalInterface->getID()))));

		return info;
	}
	catch(const std::exception& ex)
    {
    	GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
    return PVariable();
}

PVariable LoxonePeer::getDeviceDescription(PRpcClientInfo clientInfo, int32_t channel, std::map<std::string, bool> fields)
{
    try
    {
        PVariable description(Peer::getDeviceDescription(clientInfo, channel, fields));
        if(description->errorStruct || description->structValue->empty()) return description;

        //todo: find a way to display the additional information in the admin ui. till now it looks like the admin ui needs to be changed to diseplay the additional infoarmtions
        description->structValue->emplace("ROOMNAME", std::make_shared<Variable>(_control->getRoom()));
        description->structValue->emplace("CATEGORIES", std::make_shared<Variable>(_control->getCat()));
        description->structValue->emplace("LOXONE_UUID", std::make_shared<Variable>(_control->getUuidAction()));

        return description;
    }
    catch(const std::exception& ex)
    {
        GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
    return PVariable();
}

PVariable LoxonePeer::getParamsetDescription(BaseLib::PRpcClientInfo clientInfo, int32_t channel, ParameterGroup::Type::Enum type, uint64_t remoteID, int32_t remoteChannel, bool checkAcls)
{
	try
	{
		if(_disposing) return Variable::createError(-32500, "Peer is disposing.");
		if(channel < 0) channel = 0;
		Functions::iterator functionIterator = _rpcDevice->functions.find(channel);
		if(functionIterator == _rpcDevice->functions.end()) return Variable::createError(-2, "Unknown channel");
		PParameterGroup parameterGroup = functionIterator->second->getParameterGroup(type);
		if(!parameterGroup) return Variable::createError(-3, "Unknown parameter set");

		return Peer::getParamsetDescription(clientInfo, channel, parameterGroup, checkAcls);
	}
	catch(const std::exception& ex)
    {
        GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
    return Variable::createError(-32500, "Unknown application error.");
}

PVariable LoxonePeer::putParamset(BaseLib::PRpcClientInfo clientInfo, int32_t channel, ParameterGroup::Type::Enum type, uint64_t remoteID, int32_t remoteChannel, PVariable variables, bool checkAcls, bool onlyPushing)
{
	try
	{
		if(_disposing) return Variable::createError(-32500, "Peer is disposing.");
		if(channel < 0) channel = 0;
        if(remoteChannel < 0) remoteChannel = 0;
		Functions::iterator functionIterator = _rpcDevice->functions.find(channel);
		if(functionIterator == _rpcDevice->functions.end()) return Variable::createError(-2, "Unknown channel");
        if(type == ParameterGroup::Type::none) type = ParameterGroup::Type::link;
		PParameterGroup parameterGroup = functionIterator->second->getParameterGroup(type);
		if(!parameterGroup) return Variable::createError(-3, "Unknown parameter set");
		if(variables->structValue->empty()) return PVariable(new Variable(VariableType::tVoid));

		auto central = getCentral();
		if(!central) return Variable::createError(-32500, "Could not get central.");

        if(type == ParameterGroup::Type::Enum::config)
        {
            bool configChanged = false;
            for(Struct::iterator i = variables->structValue->begin(); i != variables->structValue->end(); ++i)
            {
                if(i->first.empty() || !i->second) continue;
                std::unordered_map<uint32_t, std::unordered_map<std::string, BaseLib::Systems::RpcConfigurationParameter>>::iterator channelIterator = configCentral.find(channel);
                if(channelIterator == configCentral.end()) continue;
                std::unordered_map<std::string, BaseLib::Systems::RpcConfigurationParameter>::iterator parameterIterator = channelIterator->second.find(i->first);
                if(parameterIterator == channelIterator->second.end()) continue;
                BaseLib::Systems::RpcConfigurationParameter& parameter = parameterIterator->second;
                if(!parameter.rpcParameter) continue;

                std::vector<uint8_t> parameterData;
                parameter.rpcParameter->convertToPacket(i->second, parameter.mainRole(), parameterData);
                parameter.setBinaryData(parameterData);
                if(parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
                else saveParameter(0, ParameterGroup::Type::Enum::config, channel, i->first, parameterData);
                GD::out.printInfo("Info: Parameter " + i->first + " of peer " + std::to_string(_peerID) + " and channel " + std::to_string(channel) + " was set to 0x" + BaseLib::HelperFunctions::getHexString(parameterData) + ".");
                if(parameter.rpcParameter->physical->operationType != IPhysical::OperationType::Enum::config && parameter.rpcParameter->physical->operationType != IPhysical::OperationType::Enum::configString) continue;
                configChanged = true;
            }
            if(configChanged) raiseRPCUpdateDevice(_peerID, channel, _serialNumber + ":" + std::to_string(channel), 0);
        }
		else if(type == ParameterGroup::Type::Enum::variables)
		{
			for(Struct::iterator i = variables->structValue->begin(); i != variables->structValue->end(); ++i)
			{
				if(i->first.empty() || !i->second) continue;

				if(checkAcls && !clientInfo->acls->checkVariableWriteAccess(central->getPeer(_peerID), channel, i->first)) continue;

				setValue(clientInfo, channel, i->first, i->second, true);
			}
		}
		else
		{
			return Variable::createError(-3, "Parameter set type is not supported.");
		}
		return std::make_shared<Variable>(VariableType::tVoid);
	}
	catch(const std::exception& ex)
    {
        GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
    return Variable::createError(-32500, "Unknown application error.");
}

PVariable LoxonePeer::getParamset(BaseLib::PRpcClientInfo clientInfo, int32_t channel, ParameterGroup::Type::Enum type, uint64_t remoteID, int32_t remoteChannel, bool checkAcls)
{
	try
	{
		if(_disposing) return Variable::createError(-32500, "Peer is disposing.");
		if(channel < 0) channel = 0;
		if(remoteChannel < 0) remoteChannel = 0;
		Functions::iterator functionIterator = _rpcDevice->functions.find(channel);
		if(functionIterator == _rpcDevice->functions.end()) return Variable::createError(-2, "Unknown channel");
		PParameterGroup parameterGroup = functionIterator->second->getParameterGroup(type);
		if(!parameterGroup) return Variable::createError(-3, "Unknown parameter set");
		PVariable variables(new Variable(VariableType::tStruct));

		auto central = getCentral();
		if(!central) return Variable::createError(-32500, "Could not get central.");

		for(Parameters::iterator i = parameterGroup->parameters.begin(); i != parameterGroup->parameters.end(); ++i)
		{
			if(i->second->id.empty()) continue;
			if(!i->second->visible && !i->second->service && !i->second->internal && !i->second->transform)
			{
				GD::out.printDebug("Debug: Omitting parameter " + i->second->id + " because of it's ui flag.");
				continue;
			}
			PVariable element;
			if(type == ParameterGroup::Type::Enum::variables)
			{
				if(checkAcls && !clientInfo->acls->checkVariableReadAccess(central->getPeer(_peerID), channel, i->first)) continue;
				if(!i->second->readable) continue;
				if(valuesCentral.find(channel) == valuesCentral.end()) continue;
				if(valuesCentral[channel].find(i->second->id) == valuesCentral[channel].end()) continue;
				auto& parameter = valuesCentral[channel][i->second->id];
				std::vector<uint8_t> parameterData = valuesCentral[channel][i->second->id].getBinaryData();
				element = i->second->convertFromPacket(parameterData, parameter.mainRole(), false);
			}
            else if(type == ParameterGroup::Type::Enum::config)
            {
                if(configCentral.find(channel) == configCentral.end()) continue;
                if(configCentral[channel].find(i->second->id) == configCentral[channel].end()) continue;
                auto& parameter = configCentral[channel][i->second->id];
                std::vector<uint8_t> parameterData = parameter.getBinaryData();
                element = i->second->convertFromPacket(parameterData, parameter.mainRole(), false);
            }

			if(!element) continue;
			if(element->type == VariableType::tVoid) continue;
			variables->structValue->insert(StructElement(i->second->id, element));
		}
		return variables;
	}
	catch(const std::exception& ex)
    {
        GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
    return Variable::createError(-32500, "Unknown application error.");
}

PVariable LoxonePeer::setValue(BaseLib::PRpcClientInfo clientInfo, uint32_t channel, std::string valueKey, PVariable value, bool wait)
{
	try
    {
		Peer::setValue(clientInfo, channel, valueKey, value, wait); //Ignore result, otherwise setHomegearValue might not be executed
		if (_disposing) return Variable::createError(-32500, "Peer is disposing.");
		if (valueKey.empty()) return Variable::createError(-5, "Value key is empty.");
		if (channel == 0 && serviceMessages->set(valueKey, value->booleanValue)) return PVariable(new Variable(VariableType::tVoid));
		std::unordered_map<uint32_t, std::unordered_map<std::string, RpcConfigurationParameter>>::iterator channelIterator = valuesCentral.find(channel);
		if (channelIterator == valuesCentral.end()) return Variable::createError(-2, "Unknown channel.");
		std::unordered_map<std::string, RpcConfigurationParameter>::iterator parameterIterator = channelIterator->second.find(valueKey);
		if (parameterIterator == channelIterator->second.end()) return Variable::createError(-5, "Unknown parameter (1).");
		PParameter rpcParameter = parameterIterator->second.rpcParameter;
		if (!rpcParameter) return Variable::createError(-5, "Unknown parameter (2).");
		if (rpcParameter->logical->type == ILogical::Type::tAction && !value->booleanValue) return Variable::createError(-5, "Parameter of type action cannot be set to \"false\".");
		BaseLib::Systems::RpcConfigurationParameter& parameter = parameterIterator->second;
		std::shared_ptr<std::vector<std::string>> valueKeys(new std::vector<std::string>());
		std::shared_ptr<std::vector<PVariable>> values(new std::vector<PVariable>());

		if (rpcParameter->physical->operationType == IPhysical::OperationType::Enum::store)
		{
			std::vector<uint8_t> parameterData;
			rpcParameter->convertToPacket(value, parameter.mainRole() ,parameterData);
			parameter.setBinaryData(parameterData);
			if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
			else saveParameter(0, ParameterGroup::Type::Enum::variables, channel, valueKey, parameterData);
			value = rpcParameter->convertFromPacket(parameterData, parameter.mainRole(), false);
			if (rpcParameter->readable)
			{
				valueKeys->push_back(valueKey);
				values->push_back(value);
			}
			if (!valueKeys->empty())
			{
				std::string address(_serialNumber + ":" + std::to_string(channel));
				raiseEvent(clientInfo->initInterfaceId, _peerID, channel, valueKeys, values);
				raiseRPCEvent(clientInfo->initInterfaceId, _peerID, channel, address, valueKeys, values);
			}
			return PVariable(new Variable(VariableType::tVoid));
		}
		else if (rpcParameter->physical->operationType != IPhysical::OperationType::Enum::command) return Variable::createError(-6, "Parameter is not settable.");
		if (rpcParameter->setPackets.empty()) return Variable::createError(-6, "parameter is read only");
		std::string setRequest = rpcParameter->setPackets.front()->id;
		PacketsById::iterator packetIterator = _rpcDevice->packetsById.find(setRequest);
		if (packetIterator == _rpcDevice->packetsById.end()) return Variable::createError(-6, "No frame was found for parameter " + valueKey);
		PPacket frame = packetIterator->second;
		std::vector<uint8_t> parameterData;
		rpcParameter->convertToPacket(value, parameter.mainRole(), parameterData);
		parameter.setBinaryData(parameterData);
		if (parameter.databaseId > 0) saveParameter(parameter.databaseId, parameterData);
		else saveParameter(0, ParameterGroup::Type::Enum::variables, channel, valueKey, parameterData);
		if (_bl->debugLevel > 4) GD::out.printDebug("Debug: " + valueKey + " of peer " + std::to_string(_peerID) + " with serial number " + _serialNumber + ":" + std::to_string(channel) + " was set to " + BaseLib::HelperFunctions::getHexString(parameterData) + ", " + value->print(false, false, true) + ".");

		value = rpcParameter->convertFromPacket(parameterData, parameter.mainRole(), false);
		if (rpcParameter->readable)
		{
			valueKeys->push_back(valueKey);
			values->push_back(value);
		}

		PVariable parameters(new Variable(BaseLib::VariableType::tArray));
		for (JsonPayloads::iterator i = frame->jsonPayloads.begin(); i != frame->jsonPayloads.end(); ++i)
		{
            if ((*i)->constValueIntegerSet)
			{
				parameters->arrayValue->push_back(PVariable(new Variable((*i)->constValueInteger)));
				continue;
			}
			if ((*i)->constValueBooleanSet)
			{
				parameters->arrayValue->push_back(PVariable(new Variable((*i)->constValueBoolean)));
				continue;
			}
			if ((*i)->constValueStringSet)
			{
				parameters->arrayValue->push_back(PVariable(new Variable((*i)->constValueString)));
				continue;
			}
            if ((*i)->constValueDecimalSet)
            {
                parameters->arrayValue->push_back(PVariable(new Variable((*i)->constValueDecimal)));
                continue;
            }

			//We can't just search for param, because it is ambiguous (see for example LEVEL for HM-CC-TC).
			if ((*i)->parameterId == rpcParameter->physical->groupId)
			{
				parameters->arrayValue->push_back(_binaryDecoder->decodeResponse(parameterData));
			}
			//Search for all other parameters
			else
			{
				bool paramFound = false;
				for (std::unordered_map<std::string, BaseLib::Systems::RpcConfigurationParameter>::iterator j = valuesCentral[channel].begin(); j != valuesCentral[channel].end(); ++j)
				{
					if (!j->second.rpcParameter) continue;
					if ((*i)->parameterId == j->second.rpcParameter->physical->groupId)
					{
						std::vector<uint8_t> additionalParameterData = j->second.getBinaryData();
						parameters->arrayValue->push_back(_binaryDecoder->decodeResponse(additionalParameterData));
						paramFound = true;
						break;
					}
				}
				if(!paramFound)
				{
                    for (std::unordered_map<std::string, BaseLib::Systems::RpcConfigurationParameter>::iterator j = configCentral[channel].begin();
                         j != configCentral[channel].end(); ++j) {
                        if (!j->second.rpcParameter) continue;
                        if ((*i)->parameterId == j->second.rpcParameter->physical->groupId) {
                            std::vector<uint8_t> additionalParameterData = j->second.getBinaryData();
                            parameters->arrayValue->push_back(_binaryDecoder->decodeResponse(additionalParameterData));
                            paramFound = true;
                            break;
                        }
                    }
                }
				if (!paramFound) GD::out.printError("Error constructing packet. param \"" + (*i)->parameterId + "\" not found. Peer: " + std::to_string(_peerID) + " Serial number: " + _serialNumber + " Frame: " + frame->id);
			}
		}

		std::string command;
		bool isSecured;
		if(!_control->setValue(frame, parameters, command, isSecured)) return Variable::createError(-32500, "Loxone Control can not create packet");
        GD::out.printDebug(command);
        std::shared_ptr<LoxonePacket> packet (new LoxonePacket(command, isSecured));
		_physicalInterface->sendPacket(packet);

		if (!valueKeys->empty())
		{
			std::string address(_serialNumber + ":" + std::to_string(channel));
			raiseEvent(clientInfo->initInterfaceId, _peerID, channel, valueKeys, values);
			raiseRPCEvent(clientInfo->initInterfaceId, _peerID, channel, address, valueKeys, values);
		}

		return PVariable(new Variable(VariableType::tVoid));
	}
	catch(const std::exception& ex)
    {
        GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
    return Variable::createError(-32500, "Unknown application error. See error log for more details.");
}
//End RPC methods
}
