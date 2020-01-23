#include <memory>

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

#include "Loxone.h"
#include "Interfaces.h"
#include "LoxoneCentral.h"
#include "GD.h"

#include <iomanip>

namespace Loxone
{

Loxone::Loxone(BaseLib::SharedObjects* bl, BaseLib::Systems::IFamilyEventSink* eventHandler) : BaseLib::Systems::DeviceFamily(bl, eventHandler, LOXONE_FAMILY_ID, LOXONE_FAMILY_NAME)
{
	GD::bl = _bl;
	GD::family = this;
	GD::out.init(bl);
	GD::out.setPrefix("Module Loxone Miniserver: ");
	GD::out.printDebug("Debug: Loading module...");
	_physicalInterfaces.reset(new Interfaces(bl, _settings->getPhysicalInterfaceSettings()));
}

void Loxone::dispose()
{
	if(_disposed) return;
	DeviceFamily::dispose();

	_central.reset();
}

std::shared_ptr<BaseLib::Systems::ICentral> Loxone::initializeCentral(uint32_t deviceId, int32_t address, std::string serialNumber)
{
	return std::make_shared<LoxoneCentral>(deviceId, serialNumber, 1, this);
}

void Loxone::createCentral()
{
 	try
	{
		if(_central) return;

		int32_t seedNumber = BaseLib::HelperFunctions::getRandomNumber(1, 9999999);
		std::ostringstream stringstream;
		stringstream << "LOX" << std::setw(7) << std::setfill('0') << std::dec << seedNumber;
		std::string serialNumber(stringstream.str());

		_central.reset(new LoxoneCentral(0, serialNumber, 1, this));
		GD::out.printMessage("Created central with id " + std::to_string(_central->getId()) + ", address 0x" + BaseLib::HelperFunctions::getHexString(1, 6) + " and serial number " + serialNumber);
	}
	catch(const std::exception& ex)
    {
    	GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
}

PVariable Loxone::getPairingInfo()
{
	try
	{
		if(!_central) return std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		PVariable info = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);

		//{{{ General
		info->structValue->emplace("searchInterfaces", std::make_shared<BaseLib::Variable>(false));
		//}}}

		//{{{ Family settings
		PVariable familySettings = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		
		info->structValue->emplace("familySettings", familySettings);
		//}}}

		//{{{ Pairing methods
		PVariable pairingMethods = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		pairingMethods->structValue->emplace("searchDevices", std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct));
		info->structValue->emplace("pairingMethods", pairingMethods);
		//}}}

		//{{{ interfaces
		PVariable interfaces = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);

		PVariable interface = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		interface->structValue->emplace("name", std::make_shared<BaseLib::Variable>(std::string("MINISERVER")));
		interface->structValue->emplace("ipDevice", std::make_shared<BaseLib::Variable>(true));

		auto field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("pos", std::make_shared<BaseLib::Variable>(0));
		field->structValue->emplace("label", std::make_shared<BaseLib::Variable>(std::string("l10n.loxone.pairingInfo.id")));
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("string")));
		interface->structValue->emplace("id", field);

		field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("pos", std::make_shared<BaseLib::Variable>(1));
		field->structValue->emplace("label", std::make_shared<BaseLib::Variable>(std::string("l10n.loxone.pairingInfo.hostname")));
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("string")));
		interface->structValue->emplace("host", field);

		field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("pos", std::make_shared<BaseLib::Variable>(1));
		field->structValue->emplace("label", std::make_shared<BaseLib::Variable>(std::string("l10n.loxone.pairingInfo.username")));
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("string")));
		interface->structValue->emplace("user", field);

		field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("pos", std::make_shared<BaseLib::Variable>(1));
		field->structValue->emplace("label", std::make_shared<BaseLib::Variable>(std::string("l10n.loxone.pairingInfo.password")));
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("string")));
		interface->structValue->emplace("password", field);

		field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("string")));
		field->structValue->emplace("const", std::make_shared<BaseLib::Variable>(std::string("80")));
		interface->structValue->emplace("port", field);

		interfaces->structValue->emplace("miniserver", interface);

		info->structValue->emplace("interfaces", interfaces);
		//}}}

		return info;
	}
	catch(const std::exception& ex)
	{
		GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	return Variable::createError(-32500, "Unknown application error.");
}
}
