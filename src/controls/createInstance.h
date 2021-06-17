#ifndef CREATEINSTANCE_H
#define CREATEINSTANCE_H

#include <string>
#include "../GD.h"

#include "../LoxoneControl.h"

namespace Loxone
{
    class createInstance
    {
    public:
        static std::shared_ptr<LoxoneControl> createInstanceFromTypeString(PVariable control, std::unordered_map<std::string, std::string> &room, std::unordered_map<std::string, std::string> &cat);
        static std::shared_ptr<LoxoneControl> createInstanceFromTypeNr(uint32_t typeNr, std::shared_ptr<BaseLib::Database::DataTable> rows);
    };
}

#endif //CREATEINSTANCE_H
