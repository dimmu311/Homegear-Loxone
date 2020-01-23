/* Copyright 2013-2019 Homegear GmbH
 *
 * Homegear is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Homegear is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Homegear.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

#ifndef LOXONECENTRAL_H_
#define LOXONECENTRAL_H_

#include <homegear-base/BaseLib.h>
#include "LoxonePeer.h"
#include "LoxonePacket.h"
#include "LoxoneControl.h"
#include "LoxoneLoxApp3.h"

#include <memory>
#include <mutex>
#include <string>

namespace Loxone
{

class Miniserver;

class LoxoneCentral : public BaseLib::Systems::ICentral
{
public:
	/*
	struct LoxoneControl
	{
	public:
		
		//The UUID to identify the control.
		//The Beginning of this uuid has to be the same as the Homegear Serial
		
		std::string uuidAction;
		std::string name;
		std::string type;
		std::string room;
		std::string cat;
		uint32_t defaultRating;
		bool isFavorite;
		bool isSecured;

		std::map<std::string, PVariable> variableNames;
	};
	struct Switch : LoxoneControl
	{
	public:
		std::string active;
	};
	struct WindowMonitor : LoxoneControl
	{
	public:
		PVariable details;
		std::string numOpen;
		std::string numClosed;
		std::string numTilted;
		std::string numOffline;
		std::string numLocked;
		std::string numUnlocked;
		std::string windowStates;
	};
	*/

	//In table variables
	int32_t getFirmwareVersion() { return _firmwareVersion; }
	void setFirmwareVersion(int32_t value) { _firmwareVersion = value; saveVariable(0, value); }
	//End

	LoxoneCentral(ICentralEventSink* eventHandler);
	LoxoneCentral(uint32_t deviceType, std::string serialNumber, int32_t address, ICentralEventSink* eventHandler);
	virtual ~LoxoneCentral();
	virtual void dispose(bool wait = true);

	virtual void loadVariables();
	virtual void saveVariables();
	virtual void loadPeers();
	virtual void savePeers(bool full);

	virtual void homegearShuttingDown();

	virtual bool onPacketReceived(std::string& senderID, std::shared_ptr<BaseLib::Systems::Packet> packet);
	virtual std::string handleCliCommand(std::string command);

	std::shared_ptr<LoxonePeer> getPeer(const std::string& interfaceId, const std::string& serialNumber);
	std::shared_ptr<LoxonePeer> getPeer(uint64_t id);
	std::shared_ptr<LoxonePeer> getPeer(const std::string& serialNumber);

	virtual PVariable deleteDevice(BaseLib::PRpcClientInfo clientInfo, std::string serialNumber, int32_t flags);
	virtual PVariable deleteDevice(BaseLib::PRpcClientInfo clientInfo, uint64_t peerID, int32_t flags);
//    virtual PVariable getPairingState(BaseLib::PRpcClientInfo clientInfo);
	virtual PVariable searchDevices(BaseLib::PRpcClientInfo clientInfo);
	
	//std::unordered_map<std::string, std::string> _cats;
	//std::unordered_map<std::string, std::string> _rooms;
	//std::unordered_map<std::string, std::shared_ptr<LoxoneControl>> _controls;
	//std::unordered_map<std::string, std::string> _uuidSerialPairs;
protected:
	//In table variables
	int32_t _firmwareVersion = 0;
	//End

//	std::atomic_bool _searching{false};

	std::mutex _searchDevicesMutex;

	std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<LoxonePeer>>> _peersByInterface;

	std::unique_ptr<BaseLib::Rpc::JsonDecoder> _jsonDecoder;
	

	/**
	 * Creates a new peer. The method does not add the peer to the peer arrays.
	 *
	 * @param nodeId The node ID of the peer.
	 * @param firmwareVersion The firmware version.
	 * @param deviceType The device type.
	 * @param serialNumber The serial number.
	 * @param save (default true) Set to "true" to save the peer in the database.
	 * @return Returns a pointer to the newly created peer on success. If the creation was not successful, a nullptr is returned.
	 */
	std::shared_ptr<LoxonePeer> createPeer(uint32_t deviceType, const std::string& serialNumber, std::shared_ptr<Miniserver> interface, std::shared_ptr<LoxoneControl> control, bool save = true);
	void deletePeer(uint64_t id);

	void init();

	LoxoneLoxApp3 _LoxApp3;
	std::unordered_map <std::string, UuidVariablePeer> _uuidVariablePeerMap;
};

}

#endif
