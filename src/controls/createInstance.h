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
        static const std::unordered_map<std::string, LoxoneControl * (*)(PVariable, std::string, std::string)> _controlsMap;
        static const std::unordered_map<uint32_t, LoxoneControl * (*)(std::shared_ptr < BaseLib::Database::DataTable > )> _uintControlsMap;
    };
}

#endif //HOMEGEAR_LOXONE_MINISERVER_CREATEINSTANCE_H
