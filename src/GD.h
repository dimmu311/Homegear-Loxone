#ifndef GD_H_
#define GD_H_

#define LOXONE_FAMILY_ID 65
#define LOXONE_FAMILY_NAME "Loxone"

#include <homegear-base/BaseLib.h>

using namespace BaseLib;
using namespace BaseLib::DeviceDescription;

namespace Loxone
{

class Loxone;
class Miniserver;

class GD
{
public:
	virtual ~GD();

	static BaseLib::SharedObjects* bl;
	static Loxone* family;
	static BaseLib::Output out;
    static std::map<std::string, std::shared_ptr<Miniserver>> physicalInterfaces;
    static std::shared_ptr<Miniserver> defaultPhysicalInterface;
private:
	GD();
};

}

#endif
