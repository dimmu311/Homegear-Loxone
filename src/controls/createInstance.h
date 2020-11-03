#ifndef HOMEGEAR_LOXONE_MINISERVER_CREATEINSTANCE_H
#define HOMEGEAR_LOXONE_MINISERVER_CREATEINSTANCE_H

#include <string>
#include "../GD.h"

#include "../LoxoneControl.h"

namespace Loxone
{
    class createInstance
    {
    public:
        static std::shared_ptr<LoxoneControl> createInstanceFromTypeString(PVariable control, std::string room, std::string cat);
        static std::shared_ptr<LoxoneControl> createInstanceFromTypeNr(uint32_t typeNr, std::shared_ptr<BaseLib::Database::DataTable> rows);
    };
}

#endif //HOMEGEAR_LOXONE_MINISERVER_CREATEINSTANCE_H
