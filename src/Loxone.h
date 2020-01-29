#ifndef LOXONE_H_
#define LOXONE_H_

#include <homegear-base/BaseLib.h>

using namespace BaseLib;

namespace Loxone
{
class LoxoneCentral;

class Loxone : public BaseLib::Systems::DeviceFamily
{
public:
	Loxone(BaseLib::SharedObjects* bl, BaseLib::Systems::IFamilyEventSink* eventHandler);
    Loxone(const Loxone&) = delete;
    Loxone& operator=(const Loxone&) = delete;
	virtual ~Loxone() = default;
	virtual void dispose();

	virtual bool hasPhysicalInterface() { return false; }
	virtual PVariable getPairingInfo();
protected:
	virtual void createCentral();
	virtual std::shared_ptr<BaseLib::Systems::ICentral> initializeCentral(uint32_t deviceId, int32_t address, std::string serialNumber);
};

}

#endif
