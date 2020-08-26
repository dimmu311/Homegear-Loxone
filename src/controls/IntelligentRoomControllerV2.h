#ifndef LOXONE_MINISERVER_INTELLIGENTROOMCONTROLLERV2_H
#define LOXONE_MINISERVER_INTELLIGENTROOMCONTROLLERV2_H

#include "../LoxoneControl.h"

namespace Loxone
{
    class IntelligentRoomControllerV2 : public LoxoneControl
    {
        /*
        "15dd4845-0192-3f34-ffff14b9c0f9d460":{
            "name":"Intelligente Raumregelung",
            "type":"IRoomControllerV2",
            "uuidAction":"15dd4845-0192-3f34-ffff14b9c0f9d460",
            "room":"140032b9-0146-2eb7-ffff14b9c0f9d460",
            "cat":"140032b9-00f1-2d7a-ffff14b9c0f9d460",
            "defaultRating":2,
            "isFavorite":false,
            "isSecured":false,
            "details":{
                "timerModes":[
                    {
                        "name":"Anwesend",
                        "description":"Komfortbetrieb",
                        "id":1
                    },{
                        "name":"Abwesend",
                        "description":"Sparbetrieb",
                        "id":0
                    },{
                        "name":"Aus",
                        "description":"Gebäudeschutz",
                        "id":2
                    }
                ],
                "format":"%.1f°",
                "connectedInputs":0
            },
            */
            /*
            "states":{
                "tempActual":"15dd4845-0192-3f0c-ffff09b0870b142f",
                "tempTarget":"15dd4845-0192-3f32-ffff09b0870b142f",
                "comfortTemperature":"15dd4845-0192-3f14-ffff09b0870b142f",
                "comfortTolerance":"15dd4845-0192-3f15-ffff09b0870b142f",
                "absentMinOffset":"15dd4845-0192-3f16-ffff09b0870b142f",
                "absentMaxOffset":"15dd4845-0192-3f17-ffff09b0870b142f",
                "frostProtectTemperature":"15dd4845-0192-3f18-ffff09b0870b142f",
                "heatProtectTemperature":"15dd4845-0192-3f19-ffff09b0870b142f",
                "activeMode":"15dd4845-0192-3f06-ffff09b0870b142f",
                "comfortTemperatureOffset":"15dd4845-0192-3f01-ffff09b0870b142f",
                "overrideEntries":"15dd4845-0192-3f02-ffff09b0870b142f",
                "prepareState":"15dd4845-0192-3f03-ffff09b0870b142f",
                "useOutdoor":"15dd4845-0192-3f04-ffff09b0870b142f",
                "operatingMode":"15dd4845-0192-3f07-ffff09b0870b142f",
                "overrideReason":"15dd4845-0192-3f09-ffff09b0870b142f",
                "openWindow":"15dd4845-0192-3f0d-ffff09b0870b142f",
                "modeList":"15dd4845-0192-3f08-ffff09b0870b142f"},
                "subControls":{
                    "15dd4845-0192-3f00-ffff09b0870b142f":{
                        "name":"Heating and Cooling",
                        "type":"IRCV2Daytimer",
                        "uuidAction":"15dd4845-0192-3f00-ffff09b0870b142f",
                        "defaultRating":0,
                        "isFavorite":false,
                        "isSecured":false,
                        "details":{"
                            analog":true,
                            "format":"%.1f°"
                        },
                        "states":{
                            "entriesAndDefaultValue":"15dd4845-0192-3f00-ffff09b0870b142f",
                            "mode":"15dd4845-0192-3f33-ffff09b0870b142f",
                            "modeList":"15dd4845-0192-3f08-ffff09b0870b142f",
                            "value":"15dd4845-0192-3f06-ffff09b0870b142f"
                        }
                    }
                }
            }
            */
            //TODO: Handle commands override/{modeId}/[{until}]/[{temp}], stopOverride, setComfortModeTemp/{temp}, set/ and modeslist
    public:
        IntelligentRoomControllerV2(PVariable control, std::string room, std::string cat);
        IntelligentRoomControllerV2(std::shared_ptr<BaseLib::Database::DataTable> rows);
        uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
    protected:
    };
}

#endif //LOXONE_MINISERVER_INTELLIGENTROOMCONTROLLERV2_H
