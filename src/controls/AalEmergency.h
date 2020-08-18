#ifndef LOXONE_MINISERVER_AALEMERGENCY_H
#define LOXONE_MINISERVER_AALEMERGENCY_H

#include "../LoxoneControl.h"

namespace Loxone
{
    class AalEmergency : public LoxoneControl
    {
        /*
         "15df68c2-02e1-07a5-ffff14b9c0f9d460":{
            "name":"Notfall Alarm",
            "type":"AalEmergency",
            "uuidAction":"15df68c2-02e1-07a5-ffff14b9c0f9d460",
            "room":"140032b9-0146-2eb7-ffff14b9c0f9d460",
            "cat":"140032b9-00f7-2da2-ffff14b9c0f9d460",
            "defaultRating":0,
            "isFavorite":false,
            "isSecured":false,
            "states":{
                "status":"15df68c2-02e1-0795-ffffd619d970d325",
                "disableEndTime":"15df68c2-02e1-0796-ffffd619d970d325",
                "resetActive":"15df68c2-02e1-0798-ffffd619d970d325"
            },
            "subControls":{
                "15df68c2-02e1-0797-ffffd619d970d325/events":{
                    "name":"events",
                    "type":"Tracker",
                    "uuidAction":"15df68c2-02e1-0797-ffffd619d970d325/events",
                    "defaultRating":0,
                    "isFavorite":false,
                    "isSecured":false,
                    "details":{
                        "maxEntries":20
                    },
                    "states":{
                        "entries":"15df68c2-02e1-0797-ffffd619d970d325"
                    }
                }
            }
        }
         */
    public:
        AalEmergency(PVariable control, std::string room, std::string cat);
        AalEmergency(std::shared_ptr<BaseLib::Database::DataTable> rows);
        uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
        uint32_t getExtraData(std::list<extraData> &extraData);
    protected:
        bool _isFavorite;
    };
}

#endif //LOXONE_MINISERVER_AALEMERGENCY_H
