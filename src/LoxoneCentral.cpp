#include "LoxoneCentral.h"
#include "PhysicalInterfaces/Miniserver.h"
#include "Loxone.h"

#include <iomanip>
#include <iostream>

namespace Loxone
{

LoxoneCentral::LoxoneCentral(ICentralEventSink* eventHandler) : BaseLib::Systems::ICentral(LOXONE_FAMILY_ID, GD::bl, eventHandler)
{
	init();
}

LoxoneCentral::LoxoneCentral(uint32_t deviceID, std::string serialNumber, int32_t address, ICentralEventSink* eventHandler) : BaseLib::Systems::ICentral(LOXONE_FAMILY_ID, GD::bl, deviceID, serialNumber, address, eventHandler)
{
	init();
}

void LoxoneCentral::init()
{
    if(_initialized) return; //Prevent running init two times
    _initialized = true;

	for (auto& physicalInterface : GD::physicalInterfaces)
	{
		_physicalInterfaceEventhandlers[physicalInterface.first] = physicalInterface.second->addEventHandler((BaseLib::Systems::IPhysicalInterface::IPhysicalInterfaceEventSink*)this);
	}
}

LoxoneCentral::~LoxoneCentral()
{
    dispose();
}

void LoxoneCentral::dispose(bool wait)
{
	try
	{
		if(_disposing) return;
		_disposing = true;
		GD::out.printDebug("Removing device " + std::to_string(_deviceId) + " from physical device's event queue...");
        for(auto& physicalInterface : GD::physicalInterfaces)
        {
            //Just to make sure cycle through all physical devices. If event handler is not removed => segfault
            physicalInterface.second->removeEventHandler(_physicalInterfaceEventhandlers[physicalInterface.first]);
        }

        #if DEBUG
            if(_rawPacketLog.is_open()) _rawPacketLog.close();
        #endif
	}
    catch(const std::exception& ex)
    {
        GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
}

bool LoxoneCentral::onPacketReceived(std::string& senderId, std::shared_ptr<BaseLib::Systems::Packet> packet)
{
	try{
        if(_disposing) return false;
        PLoxonePacket loxonePacket(std::dynamic_pointer_cast<LoxonePacket>(packet));
        if(!loxonePacket) return false;

        #if DEBUG
            auto rawPacket = BaseLib::Rpc::JsonEncoder::encode(loxonePacket->getRawPacketStruct());
            if(!_rawPacketLog.is_open()) _rawPacketLog.open("/var/log/homegear/loxoneRawPacket.log", std::ios_base::app);
            if(_rawPacketLog.is_open()) _rawPacketLog << rawPacket << std::endl;
        #endif

		if(_uuidPeerIdMap.find(loxonePacket->getUuid()) == _uuidPeerIdMap.end()) {
            GD::out.printDebug("Loxone Central: onPacketReceived, Peer not known -> " + loxonePacket->getUuid());
            return false;
        }
        uint64_t peerId = _uuidPeerIdMap.at(loxonePacket->getUuid());
		auto peer = getPeer(peerId);
		if (peer){
			peer->packetReceived(loxonePacket);
			return true;
		}
    }
    catch(const std::exception& ex){
        GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
    return false;
}

void LoxoneCentral::loadPeers()
{
	try{
		std::shared_ptr<BaseLib::Database::DataTable> rows = _bl->db->getPeers(_deviceId);
		for(BaseLib::Database::DataTable::iterator row = rows->begin(); row != rows->end(); ++row){
			int32_t peerId = row->second.at(0)->intValue;
			GD::out.printMessage("Loading peer " + std::to_string(peerId));
			
			size_t nodeId = row->second.at(2)->intValue;
			std::string serial = row->second.at(3)->textValue;
			uint32_t type = row->second.at(4)->intValue;

			auto peer = std::make_shared<LoxonePeer>(peerId, nodeId, serial, _deviceId, this);
			if(!peer->load(this)) continue;

            //{{{ Generate a map uuid -> peerId
            auto uuidVariableMap = peer->getUuidVariableMap();
            if(!uuidVariableMap) continue;
            for(auto i = uuidVariableMap->begin(); i != uuidVariableMap->end(); ++i) {
                _uuidPeerIdMap.emplace(i->second, peer->getID());
            }
            //}}}

			if(!peer->getRpcDevice()) continue;
			std::lock_guard<std::mutex> peersGuard(_peersMutex);
			if(!peer->getSerialNumber().empty()) _peersBySerial[peer->getSerialNumber()] = peer;
			_peersById[peerId] = peer;
		}
	}
	catch(const std::exception& ex)
    {
    	GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
}

void LoxoneCentral::savePeers(bool full)
{
	try
	{
		std::lock_guard<std::mutex> peersGuard(_peersMutex);
		for(auto i = _peersById.begin(); i != _peersById.end(); ++i)
		{
			//Necessary, because peers can be assigned to multiple virtual devices
			if(i->second->getParentID() != _deviceId) continue;
			//We are always printing this, because the init script needs it
			GD::out.printMessage("(Shutdown) => Saving peer " + std::to_string(i->second->getID()));
			i->second->save(full, full, full);
		}
	}
	catch(const std::exception& ex)
    {
    	GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
}

std::shared_ptr<LoxonePeer> LoxoneCentral::getPeer(uint64_t id)
{
	try{
		std::lock_guard<std::mutex> peersGuard(_peersMutex);
		auto peersByIdIterator = _peersById.find(id);
		if(peersByIdIterator != _peersById.end()) return std::dynamic_pointer_cast<LoxonePeer>(peersByIdIterator->second);
	}
	catch(const std::exception& ex){
        GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
    return std::shared_ptr<LoxonePeer>();
}

std::shared_ptr<LoxonePeer> LoxoneCentral::getPeer(const std::string& serialNumber)
{
	try{
		std::lock_guard<std::mutex> peersGuard(_peersMutex);
		auto peersBySerialIterator = _peersBySerial.find(serialNumber);
		if(peersBySerialIterator != _peersBySerial.end()) return std::dynamic_pointer_cast<LoxonePeer>(peersBySerialIterator->second);
	}
	catch(const std::exception& ex){
        GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
    return std::shared_ptr<LoxonePeer>();
}

void LoxoneCentral::deletePeer(uint64_t id)
{
	try{
		std::shared_ptr<LoxonePeer> peer(getPeer(id));
		if(!peer) return;

		peer->deleting = true;
		PVariable deviceAddresses(new Variable(VariableType::tArray));
		deviceAddresses->arrayValue->push_back(PVariable(new Variable(peer->getSerialNumber())));

		PVariable deviceInfo(new Variable(VariableType::tStruct));
		deviceInfo->structValue->insert(StructElement("ID", PVariable(new Variable((int32_t)peer->getID()))));
		PVariable channels(new Variable(VariableType::tArray));
		deviceInfo->structValue->insert(StructElement("CHANNELS", channels));

		std::shared_ptr<HomegearDevice> rpcDevice = peer->getRpcDevice();
		for(Functions::iterator i = rpcDevice->functions.begin(); i != rpcDevice->functions.end(); ++i){
			deviceAddresses->arrayValue->push_back(PVariable(new Variable(peer->getSerialNumber() + ":" + std::to_string(i->first))));
			channels->arrayValue->push_back(PVariable(new Variable(i->first)));
		}

        std::vector<uint64_t> deletedIds{ id };
		raiseRPCDeleteDevices(deletedIds, deviceAddresses, deviceInfo);
		{
			std::lock_guard<std::mutex> peersGuard(_peersMutex);
			_peersBySerial.erase(peer->getSerialNumber());
			_peersById.erase(id);
		}

        int32_t i = 0;
        while(peer.use_count() > 1 && i < 600){
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            i++;
        }
        if(i == 600) GD::out.printError("Error: Peer deletion took too long.");

		peer->deleteFromDatabase();
		GD::out.printMessage("Removed peer " + std::to_string(peer->getID()));
	}
	catch(const std::exception& ex)
    {
        GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
}

std::string LoxoneCentral::handleCliCommand(std::string command)
{
    try{
		std::ostringstream stringStream;
		std::vector<std::string> arguments;

		if(command == "help" || command == "h"){
			stringStream << "List of commands (shortcut in brackets):" << std::endl << std::endl;
			stringStream << "For more information about the individual command type: COMMAND help" << std::endl << std::endl;
			stringStream << "peers list (ls)\t\tList all peers" << std::endl;
			stringStream << "peers remove (prm)\tRemove a peer (without unpairing)" << std::endl;
			stringStream << "search (sp)\t\tSearches for new devices" << std::endl;
			stringStream << "unselect (u)\t\tUnselect this device" << std::endl;
			return stringStream.str();
		}
		if(command.compare(0, 12, "peers remove") == 0 || command.compare(0, 3, "prm") == 0){
			uint64_t peerID = 0;

			std::stringstream stream(command);
			std::string element;
			int32_t offset = (command.at(1) == 'r') ? 0 : 1;
			int32_t index = 0;
			while(std::getline(stream, element, ' ')){
				if(index < 1 + offset){
					index++;
					continue;
				}
				else if(index == 1 + offset){
					if(element == "help") break;
					peerID = BaseLib::Math::getNumber(element, false);
					if(peerID == 0) return "Invalid id.\n";
				}
				index++;
			}
			if(index == 1 + offset){
				stringStream << "Description: This command removes a peer without trying to unpair it first." << std::endl;
				stringStream << "Usage: peers remove PEERID" << std::endl << std::endl;
				stringStream << "Parameters:" << std::endl;
				stringStream << "  PEERID:\tThe id of the peer to remove. Example: 513" << std::endl;
				return stringStream.str();
			}

			if(!peerExists(peerID)) stringStream << "This peer is not paired to this central." << std::endl;
			else{
				deletePeer(peerID);
				stringStream << "Removed peer " << std::to_string(peerID) << "." << std::endl;
			}
			return stringStream.str();
		}
		else if(command.compare(0, 10, "peers list") == 0 || command.compare(0, 2, "pl") == 0 || command.compare(0, 2, "ls") == 0){
			try{
				std::string filterType;
				std::string filterValue;

				std::stringstream stream(command);
				std::string element;
				int32_t offset = (command.at(1) == 'l' || command.at(1) == 's') ? 0 : 1;
				int32_t index = 0;
				while(std::getline(stream, element, ' ')){
					if(index < 1 + offset){
						index++;
						continue;
					}
					else if(index == 1 + offset){
						if(element == "help"){
							index = -1;
							break;
						}
						filterType = BaseLib::HelperFunctions::toLower(element);
					}
					else if(index == 2 + offset){
						filterValue = element;
						if(filterType == "name") BaseLib::HelperFunctions::toLower(filterValue);
					}
					index++;
				}
				if(index == -1){
					stringStream << "Description: This command lists information about all peers." << std::endl;
					stringStream << "Usage: peers list [FILTERTYPE] [FILTERVALUE]" << std::endl << std::endl;
					stringStream << "Parameters:" << std::endl;
					stringStream << "  FILTERTYPE:\tSee filter types below." << std::endl;
					stringStream << "  FILTERVALUE:\tDepends on the filter type. If a number is required, it has to be in hexadecimal format." << std::endl << std::endl;
					stringStream << "Filter types:" << std::endl;
					stringStream << "  ID: Filter by id." << std::endl;
					stringStream << "      FILTERVALUE: The id of the peer to filter (e. g. 513)." << std::endl;
					stringStream << "  ROOM: Filter by room." << std::endl;
					stringStream << "      FILTERVALUE: The 3 byte address of the peer to filter (e. g. 1DA44D)." << std::endl;
					stringStream << "  SERIAL: Filter by serial number." << std::endl;
					stringStream << "      FILTERVALUE: The serial number of the peer to filter (e. g. JEQ0554309)." << std::endl;
					stringStream << "  NAME: Filter by name." << std::endl;
					stringStream << "      FILTERVALUE: The part of the name to search for (e. g. \"1st floor\")." << std::endl;
					stringStream << "  TYPE: Filter by device type." << std::endl;
					stringStream << "      FILTERVALUE: The 2 byte device type in hexadecimal format." << std::endl;
					stringStream << "  UNREACH: List all unreachable peers." << std::endl;
					stringStream << "      FILTERVALUE: empty" << std::endl;
					return stringStream.str();
				}

				if(_peersById.empty()){
					stringStream << "No peers are paired to this central." << std::endl;
					return stringStream.str();
				}
				std::string bar(" │ ");
				const int32_t idWidth = 11;
				const int32_t nameWidth = 35;
				const int32_t roomWidth = 25;
				const int32_t serialWidth = 18;
				const int32_t typeWidth1 = 4;
				const int32_t typeWidth2 = 25;
				const int32_t unreachWidth = 7;
				std::string nameHeader("Name");
				nameHeader.resize(nameWidth, ' ');
                std::string roomHeader("Room");
                roomHeader.resize(roomWidth, ' ');
				std::string typeStringHeader("Type String");
				typeStringHeader.resize(typeWidth2, ' ');
				stringStream << std::setfill(' ')
					<< std::setw(idWidth) << "ID" << bar
					<< nameHeader << bar
					<< roomHeader << bar
					<< std::setw(serialWidth) << "Serial Number" << bar
					<< std::setw(typeWidth1) << "Type" << bar
					<< typeStringHeader << bar
					<< std::setw(unreachWidth) << "Unreach"
					<< std::endl;
				stringStream << "────────────┼─────────────────────────────────────┼───────────────────────────┼────────────────────┼──────┼───────────────────────────┼────────" << std::endl;
				stringStream << std::setfill(' ')
					<< std::setw(idWidth) << " " << bar
					<< std::setw(nameWidth) << " " << bar
					<< std::setw(roomWidth) << " " << bar
					<< std::setw(serialWidth) << " " << bar
					<< std::setw(typeWidth1) << " " << bar
					<< std::setw(typeWidth2) << " " << bar
					<< std::setw(unreachWidth) << " "
					<< std::endl;
				_peersMutex.lock();
				for(std::map<uint64_t, std::shared_ptr<BaseLib::Systems::Peer>>::iterator i = _peersById.begin(); i != _peersById.end(); ++i){
					std::shared_ptr<LoxonePeer> peer(std::dynamic_pointer_cast<LoxonePeer>(i->second));
					if(filterType == "id"){
						uint64_t id = BaseLib::Math::getNumber(filterValue, false);
						if(i->second->getID() != id) continue;
					}
					else if(filterType == "name"){
						std::string name = i->second->getName();
						if((signed)BaseLib::HelperFunctions::toLower(name).find(filterValue) == (signed)std::string::npos) continue;
					}
					else if(filterType == "room"){
                        auto myLoxonePeer = std::dynamic_pointer_cast<LoxonePeer>(i->second);
                        if(!myLoxonePeer) continue;
                        if(myLoxonePeer->getControl()->getRoom() != filterValue) continue;
					}
					else if(filterType == "serial"){
						if(i->second->getSerialNumber() != filterValue) continue;
					}
					else if(filterType == "type"){
						int32_t deviceType = BaseLib::Math::getNumber(filterValue, true);
						if((int32_t)i->second->getDeviceType() != deviceType) continue;
					}
					else if(filterType == "unreach"){
						if(i->second->serviceMessages){
							if(!i->second->serviceMessages->getUnreach()) continue;
						}
					}

					uint64_t currentID = i->second->getID();
					std::string idString = (currentID > 999999) ? "0x" + BaseLib::HelperFunctions::getHexString(currentID, 8) : std::to_string(currentID);
					stringStream << std::setw(idWidth) << std::setfill(' ') << idString << bar;
					std::string name = i->second->getName();
					size_t nameSize = BaseLib::HelperFunctions::utf8StringSize(name);
					if(nameSize > (unsigned)nameWidth)
					{
						name = BaseLib::HelperFunctions::utf8Substring(name, 0, nameWidth - 3);
						name += "...";
					}
					else name.resize(nameWidth + (name.size() - nameSize), ' ');
					stringStream << name << bar;

                    auto myLoxonePeer = std::dynamic_pointer_cast<LoxonePeer>(i->second);
                    if(!myLoxonePeer) continue;
                    std::string room = myLoxonePeer->getControl()->getRoom();
                    size_t roomSize = BaseLib::HelperFunctions::utf8StringSize(room);
                    if(roomSize > (unsigned)roomWidth)
                    {
                        room = BaseLib::HelperFunctions::utf8Substring(room, 0, roomWidth - 3);
                        room += "...";
                    }
                    else room.resize(roomWidth + (room.size() - roomSize), ' ');
                    stringStream << room << bar

						<< std::setw(serialWidth) << i->second->getSerialNumber() << bar
						<< std::setw(typeWidth1) << BaseLib::HelperFunctions::getHexString(i->second->getDeviceType(), 4) << bar;
					if(i->second->getRpcDevice())
					{
						PSupportedDevice type = i->second->getRpcDevice()->getType(i->second->getDeviceType(), i->second->getFirmwareVersion());
						std::string typeID;
						if(type) typeID = type->id;
						if(typeID.size() > (unsigned)typeWidth2)
						{
							typeID.resize(typeWidth2 - 3);
							typeID += "...";
						}
						else typeID.resize(typeWidth2, ' ');
						stringStream << typeID << bar;
					}
					else stringStream << std::setw(typeWidth2) << " " << bar;
					if(i->second->serviceMessages)
					{
						std::string configPending(i->second->serviceMessages->getConfigPending() ? "Yes" : "No");
						std::string unreachable(i->second->serviceMessages->getUnreach() ? "Yes" : "No");
						stringStream << std::setfill(' ') << std::setw(unreachWidth) << unreachable;
					}
					stringStream << std::endl << std::dec;
				}
				_peersMutex.unlock();
				stringStream << "────────────┴─────────────────────────────────────┴───────────────────────────┴────────────────────┴──────┴───────────────────────────┴────────" << std::endl;

				return stringStream.str();
			}
			catch(const std::exception& ex)
			{
				_peersMutex.unlock();
				GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
			}
		}
		else if(command.compare(0, 6, "search") == 0 || command.compare(0, 2, "sp") == 0)
		{
			std::stringstream stream(command);
			std::string element;
			int32_t offset = (command.at(1) == 'p') ? 0 : 1;
			int32_t index = 0;
			while(std::getline(stream, element, ' '))
			{
				if(index < 1 + offset)
				{
					index++;
					continue;
				}
				else if(index == 1 + offset)
				{
					if(element == "help")
					{
						stringStream << "Description: This command searches for new devices." << std::endl;
						stringStream << "Usage: search" << std::endl << std::endl;
						stringStream << "Parameters:" << std::endl;
						stringStream << "  There are no parameters." << std::endl;
						return stringStream.str();
					}
				}
				index++;
			}
			auto result = searchDevices(nullptr, "");
			stringStream << "Search completed. Found " << result->integerValue64 << " new peers." << std::endl;
			return stringStream.str();
		}
		else return "Unknown command.\n";
	}
	catch(const std::exception& ex)
    {
        GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
    return "Error executing command. See log file for more details.\n";
}

void LoxoneCentral::updatePeer(uint64_t oldId, uint64_t newId)
{
    for(auto i = _uuidPeerIdMap.begin(); i != _uuidPeerIdMap.end(); ++i) {
        if(i->second != oldId) continue;
        i->second = newId;
    }
}

std::shared_ptr<LoxonePeer> LoxoneCentral::createPeer(uint32_t deviceType, const std::string& serialNumber, std::shared_ptr<Miniserver> interface, std::shared_ptr<LoxoneControl> control, bool save)
{
	try
	{
		std::shared_ptr<LoxonePeer> peer(new LoxonePeer(_deviceId, this, control));
		peer->setDeviceType(deviceType);
		peer->setSerialNumber(serialNumber);
		uint32_t firmwareVersion = 0;
		peer->setRpcDevice(GD::family->getRpcDevices()->find(deviceType, firmwareVersion, -1));
		if(!peer->getRpcDevice()) return std::shared_ptr<LoxonePeer>();
		peer->setPhysicalInterfaceId(interface->getID());
		peer->initializeCentralConfig();
        if(save) peer->save(true, true, true); //Save and create peerID

        //{{{ Generate a map uuid -> peerId
		auto uuidVariableMap = peer->getUuidVariableMap();
        if(!uuidVariableMap) return peer;
        for(auto i = uuidVariableMap->begin(); i != uuidVariableMap->end(); ++i) {
            _uuidPeerIdMap.emplace(i->second, peer->getID());
        }
        //}}}
		return peer;
	}
    catch(const std::exception& ex)
    {
    	GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
    return std::shared_ptr<LoxonePeer>();
}

void LoxoneCentral::homegearShuttingDown()
{
    try{
        for (auto& physicalInterface : GD::physicalInterfaces){
            if(physicalInterface.second->isConnected()) physicalInterface.second->disconnect();
        }
    }
    catch(const std::exception& ex)
    {
        GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
}

//RPC functions
PVariable LoxoneCentral::deleteDevice(BaseLib::PRpcClientInfo clientInfo, std::string serialNumber, int32_t flags)
{
	try{
		if(serialNumber.empty()) return Variable::createError(-2, "Unknown device.");
		std::shared_ptr<LoxonePeer> peer = getPeer(serialNumber);
		if(!peer) return Variable::createError(-2, "Unknown device.");
		return deleteDevice(clientInfo, peer->getID(), flags);
	}
	catch(const std::exception& ex){
        GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
    return Variable::createError(-32500, "Unknown application error.");
}

PVariable LoxoneCentral::deleteDevice(BaseLib::PRpcClientInfo clientInfo, uint64_t peerId, int32_t flags)
{
	try{
		if(peerId == 0) return Variable::createError(-2, "Unknown device.");
		if(peerId >= 0x40000000) return Variable::createError(-2, "Cannot delete virtual device.");
        std::shared_ptr<LoxonePeer> peer = getPeer(peerId);
        if(!peer) return Variable::createError(-2, "Unknown device.");
		deletePeer(peerId);
		return PVariable(new Variable(VariableType::tVoid));
	}
	catch(const std::exception& ex){
        GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
    return Variable::createError(-32500, "Unknown application error.");
}
PVariable LoxoneCentral::searchDevices(BaseLib::PRpcClientInfo clientInfo, const std::string& interfaceId)
{
	try{
		std::string cashedVersion;
		std::shared_ptr<BaseLib::Database::DataTable> rows = _bl->db->getDeviceVariables(_deviceId);
		for (BaseLib::Database::DataTable::iterator row = rows->begin(); row != rows->end(); ++row){
			_variableDatabaseIds[row->second.at(2)->intValue] = row->second.at(0)->intValue;
			switch (row->second.at(2)->intValue)
			{
			case 0:
				cashedVersion = row->second.at(4)->textValue;
				break;
			}
		}
		for (auto& interface : GD::physicalInterfaces){
            if(!interface.second->isConnected()){
                GD::out.printDebug("could not search because the interface " + interface.first + " is not connected", 3);
                continue;
		    }

            std::string actualVersion = interface.second->getLoxApp3Version()->stringValue;
            if(cashedVersion == actualVersion) return std::make_shared<BaseLib::Variable>(0);
            GD::out.printMessage("get new Struct File from Interface:" + interface.first);

            _LoxApp3.parseStructFile(interface.second->getNewStructfile());
            std::string lastModified = _LoxApp3.getlastModified();

            auto controls = _LoxApp3.getControls();
            std::vector<std::shared_ptr<LoxonePeer>>newPeers;
            std::vector<std::shared_ptr<LoxonePeer>>knownPeers;
            for (auto peer = _peersById.begin(); peer != _peersById.end(); ++peer){
                auto loxonePeer = std::dynamic_pointer_cast<LoxonePeer>(peer->second);
                if(!loxonePeer) continue;
                knownPeers.push_back(loxonePeer);
            }

            for(const auto& control: controls) {
                bool found = false;
                for(auto loxonePeer = knownPeers.begin(); loxonePeer != knownPeers.end(); ++ loxonePeer) {
                    if (loxonePeer.operator*()->getControl()->getUuidAction() == control->getUuidAction()) {
                        found = true;
                        loxonePeer.operator*()->serviceMessages->endUnreach();
                        knownPeers.erase(loxonePeer);
                        break;
                    }
                }
                if(found) continue;

                auto deviceType = control->getType();
                std::string serial;
                do {
                    int32_t seedNumber = BaseLib::HelperFunctions::getRandomNumber(1, 9999999);
                    std::ostringstream stringstream;
                    stringstream << "LOX" << std::setw(7) << std::setfill('0') << std::dec << seedNumber;
                    serial = stringstream.str();
                } while(_peersBySerial.find(serial) != _peersBySerial.end());

                std::shared_ptr<LoxonePeer> peer = createPeer(deviceType, serial, interface.second, control, true);
                if (!peer || !peer->getRpcDevice()) continue;
                peer->setName(control->getName());

                std::lock_guard<std::mutex> peersGuard(_peersMutex);
                _peersBySerial[peer->getSerialNumber()] = peer;
                _peersById[peer->getID()] = peer;

                newPeers.push_back(peer);
                GD::out.printMessage("Added peer " + std::to_string(peer->getID()) + ".");
            }

            GD::out.printDebug("Size of knownPeers is now :" + std::to_string(knownPeers.size()));
            if(!knownPeers.empty()){
                for(const auto& loxonePeer: knownPeers) {
                    GD::out.printDebug("PeerId: " + std::to_string(loxonePeer->getID()) + " is paired but not in Loxone Struct file");
                    if(!loxonePeer->serviceMessages->getUnreach()) loxonePeer->serviceMessages->setUnreach(true,false);
                }
            }
            //saveVariable(0, lastModified);

            if(!newPeers.empty()){
				std::vector<uint64_t> newIds;
				newIds.reserve(newPeers.size());
				PVariable deviceDescriptions = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tArray);
				deviceDescriptions->arrayValue->reserve(100);

				for(auto& newPeer: newPeers){
					newIds.push_back(newPeer->getID());
					std::shared_ptr<std::vector<PVariable>> descriptions =  newPeer->getDeviceDescriptions(nullptr, true, std::map<std::string,bool>());
					if(!descriptions) continue;
					for(auto& description : *descriptions){
						if(deviceDescriptions->arrayValue->size()+1 > deviceDescriptions->arrayValue->capacity()) deviceDescriptions->arrayValue->reserve(deviceDescriptions->arrayValue->size()+100);
						deviceDescriptions->arrayValue->push_back(description);
					}
					{
						auto pairingState = std::make_shared<PairingState>();
						pairingState->peerId = newPeer->getID();
						pairingState->state = "success";
						std::lock_guard<std::mutex> newPeersGuard(_newPeersMutex);
						_newPeers[BaseLib::HelperFunctions::getTime()].emplace_back(std::move(pairingState));
					}
				}
				raiseRPCNewDevices(newIds, deviceDescriptions);
            }
            return std::make_shared<BaseLib::Variable>(newPeers.size());
		}
	}
	catch(const std::exception& ex)
	{
		GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	return Variable::createError(-32500, "Unknown application error.");
}
//End RPC functions
}
