#include "Factory.h"
#include "../config.h"
#include "GD.h"

BaseLib::Systems::DeviceFamily* LoxoneFactory::createDeviceFamily(BaseLib::SharedObjects* bl, BaseLib::Systems::IFamilyEventSink* eventHandler)
{
        return new Loxone::Loxone(bl, eventHandler);
}

std::string getVersion()
{
	return VERSION;
}

int32_t getFamilyId()
{
	return LOXONE_FAMILY_ID;
}

std::string getFamilyName()
{
	return LOXONE_FAMILY_NAME;
}

BaseLib::Systems::SystemFactory* getFactory()
{
	return (BaseLib::Systems::SystemFactory*)(new LoxoneFactory);
}
