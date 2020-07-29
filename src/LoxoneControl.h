#ifndef LOXONECONTROL_H_
#define LOXONECONTROL_H_

#include <string>
#include <unordered_map>
#include "GD.h"
#include "LoxonePacket.h"
#include "homegear-base/Encoding/JsonEncoder.h"
#include "homegear-base/Systems/Peer.h"

namespace Loxone
{
    struct variable_PeerId
	{
		std::string variable;
		uint32_t peerId;
	};
	class MandatoryFields
	{
		public:
		std::string _name;
		std::string _typeString;
		std::string _uuidAction;
		uint32_t _defaultRating;
		bool _isSecured;
		MandatoryFields(PVariable mandatoryFields, std::string room, std::string cat);
		MandatoryFields(std::shared_ptr<BaseLib::Database::DataTable>rows);
		void overwriteName(std::string name);
		std::string getName() { return _name; };
		std::string getUuidAction() { return _uuidAction; };
		std::string getType() { return _typeString; };
		uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
	};
	class OptionalFields
	{
		public:
		std::string _room;
		std::string _cat;
		//securedDetails -> see derived classes of the controls
		//details -> see derived classes of the controls
		//statistic -> see derived classes of the controls
		OptionalFields(PVariable optionalFields, std::string room, std::string cat);
		OptionalFields(std::shared_ptr<BaseLib::Database::DataTable>rows);
		std::string getRoom() { return _room; };
		std::string getCat() { return _cat; };
		uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
	};

	class LoxoneControl : public MandatoryFields, public OptionalFields
	{
	public:
		LoxoneControl(PVariable control, std::string room, std::string cat, uint32_t typeNr);
		LoxoneControl(std::shared_ptr<BaseLib::Database::DataTable> rows, uint32_t typeNr);

		uint32_t getType() { return _type; };
		
		std::unordered_map <std::string, variable_PeerId> getVariables() { return _uuidVariable_PeerIdMap; };

		virtual bool processPacket(PLoxoneBinaryFilePacket loxonePacket);
		virtual bool processPacket(PLoxoneTextmessagePacket loxonePacket);
		virtual bool processPacket(PLoxoneValueStatesPacket loxonePacket);
		virtual bool processPacket(PLoxoneTextStatesPacket loxonePacket);
		virtual bool processPacket(PLoxoneDaytimerStatesPacket loxonePacket);
		virtual bool processPacket(PLoxoneWeatherStatesPacket loxonePacket);

		bool setValue(std::string method, BaseLib::PVariable parameters, std::shared_ptr<LoxonePacket> packet);
        bool setValue(std::string method, BaseLib::PVariable parameters, std::string& command);
		virtual uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);

		void setValuesCentral(std::unordered_map<uint32_t, std::unordered_map<std::string, Systems::RpcConfigurationParameter>> valuesCentral) {_valuesCentral = valuesCentral; };
        //std::unordered_map<uint32_t, std::unordered_map<std::string, RpcConfigurationParameter>> configCentral;
        std::unordered_map<uint32_t, std::unordered_map<std::string, Systems::RpcConfigurationParameter>> _valuesCentral;

	protected:
		uint32_t _type;

		std::unordered_map <std::string, variable_PeerId> _uuidVariable_PeerIdMap;
		std::unique_ptr<BaseLib::Rpc::JsonEncoder> _jsonEncoder;
		PVariable _json;
		uint32_t getStatesToSave(std::list<Database::DataRow> &list, uint32_t peerID);

		PVariable _control;
		bool getValueFromStructFile(const std::string& variableId, const std::string& path, bool& value);
		bool getValueFromStructFile(const std::string& variableId, const std::string& path, uint32_t& value);
		bool getValueFromStructFile(const std::string& variableId, const std::string& path, float& value);
		bool getValueFromStructFile(const std::string& variableId, const std::string& path, std::string& value);

		std::shared_ptr<BaseLib::Database::DataTable> _rows;
		bool getValueFromDataTable(const uint32_t& variableId, bool& value);
		bool getValueFromDataTable(const uint32_t& variableId, uint32_t& value);
		bool getValueFromDataTable(const uint32_t& variableId, float& value);
		bool getValueFromDataTable(const uint32_t& variableId, std::string& value);
		bool getBinaryValueFromDataTable(const uint32_t& variableId, std::string& value);
	};
}
#endif
