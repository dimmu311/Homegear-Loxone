#ifndef SLIDER_H
#define SLIDER_H

#include "../LoxoneControl.h"

namespace Loxone
{
    class Slider : public LoxoneControl
    {
    public:
        Slider(PVariable control, std::unordered_map<std::string, std::string> &room, std::unordered_map<std::string, std::string> &cat);
        Slider(std::shared_ptr<BaseLib::Database::DataTable> rows);
        uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
    protected:
        std::string _defaultIcon;
    };
}
#endif //SLIDER_H
