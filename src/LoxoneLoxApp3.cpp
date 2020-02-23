#include "LoxoneLoxApp3.h"
#include <iostream>
#include <fstream>

namespace Loxone
{
	LoxoneLoxApp3::LoxoneLoxApp3()
	{
		_jsonEncoder = std::unique_ptr<BaseLib::Rpc::JsonEncoder>(new BaseLib::Rpc::JsonEncoder(GD::bl));
		_jsonDecoder = std::unique_ptr<BaseLib::Rpc::JsonDecoder>(new BaseLib::Rpc::JsonDecoder(GD::bl));
	}

	/*
	int32_t LoxoneLoxApp3::saveNewStructFile(BaseLib::PVariable structFile)
	{
		try
		{
			std::string jsonString;
			_jsonEncoder->encode(structFile, jsonString);

			std::ofstream file;
			file.open(_filePath);
			if (!file) return -1;
			file << jsonString;
			file.close();
			return 0;
		}
		catch (const std::exception & ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		return -1;
	}
    */
	int32_t LoxoneLoxApp3::parseStructFile(BaseLib::PVariable structFile)
	{
		try
		{
		    _structFile = structFile;
			//loadStructFile();
			//GD::out.printInfo("#########################################################################Datei gelesen");
			
			loadlastModified();
			
			for (auto i = _structFile->structValue->begin(); i != _structFile->structValue->end(); ++i)
			{
				GD::out.printInfo("######################################################" + i->first);
				/*
				autopilot
					cats
					controls
				globalStates
					lastModified
				messageCenter
				msInfo
				operatingModes
					rooms
				times
				weatherServer
				*/
			}

			loadCats();
			loadRooms();
			loadControls();
			
			return 0;
		}
		catch (const std::exception & ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
			return -1;
		}

		
		
	}

	/*
	void LoxoneLoxApp3::loadStructFile()
	{
		try
		{
			std::ifstream file;
			file.open(_filePath);
			if (!file) return;

			std::string jsonString;
			std::string line;

			while (getline(file, line, '\n'))
			{
				jsonString += line;
			}
			file.close();
			_structFile = _jsonDecoder->decode(jsonString);
		}
		catch (const std::exception & ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
	}
	*/

	void LoxoneLoxApp3::loadlastModified()
	{
		try
		{
			if (_structFile->structValue->find("lastModified") == _structFile->structValue->end()) return;
			_lastModified = _structFile->structValue->find("lastModified")->second->stringValue;
		}
		catch (const std::exception & ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
	}

	void LoxoneLoxApp3::loadCats()
	{
		try
		{
			_cats.clear();
			if (_structFile->structValue->find("cats") == _structFile->structValue->end()) return;
			auto cats = _structFile->structValue->find("cats")->second;
			for (auto i = cats->structValue->begin(); i != cats->structValue->end(); ++i)
			{
				//if (_bl->debugLevel >= 4) _bl->out.printInfo("##########CATS" + i->first + i->second->structValue->at("name")->stringValue);
				_cats[i->second->structValue->at("uuid")->stringValue] = i->second->structValue->at("name")->stringValue;
			}
		}
		catch (const std::exception & ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
	}

	void LoxoneLoxApp3::loadRooms()
	{
		try
		{
			_rooms.clear();
			if (_structFile->structValue->find("rooms") == _structFile->structValue->end()) return;
			auto rooms = _structFile->structValue->find("rooms")->second;
			for (auto i = rooms->structValue->begin(); i != rooms->structValue->end(); ++i)
			{
				//if (_bl->debugLevel >= 4) _bl->out.printInfo("##########ROOMS" + i->first+ i->second->structValue->at("name")->stringValue);
				_rooms[i->second->structValue->at("uuid")->stringValue] = i->second->structValue->at("name")->stringValue;
			}
		}
		catch (const std::exception & ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
	}

	void LoxoneLoxApp3::loadControls()
	{
		try
		{
			_controls.clear();
			if (_structFile->structValue->find("controls") == _structFile->structValue->end()) return;
			PVariable controls = _structFile->structValue->find("controls")->second;

			for (auto i = controls->structValue->begin(); i != controls->structValue->end(); ++i)
			{
				GD::out.printInfo("##########Controls" + i->first + "ist im Raum" + _rooms.find(i->second->structValue->at("room")->stringValue)->second + "und der cat" + _cats.find(i->second->structValue->at("cat")->stringValue)->second + "und hat den Namen" + i->second->structValue->at("name")->stringValue);

				std::string serial = i->first.substr(0, 18);
				if (LoxoneControl::_controlsMap.find(i->second->structValue->at("type")->stringValue) == LoxoneControl::_controlsMap.end()) continue;

				std::shared_ptr<LoxoneControl> control(LoxoneControl::_controlsMap.at(i->second->structValue->at("type")->stringValue)(i->second, _rooms.find(i->second->structValue->at("room")->stringValue)->second, _cats.find(i->second->structValue->at("cat")->stringValue)->second));
				_controls.insert({ serial, control });
			}
		}
		catch (const std::exception & ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
	}
}
