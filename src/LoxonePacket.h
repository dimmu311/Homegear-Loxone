#ifndef LOXONEPACKET_H_
#define LOXONEPACKET_H_

#include <homegear-base/BaseLib.h>

#include <map>

using namespace BaseLib;

namespace Loxone
{

	enum class LoxonePacketType
	{
		LoxoneHttpPacket,
		LoxoneWsPacket,
		LoxoneValueStatesPacket,
		LoxoneTextStatesPacket,
		LoxoneBinaryFilePacket,
		LoxoneDaytimerStatesPacket,
		LoxoneWeatherStatesPacket,
	};

class InvalidLoxonePacketException : public BaseLib::Exception
{
public:
    explicit InvalidLoxonePacketException(const std::string& message) : Exception(message) {}
};

enum commandType { http = 1, ws = 2, header = 3 };
struct LoxoneHttpCommand
{
	public:
		std::string _command;
		std::string _responseCommand;
		int32_t _code;
};
struct LoxoneTimeEntry
{
	uint32_t _mode; // 32-Bit Integer (little endian) number of mode
	uint32_t _from; // 32-Bit Integer (little endian) from-time in minutes since midnight
	uint32_t _to; // 32-Bit Integer (little endian) to-time in minutes since midnight
	uint32_t _needActivate; // 32-Bit Integer (little endian) need activate (trigger)
	double _value; // 64-Bit Float (little endian) value (if analog daytimer)
};

class LoxonePacket : public BaseLib::Systems::Packet
{
public:
	virtual ~LoxonePacket() = default;
	static const std::unordered_map<std::string, LoxoneHttpCommand> _commands;
	static const std::list<std::string> _responseCommands;
	
    std::string getResponseCommand() { return _command; };
	std::string getMessageType() { return "StateSet"; }

	void setCommand(std::string command) { _command = command; };
	std::string getCommand() { return _command; };
   	
	virtual PVariable getJsonString() { return _jsonString; }
	void setJsonString(PVariable setJsonString) { _jsonString = setJsonString; }

	LoxonePacketType getPacketType() { return _packetType; };
	std::string getUuid() { return _uuid; };

protected:
	std::string _command;
    
	PVariable _jsonString;
	std::unique_ptr<BaseLib::Rpc::JsonDecoder> _jsonDecoder;
	std::unique_ptr<BaseLib::Rpc::JsonEncoder> _jsonEncoder;
	PVariable getJson(std::string& jsonString);

	std::string _method;
	BaseLib::PVariable _parameters;
	BaseLib::PVariable _result;

	std::string _uuid;
	LoxonePacketType _packetType;

	std::string getUuidFromPacket(char* data);
	double getValueFromPacket(char* data);
};


class LoxoneHttpPacket : public LoxonePacket
{
public: 
	LoxoneHttpPacket(std::string jsonString, uint32_t responseCode);
	uint32_t getResponseCode() { return _responseCode; };
	std::string getControl() { return _control; };
	BaseLib::PVariable getValue() { return _value; };
protected:
	uint32_t _responseCode;
	std::string _control;
	BaseLib::PVariable  _value;
};

class LoxoneWsPacket : public LoxonePacket
{
public:
	LoxoneWsPacket();
	LoxoneWsPacket(std::string jsonString);
	uint32_t getResponseCode() { return _responseCode; };
	void setResponseCode(uint32_t responseCode) { _responseCode = responseCode; };
	std::string getControl() { return _control; };
	BaseLib::PVariable getValue() { return _value; };
protected:
	uint32_t _responseCode;
	std::string _control;
	BaseLib::PVariable  _value;
};

class LoxoneTextmessagePacket : public LoxonePacket
{
public:
	LoxoneTextmessagePacket(std::string jsonString);
	std::string getControl() { return _control; };
	BaseLib::PVariable getValue() { return _value; };
protected:
	std::string _control;
	BaseLib::PVariable  _value;
};

class LoxoneValueStatesPacket : public LoxonePacket
{
	/*typedef struct {
		 PUUID uuid; // 128-Bit uuid
		 double dVal; // 64-Bit Float (little endian) value
		} PACKED EvData;
	*/
public:
	LoxoneValueStatesPacket(char* packet);
	double getDValue() { return _value; };
protected:
	double _value;
};
class LoxoneTextStatesPacket : public LoxonePacket
{
public:
	/*
	typedef struct { // starts at multiple of 4
		PUUID uuid; // 128-Bit uuid
		PUUID uuidIcon; // 128-Bit uuid of icon
		unsigned long textLength; // 32-Bit Unsigned Integer (little endian)
		// text follows here
	} PACKED EvDataText;
	*/
	LoxoneTextStatesPacket() = default;
	LoxoneTextStatesPacket(char*, uint32_t len);
	std::string getUuidIcon() { return _uuidIcon; };
	std::string getText() { return _text; };
protected:
	std::string _uuidIcon;
	std::string _text;
};

class LoxoneBinaryFilePacket : public LoxonePacket
{
};
class LoxoneDaytimerStatesPacket : public LoxonePacket
{
};
class LoxoneWeatherStatesPacket : public LoxonePacket
{
};


typedef std::shared_ptr<LoxonePacket> PLoxonePacket;
typedef std::shared_ptr<LoxoneHttpPacket> PLoxoneHttpPacket;
typedef std::shared_ptr<LoxoneWsPacket> PLoxoneWsPacket;
typedef std::shared_ptr<LoxoneValueStatesPacket> PLoxoneValueStatesPacket;
typedef std::shared_ptr<LoxoneTextStatesPacket> PLoxoneTextStatesPacket;
typedef std::shared_ptr<LoxoneBinaryFilePacket> PLoxoneBinaryFilePacket;
typedef std::shared_ptr<LoxoneDaytimerStatesPacket> PLoxoneDaytimerStatesPacket;
typedef std::shared_ptr<LoxoneWeatherStatesPacket> PLoxoneWeatherStatesPacket;
}
#endif
