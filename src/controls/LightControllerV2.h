#ifndef HOMEGEAR_LOXONE_MINISERVER_LIGHTCONTROLLERV2_H
#define HOMEGEAR_LOXONE_MINISERVER_LIGHTCONTROLLERV2_H

#include "../LoxoneControl.h"

namespace Loxone {
    class LightControllerV2 : public LoxoneControl
    {
        /*
        "14f29553-0385-5bcf-ffff1239b12ff514":{
            "name":"Lichtsteuerung",
            "type":"LightControllerV2",
            "uuidAction":"14f29553-0385-5bcf-ffff1239b12ff514",
            "room":"0df9e099-020c-03a6-ffff1239b12ff514",
            "cat":"0df9e099-01dd-0310-ffff1239b12ff514",
            "defaultRating":0,
            "isFavorite":false,
            "isSecured":false,
            "details":{
                "movementScene":1,
                "masterValue":"14f29553-0385-5bcf-ffff1239b12ff514/masterValue",
                "masterColor":"14f29553-0385-5bcf-ffff1239b12ff514/masterColor"
            },
            "states":{
                "activeMoods":"14f29553-0385-5b87-ffffd050f836ca54",
                "moodList":"14f29553-0385-5b88-ffffd050f836ca54",
                "favoriteMoods":"14f29553-0385-5b89-ffffd050f836ca54",
                "additionalMoods":"14f29553-0385-5b8a-ffffd050f836ca54"
            },
            */
        /*
        "subControls":{
            "14f29553-0385-5bcf-ffff1239b12ff514/masterValue":{
                "name":"Master-Helligkeit",
                "type":"Dimmer",
                "uuidAction":"14f29553-0385-5bcf-ffff1239b12ff514/masterValue",
                "defaultRating":0,
                "isFavorite":false,
                "isSecured":false,
                "states":{
                    "position":"14f29553-0385-5bcb-ffffd050f836ca54",
                    "min":"14f29553-0385-5b8c-ffffd050f836ca54",
                    "max":"14f29553-0385-5b8d-ffffd050f836ca54",
                    "step":"14f29553-0385-5ba7-ffffd050f836ca54"
                }
            },
            "14f29553-0385-5bcf-ffff1239b12ff514/masterColor":{
                "name":"Master-Farbe","type":"ColorPickerV2",
                "uuidAction":"14f29553-0385-5bcf-ffff1239b12ff514/masterColor",
                "defaultRating":0,
                "isFavorite":false,
                "isSecured":false,
                "details":{
                    "pickerType":"Rgb"
                },
                "states":{
                    "color":"14f29553-0385-5bca-ffffd050f836ca54",
                    "sequence":"14e86293-036d-8616-ffff3ce43fa9aec5",
                    "sequenceColorIdx":"14e86293-036d-8617-ffff3ce43fa9aec5"
                }
            },
            "14f29553-0385-5bcf-ffff1239b12ff514/AI1":{
                "name":"AQ1",
                "type":"Switch",
                "uuidAction":"14f29553-0385-5bcf-ffff1239b12ff514/AI1",
                "defaultRating":0,
                "isFavorite":false,
                "isSecured":false,
                "states":{
                    "active":"14f29553-0385-5bb8-ffffd050f836ca54"
                }
            },
            "14f29553-0385-5bcf-ffff1239b12ff514/AI2":{
                "name":"AQ2",
                "type":"Dimmer",
                "uuidAction":"14f29553-0385-5bcf-ffff1239b12ff514/AI2",
                "defaultRating":0,
                "isFavorite":false,
                "isSecured":false,
                "states":{
                    "position":"14f29553-0385-5bb9-ffffd050f836ca54",
                    "min":"14f29553-0385-5ba9-ffffd050f836ca54",
                    "max":"14f29553-0385-5baa-ffffd050f836ca54",
                    "step":"14f29553-0385-5ba7-ffffd050f836ca54"
                }
            },
            "14f29553-0385-5bcf-ffff1239b12ff514/AI3":{
                "name":"AQ3",
                "type":
                "ColorPickerV2",
                "uuidAction":"14f29553-0385-5bcf-ffff1239b12ff514/AI3",
                "defaultRating":0,
                "isFavorite":false,
                "isSecured":false,
                "details":{
                    "pickerType":"Rgb"
                },
                "states":{
                    "color":"14f29553-0385-5bba-ffffd050f836ca54",
                    "sequence":"14e86293-036d-85f6-ffff3ce43fa9aec5",
                    "sequenceColorIdx":"14e86293-036d-85f7-ffff3ce43fa9aec5"
                }
            },
            "14f29553-0385-5bcf-ffff1239b12ff514/AI4":{
                "name":"AQ4","type":"ColorPickerV2",
                "uuidAction":"14f29553-0385-5bcf-ffff1239b12ff514/AI4",
                "defaultRating":0,
                "isFavorite":false,
                "isSecured":false,
                "details":{"
                    pickerType":"Lumitech"
                },
                "states":{
                    "color":"14f29553-0385-5bbb-ffffd050f836ca54",
                    "sequence":"14e86293-036d-85f8-ffff3ce43fa9aec5",
                    "sequenceColorIdx":"14e86293-036d-85f9-ffff3ce43fa9aec5"
                }
            },
            "14f29553-0385-5bcf-ffff1239b12ff514/AI5":{
                "name":"AQ5",
                "type":"Dimmer",
                "uuidAction":"14f29553-0385-5bcf-ffff1239b12ff514/AI5",
                "defaultRating":0,
                "isFavorite":false,
                "isSecured":false,
                "states":{
                    "position":"14f29553-0385-5bbc-ffffd050f836ca54",
                    "min":"14f29553-0385-5ba9-ffffd050f836ca54",
                    "max":"14f29553-0385-5baa-ffffd050f836ca54",
                    "step":"14f29553-0385-5ba7-ffffd050f836ca54"
                }
            }
        }
    }
    */
    public:
        LightControllerV2(PVariable control, std::string room, std::string cat);
        LightControllerV2(std::shared_ptr<BaseLib::Database::DataTable> rows);
        uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
    protected:
        bool _isFavorite;
        uint32_t _detMovementScene;
        std::string _detMasterValue;
        std::string _detMasterColor;
    };
}


#endif //HOMEGEAR_LOXONE_MINISERVER_LIGHTCONTROLLERV2_H
