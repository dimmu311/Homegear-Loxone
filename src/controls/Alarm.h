#ifndef HOMEGEAR_LOXONE_MINISERVER_ALARM_H
#define HOMEGEAR_LOXONE_MINISERVER_ALARM_H

#include "../LoxoneControl.h"

namespace Loxone {
    class Alarm : public LoxoneControl
    {
        /*
        14439fff-003f-2de3-ffff1239b12ff514":{
            "name":"Alarmanlage",
            "type":"Alarm",
            "uuidAction":"14439fff-003f-2de3-ffff1239b12ff514",
            "room":"0df9e099-021c-03e6-ffff1239b12ff514",
            "cat":"0df9e099-01ed-0344-ffff1239b12ff514",
            "defaultRating":0,
            "isFavorite":false,
            "isSecured":true,
            "details":{"
                alert":true,
                "presenceConnected":true
            },
            "states":{
                "armed":"14439fff-003f-2dd6-ffff751123eb8cf1",
                "nextLevel":"14439fff-003f-2daf-ffff751123eb8cf1",
                "nextLevelDelay":"14439fff-003f-2db0-ffff751123eb8cf1",
                "nextLevelDelayTotal":"14439fff-003f-2db1-ffff751123eb8cf1",
                "level":"14439fff-003f-2db2-ffff751123eb8cf1",
                "startTime":"14439fff-003f-2db3-ffff751123eb8cf1",
                "armedDelay":"14439fff-003f-2ddf-ffff751123eb8cf1",
                "armedDelayTotal":"14439fff-003f-2dc9-ffff751123eb8cf1",
                "sensors":"14439fff-003f-2de3-ffff1239b12ff514",
                "disabledMove":"14439fff-003f-2db4-ffff751123eb8cf1"
            },
            "subControls":{
                "14439fff-003f-2de3-ffff1239b12ff514/sensors":{
                    "name":"sensors",
                    "type":"Tracker",
                    "uuidAction":"14439fff-003f-2de3-ffff1239b12ff514/sensors",
                    "defaultRating":0,
                    "isFavorite":false,
                    "isSecured":false,
                    "details":{"maxEntries":40},
                    "states":{"entries":"14439fff-003f-2de3-ffff1239b12ff514"}
                }
            }
        }
        */
    public:
        Alarm(PVariable control, std::string room, std::string cat);
        Alarm(std::shared_ptr<BaseLib::Database::DataTable> rows);
        uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
    protected:
        bool _Alert;
        bool _PresenceConnected;

        //ToDo subControls;
    };
}
#endif //HOMEGEAR_LOXONE_MINISERVER_ALARM_H
