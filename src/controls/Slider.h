#ifndef HOMEGEAR_LOXONE_MINISERVER_SLIDER_H
#define HOMEGEAR_LOXONE_MINISERVER_SLIDER_H

#include "../LoxoneControl.h"

namespace Loxone
{
    class Slider : public LoxoneControl
    {
        //TODO: maybe a second value Variable wich only can send values inbetween min and max, see details....
        /*
        "0e3dca32-0006-e500-ffff1239b12ff514":{
            "name":"Volume",
            "type":"Slider",
            "uuidAction":"0e3dca32-0006-e500-ffff1239b12ff514",
            "room":"0df9e099-020c-03b2-ffff1239b12ff514",
            "cat":"0df9e099-01dd-0320-ffff1239b12ff514",
            "defaultRating":0,
            "isFavorite":false,
            "isSecured":false,
            "defaultIcon":null,
            "details":{
                "format":"%.0f%%",
                "min":0.000,
                "max":100.000,
                "step":1.000
            },
            "states":{
                "value":"0e3dca32-0006-e500-ffff1239b12ff514",
                "error":"0e3dca32-0006-e4ff-ffff2c564cfa6c10"
            }
        }
        */
    public:
        Slider(PVariable control, std::string room, std::string cat);
        Slider(std::shared_ptr<BaseLib::Database::DataTable> rows);
        uint32_t getDataToSave(std::list<Database::DataRow> &list, uint32_t peerID);
    protected:
        std::string _defaultIcon;
    };
}
#endif //HOMEGEAR_LOXONE_MINISERVER_SLIDER_H
