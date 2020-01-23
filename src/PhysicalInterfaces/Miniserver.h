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

#ifndef MINISERVER_H
#define MINISERVER_H

#include "../LoxonePacket.h"
#include "../LoxoneEncryption.h"

namespace Loxone
{

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

	struct LoxoneHeader
	{
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
		Identifier identifier;
		int32_t loxonePayloadLength = 0;
		bool nextIsHeader = true;
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

    std::mutex _sendPacketMutex;
    std::mutex _getResponseMutex;
    std::mutex _responsesMutex;
    std::unordered_map<std::string, std::shared_ptr<Request>> _responses;
	//std::unordered_map<LoxoneCommand, std::shared_ptr<Request>> _responses;
	std::unordered_map<std::string, std::list<PLoxonePacket>> _responseCollections;


    void listen();
    void init();
	void keepAlive();

	void processHttpPacket(const std::vector<char>& data, uint32_t responseCode);
	void processWsPacket(std::vector<char>& data);
	void processTextmessagePacket(std::vector<char>& data);
	void processBinaryFilePacket(std::vector<char>& data);
	void processEventTableOfValueStatesPacket(std::vector<char>& data);
	void processEventTableOfTextStatesPacket(std::vector<char>& data);
	void processEventTableOfDaytimerStatesPacket(std::vector<char>& data);
	void processOutOfServiceIndicatorPacket(Identifier identifier);
	void processKeepAlivePacket(Identifier identifier);
	void processEventTableOfWeatherStatesPacket(std::vector<char>& data);

    //PLoxonePacket getResponse(LoxoneCommand responseCommand, const PLoxonePacket& requestPacket, int32_t waitForSeconds = 15);
	PLoxonePacket getResponse(const LoxoneHttpCommand& requestPacket, int32_t waitForSeconds = 15);
    
	LoxoneEncryption _loxoneEncryption;

	uint32_t _keepAliveCounter;
};


}
#endif
