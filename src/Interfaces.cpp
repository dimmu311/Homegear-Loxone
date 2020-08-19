#include "Interfaces.h"
#include "GD.h"
#include "PhysicalInterfaces/Miniserver.h"
#include "Loxone.h"

namespace Loxone
{

Interfaces::Interfaces(BaseLib::SharedObjects* bl, std::map<std::string, Systems::PPhysicalInterfaceSettings> physicalInterfaceSettings) : Systems::PhysicalInterfaces(bl, GD::family->getFamily(), physicalInterfaceSettings)
{
	create();
}

void Interfaces::create()
{
    try
	{
        for(auto settings : _physicalInterfaceSettings)
        {
            std::shared_ptr<Miniserver> device;
            if(!settings.second || settings.second->host.empty()) continue;
            GD::out.printDebug("Debug: Creating physical device. Type defined in loxone.conf is: " + settings.second->type);
            if(settings.second->type == "miniserver") device.reset(new Miniserver(settings.second));
            else GD::out.printError("Error: Unsupported physical device type: " + settings.second->type);
            if(device)
            {
                if(_physicalInterfaces.find(settings.second->id) != _physicalInterfaces.end()) GD::out.printError("Error: id used for two devices: " + settings.second->id);
                _physicalInterfaces[settings.second->id] = device;
                GD::physicalInterfaces[settings.second->id] = device;
                if(settings.second->isDefault || !GD::defaultPhysicalInterface) GD::defaultPhysicalInterface = device;
            }
        }
        if(!GD::defaultPhysicalInterface) GD::defaultPhysicalInterface = std::make_shared<Miniserver>(std::make_shared<BaseLib::Systems::PhysicalInterfaceSettings>());
	}
	catch(const std::exception& ex)
	{
		GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
}

}
