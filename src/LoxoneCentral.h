#ifndef LOXONECENTRAL_H_
#define LOXONECENTRAL_H_

#include <homegear-base/BaseLib.h>
#include "LoxonePeer.h"
#include "LoxonePacket.h"
#include "LoxoneLoxApp3.h"

#include <memory>
#include <mutex>
#include <string>

#include "LoxoneControl.h"

namespace Loxone
{

class Miniserver;

class LoxoneCentral : public BaseLib::Systems::ICentral
{
public:
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
	std::unordered_map <std::string, std::shared_ptr<variable_PeerId>> _uuidVariable_PeerIdMap;
};

}

#endif
