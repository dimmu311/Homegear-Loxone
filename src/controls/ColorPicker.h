#ifndef COLORPICKERV2_H
#define COLORPICKERV2_H

#include "../LoxoneControl.h"

namespace Loxone
{
    class ColorPicker : public LoxoneControl
    {
    public:
        ColorPicker(PVariable control, std::unordered_map<std::string, std::string> &room, std::unordered_map<std::string, std::string> &cat);
        ColorPicker(std::shared_ptr<BaseLib::Database::DataTable> rows, uint32_t typeNr);

        bool processPacket(PLoxoneTextStatesPacket loxonePacket);
        //bool processPacket(PLoxoneValueStatesPacket loxonePacket);

        bool setValue(PPacket frame, PVariable parameters, uint32_t channel, std::string &command, bool &isSecured);
    protected:
    };
}
#endif //COLORPICKERV2_H
