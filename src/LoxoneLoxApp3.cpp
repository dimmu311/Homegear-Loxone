#include "LoxoneLoxApp3.h"
#include "controls/createInstance.h"
#include "controls/Weather.h"

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
			loadWeatherServer();
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

	void LoxoneLoxApp3::loadWeatherServer()
	{
	    try{
	        _out.printDebug("Loading Weather Server from Structfile", 5);
	        if (_structFile->structValue->find("weatherServer") == _structFile->structValue->end()) return;
	        PVariable weatherServer = _structFile->structValue->find("weatherServer")->second;

	        if (weatherServer->structValue->find("states") == weatherServer->structValue->end()) return;
	        {
	            PVariable states = weatherServer->structValue->find("states")->second;
	            if(states->structValue->find("actual") == states->structValue->end()) return;
	            _weather.uuidActual = states->structValue->find("actual")->second->stringValue;
	            if(states->structValue->find("forecast") == states->structValue->end()) return;
	            _weather.uuidForecast = states->structValue->find("forecast")->second->stringValue;
	        }

	        if (weatherServer->structValue->find("format") == weatherServer->structValue->end()) return;
	        {
	            PVariable format = weatherServer->structValue->find("format")->second;
	            if(format->structValue->find("barometricPressure") == format->structValue->end()) return;
	            _weather.format.barometricPressure = format->structValue->find("barometricPressure")->second->stringValue;
	            if(format->structValue->find("precipitation") == format->structValue->end()) return;
	            _weather.format.precipitation = format->structValue->find("precipitation")->second->stringValue;
	            if(format->structValue->find("relativeHumidity") == format->structValue->end()) return;
	            _weather.format.relativeHumidity = format->structValue->find("relativeHumidity")->second->stringValue;
	            if(format->structValue->find("temperature") == format->structValue->end()) return;
	            _weather.format.temperature = format->structValue->find("temperature")->second->stringValue;
	            if(format->structValue->find("windSpeed") == format->structValue->end()) return;
	            _weather.format.windSpeed = format->structValue->find("windSpeed")->second->stringValue;
	        }

	        if (weatherServer->structValue->find("weatherTypeTexts") == weatherServer->structValue->end()) return;
	        {
	            PVariable weatherTypeTexts = weatherServer->structValue->find("weatherTypeTexts")->second;
	            _weather.weatherTypeTexts.empty();
	            for(auto i = weatherTypeTexts->structValue->begin(); i != weatherTypeTexts->structValue->end(); ++i){
	                _weather.weatherTypeTexts.emplace((uint8_t)Math::getNumber(i->first), i->second->stringValue);
	            }
	        }
	        if (weatherServer->structValue->find("weatherFieldTypes") == weatherServer->structValue->end()) return;
	        {
	            PVariable weatherFieldTypes = weatherServer->structValue->find("weatherFieldTypes")->second;
	            for(auto i = weatherFieldTypes->structValue->begin(); i != weatherFieldTypes->structValue->end(); ++i){
	                weather::sWeatherFieldTypes sWeatherFieldTypes;
	                if(i->second->structValue->find("id") != i->second->structValue->end()) sWeatherFieldTypes.id = i->second->structValue->find("id")->second->integerValue;
	                if(i->second->structValue->find("name") != i->second->structValue->end()) sWeatherFieldTypes.name = i->second->structValue->find("name")->second->stringValue;
	                if(i->second->structValue->find("analog") != i->second->structValue->end()) sWeatherFieldTypes.analog = i->second->structValue->find("analog")->second->booleanValue;
	                if(i->second->structValue->find("unit") != i->second->structValue->end()) sWeatherFieldTypes.unit = i->second->structValue->find("unit")->second->stringValue;
	                if(i->second->structValue->find("format") != i->second->structValue->end()) sWeatherFieldTypes.format = i->second->structValue->find("format")->second->stringValue;
	                _weather.weatherFieldTypes.emplace((uint8_t)Math::getNumber(i->first), sWeatherFieldTypes);
	            }
	        }

	        std::shared_ptr<LoxoneControl> control = std::make_shared<Weather>(_weather);
	        if(!control) return;
	        _controls.push_back(control);
	    }
        catch (const std::exception & ex){
            _out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
        }
	}
}
