#ifndef LOXONECONTROL_H_
#define LOXONECONTROL_H_

#include <string>
#include "GD.h"
#include "LoxonePacket.h"
#include "homegear-base/Encoding/JsonEncoder.h"

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
		static const std::unordered_map<std::string, LoxoneControl* (*)(PVariable, std::string, std::string)>_controlsMap;
        static const std::unordered_map<uint32_t, LoxoneControl* (*)(std::shared_ptr<BaseLib::Database::DataTable>)> _uintControlsMap;

		LoxoneControl(PVariable control, std::string room, std::string cat, uint32_t typeNr);
		LoxoneControl(std::shared_ptr<BaseLib::Database::DataTable> rows, uint32_t typeNr);

		uint32_t getType() { return _type; };
		
		std::unordered_map <std::string, variable_PeerId> getVariables() { return _uuidVariable_PeerIdMap; };
		void addBooleanState(double value, std::string& variable);

		virtual bool processPacket(PLoxoneBinaryFilePacket loxonePacket);
		virtual bool processPacket(PLoxoneTextmessagePacket loxonePacket);
		virtual bool processPacket(PLoxoneValueStatesPacket loxonePacket);
		virtual bool processPacket(PLoxoneTextStatesPacket loxonePacket);
		virtual bool processPacket(PLoxoneDaytimerStatesPacket loxonePacket);
		virtual bool processPacket(PLoxoneWeatherStatesPacket loxonePacket);

		virtual bool setValue(std::string method, BaseLib::PVariable parameters, std::shared_ptr<LoxonePacket> packet);
		virtual uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
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
		Pushbutton(std::shared_ptr<BaseLib::Database::DataTable> rows);
		bool processPacket(PLoxoneValueStatesPacket loxonePacket);
		uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
	protected:
		bool _isFavorite;
		std::string _defaultIcon;
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
		Slider(std::shared_ptr<BaseLib::Database::DataTable> rows);
		uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
	protected:
		bool _isFavorite;
		std::string _defaultIcon;

		std::string _detFormat;
		float _detMin;
		float _detMax;
		float _detStep;
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
		Dimmer(std::shared_ptr<BaseLib::Database::DataTable> rows);
		uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
	protected:
		bool _isFavorite;
	};

	class LightControllerV2 : public LoxoneControl
	{
		/*
		"14f29553-0385-5bcf-ffff1239b12ff514":{
			"name":"Lichtsteuerung",
			"type":"LightControllerV2",
			"uuidAction":"14f29553-0385-5bcf-ffff1239b12ff514",
			"room":"0df9e099-020c-03a6-ffff1239b12ff514",
			"cat":"0df9e099-01dd-0310-ffff1239b12ff514",
			"defaultRating":0,
			"isFavorite":false,
			"isSecured":false,
			"details":{
				"movementScene":1,
				"masterValue":"14f29553-0385-5bcf-ffff1239b12ff514/masterValue",
				"masterColor":"14f29553-0385-5bcf-ffff1239b12ff514/masterColor"
			},
			"states":{
				"activeMoods":"14f29553-0385-5b87-ffffd050f836ca54",
				"moodList":"14f29553-0385-5b88-ffffd050f836ca54",
				"favoriteMoods":"14f29553-0385-5b89-ffffd050f836ca54",
				"additionalMoods":"14f29553-0385-5b8a-ffffd050f836ca54"
			},
			*/
			/*
			"subControls":{
				"14f29553-0385-5bcf-ffff1239b12ff514/masterValue":{
					"name":"Master-Helligkeit",
					"type":"Dimmer",
					"uuidAction":"14f29553-0385-5bcf-ffff1239b12ff514/masterValue",
					"defaultRating":0,
					"isFavorite":false,
					"isSecured":false,
					"states":{
						"position":"14f29553-0385-5bcb-ffffd050f836ca54",
						"min":"14f29553-0385-5b8c-ffffd050f836ca54",
						"max":"14f29553-0385-5b8d-ffffd050f836ca54",
						"step":"14f29553-0385-5ba7-ffffd050f836ca54"
					}
				},
				"14f29553-0385-5bcf-ffff1239b12ff514/masterColor":{
					"name":"Master-Farbe","type":"ColorPickerV2",
					"uuidAction":"14f29553-0385-5bcf-ffff1239b12ff514/masterColor",
					"defaultRating":0,
					"isFavorite":false,
					"isSecured":false,
					"details":{
						"pickerType":"Rgb"
					},
					"states":{
						"color":"14f29553-0385-5bca-ffffd050f836ca54",
						"sequence":"14e86293-036d-8616-ffff3ce43fa9aec5",
						"sequenceColorIdx":"14e86293-036d-8617-ffff3ce43fa9aec5"
					}
				},
				"14f29553-0385-5bcf-ffff1239b12ff514/AI1":{
					"name":"AQ1",
					"type":"Switch",
					"uuidAction":"14f29553-0385-5bcf-ffff1239b12ff514/AI1",
					"defaultRating":0,
					"isFavorite":false,
					"isSecured":false,
					"states":{
						"active":"14f29553-0385-5bb8-ffffd050f836ca54"
					}
				},
				"14f29553-0385-5bcf-ffff1239b12ff514/AI2":{
					"name":"AQ2",
					"type":"Dimmer",
					"uuidAction":"14f29553-0385-5bcf-ffff1239b12ff514/AI2",
					"defaultRating":0,
					"isFavorite":false,
					"isSecured":false,
					"states":{
						"position":"14f29553-0385-5bb9-ffffd050f836ca54",
						"min":"14f29553-0385-5ba9-ffffd050f836ca54",
						"max":"14f29553-0385-5baa-ffffd050f836ca54",
						"step":"14f29553-0385-5ba7-ffffd050f836ca54"
					}
				},
				"14f29553-0385-5bcf-ffff1239b12ff514/AI3":{
					"name":"AQ3",
					"type":
					"ColorPickerV2",
					"uuidAction":"14f29553-0385-5bcf-ffff1239b12ff514/AI3",
					"defaultRating":0,
					"isFavorite":false,
					"isSecured":false,
					"details":{
						"pickerType":"Rgb"
					},
					"states":{
						"color":"14f29553-0385-5bba-ffffd050f836ca54",
						"sequence":"14e86293-036d-85f6-ffff3ce43fa9aec5",
						"sequenceColorIdx":"14e86293-036d-85f7-ffff3ce43fa9aec5"
					}
				},
				"14f29553-0385-5bcf-ffff1239b12ff514/AI4":{
					"name":"AQ4","type":"ColorPickerV2",
					"uuidAction":"14f29553-0385-5bcf-ffff1239b12ff514/AI4",
					"defaultRating":0,
					"isFavorite":false,
					"isSecured":false,
					"details":{"
						pickerType":"Lumitech"
					},
					"states":{
						"color":"14f29553-0385-5bbb-ffffd050f836ca54",
						"sequence":"14e86293-036d-85f8-ffff3ce43fa9aec5",
						"sequenceColorIdx":"14e86293-036d-85f9-ffff3ce43fa9aec5"
					}
				},
				"14f29553-0385-5bcf-ffff1239b12ff514/AI5":{
					"name":"AQ5",
					"type":"Dimmer",
					"uuidAction":"14f29553-0385-5bcf-ffff1239b12ff514/AI5",
					"defaultRating":0,
					"isFavorite":false,
					"isSecured":false,
					"states":{
						"position":"14f29553-0385-5bbc-ffffd050f836ca54",
						"min":"14f29553-0385-5ba9-ffffd050f836ca54",
						"max":"14f29553-0385-5baa-ffffd050f836ca54",
						"step":"14f29553-0385-5ba7-ffffd050f836ca54"
					}
				}
			}
		}
		*/
	public:
		LightControllerV2(PVariable control, std::string room, std::string cat);
		LightControllerV2(std::shared_ptr<BaseLib::Database::DataTable> rows);
		uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
	protected:
		bool _isFavorite;
		uint32_t _detMovementScene;
		std::string _detMasterValue;
		std::string _detMasterColor;
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
		Alarm(std::shared_ptr<BaseLib::Database::DataTable> rows);
		uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
	protected:
		bool _detAlert;
		bool _detPresenceConnected;

		//ToDo subControls;
	};

	class MediaClient : public LoxoneControl
	{
		/*
			"14eef2d6-03ce-80a8-ffff1239b12ff514":{"name":"Music Server Zone","type":"MediaClient","uuidAction":"14eef2d6-03ce-80a8-ffff1239b12ff514","room":"0df9e099-020c-03a6-ffff1239b12ff514","cat":"0df9e099-01dd-0320-ffff1239b12ff514","defaultRating":0,"isFavorite":false,"isSecured":false,"details":{"playerid":-1,"clientType":0,"parameterConnections":0},"states":{"power":"14eef2d6-03ce-8057-ffff9bb655d4afc8","serverState":"14eef2d6-03ce-8067-ffff9bb655d4afc8","playState":"14eef2d6-03ce-805b-ffff9bb655d4afc8","clientState":"14eef2d6-03ce-8068-ffff9bb655d4afc8","volume":"14eef2d6-03ce-8058-ffff9bb655d4afc8","maxVolume":"14eef2d6-03ce-8059-ffff9bb655d4afc8","mute":"14eef2d6-03ce-805c-ffff9bb655d4afc8","shuffle":"14eef2d6-03ce-805f-ffff9bb655d4afc8","repeat":"14eef2d6-03ce-8060-ffff9bb655d4afc8","source":"14eef2d6-03ce-805a-ffff9bb655d4afc8","sourceList":"14eef2d6-03ce-805e-ffff9bb655d4afc8","songName":"14eef2d6-03ce-805d-ffff9bb655d4afc8","duration":"14eef2d6-03ce-8062-ffff9bb655d4afc8","progress":"14eef2d6-03ce-8061-ffff9bb655d4afc8","album":"14eef2d6-03ce-8063-ffff9bb655d4afc8","artist":"14eef2d6-03ce-8064-ffff9bb655d4afc8","station":"14eef2d6-03ce-806a-ffff9bb655d4afc8","genre":"14eef2d6-03ce-8065-ffff9bb655d4afc8","cover":"14eef2d6-03ce-8066-ffff9bb655d4afc8","volumeStep":"14eef2d6-03ce-8069-ffff9bb655d4afc8","queueIndex":"14eef2d6-03ce-806d-ffff9bb655d4afc8","enableAirPlay":"14eef2d6-03ce-806e-ffff9bb655d4afc8","enableSpotifyConnect":"14eef2d6-03ce-806f-ffff9bb655d4afc8","alarmVolume":"14eef2d6-03ce-8098-ffff9bb655d4afc8","bellVolume":"14eef2d6-03ce-8099-ffff9bb655d4afc8","buzzerVolume":"14eef2d6-03ce-809a-ffff9bb655d4afc8","ttsVolume":"14eef2d6-03ce-809b-ffff9bb655d4afc8","ttsVolume":"14eef2d6-03ce-809b-ffff9bb655d4afc8","defaultVolume":"14eef2d6-03ce-8097-ffff9bb655d4afc8","equalizerSettings":"14eef2d6-03ce-8070-ffff9bb655d4afc8"}}
		*/
	public:
		MediaClient(PVariable control, std::string room, std::string cat);
		MediaClient(std::shared_ptr<BaseLib::Database::DataTable> rows);
		uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
	protected:
	};
}
#endif
