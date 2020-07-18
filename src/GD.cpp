#include "GD.h"
#include "PhysicalInterfaces/Miniserver.h"
#include "Loxone.h"

namespace Loxone
{
    BaseLib::SharedObjects* GD::bl = nullptr;
    Loxone* GD::family = nullptr;
    BaseLib::Output GD::out;
    std::map<std::string, std::shared_ptr<Miniserver>> GD::physicalInterfaces;
    std::shared_ptr<Miniserver> GD::defaultPhysicalInterface;
}
