#ifndef HOMEGEAR_LOXONE_MINISERVER_DIMMER_H
#define HOMEGEAR_LOXONE_MINISERVER_DIMMER_H

#include "../LoxoneControl.h"

namespace Loxone
{
    class Dimmer : public LoxoneControl
    {
        /*
        "10bb08c1-0021-45bf-ffff1239b12ff514":{
            "name":"Dimmer",
            "type":"Dimmer",
            "uuidAction":"10bb08c1-0021-45bf-ffff1239b12ff514",
            "room":"0df9e099-020c-03b2-ffff1239b12ff514",
            "cat":"0df9e099-01dd-0320-ffff1239b12ff514",
            "defaultRating":0,
            "isFavorite":false,
            "isSecured":false,
            "states":{
                "position":"10bb08c1-0021-45bd-ffff9a107b038561",
                "min":"10bb08c1-0021-45b9-ffff9a107b038561",
                "max":"10bb08c1-0021-45ba-ffff9a107b038561",
                "step":"10bb08c1-0021-45b7-ffff9a107b038561"
            }
        }
        */
    public:
        Dimmer(PVariable control, std::string room, std::string cat);
        Dimmer(std::shared_ptr<BaseLib::Database::DataTable> rows);
        uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
    protected:
        bool _isFavorite;
    };
}


#endif //HOMEGEAR_LOXONE_MINISERVER_DIMMER_H
