#ifndef LOXONECONTROL_H_
#define LOXONECONTROL_H_

#include <string>
#include "GD.h"
#include "LoxonePacket.h"
#include "homegear-base/Encoding/JsonEncoder.h"

namespace Loxone
{
	struct UuidVariablePeer
	{
		std::string variable;
		uint32_t peerId;
	};
	class LoxoneControl
	{
	public:
		static const std::unordered_map<std::string, LoxoneControl* (*)(PVariable, std::string, std::string)>_controlsMap;
        static const std::unordered_map<uint32_t, LoxoneControl* (*)(PVariable, std::string, std::string)> _uintControlsMap;

		LoxoneControl(PVariable control, std::string room, std::string cat, uint32_t typeNr);

		std::string getName() { return _name; };
		std::string getRoom() { return _room; };
		std::string getCat() { return _cat; };
		uint32_t getType() { return _type; };
		
		std::list<std::string> getUuids() { return _uuids; };
		std::string getUuidAction() { return _uuidAction; };
		std::unordered_map <std::string, UuidVariablePeer> getVariables() { return _uuidVariablePeerMap; };
		void addBooleanState(double value, std::string& variable);
		bool preProcessPacket(PLoxonePacket loxonePacket, std::string& variable);
		virtual bool processPacket(PLoxonePacket loxonePacket) { return false; };
		virtual bool setValue(std::string method, BaseLib::PVariable parameters, std::shared_ptr<LoxonePacket> packet) { return false; };
	protected:
		std::string _name;
		uint32_t _type;
		std::string _uuidAction;
		std::string _room;
		std::string _cat;
		uint32_t _defaultRating;
		bool _isFavorite = false;
		bool _isSecured = false;

		std::list<std::string> _uuids;
		std::unordered_map <std::string, UuidVariablePeer> _uuidVariablePeerMap;
		std::unique_ptr<BaseLib::Rpc::JsonEncoder> _jsonEncoder;
		PVariable _json;
	};

	class Pushbutton : public LoxoneControl
	{
		/*
		"0fd2c255-02f8-1875-ffff1239b12ff514":{
			"name":"Eingang VI2",
			"type":"Pushbutton",
			"uuidAction":"0fd2c255-02f8-1875-ffff1239b12ff514",
			"room":"0df9e099-021c-03e6-ffff1239b12ff514",
			"cat":"0df9e099-01ce-030c-ffff1239b12ff514",
			"defaultRating":0,
			"isFavorite":false,
			"isSecured":false,
			"defaultIcon":null,
			"states":{"
				active":"0fd2c255-02f8-1875-ffff1239b12ff514"
			}
		}
		*/
	public:
		Pushbutton(PVariable control, std::string room, std::string cat);
		bool processPacket(PLoxonePacket loxonePacket);
		bool setValue(std::string method, BaseLib::PVariable parameters, std::shared_ptr<LoxonePacket> packet);
	protected:
	};

	class Slider : public LoxoneControl
	{
		/*
		"0e3dca32-0006-e500-ffff1239b12ff514":{
			"name":"Volume",
			"type":"Slider",
			"uuidAction":"0e3dca32-0006-e500-ffff1239b12ff514",
			"room":"0df9e099-020c-03b2-ffff1239b12ff514",
			"cat":"0df9e099-01dd-0320-ffff1239b12ff514",
			"defaultRating":0,
			"isFavorite":false,
			"isSecured":false,
			"defaultIcon":null,
			"details":{
				"format":"%.0f%%",
				"min":0.000,
				"max":100.000,
				"step":1.000
			},
			"states":{
				"value":"0e3dca32-0006-e500-ffff1239b12ff514",
				"error":"0e3dca32-0006-e4ff-ffff2c564cfa6c10"
			}
		}
		*/
	public:
		Slider(PVariable control, std::string room, std::string cat);
		bool processPacket(PLoxonePacket loxonePacket);
		bool setValue(std::string method, BaseLib::PVariable parameters, std::shared_ptr<LoxonePacket> packet);
	protected:
		//todo: details
	};

	class Dimmer : public LoxoneControl
	{
		/*
		"10bb08c1-0021-45bf-ffff1239b12ff514":{
			"name":"Dimmer",
			"type":"Dimmer",
			"uuidAction":"10bb08c1-0021-45bf-ffff1239b12ff514",
			"room":"0df9e099-020c-03b2-ffff1239b12ff514",
			"cat":"0df9e099-01dd-0320-ffff1239b12ff514",
			"defaultRating":0,
			"isFavorite":false,
			"isSecured":false,
			"states":{
				"position":"10bb08c1-0021-45bd-ffff9a107b038561",
				"min":"10bb08c1-0021-45b9-ffff9a107b038561",
				"max":"10bb08c1-0021-45ba-ffff9a107b038561",
				"step":"10bb08c1-0021-45b7-ffff9a107b038561"
			}
		}
		*/
	public:
		Dimmer(PVariable control, std::string room, std::string cat);
		bool processPacket(PLoxonePacket loxonePacket);
		bool setValue(std::string method, BaseLib::PVariable parameters, std::shared_ptr<LoxonePacket> packet);
	protected:
		//todo: details
	};

	class Alarm : public LoxoneControl
	{
		/*
		14439fff-003f-2de3-ffff1239b12ff514":{
			"name":"Alarmanlage",
			"type":"Alarm",
			"uuidAction":"14439fff-003f-2de3-ffff1239b12ff514",
			"room":"0df9e099-021c-03e6-ffff1239b12ff514",
			"cat":"0df9e099-01ed-0344-ffff1239b12ff514",
			"defaultRating":0,
			"isFavorite":false,
			"isSecured":true,
			"details":{"
				alert":true,
				"presenceConnected":true
			},
			"states":{
				"armed":"14439fff-003f-2dd6-ffff751123eb8cf1",
				"nextLevel":"14439fff-003f-2daf-ffff751123eb8cf1",
				"nextLevelDelay":"14439fff-003f-2db0-ffff751123eb8cf1",
				"nextLevelDelayTotal":"14439fff-003f-2db1-ffff751123eb8cf1",
				"level":"14439fff-003f-2db2-ffff751123eb8cf1",
				"startTime":"14439fff-003f-2db3-ffff751123eb8cf1",
				"armedDelay":"14439fff-003f-2ddf-ffff751123eb8cf1",
				"armedDelayTotal":"14439fff-003f-2dc9-ffff751123eb8cf1",
				"sensors":"14439fff-003f-2de3-ffff1239b12ff514",
				"disabledMove":"14439fff-003f-2db4-ffff751123eb8cf1"
			},
			"subControls":{
				"14439fff-003f-2de3-ffff1239b12ff514/sensors":{
					"name":"sensors",
					"type":"Tracker",
					"uuidAction":"14439fff-003f-2de3-ffff1239b12ff514/sensors",
					"defaultRating":0,
					"isFavorite":false,
					"isSecured":false,
					"details":{"maxEntries":40},
					"states":{"entries":"14439fff-003f-2de3-ffff1239b12ff514"}
				}
			}
		}
		*/
	public:
		Alarm(PVariable control, std::string room, std::string cat);
		bool processPacket(PLoxonePacket loxonePacket);
		bool setValue(std::string method, BaseLib::PVariable parameters, std::shared_ptr<LoxonePacket> packet);
	protected:
		
	};

	class MediaClient : public LoxoneControl
	{
		/*

		*/
	public:
		MediaClient(PVariable control, std::string room, std::string cat);
		bool processPacket(PLoxonePacket loxonePacket);
		bool setValue(std::string method, BaseLib::PVariable parameters, std::shared_ptr<LoxonePacket> packet);
	protected:
	};
}
#endif
