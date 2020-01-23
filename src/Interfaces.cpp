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
