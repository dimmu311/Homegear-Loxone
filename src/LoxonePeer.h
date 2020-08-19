#ifndef LOXONEPEER_H_
#define LOXONEPEER_H_

#include "LoxonePacket.h"
#include <homegear-base/BaseLib.h>

#include <list>

#include "LoxoneControl.h"

using namespace BaseLib;
using namespace BaseLib::DeviceDescription;
using namespace BaseLib::Systems;

namespace Loxone
{
class LoxoneCentral;
class Miniserver;

class LoxonePeer : public BaseLib::Systems::Peer
{
public:
	LoxonePeer(uint32_t parentID, IPeerEventSink* eventHandler, std::shared_ptr<LoxoneControl> control);
	LoxonePeer(int32_t id, int32_t address, std::string serialNumber, uint32_t parentID, IPeerEventSink* eventHandler);
	virtual ~LoxonePeer();

    //Features
    virtual bool wireless() { return false; }
    //End features

	//{{{ In table variables
	std::string getPhysicalInterfaceId() { return _physicalInterfaceId; }
	void setPhysicalInterfaceId(std::string);
	//}}}

	std::shared_ptr<Miniserver>& getPhysicalInterface() { return _physicalInterface; }

	virtual std::string handleCliCommand(std::string command);

	virtual bool load(BaseLib::Systems::ICentral* central);
	virtual void save(bool savePeer, bool saveVariables, bool saveCentralConfig);
    //todo: update the peerid in the variable list. mybe by overloading this methode
	//virtual void updatePeer(uint64_t oldId, uint64_t newId);

	void saveUuids();
    void loadUuids();
    void setPeerIdToVariableList();
    void setConfigParameters();
    std::unordered_map <std::string, std::shared_ptr<variable_PeerId>> getVariables() { return _uuidVariable_PeerIdMap; };

	virtual void savePeers() {};
	virtual int32_t getChannelGroupedWith(int32_t channel) { return -1; }
	virtual int32_t getNewFirmwareVersion() { return 0; }
    virtual std::string getFirmwareVersionString(int32_t firmwareVersion) { return "1.0"; }
    virtual bool firmwareUpdateAvailable() { return false; }

    void homegearStarted();

	void packetReceived(std::shared_ptr<LoxonePacket> packet);

	//RPC methods
	/**
	 * {@inheritDoc}
	 */
    virtual PVariable getDeviceInfo(BaseLib::PRpcClientInfo clientInfo, std::map<std::string, bool> fields);

    /**
	 * {@inheritDoc}
	 */
	virtual PVariable getParamsetDescription(BaseLib::PRpcClientInfo clientInfo, int32_t channel, ParameterGroup::Type::Enum type, uint64_t remoteID, int32_t remoteChannel, bool checkAcls);

	/**
	 * {@inheritDoc}
	 */
	virtual PVariable getParamset(BaseLib::PRpcClientInfo clientInfo, int32_t channel, ParameterGroup::Type::Enum type, uint64_t remoteID, int32_t remoteChannel, bool checkAcls);

	/**
	 * {@inheritDoc}
	 */
	virtual PVariable putParamset(BaseLib::PRpcClientInfo clientInfo, int32_t channel, ParameterGroup::Type::Enum type, uint64_t remoteID, int32_t remoteChannel, PVariable variables, bool checkAcls, bool onlyPushing = false);

	/**
	 * {@inheritDoc}
	 */
	virtual PVariable setValue(BaseLib::PRpcClientInfo clientInfo, uint32_t channel, std::string valueKey, PVariable value, bool wait);
	//End RPC methods

	std::string _uuid;
protected:
    struct FrameValue
    {
        std::list<uint32_t> channels;
        std::vector<uint8_t> value;
    };

    struct FrameValues
    {
        std::string frameID;
        std::list<uint32_t> paramsetChannels;
        ParameterGroup::Type::Enum parameterSetType;
        std::map<std::string, FrameValue> values;
    };

	std::shared_ptr<BaseLib::Rpc::RpcEncoder> _binaryEncoder;
	std::shared_ptr<BaseLib::Rpc::RpcDecoder> _binaryDecoder;

	//In table variables:
	std::string _physicalInterfaceId;
	//End

	std::shared_ptr<Miniserver> _physicalInterface;

	virtual void setPhysicalInterface(std::shared_ptr<Miniserver> interface);

	virtual std::shared_ptr<BaseLib::Systems::ICentral> getCentral();

    void getValuesFromPacket(PLoxonePacket packet, std::vector<FrameValues>& frameValue);

	virtual PParameterGroup getParameterSet(int32_t channel, ParameterGroup::Type::Enum type);

	virtual void loadVariables(BaseLib::Systems::ICentral* central, std::shared_ptr<BaseLib::Database::DataTable>& rows);
    virtual void saveVariables();

    std::shared_ptr<LoxoneControl> _control;
    std::unordered_map <std::string, std::shared_ptr<variable_PeerId>> _uuidVariable_PeerIdMap;

};

}

#endif
