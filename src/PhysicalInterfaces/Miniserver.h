#ifndef MINISERVER_H
#define MINISERVER_H

#include "../LoxonePacket.h"
#include "LoxoneEncryption.h"

namespace Loxone
{
	class LoxoneHeader
	{
	public:
		enum class Identifier
		{
			Textmessage,
			Binary_File,
			EventTable_of_Value_States,
			EventTable_of_Text_States,
			EventTable_of_Daytimer_States,
			Out_Of_Service_Indicator,
			Keepalive_Response,
			EventTable_of_Weather_States,
		};
		/*
		1. Byte = fixed 0x03
		2. Byte = Identifier	0 = Textmessage
								1 = Binary File
								2 = Event-Table of Value-States
								3 = Event-Table of Text-States
								4 = Event-Table of Daytimer-States
								5 = Out-Of_Service Indicator (eg. during Firmwareupdate)
								6 = Keepalive Response
								7 = Event-Table of Wether-States
		3. Byte = Info
		4. Byte = Reserved for Future
		5.-8.Byte = Length of the Payload from the next Telegramm
		*/
		LoxoneHeader(){};
		Identifier identifier = Identifier::Out_Of_Service_Indicator;
		int32_t loxonePayloadLength = 0;
	};

class Miniserver : public BaseLib::Systems::IPhysicalInterface
{
public:
    explicit Miniserver(std::shared_ptr<BaseLib::Systems::PhysicalInterfaceSettings> settings);
    ~Miniserver() override;
    void startListening() override;
    void stopListening() override;
    void sendPacket(std::shared_ptr<BaseLib::Systems::Packet> packet) override;
	BaseLib::PVariable getNewStructfile();
	BaseLib::PVariable getLoxApp3Version();
    bool isOpen() override { return !_stopped; }
    uint16_t getMessageCounter();
	
	bool httpPacket = false;
	bool webSocketPacket = false;
	
protected:
    struct Request
    {
        std::mutex mutex;
        std::condition_variable conditionVariable;
        bool mutexReady = false;
        PLoxonePacket response;
    };

    BaseLib::Output _out;
    int32_t _port = 80;
	std::string _user;
	std::string _password;
    std::shared_ptr<BaseLib::TcpSocket> _tcpSocket;

	std::shared_ptr<BaseLib::WebSocket> _webSocket;

    std::atomic<uint16_t> _messageCounter{ 0 };

    std::thread _initThread;
    std::thread _keepAliveThread;
    std::thread _refreshTokenThread;

    std::mutex _sendPacketMutex;
    std::mutex _getResponseMutex;
    std::mutex _responsesMutex;
    std::unordered_map<std::string, std::shared_ptr<Request>> _responses;
	//std::unordered_map<LoxoneCommand, std::shared_ptr<Request>> _responses;
	std::unordered_map<std::string, std::list<PLoxonePacket>> _responseCollections;


    void listen();
    void init();
	void keepAlive();
    void refreshToken();
    void saveToken();

	void processHttpPacket(const std::vector<char>& data, uint32_t responseCode);
	void processWsPacket(std::vector<char>& data);
	void processTextmessagePacket(std::vector<char>& data);
	void processBinaryFilePacket(std::vector<char>& data);
	void processEventTableOfValueStatesPacket(std::vector<char>& data);
	void processEventTableOfTextStatesPacket(std::vector<char>& data);
	void processEventTableOfDaytimerStatesPacket(std::vector<char>& data);
	void processOutOfServiceIndicatorPacket();
	void processKeepAlivePacket();
	void processEventTableOfWeatherStatesPacket(std::vector<char>& data);

    PLoxonePacket getResponse(const LoxoneHttpCommand& requestPacket, int32_t waitForSeconds = 15);

    std::shared_ptr<LoxoneEncryption> _loxoneEncryption;
};


}
#endif
