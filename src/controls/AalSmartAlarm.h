#ifndef LOXONE_MINISERVER_AALSMARTALARM_H
#define LOXONE_MINISERVER_AALSMARTALARM_H

#include "../LoxoneControl.h"

namespace Loxone
{
    class AalSmartAlarm: public LoxoneControl
    {
        /*
        "15e8124a-0291-628c-ffff14b9c0f9d460":{
            "name":"AAL Smart Alarm",
            "type":"AalSmartAlarm",
            "uuidAction":"15e8124a-0291-628c-ffff14b9c0f9d460",
            "room":"140032b9-0146-2eb7-ffff14b9c0f9d460",
            "cat":"140032b9-00f7-2da2-ffff14b9c0f9d460",
            "defaultRating":0,
            "isFavorite":false,
            "isSecured":false,
            "states":{
                "alarmLevel":"15e8124a-0291-6274-ffff0a5d746f6c98",
                "alarmCause":"15e8124a-0291-6275-ffff0a5d746f6c98",
                "alarmTime":"15e8124a-0291-6276-ffff0a5d746f6c98",
                "isLocked":"15e8124a-0291-6277-ffff0a5d746f6c98",
                "isLeaveActive":"15e8124a-0291-6278-ffff0a5d746f6c98",
                "disableEndTime":"15e8124a-0291-6279-ffff0a5d746f6c98"
            },"subControls":{
                "15e8124a-0291-627a-ffff0a5d746f6c98/events":{
                    "name":"events",
                    "type":"Tracker",
                    "uuidAction":"15e8124a-0291-627a-ffff0a5d746f6c98/events",
                    "defaultRating":0,
                    "isFavorite":false,
                    "isSecured":false,
                    "details":{
                        "maxEntries":20
                    },"states":{
                        "entries":"15e8124a-0291-627a-ffff0a5d746f6c98"
                    }
                }
            }
        }
         */
    public:
        AalSmartAlarm(PVariable control, std::string room, std::string cat);
        AalSmartAlarm(std::shared_ptr<BaseLib::Database::DataTable> rows);
    protected:
    };
}
#endif //LOXONE_MINISERVER_AALSMARTALARM_H
