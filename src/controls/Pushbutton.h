#ifndef HOMEGEAR_LOXONE_MINISERVER_PUSHBUTTON_H
#define HOMEGEAR_LOXONE_MINISERVER_PUSHBUTTON_H

#include "../LoxoneControl.h"

namespace Loxone
{
class Pushbutton : public LoxoneControl
    {
    /*
    "0fd2c255-02f8-1875-ffff1239b12ff514":{
        "name":"Eingang VI2",
        "type":"Pushbutton",
        "uuidAction":"0fd2c255-02f8-1875-ffff1239b12ff514",
        "room":"0df9e099-021c-03e6-ffff1239b12ff514",
        "cat":"0df9e099-01ce-030c-ffff1239b12ff514",
        "defaultRating":0,
        "isFavorite":false,
        "isSecured":false,
        "defaultIcon":null,
        "states":{"
            active":"0fd2c255-02f8-1875-ffff1239b12ff514"
        }
    }
    */
    public:
        Pushbutton(PVariable control, std::string room, std::string cat);
        Pushbutton(std::shared_ptr<BaseLib::Database::DataTable> rows);
        uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);

        //bool processPacket(PLoxoneValueStatesPacket loxonePacket);
    protected:
        bool _isFavorite;
        std::string _defaultIcon;
    };
}

#endif //HOMEGEAR_LOXONE_MINISERVER_PUSHBUTTON_H
