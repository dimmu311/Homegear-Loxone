#ifndef LOXONECONTROL_H_
#define LOXONECONTROL_H_

#include <string>
#include <unordered_map>
#include <regex>
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
    struct extraData
    {
        std::string variable;
        uint32_t channel;
        PVariable value;
    };
	class MandatoryFields
	{
		public:
		MandatoryFields(PVariable mandatoryFields, std::string room, std::string cat);
		explicit MandatoryFields(std::shared_ptr<BaseLib::Database::DataTable>rows);
		void overwriteName(std::string name);
		std::string getName() { return _name; };
		std::string getUuidAction() {return _uuidAction;};
		virtual uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
	protected:
        std::string _name;
        std::string _typeString;
        std::string _uuidAction;
        uint32_t _defaultRating;
        bool _isSecured;
        //isFavorite is not described as a mandatoryFild in the documentation, but so far I have not found a control where isFavorit is not available
        bool _isFavorite;
	};
	class OptionalFields
	{
		public:

		OptionalFields(PVariable optionalFields, std::string room, std::string cat);
		explicit OptionalFields(std::shared_ptr<BaseLib::Database::DataTable>rows);
		std::string getRoom() { return _room; };
		std::string getCat() { return _cat; };
		virtual uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
	protected:
        std::string _room;
        std::string _cat;
        std::unordered_map <std::string, PVariable> _detailsMap;
        //states ->see controls class

        //todo: implement the following
        //securedDetails
        //statistic
        //restrictions
        //hasControlNotes
	};

	class LoxoneControl : public MandatoryFields, public OptionalFields
	{
	public:
        LoxoneControl(PVariable control, std::string room, std::string cat, uint32_t typeNr);
		LoxoneControl(std::shared_ptr<BaseLib::Database::DataTable> rows, uint32_t typeNr);

        uint32_t getType() { return _type; };
		
		std::unordered_map <std::string, std::shared_ptr<variable_PeerId>> getVariables() { return _uuidVariable_PeerIdMap; };

		virtual bool processPacket(PLoxoneBinaryFilePacket loxonePacket);
		virtual bool processPacket(PLoxoneTextmessagePacket loxonePacket);
		virtual bool processPacket(PLoxoneValueStatesPacket loxonePacket);
		virtual bool processPacket(PLoxoneTextStatesPacket loxonePacket);
		virtual bool processPacket(PLoxoneDaytimerStatesPacket loxonePacket);
		virtual bool processPacket(PLoxoneWeatherStatesPacket loxonePacket);

        virtual bool packetReceived(uint32_t peerID, PLoxoneDaytimerStatesPacket loxonePacket, std::unordered_map<uint32_t, std::unordered_map<std::string, Systems::RpcConfigurationParameter>> &valuesCentral){return false;};

        bool getValueFromVariable(BaseLib::PVariable variable, std::string& command);
        virtual bool setValue(PPacket frame, PVariable parameters, uint32_t channel, std::string &command, bool &isSecured);
        virtual bool setValue(uint32_t channel, std::string valueKey, PVariable value, std::unordered_map<uint32_t, std::unordered_map<std::string, Systems::RpcConfigurationParameter>> &valuesCentral, std::string &command, bool &isSecured){return false;};
        virtual uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
        virtual uint32_t getExtraData(std::list<extraData> &extraData);
        protected:
		uint32_t _type;

		std::unordered_map <std::string, std::shared_ptr<variable_PeerId>> _uuidVariable_PeerIdMap;
		PVariable _json;
        std::shared_ptr<BaseLib::Rpc::RpcEncoder>_RpcEncoder;
        std::shared_ptr<BaseLib::Rpc::RpcDecoder>_RpcDecoder;

		uint32_t getStatesToSave(std::list<Database::DataRow> &list, uint32_t peerID);
        uint32_t getDetailsToSave(std::list<Database::DataRow> &list, uint32_t peerID);

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
