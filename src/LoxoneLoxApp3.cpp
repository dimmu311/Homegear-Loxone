#include "LoxoneLoxApp3.h"
#include "controls/createInstance.h"
#include <iostream>
#include <fstream>

namespace Loxone
{
	LoxoneLoxApp3::LoxoneLoxApp3()
	{
        _out.init(GD::bl);
        _out.setPrefix(GD::out.getPrefix() + "LoxApp3: ");
	}

	int32_t LoxoneLoxApp3::parseStructFile(BaseLib::PVariable structFile)
	{
		try{
		    _structFile = structFile;
			loadlastModified();
			
			_out.printDebug("Parse Struct File", 5);
			for (auto i = _structFile->structValue->begin(); i != _structFile->structValue->end(); ++i){
				_out.printDebug("Struct File at: " + i->first, 5);
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
		catch (const std::exception & ex){
			_out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
			return -1;
		}
	}

	void LoxoneLoxApp3::loadlastModified()
	{
		try{
			if (_structFile->structValue->find("lastModified") == _structFile->structValue->end()) return;
			_lastModified = _structFile->structValue->find("lastModified")->second->stringValue;
		}
		catch (const std::exception & ex){
			_out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
	}

	void LoxoneLoxApp3::loadCats()
	{
		try{
			_cats.clear();
			if (_structFile->structValue->find("cats") == _structFile->structValue->end()) return;
			auto cats = _structFile->structValue->find("cats")->second;
			for (auto i = cats->structValue->begin(); i != cats->structValue->end(); ++i){
				_cats[i->second->structValue->at("uuid")->stringValue] = i->second->structValue->at("name")->stringValue;
			}
		}
		catch (const std::exception & ex){
			_out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
	}

	void LoxoneLoxApp3::loadRooms()
	{
		try{
			_rooms.clear();
			if (_structFile->structValue->find("rooms") == _structFile->structValue->end()) return;
			auto rooms = _structFile->structValue->find("rooms")->second;
			for (auto i = rooms->structValue->begin(); i != rooms->structValue->end(); ++i){
				_rooms[i->second->structValue->at("uuid")->stringValue] = i->second->structValue->at("name")->stringValue;
			}
		}
		catch (const std::exception & ex){
			_out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
	}

	void LoxoneLoxApp3::loadControls()
	{
		try{
			_out.printDebug("Loading Controls from Structfile", 5);
			_controls.clear();
			if (_structFile->structValue->find("controls") == _structFile->structValue->end()) return;
			PVariable controls = _structFile->structValue->find("controls")->second;

			for (auto i = controls->structValue->begin(); i != controls->structValue->end(); ++i){
				_out.printDebug("Loading Control " + i->second->structValue->at("name")->stringValue + " with uuid " + i->first, 5);
				std::shared_ptr<LoxoneControl> control = createInstance::createInstanceFromTypeString(i->second, _rooms,_cats);
				if(!control) continue;
				_controls.push_back(control);

				if (i->second->structValue->find("subControls") != i->second->structValue->end()){
					_out.printDebug("Control has subcontrol", 5);
					PVariable subControls = i->second->structValue->find("subControls")->second;
					for (auto j = subControls->structValue->begin(); j != subControls->structValue->end(); ++j){
						_out.printDebug("Loading subControl " + j->second->structValue->at("name")->stringValue + " with serial " + j->first, 5);
						std::shared_ptr<LoxoneControl> subControl = createInstance::createInstanceFromTypeString(j->second, _rooms, _cats);
                        if(!subControl) continue;
                        subControl->overwriteName(i->second->structValue->at("name")->stringValue + " ||| " + j->second->structValue->at("name")->stringValue);
                        subControl->overwrite(control->getRoom(), control->getCat());
                        _controls.push_back(subControl);
					}
				}
			}
		}
		catch (const std::exception & ex){
			_out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
	}
}
