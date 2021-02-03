#include "LoxoneLoxApp3.h"
#include "controls/createInstance.h"
#include <iostream>
#include <fstream>

namespace Loxone
{
	LoxoneLoxApp3::LoxoneLoxApp3()
	{
		_jsonEncoder = std::unique_ptr<BaseLib::Rpc::JsonEncoder>(new BaseLib::Rpc::JsonEncoder(GD::bl));
		_jsonDecoder = std::unique_ptr<BaseLib::Rpc::JsonDecoder>(new BaseLib::Rpc::JsonDecoder(GD::bl));
	}

	int32_t LoxoneLoxApp3::parseStructFile(BaseLib::PVariable structFile)
	{
		try
		{
		    _structFile = structFile;
			loadlastModified();
			
			if (GD::bl->debugLevel >= 5) GD::out.printInfo("Parse Struct File");
			for (auto i = _structFile->structValue->begin(); i != _structFile->structValue->end(); ++i)
			{
				if (GD::bl->debugLevel >= 5) GD::out.printInfo("Struct File at: " + i->first);
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
			if (GD::bl->debugLevel >= 5) GD::out.printInfo("Loading Controls from Structfile");
			_controls.clear();
			if (_structFile->structValue->find("controls") == _structFile->structValue->end()) return;
			PVariable controls = _structFile->structValue->find("controls")->second;

			for (auto i = controls->structValue->begin(); i != controls->structValue->end(); ++i)
			{
				std::string serial = i->first;
				if(serial.length() > 18)
				{
					serial = serial.substr(0,18);
					serial[17] = '.';
					serial[16] = '.';
				}

				if (GD::bl->debugLevel >= 5) GD::out.printInfo("Loading Control " + i->second->structValue->at("name")->stringValue + " with serial " + serial);
				std::shared_ptr<LoxoneControl> control = createInstance::createInstanceFromTypeString(i->second, _rooms.find(i->second->structValue->at("room")->stringValue)->second, _cats.find(i->second->structValue->at("cat")->stringValue)->second);
				if(!control) continue;

				if(_controls.find(serial) != _controls.end()) serial[17] = '1';

                _controls.insert({ serial, control });

				if (i->second->structValue->find("subControls") != i->second->structValue->end())
				{
					if (GD::bl->debugLevel >= 5) GD::out.printInfo("Control has subcontrol");
					PVariable subControls = i->second->structValue->find("subControls")->second;
					for (auto j = subControls->structValue->begin(); j != subControls->structValue->end(); ++j)
					{
						std::string subSerial = j->first;
						if(subSerial.length() > 18)
						{
							if(subSerial.find("/") != std::string::npos)
							{
								std::string sub = subSerial.substr(subSerial.find("/"));
								subSerial = subSerial.substr(0, 18 -2 - sub.length());
								subSerial.push_back('.');
								subSerial.push_back('.');
								subSerial.append(sub.begin(),sub.end());
							}
							else
                            {
                                subSerial = subSerial.substr(0,18);
                                subSerial[17] = '.';
                                subSerial[16] = '.';
                            }
						}
						if (GD::bl->debugLevel >= 5) GD::out.printInfo("Loading subControl " + j->second->structValue->at("name")->stringValue + " with serial " + subSerial);
						std::shared_ptr<LoxoneControl> subControl = createInstance::createInstanceFromTypeString(j->second, _rooms.find(i->second->structValue->at("room")->stringValue)->second, _cats.find(i->second->structValue->at("cat")->stringValue)->second);
                        if(!subControl) continue;
                        subControl->overwriteName(i->second->structValue->at("name")->stringValue + " ||| " + j->second->structValue->at("name")->stringValue);

                        if(_controls.find(subSerial) != _controls.end()) subSerial[subSerial.find(".")] = '1';

						_controls.insert({ subSerial, subControl });
					}
				}
			}
		}
		catch (const std::exception & ex)
		{
			GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
	}
}
